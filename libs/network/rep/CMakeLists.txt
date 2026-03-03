add_library(rep STATIC source/rep.cpp)
add_library(syoch-robotics::rep ALIAS rep)

target_include_directories(rep PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/rep>
)

target_link_libraries(rep PUBLIC
  common
  random
  fep
)

install(TARGETS rep
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/rep
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/rep
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
