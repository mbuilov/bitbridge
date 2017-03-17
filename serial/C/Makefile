include $(dir $(lastword $(MAKEFILE_LIST)))../../top.mk

TO_MAKE := compiler lib

ifneq ($(filter WINXX,$(OS)),)
TO_MAKE += version
endif

include $(MTOP)/parallel.mk
