add_library(syoch-robotics-logger STATIC
  source/char_logger.cpp
  source/generic_logger.cpp
  source/logger-core.cpp
  source/logger.cpp
)

target_link_libraries(syoch-robotics-logger PRIVATE
  syoch-robotics-common
)

# If not non-thread defined, added syoch-robotics-thread library with target_link_libraries
if(NOT NON_THREAD)
  target_link_libraries(syoch-robotics-logger PRIVATE
    syoch-robotics-thread
  )
  target_compile_definitions(syoch-robotics-logger PRIVATE
    -DUSE_THREAD
  )
endif()

target_include_directories(syoch-robotics-logger PUBLIC
  "./include"
)