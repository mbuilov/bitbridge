ifndef MAKE_C_EVAL
include $(dir $(lastword $(MAKEFILE_LIST)))/../../../config.mk
endif
TOOL_MODE := 1
$(MAKE_C_EVAL)
EXE     := bbcompc S
SRC     := compiler.c pack_gen.c struct_gen.c
INCLUDE := .. $(TOP)/include
LIBS    := bridge_model
$(DEFINE_TARGETS)
