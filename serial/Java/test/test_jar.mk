include $(MTOP)/java.mk
GENERATED   := $(GEN_DIR)/bridge_jtest/bridge_test_lib/gen_test.java $(GEN_DIR)/bridge_jtest/gen_test.md
JAR         := bridge_test_lib
JSRC        := $(firstword $(GENERATED))
JARS        := bitbridge
$(call ADD_GENERATED,$(GENERATED))
$(call MULTI_TARGET,$(GENERATED),$(call GET_TOOL,bbcompj) test.sd,$$(call ospath,$$<) -s$$(call ospath,$$(word \
  2,$$^)) -j$(call ospath,$(word 1,$(GENERATED))) -m$(call ospath,$(word 2,$(GENERATED))) -pbridge_test_lib)
$(DEFINE_TARGETS)
