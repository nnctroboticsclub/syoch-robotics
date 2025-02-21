add_library(syoch-robotics-simple-can INTERFACE)

target_link_libraries(syoch-robotics-simple-can INTERFACE
  dcan
  __idf_driver
)

target_include_directories(syoch-robotics-simple-can INTERFACE
  "./include"
)
