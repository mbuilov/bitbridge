ifndef MAKE_C_EVAL
include $(dir $(lastword $(MAKEFILE_LIST)))/../../../config.mk
endif
TOOL_MODE := 1
$(MAKE_C_EVAL)
EXE     := bbcompj S
SRC     := jcompiler.c jpack_gen.c jstruct_gen.c
INCLUDE := .. $(TOP)/include
LIBS    := bridge_model
$(DEFINE_TARGETS)
