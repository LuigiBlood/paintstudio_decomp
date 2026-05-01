#!/usr/bin/env python3

import argparse
import json
from pathlib import Path
import sys
from typing import Any, Dict


def fail(message: str) -> "None":
    print(f"Error: {message}", file=sys.stderr)
    sys.exit(1)


def load_progress(progress_path: Path) -> Dict[str, Any]:
    try:
        with progress_path.open("r", encoding="utf-8") as progress_file:
            return json.load(progress_file)
    except OSError as exc:
        fail(f"Could not read `{progress_path}`: {exc}")
    except json.JSONDecodeError as exc:
        fail(f"Could not parse JSON from `{progress_path}`: {exc}")


def format_ratio(ratio: Any) -> str:
    if not isinstance(ratio, (int, float)):
        fail(f"Expected a numeric ratio, got `{type(ratio).__name__}`")

    return f"{ratio * 100:.2f}%"


def build_badge(label: str, ratio: Any) -> Dict[str, Any]:
    return {
        "schemaVersion": 1,
        "label": label,
        "message": format_ratio(ratio),
        "color": "blue",
    }


def write_badge(output_path: Path, badge_data: Dict[str, Any]):
    output_path.parent.mkdir(parents=True, exist_ok=True)

    try:
        with output_path.open("w", encoding="utf-8") as output_file:
            json.dump(badge_data, output_file, indent=2)
            output_file.write("\n")
    except OSError as exc:
        fail(f"Could not write `{output_path}`: {exc}")


def main():
    parser = argparse.ArgumentParser(description="Generate Shields endpoint JSON files from progress data.")
    parser.add_argument("progress_json", type=Path, help="Path to the JSON file emitted by tools/progress.py.")
    parser.add_argument("output_dir", type=Path, help="Directory where badge JSON files should be written.")
    args = parser.parse_args()

    progress = load_progress(args.progress_json)

    try:
        code_ratio = progress["code"]["decompiled_ratio"]
        data_ratio = progress["data"]["decompiled_ratio"]
    except KeyError as exc:
        fail(f"Missing key in progress JSON: {exc}")

    write_badge(args.output_dir / "code.json", build_badge("Code", code_ratio))
    write_badge(args.output_dir / "data.json", build_badge("Data", data_ratio))


if __name__ == "__main__":
    main()