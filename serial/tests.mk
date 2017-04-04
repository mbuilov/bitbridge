include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
TO_MAKE := C/test C/examples Java/test Java/examples
include $(MTOP)/parallel.mk
