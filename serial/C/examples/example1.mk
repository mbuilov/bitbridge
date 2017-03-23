include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk
include $(MTOP)/exts/ctest.mk

ifndef NO_STATIC

VARIANTS  := $(filter-out D,$(call FILTER_VARIANTS_LIST,LIB,$(BITBRIDGE_LIB_VARIANTS)))
EXE       := $(if $(VARIANTS),bridge_example1 $(firstword $(filter-out R,$(VARIANTS))))
LIBS      := $(BITBRIDGE_LIB_NAME)
GENERATED := $(GEN_DIR)/bridge_example1/gen_example.c $(GEN_DIR)/bridge_example1/gen_example.h
SRC       := $(GEN_DIR)/bridge_example1/gen_example.c example1.c
INCLUDE   := $(GEN_DIR)/bridge_example1 $(TOP)/serial/C/lib
SDEPS     := $(call FORM_SDEPS,example1.c,$(GEN_DIR)/bridge_example1/gen_example.h)

ifdef DEBUG
DEFINES := BITBRIDGE_DEBUG
endif

$(call ADD_GENERATED,$(GENERATED))

$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) example1.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -c$(call ospath,$(word 1,$(GENERATED))) -h$(call ospath,$(word 2,$(GENERATED))))

USE := cmn_headers.mk

$(DO_TEST_EXE)

endif # !NO_STATIC

$(DEFINE_TARGETS)
