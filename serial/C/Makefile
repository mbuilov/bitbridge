include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk
include $(MTOP)/c.mk
EXE     := bbcompc S
SRC     := compiler.c pack_gen.c struct_gen.c
INCLUDE := $(TOP)/serial/model
LIBS    := bridge_model
USE     := cmn_headers gtparser
$(DEFINE_TARGETS)
