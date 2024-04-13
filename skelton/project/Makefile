all:

-include env.home.mk
-include syoch-robotics/makefile.d/all.mk

lc:
	wc -ml \
		$$(find ./stm32-main/src -type f -a \( -name *.c -o -name *.cpp -o -name *.h -o -name *.hpp \)) \
		$$(find ./stm32-enc/src -type f -a \( -name *.c -o -name *.cpp -o -name *.h -o -name *.hpp \)) \
		$$(find ./esp32/main -type f -a \( -name *.c -o -name *.cpp -o -name *.h -o -name *.hpp \)) \
		$$(find ./syoch-robotics -type f -a \( -name *.c -o -name *.cpp -o -name *.h -o -name *.hpp \))

ESP_SKIP_COMPILE ?= 0
$(eval $(call ESP32_DefineRules,e,$(PWD)/esp32,$(ESP_SKIP_COMPILE)))

