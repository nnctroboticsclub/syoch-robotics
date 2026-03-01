add_library(ssp INTERFACE)
add_library(syoch-robotics::ssp ALIAS ssp)

target_include_directories(ssp INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/syoch-robotics/ssp>
)
install(TARGETS ssp
  EXPORT sroboTargets
  INCLUDES DESTINATION include/syoch-robotics/ssp
)
install(DIRECTORY include/
  DESTINATION include/syoch-robotics/ssp
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)
