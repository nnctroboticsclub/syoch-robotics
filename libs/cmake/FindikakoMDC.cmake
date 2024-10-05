include(FetchContent)

FetchContent_Populate(ikakoMDC
  GIT_REPOSITORY git@github.com:nnctroboticsclub/ikakoMDC.git
  GIT_TAG 156c6f30ca7d5d96542bae1edf4bad3487494c7a
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/ikakoMDC/src
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/ikakoMDC/build
  SUBBUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/ikakoMDC/subbuild
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ikakoMDC
  REQUIRED_VARS
    ikakomdc_SOURCE_DIR
)

find_package(StaticMbedOS REQUIRED)
find_package(ikarashiCAN_mk2 REQUIRED)

if(ikakoMDC_FOUND AND NOT TARGET ikakoMDC)
  add_library(ikakoMDC STATIC)
  target_include_directories(ikakoMDC PUBLIC
    ${ikakomdc_SOURCE_DIR}
    ${ikakomdc_SOURCE_DIR}/lpf
    ${ikakomdc_SOURCE_DIR}/PID
  )
  target_sources(ikakoMDC PUBLIC
    ${ikakomdc_SOURCE_DIR}/ikakoMDC.cpp
    ${ikakomdc_SOURCE_DIR}/lpf/lpf.cpp
    ${ikakomdc_SOURCE_DIR}/PID/PID.cpp
  )
  target_link_libraries(ikakoMDC PUBLIC
    StaticMbedOS
    ikarashiCAN_mk2
  )
endif()


