include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk
TO_MAKE := jcompiler lib
ifneq ($(filter check tests clean,$(MAKECMDGOALS)),)
TO_MAKE += test examples
endif
include $(MTOP)/parallel.mk
