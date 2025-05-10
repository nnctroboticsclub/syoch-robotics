add_library(linked-allocator STATIC
  source/linked-allocator.cpp
)
srobo1_target(linked-allocator binary)

add_library(use-linked-allocator STATIC
  libc-override/override.cpp
)

target_link_libraries(use-linked-allocator PUBLIC
  linked-allocator
)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  target_sources(use-linked-allocator INTERFACE source/mbed-init.cpp)
endif()
