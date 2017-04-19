include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/java.mk

JAR      := $(BITBRIDGE_JAR_NAME)
JSRC     := bitbridge/BridgePriv.java bitbridge/Bridge.java
MANIFEST := $(GEN_DIR)/$(BITBRIDGE_JAR_NAME).mf

$(call ADD_GENERATED,$(MANIFEST))

$(MANIFEST):
	$(call SUP,GEN,$@)$(call ECHO,$(call \
  JAR_VERSION_MANIFEST,bitbridge/,$(BITBRIDGE_LIB_DESCRIPTION),$(\
)$(PRODUCT_VER),$(VENDOR_NAME),bitbridge,$(PRODUCT_VER),$(VENDOR_NAME))) > $@

$(DEFINE_TARGETS)
