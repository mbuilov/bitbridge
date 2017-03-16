include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk

ifndef NO_SHARED

VARIANTS  := $(call FILTER_VARIANTS_LIST,DLL,$(BITBRIDGE_DLL_VARIANTS))
EXE       := $(if $(VARIANTS),bridge_example3 $(firstword $(filter-out R,$(VARIANTS))))
DLLS      := $(BITBRIDGE_LIB_NAME)
GENERATED := $(GEN_DIR)/bridge_example3/gen_example.c $(GEN_DIR)/bridge_example3/gen_example.h
SRC       := $(GEN_DIR)/bridge_example3/gen_example.c example3.c
INCLUDE   := $(GEN_DIR)/bridge_example3 $(TOP)/serial/C/lib
SDEPS     := $(call FORM_SDEPS,example3.c,$(GEN_DIR)/bridge_example3/gen_example.h)
DEFINES   := BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)

ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif

$(call ADD_GENERATED,$(GENERATED))

$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) example3.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -c$(call ospath,$(word 1,$(GENERATED))) -h$(call ospath,$(word 2,$(GENERATED))))

EXE_NEED_LIBMEMSTACK := static $(lastword $(EXE))
USE := memstack.mk cmn_headers.mk

endif # !NO_SHARED

$(DEFINE_TARGETS)
