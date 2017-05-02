include $(dir $(lastword $(MAKEFILE_LIST)))top.mk
include $(MTOP)/defs.mk

ifeq ($(filter distclean,$(MAKECMDGOALS)),)

TO_MAKE := serial

ifneq ($(filter WINXX,$(OS)),)
TO_MAKE += version
endif

ifeq (LINUX,$(OS))

PREFIX         ?= /usr/local
EXEC_PREFIX    ?= $(PREFIX)
INCLUDEDIR     ?= $(PREFIX)/include
LIBDIR         ?= $(EXEC_PREFIX)/lib
PKG_CONFIG_DIR ?= $(LIBDIR)/pkgconfig
INSTALL        ?= install
LDCONFIG       ?= /sbin/ldconfig

else ifeq (WINXX,$(OS))

PREFIX     ?= C:\devel
INCLUDEDIR ?= $(PREFIX)\include
LIBDIR     ?= $(PREFIX)\lib

endif # WINXX

include $(MTOP)/parallel.mk

endif # !distclean
