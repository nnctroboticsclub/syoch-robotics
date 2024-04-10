# Parameters

EMU_ROOT ?= $(dir $(lastword $(MAKEFILE_LIST)))

$(info EMU_ROOT: $(EMU_ROOT))
$(info s: $(EMU_ROOT)/stm32)

SRCDIR ?= stm32-main

STRICT_TARGET ?= src syoch-robotics/robotics
NONSTRICT_TARGET ?= ikakoMDC ikarashiCAN_mk2 bno055 ikako_c620

DEFINES += STM32F446xx
INCDIR += $(EMU_ROOT)/stm32 $(shell find $(SRCDIR) -type d -a -not -path *mbed-os* -a -not -path *BUILD* -not -path *.git*) $(SRCDIR)/syoch-robotics

# General Functions

FindSourceCode = $(shell find $(1) -name '*.c' -o -name '*.cpp')
SourceToObject = build/$(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(1)))

GetCompiler_C = $(CC)
GetCompiler_CXX = $(CXX)
GetCompiler = $(GetCompiler_$(strip $(1)))

GetExtension_C = c
GetExtension_CXX = cpp
GetExtension = $(GetExtension_$(strip $(1)))

# Sub parametes

aAllSources := $(foreach t, $(STRICT_TARGET), $(call FindSourceCode, $(SRCDIR)/$t/))
aAllSources += $(foreach t, $(NONSTRICT_TARGET), $(call FindSourceCode, $(SRCDIR)/$t/))
aAllSources += $(call FindSourceCode, $(EMU_ROOT)/stm32)

aObjects := $(foreach s, $(aAllSources), $(call SourceToObject, $(s)))

# Decided flags

CStrictFlags := -Wall -Wextra -Werror

CCommonFlags = -O3 $(addprefix -I, $(INCDIR)) $(addprefix -D, $(DEFINES))
CCommonFlags += -funsigned-char -D __MBED__

CFLAGS += $(CCommonFlags) -std=c11
CXXFLAGS += $(CCommonFlags) -std=c++17

GetStrictFlag_0 = $$(CStrictFlags)
GetStrictFlag_1 =
GetStrictFlag = $(GetStrictFlag_$(strip $(1)))

GetLangFlag = $$($(strip $(1))FLAGS)

# (lang, is_non_strict)
GetFlag = $(call GetLangFlag, $(1)) $(call GetStrictFlag, $(2))

# Visualize

sGetHeader_Lang_C = $(shell tput setaf 3; echo " C "; tput sgr0)
sGetHeader_Lang_CXX = $(shell tput setaf 2; echo "CXX"; tput sgr0)
sGetHeader_Lang_LD = $(shell tput setaf 5; echo "LD "; tput sgr0)
sGetHeader_Lang = $(sGetHeader_Lang_$(strip $(1)))

sHeaderStrict := $(shell tput setaf 4; echo "-"; tput sgr0)
sHeaderNonStrict := $(shell tput setaf 1; echo "!"; tput sgr0)

sGetHeader_Strict_0 = $(sHeaderStrict)
sGetHeader_Strict_1 = $(sHeaderNonStrict)
# (is_non_strict)
sGetHeader_Strict = $(sGetHeader_Strict_$(strip $(1)))

# (lang, is_non_strict)
sGetHeader = $(call sGetHeader_Lang, $(1))$(call sGetHeader_Strict, $(2))

sDone := $(shell tput setaf 2; echo "Done"; tput sgr0)

# Rules

.PHONY: all
all: build/stm32-main.elf run

.PHONY: clean
clean:
	@echo "[$(sHeaderLD)] Cleaning"; tput sgr0
	@rm -rf build
	@echo "[$(sHeaderLD)] Cleaning - $(sDone)"; tput sgr0

.PHONY: run
run: ./build/stm32-main.elf
	@echo "[$(sHeaderLD)] Running stm32-main.elf"; tput sgr0
	@./build/stm32-main.elf

# (prefix, isNonStrict, lang)
define DefineRule
$(eval lPrefix := $(1)) \
$(eval sLang := $(call sGetHeader_Lang, $(3))) \
$(eval sStrict := $(call sGetHeader_Strict, $(2))) \
$(eval sHeader := $(call sGetHeader, $(3), $(2))) \
$(eval lCompiler := $(call GetCompiler, $(3))) \
$(eval lExt := $(call GetExtension, $(3)))
build/$(lPrefix)/%.o: $(lPrefix)/%.$(lExt)
	@echo "[$(sHeader)] Compiling $$<"; tput sgr0
	@[ -d $$(dir $$@) ] || mkdir -p $$(dir $$@)
	@$(lCompiler) \
		$(call GetFlag, $(3), $(2)) \
		-MMD -MP -MF $$@.d \
		-c -o $$@ $$<
endef

$(foreach t, $(STRICT_TARGET), $(eval $(call DefineRule, $(SRCDIR)/$t, 0, C)))
$(foreach t, $(STRICT_TARGET), $(eval $(call DefineRule, $(SRCDIR)/$t, 0, CXX)))
$(foreach t, $(NONSTRICT_TARGET), $(eval $(call DefineRule, $(SRCDIR)/$t, 1, C)))
$(foreach t, $(NONSTRICT_TARGET), $(eval $(call DefineRule, $(SRCDIR)/$t, 1, CXX)))
-include $(aObjects:.o=.o.d)
$(eval $(call DefineRule, $(EMU_ROOT)/stm32, 0, C))
$(eval $(call DefineRule, $(EMU_ROOT)/stm32, 0, CXX))

build/stm32-main.elf: $(aObjects)
	@echo "[$(sHeaderLD)] Linking $@"; tput sgr0
	@$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $^