add_library(dcan STATIC
  source/dcan.cpp
  source/can_module.cpp
)
add_library(syoch-robotics::dcan ALIAS dcan)

target_link_libraries(dcan PUBLIC
  timer
  common
)

target_include_directories(dcan PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/dcan>
)
install(TARGETS dcan
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/dcan
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/dcan
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
