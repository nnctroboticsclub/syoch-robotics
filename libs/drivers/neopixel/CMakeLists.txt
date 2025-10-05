add_library(syoch-robotics-neopixel STATIC
  source/neopixel.cpp
  source/neopixel_driver.cpp
)

target_link_libraries(syoch-robotics-neopixel PUBLIC
  ${SYOCH_ROBOTICS_DEPENDENCIES}
  syoch-robotics-common
  syoch-robotics-gpio
)

target_include_directories(syoch-robotics-neopixel PUBLIC
  "./include"
)