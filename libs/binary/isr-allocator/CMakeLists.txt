add_library(syoch-robotics-isr-allocator STATIC source/isr-allocator.cpp)
add_library(syoch-robotics::isr-allocator ALIAS syoch-robotics-isr-allocator)

target_link_libraries(syoch-robotics-isr-allocator PRIVATE syoch-robotics-linked-allocator)

target_include_directories(syoch-robotics-isr-allocator PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/isr-allocator>
)

install(TARGETS syoch-robotics-isr-allocator
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/isr-allocator
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/isr-allocator
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
