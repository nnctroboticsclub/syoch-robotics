add_library(random STATIC source/random.cpp)
add_library(syoch-robotics::random ALIAS random)
target_include_directories(random PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/random>
)
target_link_libraries(random PUBLIC
  Nano::NanoHW
)

install(TARGETS random
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/random
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/random
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
