include $(MTOP)/c.mk
EXE       := bridge_example3
LIBS      := memstack
DLLS      := bitbridge
GENERATED := $(GEN_DIR)/bridge_example3/gen_example.h $(GEN_DIR)/bridge_example3/gen_example.c
INCLUDE   := $(GEN_DIR)/bridge_example3 $(TOP)/bridge/serial/C/lib $(TOP)/memstack $(TOP)/include
SRC       := $(GEN_DIR)/bridge_example3/gen_example.c example3.c
SDEPS     := example3.c $(GEN_DIR)/bridge_example3/gen_example.h
DEFINES   := BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG MEMSTACK_DEBUG
endif
$(call ADD_GENERATED,$(GENERATED))
$(call MULTI_TARGET,$(GENERATED),$(call GET_TOOL,bbcompc) example3.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -h$(call ospath,$(word 1,$(GENERATED))) -c$(call ospath,$(word 2,$(GENERATED))))
$(DEFINE_TARGETS)
