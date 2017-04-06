include $(dir $(lastword $(MAKEFILE_LIST)))../top.mk
TO_MAKE := model C
ifndef NO_JAVA
TO_MAKE += Java
endif
include $(MTOP)/parallel.mk
