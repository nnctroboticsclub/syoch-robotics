add_library(logger STATIC
  src/char_logger.cpp
  src/generic_logger.cpp
  src/logger-core.cpp
  src/log_sink.cpp
)
add_library(syoch-robotics::logger ALIAS logger)

target_link_libraries(logger PRIVATE
  common
  context-detector
)
target_link_libraries(logger PUBLIC
  Nano::Nano
  Nano::NanoHW
)

target_include_directories(logger PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/logger>
)

install(TARGETS logger
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/logger
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/logger
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
