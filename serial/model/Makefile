include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk
TOOL_MODE := 1
include $(MTOP)/c.mk
LIB     := bridge_model S
SRC     := model.c utils.c ptypes.c parser.c
USE     := gtparser
$(DEFINE_TARGETS)
