# [Init] Container
i_c:
	sudo apt update
	sudo apt install -y tmux

# [Init] General
i:
	[ ! -e /usr/local/bin/websocat ] && sudo cp /workspaces/korobo2023/websocat.x86_64-unknown-linux-musl /usr/local/bin/websocat; true

	mbed config -G GCC_ARM_PATH /opt/gcc-arm-none-eabi-10.3-2021.10/bin
	mbed toolchain -G GCC_ARM

	[ -z $${IDF_PATH+x} ] && exec bash -c ". /opt/esp-idf/export.sh; exec bash"; true


# [Init] Mbed
im:
	cd stm32-main; mbed deploy


# [Init]
id:
	sudo bash -c 'echo nameserver 8.8.8.8 > /etc/resolv.conf'