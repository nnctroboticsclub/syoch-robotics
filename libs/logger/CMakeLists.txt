add_library(syoch-robotics-logger STATIC
  source/char_logger.cpp
  source/generic_logger.cpp
  source/logger-core.cpp
  source/logger.cpp
  source/log_sink.cpp
)

target_link_libraries(syoch-robotics-logger PRIVATE
  syoch-robotics-common
  syoch-robotics-temp-buffer
  syoch-robotics-context-detector
)

if(NOT ${NON_THREAD})
  message(STATUS "Using thread (logger) <<< TRACK ME!")
  target_link_libraries(syoch-robotics-logger PRIVATE
    syoch-robotics-thread
  )
  target_compile_definitions(syoch-robotics-logger PRIVATE
    -DUSE_THREAD
  )
endif()

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed" OR "${SYOCH_ROBOTICS_TARGET}" STREQUAL "STM32 HAL")
  target_compile_definitions(syoch-robotics-logger PRIVATE
    -DLOG_FOR_MBED -DTARGET_${MBED_TARGET}=1
  )
endif()

target_include_directories(syoch-robotics-logger PUBLIC
  "./include"
)