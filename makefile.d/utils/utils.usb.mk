# List USB devices
lu:
	@{ for D in /sys/bus/usb/devices/*; do \
		printf "%s:%s on %s-%s (%32s) :: %s\n" \
			$$(cat $$D/idVendor) $$(cat $$D/idProduct) \
			$$(cat $$D/busnum) $$(cat $$D/devpath) \
			$$(cat $$D/serial) \
			"$$(cat $$D/product)"; \
  done; } 2>/dev/null | grep -v "^: on -"