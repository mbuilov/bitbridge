MEMSTACK_PREFIX   ?= $(if $(OSTYPE_UNIX),/usr,C:/devel)
MEMSTACK_INCLUDE  ?= $(MEMSTACK_PREFIX)/include
MEMSTACK_LIBDIR   ?= $(MEMSTACK_PREFIX)/lib
MEMSTACK_DLLDIR   ?= $(MEMSTACK_LIBDIR)
MEMSTACK_LIB_NAME ?= memstack$(if $(DEBUG),d)

ifdef DEBUG
DEFINES += MEMSTACK_DEBUG
endif

SYSINCLUDE += $(MEMSTACK_INCLUDE)

ifndef NEED_LIBMEMSTACK
# NEED_LIBMEMSTACK value format: {EXE,DLL} {IMP,LIB} {S,R,<empty>}
$(error NEED_LIBMEMSTACK must be defined, for example: EXE IMP S)
endif

# note: DEP_IMP_SUFFIX or DEP_LIB_SUFFIX - defined in $(MTOP)/c.mk
SYSLIBS += $(call qpath,$(call ospath,$(MEMSTACK_LIBDIR)/$(foreach \
  t,$(firstword $(NEED_LIBMEMSTACK)),$(foreach \
  k,$(word 2,$(NEED_LIBMEMSTACK)),$($k_PREFIX)$(MEMSTACK_LIB_NAME)$(call \
  DEP_$k_SUFFIX,$t,$(lastword $(NEED_LIBMEMSTACK)),$(MEMSTACK_LIB_NAME))$($k_SUFFIX)))))

# reset after use
NEED_LIBMEMSTACK:=
