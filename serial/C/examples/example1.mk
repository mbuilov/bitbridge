include $(MTOP)/c.mk
EXE       := bridge_example1
LIBS      := bitbridge
GENERATED := $(GEN_DIR)/bridge_example1/gen_example.h $(GEN_DIR)/bridge_example1/gen_example.c
INCLUDE   := $(GEN_DIR)/bridge_example1 $(TOP)/bridge/serial/C/lib $(TOP)/include
SRC       := $(GEN_DIR)/bridge_example1/gen_example.c example1.c
SDEPS     := example1.c $(GEN_DIR)/bridge_example1/gen_example.h
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif
$(call ADD_GENERATED,$(GENERATED))
$(call MULTI_TARGET,$(GENERATED),$(call GET_TOOL,bbcompc) example1.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -h$(call ospath,$(word 1,$(GENERATED))) -c$(call ospath,$(word 2,$(GENERATED))))
$(DEFINE_TARGETS)
