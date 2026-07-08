XBE_TITLE = xbox_kernel_test_suite
GIT_VERSION = "$(shell git describe --always --tags --first-parent --dirty)"

recursivewildcard = $(foreach d,$(wildcard $(1:=/*)),$(call recursivewildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRCS := $(call recursivewildcard,src,*.c) $(call recursivewildcard,src,*.cpp)

NXDK_CFLAGS   = -I$(CURDIR)/src -DGIT_VERSION=\"$(GIT_VERSION)\"
NXDK_CXXFLAGS = -I$(CURDIR)/src -DGIT_VERSION=\"$(GIT_VERSION)\"
NXDK_CXX = y

include $(NXDK_DIR)/Makefile
