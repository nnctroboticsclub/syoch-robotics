add_library(syoch-robotics-thread INTERFACE)
target_link_libraries(syoch-robotics-thread INTERFACE Nano::NanoHW)
target_include_directories(syoch-robotics-thread INTERFACE include)
