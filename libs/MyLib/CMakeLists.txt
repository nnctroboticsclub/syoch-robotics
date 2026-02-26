add_library(MyLib STATIC
  src/lib.cpp
)

add_library(syoch-robotics::MyLib ALIAS MyLib)

target_include_directories(MyLib PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/MyLib>
)

install(TARGETS syoch-robotics-MyLib
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/MyLib
)

install(DIRECTORY include/
  DESTINATION include/syoch-robotics/MyLib
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
