add_library(simple-can STATIC source/simple_can.cpp)
srobo1_target(simple-can network)

target_link_libraries(simple-can PRIVATE StaticMbedOS)
target_link_libraries(simple-can PUBLIC dcan)

