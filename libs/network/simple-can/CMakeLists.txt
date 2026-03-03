add_library(simple-can INTERFACE)
add_library(syoch-robotics::simple-can ALIAS simple-can)

target_link_libraries(simple-can INTERFACE Nano::NanoHW)

target_include_directories(simple-can INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/simple-can>
)

install(TARGETS simple-can
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/simple-can
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/simple-can
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
