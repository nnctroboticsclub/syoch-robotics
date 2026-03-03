add_library(timer STATIC
  source/timer.cpp
)
add_library(syoch-robotics::timer ALIAS timer)

target_link_libraries(timer PUBLIC
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)

target_include_directories(timer PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/timer>
)

install(TARGETS timer
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/timer
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/timer
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
