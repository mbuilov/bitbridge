include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk
EXE     := bbcompc S
SRC     := compiler.c pack_gen.c struct_gen.c
INCLUDE := $(TOP)/serial/model
DEFINES := BITBRIDGE_C_COMPILER_COPYRIGHT=$(call STRING_DEFINE,"$(VENDOR_COPYRIGHT)")
LIBS    := bridge_model
USE     := cmn_headers.mk gtparser_s.mk
$(DEFINE_TARGETS)
