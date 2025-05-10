add_library(simple-can INTERFACE)
srobo1_target(simple-can network)

target_link_libraries(simple-can INTERFACE
  dcan
  __idf_driver
)
