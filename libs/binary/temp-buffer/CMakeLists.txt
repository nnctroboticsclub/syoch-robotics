add_library(syoch-robotics-temp-buffer STATIC
  source/temp_buffer.cpp
)

target_link_libraries(syoch-robotics-temp-buffer PRIVATE
  syoch-robotics-context-detector
)

target_include_directories(syoch-robotics-temp-buffer PUBLIC
  include
)

