add_library(syoch-robotics-random STATIC source/random.cpp)
add_library(syoch-robotics::random ALIAS syoch-robotics-random)
target_include_directories(syoch-robotics-random PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/random>
)
target_link_libraries(syoch-robotics-random PUBLIC
  Nano::NanoHW
)

install(TARGETS syoch-robotics-random
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/random
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/random
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
