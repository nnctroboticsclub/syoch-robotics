message(STATUS "Sync Stream library is being built")

add_library(syoch-robotics-sync-stream INTERFACE)
target_include_directories(syoch-robotics-sync-stream INTERFACE include)
target_link_libraries(syoch-robotics-sync-stream INTERFACE syoch-robotics-thread)
