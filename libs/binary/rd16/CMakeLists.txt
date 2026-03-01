add_library(rd16 INTERFACE)
add_library(syoch-robotics::rd16 ALIAS rd16)

target_include_directories(rd16 INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/rd16>
)

install(TARGETS rd16
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/rd16
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/rd16
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
