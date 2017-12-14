XBE_TITLE=kernel_test_suite
SRCS = $(wildcard $(CURDIR)/*.c)
NXDK_DIR = $(CURDIR)/../..
include $(NXDK_DIR)/Makefile
