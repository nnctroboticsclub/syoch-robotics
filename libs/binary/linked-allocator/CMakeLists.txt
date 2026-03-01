add_library(syoch-robotics-linked-allocator STATIC source/linked-allocator.cpp)
add_library(syoch-robotics::linked-allocator ALIAS syoch-robotics-linked-allocator)
target_include_directories(syoch-robotics-linked-allocator PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/linked-allocator>
)
install(TARGETS syoch-robotics-linked-allocator
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/linked-allocator
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/linked-allocator
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)


add_library(syoch-robotics-use-linked-allocator STATIC
  libc-override/override.cpp
)
add_library(syoch-robotics::use-linked-allocator ALIAS syoch-robotics-use-linked-allocator)
target_link_libraries(syoch-robotics-use-linked-allocator PUBLIC
  syoch-robotics-linked-allocator
)
install(TARGETS syoch-robotics-use-linked-allocator
  EXPORT sroboTargets
)

add_library(syoch-robotics-linked-allocator-mbed STATIC
  source/mbed-init.cpp
)
add_library(syoch-robotics::linked-allocator-mbed ALIAS syoch-robotics-linked-allocator-mbed)
target_link_libraries(syoch-robotics-linked-allocator-mbed PUBLIC
  syoch-robotics-linked-allocator
)
install(TARGETS syoch-robotics-linked-allocator-mbed
  EXPORT sroboTargets
)
