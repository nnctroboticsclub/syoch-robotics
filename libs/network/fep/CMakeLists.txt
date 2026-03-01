add_library(fep STATIC
  source/fep_raw_driver.cpp
)
add_library(syoch-robotics::fep ALIAS fep)

target_link_libraries(fep PUBLIC
  syoch-robotics-common
  syoch-robotics-logger
  syoch-robotics-timer
  Nano::NanoHW
)

target_include_directories(fep PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/timer>
)

install(TARGETS fep
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/fep
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/fep
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
