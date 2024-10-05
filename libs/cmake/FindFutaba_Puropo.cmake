include(FetchContent)

FetchContent_Populate(Futaba_Puropo
  GIT_REPOSITORY git@github.com:nnctroboticsclub/Futaba_Puropo.git
  GIT_TAG 2e5d6d59754e0f4622dc85e7a4ccaf80a666e47b
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/Futaba_Puropo/src
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/Futaba_Puropo/build
  SUBBUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/Futaba_Puropo/subbuild
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Futaba_Puropo
  REQUIRED_VARS
    futaba_puropo_SOURCE_DIR
)

find_package(StaticMbedOS REQUIRED)

if(Futaba_Puropo_FOUND AND NOT TARGET Futaba_Puropo)
  add_library(Futaba_Puropo STATIC)
  target_include_directories(Futaba_Puropo PUBLIC
    ${futaba_puropo_SOURCE_DIR}
  )
  target_sources(Futaba_Puropo PUBLIC
    ${futaba_puropo_SOURCE_DIR}/puropo.cpp
  )
  target_link_libraries(Futaba_Puropo PUBLIC
    StaticMbedOS
  )
endif()


