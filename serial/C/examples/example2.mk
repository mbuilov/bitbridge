include $(MTOP)/c.mk
EXE       := bridge_example2
LIBS      := bitbridge memstack
GENERATED := $(GEN_DIR)/bridge_example2/gen_example.h $(GEN_DIR)/bridge_example2/gen_example.c
INCLUDE   := $(GEN_DIR)/bridge_example2 $(TOP)/bridge/serial/C/lib $(TOP)/memstack $(TOP)/include
SRC       := $(GEN_DIR)/bridge_example2/gen_example.c example2.c
SDEPS     := example2.c $(GEN_DIR)/bridge_example2/gen_example.h
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG MEMSTACK_DEBUG
endif
$(call ADD_GENERATED,$(GENERATED))
$(call MULTI_TARGET,$(GENERATED),$(call GET_TOOL,bbcompc) example2.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -h$(call ospath,$(word 1,$(GENERATED))) -c$(call ospath,$(word 2,$(GENERATED))))
$(DEFINE_TARGETS)
