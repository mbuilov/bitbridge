MEMSTACK_PREFIX  ?= /usr
MEMSTACK_INCLUDE ?= $(MEMSTACK_PREFIX)/include
MEMSTACK_LIBDIR  ?= $(MEMSTACK_PREFIX)/lib
LIBMEMSTACK_NAME ?= memstack
# LIBMEMSTACK_NEED - specifies target EXE or DLL for which memstack static library is needed + optional variant of target EXE or DLL
# example: LIBMEMSTACK_NEED := EXE S
LIBMEMSTACK      ?= $(LIB_PREFIX)$(LIBMEMSTACK_NAME)$(if $(DEBUG),d)$(call \
  DEP_LIB_SUFFIX,$(firstword $(LIBMEMSTACK_NEED)),$(lastword $(LIBMEMSTACK_NEED)))$(LIB_SUFFIX)
# reset after use
LIBMEMSTACK_NEED :=
ifdef DEBUG
DEFINES += MEMSTACK_DEBUG
endif
INCLUDE += $(MEMSTACK_INCLUDE)
SYSLIBS += $(MEMSTACK_LIBDIR)/$(LIBMEMSTACK)
