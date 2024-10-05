include(FetchContent)

FetchContent_Populate(mbed-ce
  GIT_REPOSITORY git@github.com:mbed-ce/mbed-os.git
  GIT_TAG 3297baedff11f68657f656d17ec738ee59ea3e8f
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/mbed-ce/src
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/mbed-ce/build
  SUBBUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/mbed-ce/subbuild
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(mbed-ce
  REQUIRED_VARS
    mbed-ce_SOURCE_DIR
)