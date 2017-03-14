ifndef NO_STATIC
include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk
EXE       := bridge_example1
LIBS      := $(BITBRIDGE_LIB_NAME)
GENERATED := $(GEN_DIR)/bridge_example1/gen_example.h $(GEN_DIR)/bridge_example1/gen_example.c
INCLUDE   := $(GEN_DIR)/bridge_example1 $(TOP)/serial/C/lib
SRC       := $(GEN_DIR)/bridge_example1/gen_example.c example1.c
SDEPS     := $(call FORM_SDEPS,$(wordlist 2,999999,$(SRC)),$(firstword $(GENERATED)))
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif
$(call ADD_GENERATED,$(GENERATED))
$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompc$(EXE_SUFFIX) example1.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -h$(call ospath,$(word 1,$(GENERATED))) -c$(call ospath,$(word 2,$(GENERATED))))
$(DEFINE_TARGETS)
endif
