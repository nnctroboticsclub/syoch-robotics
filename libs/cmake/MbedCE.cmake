include(MbedCE-Toolchain)

find_package(mbed-ce REQUIRED)
add_subdirectory(${mbed-ce_SOURCE_DIR} mbed-ce)