add_library(syoch-robotics-linked-allocator STATIC
  source/linked-allocator.cpp
)


target_include_directories(syoch-robotics-linked-allocator PUBLIC
  include
)



add_library(syoch-robotics-use-linked-allocator STATIC
libc-override/override.cpp
)

target_link_libraries(syoch-robotics-use-linked-allocator PUBLIC
syoch-robotics-linked-allocator
)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  target_sources(syoch-robotics-use-linked-allocator INTERFACE source/mbed-init.cpp)
endif()
