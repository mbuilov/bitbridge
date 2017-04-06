include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk
include $(MTOP)/exts/ctest.mk

ifndef NO_SHARED

# get all variants of bitbridge dll
VARIANTS  := $(call FILTER_VARIANTS_LIST,DLL,$(BITBRIDGE_DLL_VARIANTS))

# prefer non-default variant, if available
DLL       := $(if $(VARIANTS),bridge_ex4 $(firstword $(VARIANTS)))

DLLS      := $(BITBRIDGE_LIB_NAME)
GENERATED := $(GEN_DIR)/bridge_example4/gen_example.c $(GEN_DIR)/bridge_example4/gen_example.h
SRC       := $(GEN_DIR)/bridge_example4/gen_example.c
INCLUDE   := $(GEN_DIR)/bridge_example4 $(TOP)/serial/C/lib

# export symbols from building dll
DEFINES   := EXAMPLE4_EXPORTS=$(DLL_EXPORTS_DEFINE)

# import symbols from bitbridge dll
DEFINES   += BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)

ifdef DEBUG
DEFINES   += BITBRIDGE_DEBUG
endif

# add generated files to build
$(call ADD_GENERATED,$(GENERATED))

# specify how to build generated files:
# 1) generated files must be rebuilt if any of $(BIN_DIR)/bbcompc$(EXE_SUFFIX) or example4.sd changes
# 2) to generate files, run $(BIN_DIR)/bbcompc$(EXE_SUFFIX) with parameters
# 3) specify EXAMPLE4_EXPORTS prefix for generated public functions
$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) example4.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -c$(call ospath,$(word 1,$(GENERATED))) -h$(call ospath,$(word 2,$(GENERATED))) -dEXAMPLE4_EXPORTS)

# append directory containing common headers to SYSINCLUDE variable
USE := cmn_headers.mk

# continue makefile, start defining new target with preserved values of VARIANTS, DLLS, INCLUDE and USE variables
$(call MAKE_CONTINUE,VARIANTS DLLS INCLUDE USE)

# prefer non-default variant, if available
EXE     := $(if $(VARIANTS),bridge_example4 $(firstword $(VARIANTS)))

# additionally  link just built bridge_ex4 dll to building executable
DLLS    += bridge_ex4
SRC     := example4.cpp

# specify that example4.cpp must be rebuilt if $(GEN_DIR)/bridge_example4/gen_example.h changes
# note: this is only needed if building without auto-dependencies generation
SDEPS   := $(call FORM_SDEPS,example4.cpp,$(GEN_DIR)/bridge_example4/gen_example.h)

# specify path for searching libraries on runtime for built executable
# note: initialize RPATH as recursive variable, because MEMSTACK_LIBDIR value will be set later - while including USEs
RPATH    = $(MEMSTACK_LIBDIR) $(LIB_DIR)

# import symbols from bitbridge dll
DEFINES := BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)

# import symbols from just built bridge_ex4 dll
DEFINES += EXAMPLE4_EXPORTS=$(DLL_IMPORTS_DEFINE)

ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif

# specify which variant of memstack static library is needed for built test executable
NEED_LIBMEMSTACK := EXE IMP $(lastword $(EXE))
USE += memstack.mk

# create necessary simlinks to shared objects to run test executable
$(call DO_TEST_EXE,$(DLLS:=.$(call ver_major,$(PRODUCT_VER))),,$(call \
  ospath,$(LIB_DIR))$(PATHSEP)$(subst ?, ,$(call ospath,$(MEMSTACK_DLLDIR))))

endif # !NO_SHARED

$(DEFINE_TARGETS)
