include $(dir $(lastword $(MAKEFILE_LIST)))../../../top.mk
include $(MTOP)/java.mk
include $(MTOP)/exts/jtest.mk

JAR      := bridge_test
JARS     := bridge_test_lib $(BITBRIDGE_JAR_NAME)
JSRC     := BridgeTest.java TestFn1.java

$(call DO_TEST_JAR,BridgeTest)

$(DEFINE_TARGETS)
