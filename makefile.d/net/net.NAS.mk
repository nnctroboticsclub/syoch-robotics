$(NAS_PATH):
	sudo mkdir $(NAS_PATH)

$(NAS_MOUNT_FLAGS): $(NAS_PATH)
	@if mount | grep $(NAS_PATH) 2>&1 1>/dev/null; then \
		echo "Already Mounted"; \
	else \
		sudo mount $(NAS_ADDR) $(NAS_PATH) -t cifs $(NAS_FLAGS); \
		echo "Mounted"; \
	fi

$(NAS_FLASH): $(NAS_MOUNT_FLAGS)
	[ -d $(NAS_FLASH) ] || sudo mkdir $(NAS_FLASH)

$(NAS_FLASH_PATH): $(NAS_FLASH)
	[ -d $(NAS_FLASH_PATH) ] || sudo mkdir $(NAS_FLASH_PATH)

$(NAS_FLASH_MDC): $(NAS_FLASH)
	[ -d $(NAS_FLASH_MDC) ] || sudo mkdir $(NAS_FLASH_MDC)

nsn: $(NAS_FLASH_MDC)
	cp /workspaces/nishiwakiMDC/BUILD/NUCLEO_F446RE/GCC_ARM/nishiwakiMDC.bin $(NAS_FLASH_MDC)/nishiwakiMDC-$(TIME).bin