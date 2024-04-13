ns:
	@[ '$(TAG)' = '' ] && echo "TAG is not set" && exit 1 || true
	@[ '$(LOG_TAG)' = '' ] && echo "LOG_TAG is not set" && exit 1 || true
	@[ '$(SERIAL)' = '' ] && echo "SERIAL is not set" && exit 1 || true
	@[ '$(PROJECT_DIR)' = '' ] && echo "PROJECT_DIR is not set" && exit 1 || true
	@[ '$(MOUNT)' = '' ] && echo "MOUNT is not set" && exit 1 || true

	@$(eval TAG_UP := $(shell echo $(TAG) | tr a-z A-Z))
	@echo '$(TAG_UP)_LOG_TAG      := $(LOG_TAG)' >> Makefile
	@echo '$(TAG_UP)_SKIP_COMPILE ?= 0' >> Makefile
	@echo '$(TAG_UP)_SERIAL       ?= $(SERIAL)' >> Makefile
	@echo '$$(eval $$(call STM32_DefineRules,s1,$$(ESP32_IP),$$($(TAG_UP)_LOG_TAG),$$(PWD)/$(PROJECT_DIR),$$($(TAG_UP)_SKIP_COMPILE),NUCLEO_F446RE,$(MOUNT),$$($(TAG_UP)_SERIAL)))' >> Makefile
	@echo '' >> Makefile

	cp -r $(SELF)/../skelton/mbed-os6 $(PROJECT_DIR)
	cd $(PROJECT_DIR) && mbed deploy
	ln -sr syoch-robotics/libs/mbed $(PROJECT_DIR)/syoch-robotics

ne:
	@[ '$(TAG)' = '' ] && echo "TAG is not set" && exit 1 || true
	@[ '$(PROJECT_DIR)' = '' ] && echo "PROJECT_DIR is not set" && exit 1 || true
	$(eval TAG_UP := $(shell echo $(TAG) | tr a-z A-Z))
	@echo '$(TAG_UP)_SKIP_COMPILE ?= 0' >> Makefile
	@echo '$$(eval $$(call ESP32_DefineRules,$(TAG),$$(PWD)/$(PROJECT_DIR),$$($(TAG_UP)_SKIP_COMPILE)))' >> Makefile
	@echo '' >> Makefile

	cp -r $(SELF)/../skelton/esp-idf $(PROJECT_DIR)
	mkdir $(PROJECT_DIR)/components
	ln -sr syoch-robotics/libs/espidf $(PROJECT_DIR)/components/syoch-robotics