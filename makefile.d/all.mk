rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

TIME := $(shell date +%H%m%d-%H%M%S)

SELF := $(firstword $(filter %/all.mk, $(MAKEFILE_LIST)))
SELF := $(dir $(realpath $(SELF)))
SELF := $(abspath $(SELF))
ALL_MK := $(call rwildcard, $(SELF), %.mk)
ALL_MK := $(filter-out $(SELF)/all.mk, $(ALL_MK))

$(info TIME: $(TIME))

-include $(wildcard $(SELF)/env/*.mk)
-include $(ALL_MK)

