add_library(rep STATIC source/rep.cpp)

target_include_directories(rep PUBLIC
  "./include"
)

target_link_libraries(rep PUBLIC
  syoch-robotics-common
  syoch-robotics-random
  syoch-robotics-thread
  fep
)