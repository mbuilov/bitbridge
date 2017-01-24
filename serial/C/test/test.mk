include $(dir $(lastword $(MAKEFILE_LIST)))/../../../project/c.mk
EXE     := bridge_test_static
LIBS    := bridge_test_lib bitbridge memstack
INCLUDE := $(GEN_DIR)/bridge_test $(TOP)/bridge/serial/C/lib $(TOP)/memstack $(TOP)/include
SRC     := bridge_test.c test_fn1.c
SDEPS   := $(addprefix $(GEN_DIR)/bridge_test/gen_test.h ,$(SRC))
DEFINES := BRIDGE_TEST_EXPORTS=
ifdef DEBUG
DEFINES += MEMSTACK_DEBUG BITBRIDGE_DEBUG
endif
$(call MAKE_CONTINUE,INCLUDE SRC SDEPS)
EXE     := bridge_test
DLLS    := bridge_test_lib bitbridge memstack
DEFINES += MEMSTACK_EXPORTS=$(DLL_IMPORTS_DEFINE)
DEFINES += BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)
DEFINES += BRIDGE_TEST_EXPORTS=$(DLL_IMPORTS_DEFINE)
ifdef DEBUG
DEFINES += MEMSTACK_DEBUG BITBRIDGE_DEBUG
endif
$(DEFINE_TARGETS)
