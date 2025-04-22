add_library(syoch-robotics-simple-can STATIC source/simple_can.cpp)
target_link_libraries(syoch-robotics-simple-can PRIVATE StaticMbedOS)
target_link_libraries(syoch-robotics-simple-can PUBLIC dcan)
target_include_directories(syoch-robotics-simple-can PUBLIC include)
