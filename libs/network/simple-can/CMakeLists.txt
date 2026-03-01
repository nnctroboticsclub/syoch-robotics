add_library(syoch-robotics-simple-can INTERFACE)
add_library(syoch-robotics::simple-can ALIAS syoch-robotics-simple-can)

target_link_libraries(syoch-robotics-simple-can INTERFACE Nano::NanoHW)

target_include_directories(syoch-robotics-simple-can INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/simple-can>
)

install(TARGETS syoch-robotics-simple-can
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/simple-can
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/simple-can
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
