include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk
include $(MTOP)/exts/ctest.mk

ifndef NO_SHARED

# get all variants of bitbridge dll
VARIANTS  := $(call FILTER_VARIANTS_LIST,DLL,$(BITBRIDGE_DLL_VARIANTS))

# prefer non-default variant, if available
EXE       := $(if $(VARIANTS),bridge_example3 $(firstword $(filter-out R,$(VARIANTS))))

DLLS      := $(BITBRIDGE_LIB_NAME)
GENERATED := $(GEN_DIR)/bridge_example3/gen_example.c $(GEN_DIR)/bridge_example3/gen_example.h
SRC       := $(GEN_DIR)/bridge_example3/gen_example.c example3.c
INCLUDE   := $(GEN_DIR)/bridge_example3 $(TOP)/serial/C/lib

# specify that example3.c must be rebuilt if $(GEN_DIR)/bridge_example3/gen_example.h changes
# note: this is only needed if building without auto-dependencies generation
SDEPS     := $(call FORM_SDEPS,example3.c,$(GEN_DIR)/bridge_example3/gen_example.h)

# specify path for searching libraries on runtime for built executable
RPATH     := $(LIB_DIR)

DEFINES   := BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)

ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif

# add generated files to build
$(call ADD_GENERATED,$(GENERATED))

# specify how to build generated files:
# 1) generated files must be rebuilt if any of $(BIN_DIR)/bbcompc$(EXE_SUFFIX) or example3.sd changes
# 2) to generate files, run $(BIN_DIR)/bbcompc$(EXE_SUFFIX) with parameters
$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) example3.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -c$(call ospath,$(word 1,$(GENERATED))) -h$(call ospath,$(word 2,$(GENERATED))))

# specify which variant of memstack static library is needed for built test executable
# append directory containing common headers to INCLUDE variable
NEED_LIBMEMSTACK := EXE LIB $(lastword $(EXE))
USE := memstack.mk cmn_headers.mk

# create necessary simlinks to shared objects to run test executable
$(call DO_TEST_EXE,$(DLLS:=.$(call ver_major,$(PRODUCT_VER))))

endif # !NO_SHARED

$(DEFINE_TARGETS)
