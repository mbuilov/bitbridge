include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk
include $(MTOP)/exts/ctest.mk

ifndef NO_SHARED

VARIANTS    := $(call FILTER_VARIANTS_LIST,DLL,$(BITBRIDGE_DLL_VARIANTS))
DLL         := $(if $(VARIANTS),bridge_ex4 $(firstword $(VARIANTS)))
DLLS        := $(BITBRIDGE_LIB_NAME)$(call DLL_VAR_SUFFIX,DLL,$(firstword $(VARIANTS)),$(VARIANTS))
GENERATED   := $(GEN_DIR)/bridge_example4/gen_example.c $(GEN_DIR)/bridge_example4/gen_example.h
DLL_SRC     := $(GEN_DIR)/bridge_example4/gen_example.c
INCLUDE     := $(GEN_DIR)/bridge_example4 $(TOP)/serial/C/lib
DEFINES     := BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)
DLL_DEFINES := EXAMPLE4_EXPORTS=$(DLL_EXPORTS_DEFINE)
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif

$(call ADD_GENERATED,$(GENERATED))
$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) example4.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -c$(call ospath,$(word 1,$(GENERATED))) -h$(call ospath,$(word 2,$(GENERATED))) -dEXAMPLE4_EXPORTS)

EXE         := $(if $(VARIANTS),bridge_example4 $(firstword $(VARIANTS)))
EXE_DLLS    := bridge_ex4
SRC         := example4.cpp
SDEPS       := $(call FORM_SDEPS,example4.cpp,$(GEN_DIR)/bridge_example4/gen_example.h)
EXE_DEFINES := EXAMPLE4_EXPORTS=$(DLL_IMPORTS_DEFINE)

EXE_NEED_LIBMEMSTACK := shared $(lastword $(EXE))
USE := memstack.mk cmn_headers.mk

$(call DO_TEST_EXE,$(addsuffix .$(call ver_major,$(PRODUCT_VER)),$(DLLS) $(EXE_DLLS)))

endif # !NO_SHARED

$(DEFINE_TARGETS)
