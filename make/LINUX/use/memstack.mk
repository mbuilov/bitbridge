MEMSTACK_PREFIX  ?= /usr
MEMSTACK_INCLUDE ?= $(MEMSTACK_PREFIX)/include
MEMSTACK_LIBDIR  ?= $(MEMSTACK_PREFIX)/lib
LIBMEMSTACK_NAME ?= memstack

ifdef DEBUG
DEFINES += MEMSTACK_DEBUG
endif

INCLUDE += $(MEMSTACK_INCLUDE)

# generate memstack library name
# $1 - EXE static S
LIBMEMSTACK ?= $(foreach t,$(firstword $1),$(if $($t),$(MEMSTACK_LIBDIR)/$(foreach k,$(if $(filter static,$(word \
  2,$1)),LIB,IMP),$($k_PREFIX)$(LIBMEMSTACK_NAME)$(if $(DEBUG),d)$(call DEP_$k_SUFFIX,$t,$(lastword $1))$($k_SUFFIX))))

# EXE_NEED_LIBMEMSTACK/DLL_NEED_LIBMEMSTACK - specifies
# kind of required memstack library (static or dynamic)
# for target EXE/DLL + optional variant of target EXE/DLL
# example: EXE_NEED_LIBMEMSTACK := static S
EXE_SYSLIBS += $(call LIBMEMSTACK,EXE $(EXE_NEED_LIBMEMSTACK))
DLL_SYSLIBS += $(call LIBMEMSTACK,DLL $(DLL_NEED_LIBMEMSTACK))

# reset after use
EXE_NEED_LIBMEMSTACK:=
DLL_NEED_LIBMEMSTACK:=
