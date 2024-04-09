DEVICE_LIST :=

# device define
# (type)-(tag){-(optional)}

define AddDevice
DEVICE_LIST += $(1)
endef

.PHONY: sdl
sdl:
	@tput setaf 4; echo --- Device List ---; tput sgr0
	@echo $(DEVICE_LIST)