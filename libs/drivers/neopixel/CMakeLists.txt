add_library(syoch-robotics-neopixel STATIC source/neopixel.cpp)
add_library(syoch-robotics::neopixel ALIAS syoch-robotics-neopixel)

target_link_libraries(syoch-robotics-neopixel PUBLIC
  syoch-robotics-common
  Nano::NanoHW
)

target_include_directories(syoch-robotics-neopixel PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/neopixel>
)

install(TARGETS syoch-robotics-neopixel
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/neopixel
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/neopixel
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
