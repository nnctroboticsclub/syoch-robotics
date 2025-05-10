add_library(temp-buffer STATIC
  source/temp_buffer.cpp
)

target_link_libraries(temp-buffer PRIVATE
  context-detector
)

srobo1_target(temp-buffer binary)