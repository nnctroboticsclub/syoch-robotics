add_library(mbed-uart STATIC)
add_library(syoch-robotics::mbed-uart ALIAS mbed-uart)

target_link_libraries(mbed-uart PUBLIC
  common
  logger
  Nano::NanoHW_MbedIF
)

target_include_directories(mbed-uart PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/mbed-uart>
)

target_sources(mbed-uart PRIVATE
  source/uart_stream.cpp
)

install(TARGETS mbed-uart
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/mbed-uart
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/mbed-uart
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
