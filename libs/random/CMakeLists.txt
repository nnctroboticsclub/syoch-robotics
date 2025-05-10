add_library(random STATIC source/random.cpp)
srobo1_target(random random)

target_link_libraries(random PRIVATE
  thread
)
