include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
TO_MAKE := C/test C/examples Java/test
include $(MTOP)/parallel.mk
