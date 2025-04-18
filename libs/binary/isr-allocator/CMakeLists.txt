add_library(syoch-robotics-isr-allocator STATIC source/isr-allocator.cpp)
target_link_libraries(syoch-robotics-isr-allocator PRIVATE syoch-robotics-linked-allocator)


target_include_directories(syoch-robotics-isr-allocator PUBLIC
  include
)