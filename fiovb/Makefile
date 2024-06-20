export V ?= 0

HOST_CROSS_COMPILE ?= $(CROSS_COMPILE)
TA_CROSS_COMPILE ?= $(CROSS_COMPILE)

ifdef CFG_FIOVB_VENDOR_CREATE
ifndef CFG_FIOVB_VENDOR_PREFIX
$(error CFG_FIOVB_VENDOR_CREATE is set but no CFG_FIOVB_VENDOR_PREFIX was provided)
endif
endif

.PHONY: all
all:
	$(MAKE) -C host CROSS_COMPILE="$(HOST_CROSS_COMPILE)" --no-builtin-variables
	$(MAKE) -C ta CROSS_COMPILE="$(TA_CROSS_COMPILE)" LDFLAGS=""

.PHONY: clean
clean:
	$(MAKE) -C host clean
	$(MAKE) -C ta clean
