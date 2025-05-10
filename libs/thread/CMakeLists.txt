add_library(thread STATIC
  source/thread.cpp
)
srobo1_target(thread thread)

target_link_libraries(thread PUBLIC
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)