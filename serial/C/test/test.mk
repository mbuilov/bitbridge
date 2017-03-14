include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk

EXE     := bridge_test_static S
LIBS    := bridge_test_lib $(BITBRIDGE_LIB_NAME)
INCLUDE := $(GEN_DIR)/bridge_test $(TOP)/serial/C/lib
SRC     := bridge_test.c test_fn1.c
SDEPS   := $(call FORM_SDEPS,$(SRC),$(GEN_DIR)/bridge_test/gen_test.h)
DEFINES := BRIDGE_TEST_EXPORTS=
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif
USE     := memstack_static cmn_headers

$(call MAKE_CONTINUE,INCLUDE SRC SDEPS)

EXE     := bridge_test
DLLS    := bridge_test_lib $(BITBRIDGE_LIB_NAME)
DEFINES := BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)
DEFINES += BRIDGE_TEST_EXPORTS=$(DLL_IMPORTS_DEFINE)
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif
USE     := memstack_dynamic cmn_headers

$(DEFINE_TARGETS)
