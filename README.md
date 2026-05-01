# Mario Artist Paint Studio Decompilation

Required base ROM file:

- `dmpj.d64` with MD5 `3546bc6d93928a28285a91478bc41b68`

Convert a Mario Artist Paint Studio NDD dump to D64 with [leo64dd_python](https://github.com/LuigiBlood/leo64dd_python).
The RAM area should be only `0xFF`.


## Quick Start

If dependencies are already installed and `dmpj.d64` is in the repo root:

```sh
python3 -m pip install -r tools/splat/requirements.txt
make setup
make
```

## Prerequisites

- Python 3
- `make`
- `git`
- `curl`
- `md5sum`
- a MIPS binutils toolchain that provides one of these prefixes:
	- `mips-linux-gnu-*`
	- `mipsel-linux-gnu-*`
	- `mips64-elf-*`
	- `mips-elf-*`

`make setup` will:

- initialize/update submodules
- download IDO 7.1 into `tools/ido_recomp/<host>/7.1`
- run `tools/splat` to regenerate `asm/`, `bin/`, and `assets/` from `dmpj.d64`

## Linux

Ubuntu/Debian:

```sh
sudo apt-get update
sudo apt-get install -y \
	binutils-mips-linux-gnu \
	gcc \
	git \
	libc-dev \
	libc6-dev \
	make \
	ninja-build \
	openssl \
	python3 \
	python3-pip \
	curl
```

Install Python dependencies:

```sh
python3 -m pip install -r tools/splat/requirements.txt
```

Then place `dmpj.d64` in the repository root and run:

```sh
make setup
make
```

## macOS

Install Xcode Command Line Tools first if they are not already installed:

```sh
xcode-select --install
```

Install the required packages with Homebrew:

```sh
brew install \
	coreutils \
	git \
	make \
	mipsel-linux-gnu-binutils \
	python
```

Install the Python dependencies:

```sh
python3 -m pip install -r tools/splat/requirements.txt
```

Then place `dmpj.d64` in the repository root and run:

```sh
make setup
make
```
