add_library(ikako-mdc STATIC source/ikako_mdc.cpp)
add_library(syoch-robotics::ikako-mdc ALIAS ikako-mdc)

find_package(ikarashiCAN_mk2 REQUIRED)
find_package(ikakoMDC REQUIRED)
find_package(Nano REQUIRED)

target_link_libraries(ikako-mdc PUBLIC
  dcan
  ikarashiCAN_mk2::ikarashiCAN_mk2
  ikakoMDC::ikakoMDC
  common
  Nano::NanoHW_MbedIF
)

target_include_directories(ikako-mdc PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/ikako-mdc>
)

install(TARGETS ikako-mdc
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/ikako-mdc
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/ikako-mdc
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
