XBE_TITLE=kernel\ test\ suite
recursivewildcard=$(foreach d,$(wildcard $(1:=/*)),$(call recursivewildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRCS = $(call recursivewildcard,src,*.c)
SRCS := $(SRCS) $(call recursivewildcard,src,*.cpp)
GIT_VERSION = "$(shell git describe --always --tags --first-parent --dirty)"
NXDK_CFLAGS = -I$(CURDIR)/src -DGIT_VERSION=\"$(GIT_VERSION)\"
NXDK_CXXFLAGS = -I$(CURDIR)/src -DGIT_VERSION=\"$(GIT_VERSION)\"
NXDK_CXX = y
include $(NXDK_DIR)/Makefile
