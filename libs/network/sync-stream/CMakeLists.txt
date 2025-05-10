add_library(sync-stream INTERFACE)
srobo1_target(sync-stream network)

target_link_libraries(sync-stream INTERFACE thread)