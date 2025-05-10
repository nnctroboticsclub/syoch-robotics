add_library(logger STATIC
  source/char_logger.cpp
  source/generic_logger.cpp
  source/logger-core.cpp
  source/logger.cpp
  source/log_sink.cpp
)
srobo1_target(logger logger)

target_link_libraries(logger PRIVATE
  common
  temp-buffer
  context-detector
)

if(NOT ${NON_THREAD})
  message(STATUS "Using thread (logger) <<< TRACK ME!")
  target_link_libraries(logger PRIVATE thread)
  target_compile_definitions(logger PRIVATE -DUSE_THREAD)
endif()

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  target_compile_definitions(logger PRIVATE
    -DLOG_FOR_MBED -DTARGET_${MBED_TARGET}=1
    -DSYNC_PROTECT=1
  )
else()
  target_compile_definitions(logger PRIVATE -DSYNC_PROTECT=0)
endif()
