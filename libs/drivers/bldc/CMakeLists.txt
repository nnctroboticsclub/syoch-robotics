add_library(syoch-robotics-bldc STATIC source/BLDC.cpp)

target_link_libraries(syoch-robotics-bldc PUBLIC
  Nano::NanoHW
  syoch-robotics-common
)

target_include_directories(syoch-robotics-bldc PUBLIC include)