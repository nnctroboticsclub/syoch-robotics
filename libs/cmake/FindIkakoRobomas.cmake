include(FetchContent)

FetchContent_Populate(IkakoRobomas
  GIT_REPOSITORY git@github.com:nnctroboticsclub/IkakoRobomas.git
  GIT_TAG 98336d610bb255c57565b571e47c404c0070d49d
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/IkakoRobomas/src
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/IkakoRobomas/build
  SUBBUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/IkakoRobomas/subbuild
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IkakoRobomas
  REQUIRED_VARS
    ikakorobomas_SOURCE_DIR
)

if(IkakoRobomas_FOUND AND NOT TARGET IkakoRobomas)
  add_library(IkakoRobomas STATIC)
  target_include_directories(IkakoRobomas PUBLIC
    ${ikakorobomas_SOURCE_DIR}
  )
  target_sources(IkakoRobomas PUBLIC
    ${ikakorobomas_SOURCE_DIR}/ikako_m2006.cpp
    ${ikakorobomas_SOURCE_DIR}/ikako_m3508.cpp
    ${ikakorobomas_SOURCE_DIR}/ikako_robomas.cpp
  )
  target_link_libraries(IkakoRobomas PUBLIC
    ikarashiCAN_mk2
    MotorController
  )
endif()


