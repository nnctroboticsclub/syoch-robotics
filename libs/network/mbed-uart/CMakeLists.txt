add_library(mbed-uart STATIC)
srobo1_target(mbed-uart network)

target_link_libraries(mbed-uart PUBLIC
  common
  thread
  logger
)

target_sources(mbed-uart PRIVATE
  source/uart_stream.cpp
)