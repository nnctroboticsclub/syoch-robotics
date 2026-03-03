add_library(linked-allocator STATIC source/linked-allocator.cpp)
add_library(syoch-robotics::linked-allocator ALIAS linked-allocator)
target_include_directories(linked-allocator PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/linked-allocator>
)
install(TARGETS linked-allocator
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/linked-allocator
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/linked-allocator
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)


add_library(use-linked-allocator STATIC
  libc-override/override.cpp
)
add_library(syoch-robotics::use-linked-allocator ALIAS use-linked-allocator)
target_link_libraries(use-linked-allocator PUBLIC
  linked-allocator
)
install(TARGETS use-linked-allocator
  EXPORT sroboTargets
)

add_library(linked-allocator-mbed STATIC
  source/mbed-init.cpp
)
add_library(syoch-robotics::linked-allocator-mbed ALIAS linked-allocator-mbed)
target_link_libraries(linked-allocator-mbed PUBLIC
  linked-allocator
)
install(TARGETS linked-allocator-mbed
  EXPORT sroboTargets
)
