add_library(fep STATIC)

target_link_libraries(fep PUBLIC
  syoch-robotics-common
  syoch-robotics-logger
  syoch-robotics-timer
  Nano::NanoHW
)

target_include_directories(fep PUBLIC include)

target_sources(fep PRIVATE
  source/fep_raw_driver.cpp
)