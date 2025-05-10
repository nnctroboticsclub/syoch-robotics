add_library(timer STATIC source/timer.cpp)
srobo1_target(timer timer)

target_link_libraries(timer PUBLIC
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)
