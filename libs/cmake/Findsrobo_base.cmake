include(FetchContent)

FetchContent_Populate(srobo_base
  GIT_REPOSITORY git@github.com:nnctroboticsclub/srobo_base.git
  GIT_TAG 2c638f84fcb3bdf4ee6bc9e651e6ce19dba98a29
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/srobo_base/src
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/srobo_base/build
  SUBBUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/srobo_base/subbuild
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(srobo_base
  REQUIRED_VARS
    srobo_base_SOURCE_DIR
)

if(srobo_base_FOUND AND NOT TARGET srobo_base)
  add_subdirectory(${srobo_base_SOURCE_DIR} ${srobo_base_BINARY_DIR})
endif()