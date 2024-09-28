add_library(syoch-robotics-mbed-uart STATIC)

target_link_libraries(syoch-robotics-mbed-uart PUBLIC
  syoch-robotics-common
  syoch-robotics-thread
  syoch-robotics-logger
)

target_include_directories(syoch-robotics-mbed-uart PUBLIC
  "./include"
)

target_sources(syoch-robotics-mbed-uart PRIVATE
  source/uart_stream.cpp
)