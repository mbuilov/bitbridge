include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk

ifndef NO_STATIC
VARIANTS    := $(filter-out D,$(call FILTER_VARIANTS_LIST,LIB,$(BITBRIDGE_LIB_VARIANTS)))
LIB         := $(if $(VARIANTS),bridge_test_slib $(firstword $(filter-out R,$(VARIANTS))))
endif

ifndef NO_SHARED
VARIANTS    := $(call FILTER_VARIANTS_LIST,DLL,$(BITBRIDGE_DLL_VARIANTS))
DLL         := $(if $(VARIANTS),bridge_test_dlib $(firstword $(filter-out R,$(VARIANTS))))
DLL_DLLS    := $(BITBRIDGE_LIB_NAME)$(call DLL_VAR_SUFFIX,DLL,$(firstword $(filter-out R,$(VARIANTS))),$(VARIANTS))
endif

GENERATED   := $(GEN_DIR)/bridge_test/gen_test.c $(GEN_DIR)/bridge_test/gen_test.h $(GEN_DIR)/bridge_test/gen_test.md
SRC         := $(GEN_DIR)/bridge_test/gen_test.c
INCLUDE     := $(GEN_DIR)/bridge_test $(TOP)/serial/C/lib
LIB_DEFINES := BRIDGE_TEST_EXPORTS=
DLL_DEFINES := BRIDGE_TEST_EXPORTS=$(DLL_EXPORTS_DEFINE)
DLL_DEFINES += BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)

ifdef DEBUG
DEFINES := BITBRIDGE_DEBUG
endif

USE := cmn_headers.mk

$(call ADD_GENERATED,$(GENERATED))

$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) test.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -c$(call ospath,$(word 1,$(GENERATED))) -h$(call ospath,$(word 2,$(GENERATED))) -m$(call ospath,$(word \
  3,$(GENERATED))) -dBRIDGE_TEST_EXPORTS)

$(DEFINE_TARGETS)
