CMAKE=$(shell which cmake)

.PHONY: clean
clean:
	rm -rf build

.PHONY: configure
configure:
	$(CMAKE) . -B build/Host -G "Ninja Multi-Config" \
		-DCMAKE_TOOLCHAIN_FILE=/usr/share/robotics/cmake/toolchains/Host.cmake \
	 	--graphviz=build/Host/graph.dot

	$(CMAKE) . -B build/F3H -G "Ninja Multi-Config" \
		-DCMAKE_TOOLCHAIN_FILE=/usr/share/robotics/cmake/toolchains/F3H.cmake \
	 	--graphviz=build/F3H/graph.dot

	$(CMAKE) . -B build/F4H -G "Ninja Multi-Config" \
		-DCMAKE_TOOLCHAIN_FILE=/usr/share/robotics/cmake/toolchains/F4H.cmake \
	 	--graphviz=build/F4H/graph.dot


	$(CMAKE) . -B build/F4M -G "Ninja Multi-Config" \
		-DCMAKE_TOOLCHAIN_FILE=/usr/share/robotics/cmake/toolchains/F4M.cmake \
	 	--graphviz=build/F4M/graph.dot

.PHONY: build
build:
	$(CMAKE) --build build/Host
	$(CMAKE) --build build/F3H
	$(CMAKE) --build build/F4H
	$(CMAKE) --build build/F4M


.PHONY: pack
pack:
	ninja -C build/Host package
	ninja -C build/F3H package
	ninja -C build/F4H package
	ninja -C build/F4M package

.PHONY: install
install:
	ninja -C build/host install
