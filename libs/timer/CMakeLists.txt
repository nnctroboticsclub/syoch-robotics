add_library(syoch-robotics-timer STATIC
  source/timer.cpp
)

target_link_libraries(syoch-robotics-timer PUBLIC
  ${SYOCH_ROBOTICS_DEPENDENCIES}
)

target_include_directories(syoch-robotics-timer PUBLIC
  "./include"
)