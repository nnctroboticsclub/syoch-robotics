add_library(syoch-robotics OBJECT)

if (NOT DEFINED syoch-robotics-disable-features)
  target_include_directories(syoch-robotics PUBLIC
    ../ikako_c620
    ../ikakoMDC
    ../ikarashiCAN_mk2
  )
  target_link_libraries(syoch-robotics PRIVATE
    mbed-os ikako_c620 bno055 ikakoMDC ikarashiCAN_mk2
  )
  target_sources(syoch-robotics PRIVATE
    mbed-robotics/ikako_c620.registry.cpp
    mbed-robotics/simple_can.cpp
    mbed-robotics/ikako_mdc.cpp
    mbed-robotics/bno055_node.cpp
    robotics/node/BD621x.cpp
    robotics/node/BD621xFull.cpp
    robotics/node/BLDC.cpp
  )
endif()

if (NOT DEFINED syoch-robotics-disable-thread)
  target_sources(syoch-robotics PRIVATE
    robotics/platform/thread.cpp
  )
  target_link_libraries(syoch-robotics PRIVATE
    mbed-os
  )
endif()

target_include_directories(syoch-robotics PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(syoch-robotics PRIVATE
  robotics/network/rep.cpp
  robotics/network/fep/fep_raw_driver.cpp
  robotics/network/can_module.cpp
  robotics/network/dcan.cpp
  robotics/types/angle_joystick_2d.cpp
  robotics/platform/panic.cpp
  robotics/platform/timer.cpp
  robotics/platform/random.cpp
  robotics/logger/char_logger.cpp
  robotics/logger/generic_logger.cpp
  robotics/logger/logger-core.cpp
  robotics/logger/logger.cpp
  robotics/utils/neopixel_driver.cpp
  robotics/utils/neopixel.cpp
  robotics/node/node.cpp
  robotics/node/node_inspector.cpp
  robotics/types/pid_gains.cpp
  robotics/types/joystick_2d.cpp
)