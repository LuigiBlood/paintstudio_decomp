#!/usr/bin/env python3

import argparse
from dataclasses import dataclass
from io import TextIOWrapper
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
from typing import Optional

SCRIPT_DIR = Path(os.path.dirname(os.path.realpath(__file__)))
ROOT_DIR = Path(os.path.abspath(os.path.join(SCRIPT_DIR, "..")))

BUILD_PATH = ROOT_DIR.joinpath("build")

READELF_CANDIDATES = (
    "mips-linux-gnu-readelf",
    "mipsel-linux-gnu-readelf",
    "mips64-elf-readelf",
    "mips-elf-readelf",
    "readelf",
)

CODE_SECTION_PREFIXES = (".text",)
DATA_SECTION_PREFIXES = (
    ".data",
    ".rodata",
    ".bss",
    ".sdata",
    ".sbss",
    ".rdata",
    ".lit4",
    ".lit8",
)

ZERO_SIZE_LABEL_RE = re.compile(r"^\.?L[0-9A-Fa-f_]+$")
SECTION_HEADER_RE = re.compile(
    r"^\s*\[\s*\d+\]\s+(?P<name>\S+)\s+\S+\s+\S+\s+\S+\s+(?P<size>[0-9A-Fa-f]+)\b"
)
GLOBAL_ASM_RE = re.compile(r'^\s*(?:#pragma\s+)?GLOBAL_ASM\("(?P<path>[^"]+)"\)')
CONTROL_KEYWORDS = {"if", "for", "while", "switch", "else", "case", "do"}


@dataclass
class CodeProgress:
    total_bytes: int
    total_funcs: int
    decompiled_bytes: int
    decompiled_funcs: int

    @property
    def decompiled_bytes_ratio(self) -> float:
        return safe_ratio(self.decompiled_bytes, self.total_bytes)

    @property
    def decompiled_funcs_ratio(self) -> float:
        return safe_ratio(self.decompiled_funcs, self.total_funcs)


@dataclass
class DataProgress:
    total_bytes: int
    decompiled_bytes: int

    @property
    def decompiled_ratio(self) -> float:
        return safe_ratio(self.decompiled_bytes, self.total_bytes)


@dataclass
class OverallProgress:
    code: CodeProgress
    data: DataProgress

    @property
    def total_bytes(self) -> int:
        return self.code.total_bytes + self.data.total_bytes

    @property
    def decompiled_bytes(self) -> int:
        return self.code.decompiled_bytes + self.data.decompiled_bytes

    @property
    def decompiled_ratio(self) -> float:
        return safe_ratio(self.decompiled_bytes, self.total_bytes)


def safe_ratio(numerator: int, denominator: int) -> float:
    if denominator == 0:
        return 0.0
    return numerator / denominator


def fail(message: str) -> "None":
    print(f"Error: {message}", file=sys.stderr)
    sys.exit(1)


def find_tool(candidates: tuple[str, ...]) -> str:
    for tool_name in candidates:
        tool_path = shutil.which(tool_name)
        if tool_path:
            return tool_path

    fail(f"Could not find any of these tools in PATH: {', '.join(candidates)}")


READELF = find_tool(READELF_CANDIDATES)


def run_command(command: list[str], cwd: Path = ROOT_DIR) -> str:
    try:
        result = subprocess.run(
            command,
            cwd=cwd,
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
    except FileNotFoundError:
        fail(f"Missing required tool `{command[0]}`")
    except subprocess.CalledProcessError as exc:
        stderr = exc.stderr.strip()
        if stderr:
            fail(stderr)
        fail(f"Command failed: {' '.join(command)}")

    return result.stdout


def try_command(command: list[str], cwd: Path = ROOT_DIR) -> Optional[str]:
    try:
        result = subprocess.run(
            command,
            cwd=cwd,
            check=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )
    except (FileNotFoundError, subprocess.CalledProcessError):
        return None

    return result.stdout.strip()


def find_build_elf() -> Path:
    elf_paths = sorted(BUILD_PATH.glob("*.elf"))

    if len(elf_paths) == 1:
        return elf_paths[0]
    if len(elf_paths) == 0:
        fail("Could not find a built `.elf` in `build/` - make sure that the project is built")

    fail("Found multiple `.elf` files in `build/`; update tools/progress.py to select the right one explicitly")


def find_object_files() -> list[Path]:
    if not BUILD_PATH.exists():
        fail("Missing `build/` directory - make sure that the project is built")

    object_files: list[Path] = []
    for obj_path in BUILD_PATH.rglob("*.o"):
        rel_path = obj_path.relative_to(BUILD_PATH)
        if rel_path.parts and rel_path.parts[0] == "tools":
            continue
        object_files.append(obj_path)

    if not object_files:
        fail("Could not find any built object files in `build/` - make sure that the project is built")

    return object_files


def find_source_object_files() -> list[Path]:
    source_objects: list[Path] = []

    for object_path in find_object_files():
        if not object_path.name.endswith((".c.o", ".cpp.o")):
            continue

        rel_path = object_path.relative_to(BUILD_PATH)
        if rel_path.parts[:2] == ("src", "libultra"):
            continue

        source_objects.append(object_path)

    return source_objects


def get_source_path_for_object(object_path: Path) -> Path:
    rel_path = object_path.relative_to(BUILD_PATH)
    return ROOT_DIR.joinpath(rel_path.with_suffix(""))


def get_func_sizes(binary_path: Path) -> tuple[dict[str, int], int]:
    lines = run_command([READELF, "--symbols", str(binary_path)]).splitlines()

    sizes: dict[str, int] = {}
    total = 0

    for line in lines:
        if "FUNC" not in line:
            continue

        components = line.split()
        if len(components) < 8:
            continue

        try:
            size = int(components[2])
        except ValueError:
            continue

        name = components[-1]

        # Keep zero-sized asm functions, but ignore branch labels and similar local labels.
        if size == 0 and ZERO_SIZE_LABEL_RE.match(name):
            continue

        total += size
        sizes[name] = size

    return sizes, total


def parse_function_name(statement: str) -> Optional[str]:
    if not statement.endswith("{") or "(" not in statement:
        return None

    header = statement[:-1].strip()
    if not header or ";" in header:
        return None

    prefix = header.split("(", 1)[0].strip()
    if not prefix:
        return None

    name = prefix.split()[-1].strip("*")
    if name in CONTROL_KEYWORDS:
        return None
    if not re.match(r"^[A-Za-z_]\w*$", name):
        return None

    return name


def get_source_defined_funcs(source_path: Path) -> set[str]:
    try:
        lines = source_path.read_text(encoding="utf-8").splitlines()
    except OSError as exc:
        fail(f"Could not read source file `{source_path}`: {exc}")

    functions: set[str] = set()
    statement_parts: list[str] = []
    brace_depth = 0

    for line in lines:
        stripped = line.strip()

        if brace_depth == 0:
            if not stripped:
                continue

            if GLOBAL_ASM_RE.match(stripped):
                statement_parts.clear()
                continue

            if stripped.startswith("#"):
                continue

            statement_parts.append(stripped)

            delta = line.count("{") - line.count("}")
            brace_depth += delta

            if brace_depth > 0:
                name = parse_function_name(" ".join(statement_parts))
                if name is not None:
                    functions.add(name)
                statement_parts.clear()
            elif stripped.endswith(";"):
                statement_parts.clear()
        else:
            brace_depth += line.count("{") - line.count("}")

    return functions


def get_code_progress() -> CodeProgress:
    func_sizes, total_bytes = get_func_sizes(find_build_elf())

    decompiled_funcs = 0
    decompiled_bytes = 0

    for object_path in find_source_object_files():
        source_path = get_source_path_for_object(object_path)
        source_defined_funcs = get_source_defined_funcs(source_path)
        object_func_sizes, _ = get_func_sizes(object_path)

        for func_name in source_defined_funcs:
            func_size = object_func_sizes.get(func_name)
            if func_size is None:
                continue

            decompiled_funcs += 1
            decompiled_bytes += func_size

    return CodeProgress(
        total_bytes=total_bytes,
        total_funcs=len(func_sizes),
        decompiled_bytes=decompiled_bytes,
        decompiled_funcs=decompiled_funcs,
    )


def get_object_section_sizes(object_path: Path) -> tuple[int, int]:
    lines = run_command([READELF, "-W", "-S", str(object_path)]).splitlines()

    code_bytes = 0
    data_bytes = 0

    for line in lines:
        match = SECTION_HEADER_RE.match(line)
        if not match:
            continue

        section_name = match.group("name")
        section_size = int(match.group("size"), 16)
        if section_size == 0:
            continue

        if section_name.startswith(CODE_SECTION_PREFIXES):
            code_bytes += section_size
        elif section_name.startswith(DATA_SECTION_PREFIXES):
            data_bytes += section_size

    return code_bytes, data_bytes


def get_data_progress() -> DataProgress:
    total_bytes = 0
    decompiled_bytes = 0
    source_object_files = set(find_source_object_files())

    for object_path in find_object_files():
        _, data_bytes = get_object_section_sizes(object_path)
        total_bytes += data_bytes

        if object_path in source_object_files:
            decompiled_bytes += data_bytes

    return DataProgress(total_bytes=total_bytes, decompiled_bytes=decompiled_bytes)


def get_overall_progress() -> OverallProgress:
    return OverallProgress(code=get_code_progress(), data=get_data_progress())


def get_git_info() -> Optional[dict[str, object]]:
    commit_hash = try_command(["git", "rev-parse", "HEAD"])
    commit_hash_short = try_command(["git", "rev-parse", "--short=7", "HEAD"])
    commit_timestamp = try_command(["git", "show", "-s", "--format=%ct", "HEAD"])

    if commit_hash is None or commit_hash_short is None or commit_timestamp is None:
        return None

    return {
        "commit_hash": commit_hash,
        "commit_hash_short": commit_hash_short,
        "commit_timestamp": int(commit_timestamp),
    }


def output_json(progress: OverallProgress, file: TextIOWrapper):
    data: dict[str, object] = {
        "total": {
            "decompiled_ratio": progress.decompiled_ratio,
            "decompiled_bytes": progress.decompiled_bytes,
            "total_bytes": progress.total_bytes,
        },
        "code": {
            "decompiled_ratio": progress.code.decompiled_bytes_ratio,
            "decompiled_funcs_ratio": progress.code.decompiled_funcs_ratio,
            "decompiled_funcs": progress.code.decompiled_funcs,
            "decompiled_bytes": progress.code.decompiled_bytes,
            "total_funcs": progress.code.total_funcs,
            "total_bytes": progress.code.total_bytes,
        },
        "data": {
            "decompiled_ratio": progress.data.decompiled_ratio,
            "decompiled_bytes": progress.data.decompiled_bytes,
            "total_bytes": progress.data.total_bytes,
        },
    }

    git_info = get_git_info()
    if git_info is not None:
        data["git"] = git_info

    json.dump(data, file, indent=2)


def print_progress(progress: OverallProgress):
    print(
        f"{progress.code.decompiled_funcs} matched code functions / "
        f"{progress.code.total_funcs} total ({progress.code.decompiled_funcs_ratio * 100:.2f}%)"
    )
    print(
        f"{progress.code.decompiled_bytes} decompiled code bytes / "
        f"{progress.code.total_bytes} total ({progress.code.decompiled_bytes_ratio * 100:.2f}%)"
    )
    print()
    print(
        f"{progress.data.decompiled_bytes} decompiled data bytes / "
        f"{progress.data.total_bytes} total ({progress.data.decompiled_ratio * 100:.2f}%)"
    )
    print()
    print(
        f"{progress.decompiled_bytes} total decompiled bytes / "
        f"{progress.total_bytes} total ({progress.decompiled_ratio * 100:.2f}%)"
    )


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Computes and reports progress for the project.")
    parser.add_argument("-q", "--quiet", action="store_true", help="Don't print messages to stdout.", default=False)
    parser.add_argument("--json", type=argparse.FileType("w", encoding="utf-8"), help="File to write the current progress to as JSON.")

    args = parser.parse_args()

    if not args.quiet:
        print("Calculating progress...")

    progress = get_overall_progress()

    if args.json:
        with args.json as json_file:
            output_json(progress, json_file)

    if not args.quiet:
        print()
        print_progress(progress)
