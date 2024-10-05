include(FetchContent)

FetchContent_Populate(im920_rs
  GIT_REPOSITORY git@github.com:nnctroboticsclub/im920_rs.git
  GIT_TAG b9addc59d63d9afbc7bd3fbe6cfbd7da596303c6
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/im920_rs/src
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/im920_rs/build
  SUBBUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/im920_rs/subbuild
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(im920_rs
  REQUIRED_VARS
    im920_rs_SOURCE_DIR
)

if(im920_rs_FOUND AND NOT TARGET im920_rs)
  add_subdirectory(${im920_rs_SOURCE_DIR} ${im920_rs_BINARY_DIR})
endif()