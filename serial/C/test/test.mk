include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk

ifndef NO_STATIC
VARIANTS := $(filter-out D,$(call FILTER_VARIANTS_LIST,LIB,$(BITBRIDGE_LIB_VARIANTS)))
EXE      := $(if $(VARIANTS),bridge_test_static $(firstword $(filter-out R,$(VARIANTS))))
LIBS     := bridge_test_slib $(BITBRIDGE_LIB_NAME)
INCLUDE  := $(GEN_DIR)/bridge_test $(TOP)/serial/C/lib
SRC      := bridge_test.c test_fn1.c
SDEPS    := $(call FORM_SDEPS,$(SRC),$(GEN_DIR)/bridge_test/gen_test.h)
DEFINES  := BRIDGE_TEST_EXPORTS=
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif
EXE_NEED_LIBMEMSTACK := static $(lastword $(EXE))
USE := memstack.mk cmn_headers.mk
endif

$(MAKE_CONTINUE)

ifndef NO_SHARED
VARIANTS := $(call FILTER_VARIANTS_LIST,DLL,$(BITBRIDGE_DLL_VARIANTS))
EXE      := $(if $(VARIANTS),bridge_test_dynamic $(firstword $(filter-out R,$(VARIANTS))))
DLLS     := bridge_test_dlib $(BITBRIDGE_LIB_NAME)
INCLUDE  := $(GEN_DIR)/bridge_test $(TOP)/serial/C/lib
SRC      := bridge_test.c test_fn1.c
SDEPS    := $(call FORM_SDEPS,$(SRC),$(GEN_DIR)/bridge_test/gen_test.h)
DEFINES  := BRIDGE_TEST_EXPORTS=$(DLL_IMPORTS_DEFINE)
DEFINES  += BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif
EXE_NEED_LIBMEMSTACK := dynamic $(lastword $(EXE))
USE := memstack.mk cmn_headers.mk
endif

$(DEFINE_TARGETS)
