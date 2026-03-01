cmake_minimum_required(VERSION 3.25)
cmake_policy(VERSION 3.25)

set(ROBO_TOOLCHAIN_FILE ClangArmToolchain)
set(CMAKE_INSTALL_MESSAGE LAZY)

enable_testing()
project(syoch-robotics)
enable_language(C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-mfloat-abi=softfp -mfpu=fpv4-sp-d16 -mthumb -mcpu=cortex-m4)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -gfull -fstandalone-debug -gdwarf-5 -gz")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gfull -fstandalone-debug -gdwarf-5 -gz")

add_compile_options(-Os -ffunction-sections -fdata-sections)

find_package(Nano REQUIRED)
find_package(CMSIS5 REQUIRED)

add_subdirectory(libs)

install(
  EXPORT sroboTargets
  FILE sroboTargets.cmake
  NAMESPACE syoch-robotics::
  DESTINATION lib/cmake/syoch-robotics
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/syoch-roboticsConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/syoch-roboticsConfig.cmake
  INSTALL_DESTINATION lib/cmake/syoch-robotics
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/syoch-roboticsConfig.cmake
  DESTINATION lib/cmake/syoch-robotics
)
