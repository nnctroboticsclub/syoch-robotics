add_library(syoch-robotics-mbed-uart STATIC)
add_library(syoch-robotics::mbed-uart ALIAS syoch-robotics-mbed-uart)

target_link_libraries(syoch-robotics-mbed-uart PUBLIC
  syoch-robotics-common
  syoch-robotics-logger
  Nano::NanoHW_MbedIF
)

target_include_directories(syoch-robotics-mbed-uart PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/mbed-uart>
)

target_sources(syoch-robotics-mbed-uart PRIVATE
  source/uart_stream.cpp
)

install(TARGETS syoch-robotics-mbed-uart
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/mbed-uart
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/mbed-uart
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
