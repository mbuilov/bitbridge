ifndef MAKE_C_EVAL
include $(dir $(lastword $(MAKEFILE_LIST)))/../../../config.mk
endif
$(MAKE_C_EVAL)
LIB     := bitbridge
DLL     := bitbridge
SRC     := bitbridge.c
INCLUDE := $(TOP)/include
SOVER   := 1
DLL_DEFINES := BITBRIDGE_EXPORTS=$(DLL_EXPORTS_DEFINE)
ifdef DEBUG
DEFINES := BITBRIDGE_DEBUG
endif
$(DEFINE_TARGETS)
