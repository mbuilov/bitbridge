GTPARSER_PREFIX   ?= C:/devel
GTPARSER_INCLUDE  ?= $(GTPARSER_PREFIX)/include
GTPARSER_LIBDIR   ?= $(GTPARSER_PREFIX)/lib
GTPARSER_LIB_NAME ?= gtparser
LIBGTPARSER       ?= $(LIB_PREFIX)$(GTPARSER_LIB_NAME)$(LIB_SUFFIX)
LIBGTPARSER      ?= libgtparser$(call LIB_VAR_SUFFIX,S).a
INCLUDE += $(GTPARSER_INCLUDE)
SYSLIBS += $(GTPARSER_LIBDIR)/$(LIBGTPARSER)
