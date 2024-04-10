ESP32_IP        ?= 192.168.1.114

STM32_ResolveMountpointFromTag = /mnt/st-$(1)

# (project_dir)
STM32_ResolvProjectName = $(shell basename $(1))

# (project)
ESP32_ResolvExebin = $(wildcard $(1)/build/*/.bin)

# (tag, exe_bin, project_dir, skip_compile)
define ESP32_DefineRule_Compile
.PHONY: $(2)
$(2):
ifneq ($(4), 0)
		@printf "\e[92mSkipped $(shell basename $(3))'s binary Compile\e[m\n"
else
	cd $(3) && idf.py build
endif

c$(1): $(2)

endef

# (tag)
define ESP32_DefineRule_LocalFlash
f$(1):
	@echo --NOT IMPLEMENTED--

endef

# (tag)
define ESP32_DefineRule_RemoteFlash
f$(1)w:
	@echo --NOT IMPLEMENTED--

endef

# (tag, project_dir)
define ESP32_DefineRule_LocalMonitor
m$(1):
	cd $(2) && ESPBAUD=960000 idf.py monitor

endef

# (tag)
define ESP32_DefineRule_RemoteMonitor
m$(1)w:
	ESP32_IP=$(ESP32_IP) python3 ws-relay/log-reader.py "ESP32"

endef

# (tag)
define ESP32_DefineRule_RemoteReset
r$(1)w:
	@echo --NOT IMPLEMENTED--

endef

# (tag, exe_bin)
define ESP32_DefineRule_Addr2Line
a2r$(1):
	addr2line -e $(2)

endef

# (tag)
define ESP32_DefineRulesRemote
$(call ESP32_DefineRule_RemoteFlash,$(1))
$(call ESP32_DefineRule_RemoteMonitor,$(1))
$(call ESP32_DefineRule_RemoteReset,$(1))
endef

# (tag, exe_bin, project_dir, skip_compile)
define ESP32_DefineRulesLocal
$(call ESP32_DefineRule_Compile,$(1),$(2),$(3),$(4))
$(call ESP32_DefineRule_LocalFlash,$(1))
$(call ESP32_DefineRule_LocalMonitor,$(1),$(3))
$(call ESP32_DefineRule_Addr2Line,$(1),$(2))
endef

# (tag, exe_bin, project_dir, skip_compile)
define ESP32_DefineRules_
$(call ESP32_DefineRulesRemote,$(1))
$(call ESP32_DefineRulesLocal,$(1),$(2),$(3),$(4))

$(call AddDevice,esp32-$(1))
endef

# (tag, project_dir, skip_compile)
define ESP32_DefineRules
$(eval $(call ESP32_DefineRules_,$(1),$(call ESP32_ResolvExebin,$(2)),$(2),$(3)))
endef

