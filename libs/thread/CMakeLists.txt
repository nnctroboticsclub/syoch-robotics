add_library(syoch-robotics-thread STATIC
  source/thread.cpp
)

target_link_libraries(syoch-robotics-thread PUBLIC
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)

target_include_directories(syoch-robotics-thread PUBLIC
  "./include"
)