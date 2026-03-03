add_library(neopixel STATIC source/neopixel.cpp)
add_library(syoch-robotics::neopixel ALIAS neopixel)

target_link_libraries(neopixel PUBLIC
  common
  Nano::NanoHW
)

target_include_directories(neopixel PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/neopixel>
)

install(TARGETS neopixel
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/neopixel
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/neopixel
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
