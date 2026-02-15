add_library(syoch-robotics-logger STATIC
  source/char_logger.cpp
  source/generic_logger.cpp
  source/logger-core.cpp
  source/log_sink.cpp
)

target_link_libraries(syoch-robotics-logger PRIVATE
  syoch-robotics-common
  syoch-robotics-context-detector
)
target_link_libraries(syoch-robotics-logger PUBLIC
  Nano::Nano
  Nano::NanoHW
)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  target_compile_definitions(syoch-robotics-logger PRIVATE
    -DLOG_FOR_MBED -DTARGET_${MBED_TARGET}=1
    -DSYNC_PROTECT=1
  )
else()
  target_compile_definitions(syoch-robotics-logger PRIVATE
    -DSYNC_PROTECT=0
  )
endif()

target_include_directories(syoch-robotics-logger PUBLIC
  "./include"
)