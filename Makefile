include $(dir $(lastword $(MAKEFILE_LIST)))top.mk
include $(MTOP)/defs.mk

ifneq ($(filter distclean,$(MAKECMDGOALS)),)
distclean:
	$(call RM,$(CLOBBER_DIRS))
else

TO_MAKE := serial

ifneq ($(filter WINXX,$(OS)),)
TO_MAKE += version
endif

ifneq ($(filter check tests clean,$(MAKECMDGOALS)),)
check tests: all
TO_MAKE += serial/tests.mk
endif

include $(MTOP)/parallel.mk

endif # !distclean
