add_library(isr-allocator STATIC source/isr-allocator.cpp)
target_link_libraries(isr-allocator PRIVATE linked-allocator)

srobo1_target(isr-allocator binary)