include(FetchContent)

FetchContent_Populate(ikarashiCAN_mk2
  GIT_REPOSITORY git@github.com:nnctroboticsclub/ikarashiCAN_mk2.git
  GIT_TAG 2c2bfe57a470c430bb6d0e0ebd95760cf5d5ede3
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/ikarashiCAN_mk2/src
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/ikarashiCAN_mk2/build
  SUBBUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/ikarashiCAN_mk2/subbuild
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ikarashiCAN_mk2
  REQUIRED_VARS
    ikarashican_mk2_SOURCE_DIR
)

find_package(StaticMbedOS REQUIRED)

if(ikarashiCAN_mk2_FOUND AND NOT TARGET ikarashiCAN_mk2)
  add_library(ikarashiCAN_mk2 STATIC)
  target_include_directories(ikarashiCAN_mk2 PUBLIC
    ${ikarashican_mk2_SOURCE_DIR}
    ${ikarashican_mk2_SOURCE_DIR}/NoMutexCAN-master
  )
  target_sources(ikarashiCAN_mk2 PUBLIC
    ${ikarashican_mk2_SOURCE_DIR}/ikarashiCAN_mk2.cpp
  )
  target_link_libraries(ikarashiCAN_mk2 PUBLIC
    StaticMbedOS
  )
endif()


