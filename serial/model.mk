ifndef MAKE_C_EVAL
include $(dir $(lastword $(MAKEFILE_LIST)))/../../config.mk
endif
TOOL_MODE := 1
$(MAKE_C_EVAL)
LIB     := bridge_model S
SRC     := model.c utils.c ptypes.c parser.c \
  $(TOP)/parser/name_scanner.c \
  $(TOP)/parser/int_scanner.c \
  $(TOP)/parser/parser_base.c \
  $(TOP)/parser/parser_err.c
INCLUDE := $(TOP)/parser
$(DEFINE_TARGETS)
