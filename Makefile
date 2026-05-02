#################################################################
#              Mario Artist Paint Studio Makefile               #
#################################################################

BASEROM = dmpj.d64
TARGET = dmpj
NON_MATCHING ?= 0
RUN_CC_CHECK ?= 0
WERROR ?= 0

# Fail early if baserom does not exist
ifeq ($(wildcard $(BASEROM)),)
$(error Baserom `$(BASEROM)' not found.)
endif

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
  HOST_OS := linux
else ifeq ($(UNAME_S),Darwin)
  HOST_OS := macos
else
  $(error Unsupported host/building OS <$(UNAME_S)>)
endif
UNAME_M := $(shell uname -m)

BUILD_DIR := build
ROM := $(TARGET).d64
ELF := $(BUILD_DIR)/$(TARGET).elf
LD_SCRIPT := $(TARGET).ld
LD_MAP := $(BUILD_DIR)/$(TARGET).map
ASM_DIRS := asm asm/data asm/main asm/capture \
	asm/overlays/printer asm/overlays/saveload asm/overlays/camera \
	asm/data/main asm/data/capture asm/data/overlays/printer asm/data/overlays/saveload asm/data/overlays/camera \
	asm/leo/lib asm/data/leo/lib
DATA_DIRS := bin assets
SRC_DIRS := $(shell test ! -d src || find src -type d)

########## Make tools ##########

IDO_STATIC_RECOMP_VERSION := v1.2
IDO_RECOMP_ROOT := tools/ido_recomp/$(HOST_OS)
IDO_71_RECOMP_DIR := $(IDO_RECOMP_ROOT)/7.1
IDO_71_RECOMP_CC := $(IDO_71_RECOMP_DIR)/cc

ifeq ($(HOST_OS),linux)
ifneq ($(filter arm64 aarch64 armv7l armv6l,$(UNAME_M)),)
	IDO_RECOMP_PLATFORM_ID := linux-arm
else
	IDO_RECOMP_PLATFORM_ID := linux
endif
else ifeq ($(HOST_OS),macos)
	IDO_RECOMP_PLATFORM_ID := macos
endif

ido_download_url = https://github.com/decompals/ido-static-recomp/releases/download/$(IDO_STATIC_RECOMP_VERSION)/ido-$(1)-recomp-$(IDO_RECOMP_PLATFORM_ID).tar.gz

#DUMMY != make -s -C tools >&2 || echo FAIL
#ifeq ($(DUMMY),FAIL)
  #$(error Failed to build tools)
#endif

###################### Other Tools ######################

C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
S_FILES := $(foreach dir,$(SRC_DIRS) $(ASM_DIRS),$(wildcard $(dir)/*.s))
DATA_FILES := $(foreach dir,$(DATA_DIRS),$(wildcard $(dir)/*.bin))
PNG_FILES     := $(foreach dir,$(DATA_DIRS),$(wildcard $(dir)/*.png))

PNG_INC_FILES := $(foreach f,$(PNG_FILES:.png=.inc),$(BUILD_DIR)/$f)

# Object files
O_FILES := $(foreach file,$(C_FILES),$(BUILD_DIR)/$(file:.c=.c.o)) \
           $(foreach file,$(S_FILES),$(BUILD_DIR)/$(file:.s=.s.o)) \
           $(foreach file,$(DATA_FILES),$(BUILD_DIR)/$(file:.bin=.bin.o)) \

DECOMP_C_OBJS := $(filter %.c.o,$(filter-out $(BUILD_DIR)/src/libultra%,$(O_FILES)))
DECOMP_BMHERO := $(DECOMP_C_OBJS)
DEP_FILES := $(O_FILES:.o=.d) $(DECOMP_C_OBJS:.o=.asmproc.d)

SPLAT_YAML := splat.yaml
SPLAT = $(PYTHON) tools/splat/split.py $(SPLAT_YAML)

##################### Compiler Options #######################
findcmd = $(shell type $(1) >/dev/null 2>/dev/null; echo $$?)

NON_BUILD_GOALS := clean distclean setup split submodules tools asset_files print-%
NEEDS_CROSS := $(if $(MAKECMDGOALS),$(if $(filter-out $(NON_BUILD_GOALS),$(MAKECMDGOALS)),1,0),1)

ifneq ($(NEEDS_CROSS),0)
ifeq ($(call findcmd,mips-linux-gnu-ld),0)
  CROSS := mips-linux-gnu-
else ifeq ($(call findcmd,mipsel-linux-gnu-ld),0)
	CROSS := mipsel-linux-gnu-
else ifeq ($(call findcmd,mips64-elf-ld),0)
  CROSS := mips64-elf-
else ifeq ($(call findcmd,mips-elf-ld),0)
  CROSS := mips-elf-
else
  $(error Missing cross compilation toolchain)
endif
endif

AS = $(CROSS)as
LD = $(CROSS)ld
OBJDUMP = $(CROSS)objdump
OBJCOPY = $(CROSS)objcopy
ifeq ($(HOST_OS),macos)
  CPP := clang -E -P -x c
else
  CPP := cpp
endif

#CC         := $(QEMU_IRIX) -L tools/ido7.1_compiler tools/ido7.1_compiler/usr/bin/cc
#CC_OLD     := $(QEMU_IRIX) -L tools/ido5.3_compiler tools/ido5.3_compiler/usr/bin/cc

# game uses IDO 7.1
CC = $(IDO_71_RECOMP_CC)
CC_OLD = $(IDO_71_RECOMP_CC)
ASMPROC_DIR := tools/asmproc
ASMPROC = python3 $(ASMPROC_DIR)/build.py
ASMPROC_FILES := $(ASMPROC_DIR)/build.py $(ASMPROC_DIR)/asm_processor.py $(ASMPROC_DIR)/prelude.inc
ASMPROC_FLAGS := --input-enc utf-8 --output-enc euc-jp

MIPS_VERSION := -mips2

OPTFLAGS := -O2

OBJDUMP_FLAGS := -d -r -z -Mreg-names=32

# include locations
INC_DIRS := include include/libultra include/PR include/audio include/ido . src/boot/malloc
IINCS := $(foreach d,$(INC_DIRS),-I$d)
# defines for SGI IDO
CDEFS := -D_LANGUAGE_C -DF3DEX_GBI -DNDEBUG -D_FINALROM -DBUILD_VERSION=VERSION_J -DSTDC_HEADERS

ifeq ($(ENABLE_ISPRINT),1)
  CDEFS += -DENABLE_ISPRINT
endif

ifneq ($(RUN_CC_CHECK),0)
  CHECK_WARNINGS := -Wall -Wextra
  CHECK_DEFS := -D_MIPS_SZLONG=32 -DNON_MATCHING
  CHECK_FLAGS := -fno-builtin -funsigned-char -fdiagnostics-color -std=gnu89 $(CDEFS) $(CHECK_DEFS) $(IINCS) -nostdinc -Wno-unknown-pragmas
  CC_CHECK := gcc -fsyntax-only

  ifeq ($(HOST_OS),linux)
    CC_CHECK += -m32
  endif

  ifneq ($(WERROR),0)
    CHECK_WARNINGS += -Werror
  endif
else
  CC_CHECK := @:
endif

######################## Flags #############################

ASFLAGS = -EB -mtune=vr4300 -march=vr4300 $(IINCS) -32
LDFLAGS_ENDIAN = -EB

# we support Microsoft extensions such as anonymous structs, which the compiler does support but warns for their usage. Surpress the warnings with -woff.
CFLAGS  = -G 0 -non_shared -use_readonly_const -Xfullwarn -Xcpluscomm $(IINCS) -Wab,-r4300_mul $(CDEFS) -woff 649,838,712,807 $(MIPS_VERSION)

LDFLAGS = $(LDFLAGS_ENDIAN) -T tools/undefined_syms.txt -T undefined_syms_auto.txt -T undefined_funcs_auto.txt -T $(BUILD_DIR)/$(LD_SCRIPT) -Map $(BUILD_DIR)/$(TARGET).map --no-check-sections


######################## Targets #############################

$(foreach dir,$(SRC_DIRS) $(ASM_DIRS) $(DATA_DIRS) $(COMPRESSED_DIRS) $(MAP_DIRS) $(BGM_DIRS),$(shell mkdir -p build/$(dir)))

######################## Build #############################

default: all

LD_SCRIPT = $(TARGET).ld

all: $(BUILD_DIR) $(BUILD_DIR)/$(ROM) verify tools

tools: $(ASMPROC_FILES)

distclean:
	rm -rf asm bin assets $(BUILD_DIR) undefined_syms_auto.txt undefined_funcs_auto.txt

clean:
	rm -rf $(BUILD_DIR)

submodules:
	git submodule update --init --recursive

split:
	rm -rf $(DATA_DIRS) $(ASM_DIRS) && ./tools/splat/split.py $(SPLAT_YAML)

$(IDO_71_RECOMP_CC):
	@rm -rf $(IDO_71_RECOMP_DIR)
	@mkdir -p $(IDO_71_RECOMP_DIR)
	@archive="$$(mktemp -t ido-7.1-recomp.XXXXXX)"; \
		curl --fail --location --output "$$archive" "$(call ido_download_url,7.1)"; \
		tar -xzf "$$archive" -C "$(IDO_71_RECOMP_DIR)"; \
		rm -f "$$archive"

$(ASMPROC_FILES):
	@test -f $@ || (echo "Missing asmproc submodule at $(ASMPROC_DIR). Run 'make submodules' or 'git submodule update --init --recursive'." && false)

setup: distclean submodules $(IDO_71_RECOMP_CC) split

#==============================================================================#
# Texture Generation                                                           #
#==============================================================================#

$(BUILD_DIR)/%.inc: %.png
	$(PIGMENT64) to-bin --c-array --format $(subst .,,$(suffix $*)) -o $@ $<

# TODO: Determine what causes this
O_FILES_PRINTER := $(BUILD_DIR)/asm/overlays/printer/%.s.o $(BUILD_DIR)/asm/data/overlays/printer/%.s.o

$(O_FILES_PRINTER): ASFLAGS += -no-pad-sections

# ------------------------------------------------------------------------------

$(BUILD_DIR):
	echo $(C_FILES)
	mkdir $(BUILD_DIR)

$(BUILD_DIR)/$(LD_SCRIPT): $(LD_SCRIPT)
	@mkdir -p $(shell dirname $@)
	$(CPP) -P -DBUILD_DIR=$(BUILD_DIR) -o $@ $<

$(BUILD_DIR)/$(TARGET).bin: $(BUILD_DIR)/$(TARGET).elf
	$(OBJCOPY) $< $@ -O binary

$(BUILD_DIR)/$(TARGET).elf: $(PNG_INC_FILES) $(O_FILES) $(BUILD_DIR)/$(LD_SCRIPT)
	@$(LD) $(LDFLAGS) -o $@

$(BUILD_DIR)/%.c.o: %.c Makefile $(ASMPROC_FILES)
	$(CC_CHECK) $(CHECK_FLAGS) $(CHECK_WARNINGS) -MMD -MP -MT $@ -MF $(@:.o=.d) $<
	$(ASMPROC) $(ASMPROC_FLAGS) $(CC) -- $(AS) $(ASFLAGS) -- -c $(CFLAGS) $(OPTFLAGS) -o $@ $<

$(BUILD_DIR)/src/libultra/libc/ll.c.o: src/libultra/libc/ll.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) -o $@ $<
	python3 tools/set_o32abi_bit.py $@
	@$(OBJDUMP) $(OBJDUMP_FLAGS) $@ > $(@:.o=.s)

$(BUILD_DIR)/src/libultra/libc/llcvt.c.o: src/libultra/libc/llcvt.c
	$(CC) -c $(CFLAGS) $(OPTFLAGS) -o $@ $<
	python3 tools/set_o32abi_bit.py $@
	@$(OBJDUMP) $(OBJDUMP_FLAGS) $@ > $(@:.o=.s)

$(BUILD_DIR)/%.s.o: %.s
	iconv --from UTF-8 --to EUC-JP $^ | $(AS) $(ASFLAGS) -o $@

$(BUILD_DIR)/%.bin.o: %.bin
	$(LD) $(LDFLAGS_ENDIAN) -r -b binary -o $@ $<

# final z64 updates checksum
$(BUILD_DIR)/$(ROM): $(BUILD_DIR)/$(TARGET).bin
	@cp $< $@

verify: $(BUILD_DIR)/$(ROM)
	md5sum -c checksum.md5

## Order-only prerequisites
# These ensure e.g. the PNG_INC_FILES are built before the O_FILES.
# The intermediate phony targets avoid quadratically-many dependencies between the targets and prerequisites.

asset_files: $(PNG_INC_FILES)
$(O_FILES): | asset_files
o_files: $(O_FILES)

.PHONY: asset_files o_files all clean distclean default split setup

print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true

-include $(DEP_FILES)
