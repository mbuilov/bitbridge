include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk

ifdef NO_STATIC
ifdef NO_SHARED
$(error cannot link $(BITBRIDGE_LIB_NAME) library for tests)
endif
endif

GENERATED   := $(GEN_DIR)/bridge_test/gen_test.h $(GEN_DIR)/bridge_test/gen_test.c $(GEN_DIR)/bridge_test/gen_test.md

ifndef NO_STATIC
LIB         := bridge_test_lib S
endif

ifndef NO_SHARED
DLL         := bridge_test_lib
DLL_DLLS    := $(BITBRIDGE_LIB_NAME)
endif

SRC         := $(word 2,$(GENERATED))
INCLUDE     := $(GEN_DIR)/bridge_test $(TOP)/serial/C/lib
LIB_DEFINES := BRIDGE_TEST_EXPORTS=
DLL_DEFINES := BRIDGE_TEST_EXPORTS=$(DLL_EXPORTS_DEFINE)
DLL_DEFINES += BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)

ifdef DEBUG
DEFINES := BITBRIDGE_DEBUG
endif

USE := cmn_headers

$(call ADD_GENERATED,$(GENERATED))

$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) test.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -h$(call ospath,$(word 1,$(GENERATED))) -c$(call ospath,$(word 2,$(GENERATED))) -m$(call ospath,$(word \
  3,$(GENERATED))) -dBRIDGE_TEST_EXPORTS)

$(DEFINE_TARGETS)
