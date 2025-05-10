add_library(bd621x OBJECT source/BD621x.cpp source/BD621xFull.cpp)
srobo1_target(bd621x)


target_link_libraries(bd621x PUBLIC common)
target_link_libraries(bd621x PRIVATE gpio)
