add_library(syoch-robotics-simple-can INTERFACE)
target_link_libraries(syoch-robotics-simple-can INTERFACE Nano::NanoHW)
target_include_directories(syoch-robotics-simple-can INTERFACE include)
