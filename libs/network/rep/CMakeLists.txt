add_library(rep STATIC source/rep.cpp)
srobo1_target(rep network)

target_link_libraries(rep PUBLIC
  common
  random
  thread
  fep
)