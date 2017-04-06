include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk
include $(MTOP)/exts/ctest.mk

ifndef NO_STATIC

# filter-out variant of static library intended for linking with dll
VARIANTS  := $(filter-out D,$(call FILTER_VARIANTS_LIST,LIB,$(BITBRIDGE_LIB_VARIANTS)))

# prefer non-default variant, if available
EXE       := $(if $(VARIANTS),bridge_example1 $(firstword $(filter-out R,$(VARIANTS))))

LIBS      := $(BITBRIDGE_LIB_NAME)
GENERATED := $(GEN_DIR)/bridge_example1/gen_example.c $(GEN_DIR)/bridge_example1/gen_example.h
SRC       := $(GEN_DIR)/bridge_example1/gen_example.c example1.c
INCLUDE   := $(GEN_DIR)/bridge_example1 $(TOP)/serial/C/lib

# specify that example1.c must be rebuilt if $(GEN_DIR)/bridge_example1/gen_example.h changes
# note: this is only needed if building without auto-dependencies generation
SDEPS     := $(call FORM_SDEPS,example1.c,$(GEN_DIR)/bridge_example1/gen_example.h)

ifdef DEBUG
DEFINES := BITBRIDGE_DEBUG
endif

# add generated files to build
$(call ADD_GENERATED,$(GENERATED))

# specify how to build generated files:
# 1) generated files must be rebuilt if any of $(BIN_DIR)/bbcompc$(EXE_SUFFIX) or example1.sd changes
# 2) to generate files, run $(BIN_DIR)/bbcompc$(EXE_SUFFIX) with parameters
$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) example1.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -c$(call ospath,$(word 1,$(GENERATED))) -h$(call ospath,$(word 2,$(GENERATED))))

# append directory containing common headers to SYSINCLUDE variable
USE := cmn_headers.mk

# specify that built EXE must be tested
$(DO_TEST_EXE)

endif # !NO_STATIC

$(DEFINE_TARGETS)
