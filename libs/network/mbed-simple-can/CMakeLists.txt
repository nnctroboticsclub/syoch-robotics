add_library(syoch-robotics-simple-can STATIC source/simple_can.cpp)
target_link_libraries(syoch-robotics-simple-can PUBLIC ${SYOCH_ROBOTICS_DEPENDENCIES} dcan)
target_include_directories(syoch-robotics-simple-can PUBLIC include)
