include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/java.mk
JAR      := bridge_test
JARS     := bridge_test_lib $(BITBRIDGE_JAR_NAME)
JSRC     := BridgeTest.java TestFn1.java
MANIFEST := $(GEN_DIR)/bridge_test.mf
$(call ADD_GENERATED,$(MANIFEST))
define MANIFEST_TEXT
Class-Path: $(BITBRIDGE_JAR_NAME).jar bridge_test_lib.jar
Main-Class: BridgeTest
endef
$(MANIFEST): MANIFEST_TEXT := $(MANIFEST_TEXT)
$(MANIFEST):
	$(call SUP,GEN,$@)$(call ECHO,$(MANIFEST_TEXT)) > $@
$(DEFINE_TARGETS)
