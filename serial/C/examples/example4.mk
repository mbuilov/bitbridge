include $(MTOP)/c.mk
DLL         := bridge_ex4
DLLS        := bitbridge
GENERATED   := $(GEN_DIR)/bridge_example4/gen_example.h $(GEN_DIR)/bridge_example4/gen_example.c
SRC         := $(GEN_DIR)/bridge_example4/gen_example.c
INCLUDE     := $(GEN_DIR)/bridge_example4 $(TOP)/bridge/serial/C/lib $(TOP)/include
DEFINES     := EXAMPLE4_EXPORTS=$(DLL_EXPORTS_DEFINE) BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE)
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG
endif
$(call ADD_GENERATED,$(GENERATED))
$(call MULTI_TARGET,$(GENERATED),$(call GET_TOOL,bbcompc) example4.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -h$(call ospath,$(word 1,$(GENERATED))) -c$(call ospath,$(word 2,$(GENERATED))) -dEXAMPLE4_EXPORTS)
$(MAKE_CONTINUE)
EXE         := bridge_example4
DLLS        := bridge_ex4 bitbridge memstack
SRC         := example4.cpp
SDEPS       := example4.cpp $(GEN_DIR)/bridge_example4/gen_example.h
INCLUDE     := $(GEN_DIR)/bridge_example4 $(TOP)/bridge/serial/C/lib $(TOP)/memstack $(TOP)/include
DEFINES     := EXAMPLE4_EXPORTS=$(DLL_IMPORTS_DEFINE) BITBRIDGE_EXPORTS=$(DLL_IMPORTS_DEFINE) MEMSTACK_EXPORTS=$(DLL_IMPORTS_DEFINE)
ifdef DEBUG
DEFINES += BITBRIDGE_DEBUG MEMSTACK_DEBUG
endif
$(DEFINE_TARGETS)
