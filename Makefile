export V ?= 0

OUTPUT_DIR := $(CURDIR)/out

.PHONY: all
all: fiovb-target prepare-for-rootfs

.PHONY: clean
clean: fiovb-clean prepare-for-rootfs-clean

fiovb-target:
	@echo "Building fiovb"
	$(MAKE) -C fiovb CROSS_COMPILE="$(HOST_CROSS_COMPILE)" || exit -1

fiovb-clean:
	@echo "Cleaning fiovb"
	$(MAKE) -C fiovb clean || exit -1;

prepare-for-rootfs: fiovb-target
	@echo "Copying fiovb CA  to $(OUTPUT_DIR)..."
	@mkdir -p $(OUTPUT_DIR)
	@mkdir -p $(OUTPUT_DIR)/ca
	if [ -e fiovb/host/fiovb ]; then \
		cp -p fiovb/host/fiovb $(OUTPUT_DIR)/ca/; \
	fi

prepare-for-rootfs-clean:
	@rm -rf $(OUTPUT_DIR)/ca
	@rmdir --ignore-fail-on-non-empty $(OUTPUT_DIR) || test ! -e $(OUTPUT_DIR)
