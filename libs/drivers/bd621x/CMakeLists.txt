add_library(bd621x INTERFACE)
target_include_directories(bd621x INTERFACE include)
target_link_libraries(bd621x INTERFACE Nano::NanoHW)
target_link_libraries(bd621x INTERFACE syoch-robotics-common)
