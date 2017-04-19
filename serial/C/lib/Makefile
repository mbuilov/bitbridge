include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/c.mk

SRC     := bitbridge.c
INCLUDE := .
MODVER  := $(BITBRIDGE_LIB_VER)
ifdef DEBUG
DEFINES := BITBRIDGE_DEBUG
endif
USE     := cmn_headers.mk

ifndef NO_STATIC
LIB     := $(BITBRIDGE_LIB_NAME) $(BITBRIDGE_LIB_VARIANTS)
$(call MAKE_CONTINUE,SRC INCLUDE MODVER DEFINES USE)
endif

ifndef NO_SHARED
DLL     := $(BITBRIDGE_LIB_NAME) $(BITBRIDGE_DLL_VARIANTS)
DEFINES += BITBRIDGE_EXPORTS=$(DLL_EXPORTS_DEFINE)
endif

$(DEFINE_TARGETS)

# installation

DLL := $(if $(NO_SHARED),,$(BITBRIDGE_LIB_NAME) $(BITBRIDGE_DLL_VARIANTS))
LIB := $(if $(NO_STATIC),,$(BITBRIDGE_LIB_NAME) $(BITBRIDGE_LIB_VARIANTS))

LIBRARY_NAME := bitbridge
LIBRARY_HDIR := bitbridge

LIBRARY_HEADERS := $(addprefix $(TOP)/bitbridge/,\
  bitbridge.h \
  bitbridge_alloc.h \
  bitbridge_comn.h \
  bitbridge_heap.inl \
  bitbridge_log_cb.inl \
  bitbridge_memstack.inl \
  bitbridge_memstack_ref.inl)

define BITBRIDGE_PC_COMMENT
Author:  $(VENDOR_NAME)
License: $(BITBRIDGE_LIB_LICENCE)
endef

# generate contents of pkg-config .pc-file
# $1 - static or dynamic library name
# $2 - library variant
BITBRIDGE_PC_GENERATOR = $(call \
  PKGCONF_DEF_TEMPLATE,$1,$(MODVER),$(BITBRIDGE_LIB_DESC),$(BITBRIDGE_PC_COMMENT),$(VENDOR_URL),,,,$(addprefix \
  -D,$(DEFINES))$(call VARIANT_CFLAGS,$2))

LIBRARY_PC_GEN := BITBRIDGE_PC_GENERATOR

include $(MTOP)/exts/install_lib.mk
