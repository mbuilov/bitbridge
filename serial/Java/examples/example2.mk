include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/java.mk
include $(MTOP)/exts/jtest.mk

JAR       := bridge_example2
JARS      := $(BITBRIDGE_JAR_NAME)
GENERATED := $(GEN_DIR)/bridge_example2/ex2/gen_example2.java
JSRC      := $(GEN_DIR)/bridge_example2/ex2/gen_example2.java example2.java

# add generated file to build
$(call ADD_GENERATED,$(GENERATED))

# specify how to build generated files:
# 1) generated files must be rebuilt if any of $(BIN_DIR)/bbcompj$(EXE_SUFFIX) or example2.sd changes
# 2) to generate files, run $(BIN_DIR)/bbcompj$(EXE_SUFFIX) with parameters
$(call MULTI_TARGET,$(GENERATED),$(BIN_DIR)/bbcompj$(EXE_SUFFIX) example2.sd,$$(call \
  ospath,$$<) -s$$(call ospath,$$(word 2,$$^)) -j$(call ospath,$(GENERATED)) -pex2)

# specify that built JAR must be tested
$(call DO_TEST_JAR,example2)

$(DEFINE_TARGETS)
