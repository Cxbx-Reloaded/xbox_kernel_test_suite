XBE_TITLE=kernel\ test\ suite
recursivewildcard=$(foreach d,$(wildcard $(1:=/*)),$(call recursivewildcard,$d,$2) $(filter $(subst *,%,$2),$d))
SRCS = $(call recursivewildcard,src,*.c)
GIT_VERSION = "$(shell git describe --always --tags --first-parent --dirty)"
NXDK_CFLAGS = -I$(CURDIR)/src -DGIT_VERSION=\"$(GIT_VERSION)\"
include $(NXDK_DIR)/Makefile
