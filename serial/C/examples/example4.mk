include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk
include $(MTOP)/exts/ctest.mk

ifndef NO_SHARED

VARIANTS  := $(call FILTER_VARIANTS_LIST,DLL,$(BITBRIDGE_DLL_VARIANTS))
DLL       := $(if $(VARIANTS),bridge_ex4 $(firstword $(VARIANTS)))
DLLS      := $(BITBRIDGE_LIB_NAME)
GENERATED := $(GEN_DIR)/bridge_example4/gen_example.c $(GEN_DIR)/bridge_example4/gen_example.h
SRC       := $(GEN_DIR)/bridge_example4/gen_example.c
INCLUDE   := $(GEN_DIR)/bridge_example4 $(TOP)/serial/C/lib
DEFINES   := EXAMPLE4_EXPORTS=$(DLL_EXPORTS_DEFINE)
DEFINES   += BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)
ifdef DEBUG
DEFINES   += BITBRIDGE_DEBUG
endif

$(call ADD_GENERATED,$(GENERATED))
$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) example4.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -c$(call ospath,$(word 1,$(GENERATED))) -h$(call ospath,$(word 2,$(GENERATED))) -dEXAMPLE4_EXPORTS)

USE := cmn_headers.mk

$(call MAKE_CONTINUE,VARIANTS DLLS INCLUDE)

EXE     := $(if $(VARIANTS),bridge_example4 $(firstword $(VARIANTS)))
DLLS    += bridge_ex4
SRC     := example4.cpp
SDEPS   := $(call FORM_SDEPS,example4.cpp,$(GEN_DIR)/bridge_example4/gen_example.h)
RPATH    = $(MEMSTACK_LIBDIR) $(LIB_DIR)
DEFINES := BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)
DEFINES += EXAMPLE4_EXPORTS=$(DLL_IMPORTS_DEFINE)
ifdef DEBUG
DEFINES   += BITBRIDGE_DEBUG
endif

NEED_LIBMEMSTACK := EXE IMP $(lastword $(EXE))
USE := memstack.mk cmn_headers.mk

$(call DO_TEST_EXE,$(DLLS:=.$(call ver_major,$(PRODUCT_VER))),,$(call \
  ospath,$(LIB_DIR))$(PATHSEP)$(subst ?, ,$(call ospath,$(MEMSTACK_DLLDIR))))

endif # !NO_SHARED

$(DEFINE_TARGETS)
