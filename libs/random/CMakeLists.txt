add_library(syoch-robotics-random STATIC
  source/random.cpp
)

target_link_libraries(syoch-robotics-random PRIVATE
  syoch-robotics-thread
)

target_include_directories(syoch-robotics-random PUBLIC
  "./include"
)