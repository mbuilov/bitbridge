include $(dir $(lastword $(MAKEFILE_LIST)))/../../../project/c.mk
GENERATED   := $(GEN_DIR)/bridge_test/gen_test.h $(GEN_DIR)/bridge_test/gen_test.c $(GEN_DIR)/bridge_test/gen_test.md
LIB         := bridge_test_lib
DLL         := bridge_test_lib
DLL_DLLS    := bitbridge
SRC         := $(word 2,$(GENERATED))
INCLUDE     := $(GEN_DIR)/bridge_test $(TOP)/bridge/serial/C/lib $(TOP)/include
LIB_DEFINES := BRIDGE_TEST_EXPORTS=
DLL_DEFINES := BRIDGE_TEST_EXPORTS=$(DLL_EXPORTS_DEFINE)
DLL_DEFINES += BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)
ifdef DEBUG
DEFINES := BITBRIDGE_DEBUG
endif
$(call ADD_GENERATED,$(GENERATED))
$(call MULTI_TARGET,$(GENERATED),$(call GET_TOOL,bbcompc) test.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -h$(call ospath,$(word 1,$(GENERATED))) -c$(call ospath,$(word 2,$(GENERATED))) -m$(call ospath,$(word \
  3,$(GENERATED))) -dBRIDGE_TEST_EXPORTS)
$(DEFINE_TARGETS)
