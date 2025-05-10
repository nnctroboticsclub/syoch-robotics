add_library(fep STATIC)
srobo1_target(fep network)

target_link_libraries(fep PUBLIC
  common
  thread
  logger
  timer
  gpio
)

target_sources(fep PRIVATE
  source/fep_raw_driver.cpp
)