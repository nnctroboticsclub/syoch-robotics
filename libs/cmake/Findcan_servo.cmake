include(FetchContent)

FetchContent_Populate(can_servo
  GIT_REPOSITORY git@github.com:nnctroboticsclub/can_servo.git
  GIT_TAG e68a8af81d92ede8c92f08270f93f54c58aa22ac
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/can_servo/src
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/can_servo/build
  SUBBUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/can_servo/subbuild
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(can_servo
  REQUIRED_VARS
    can_servo_SOURCE_DIR
)

find_package(StaticMbedOS REQUIRED)
find_package(ikarashiCAN_mk2 REQUIRED)

if(can_servo_FOUND AND NOT TARGET can_servo)
  add_library(can_servo STATIC)
  target_include_directories(can_servo PUBLIC
    ${can_servo_SOURCE_DIR}
  )
  target_sources(can_servo PUBLIC
    ${can_servo_SOURCE_DIR}/can_servo.cpp
  )
  target_link_libraries(can_servo PUBLIC
    StaticMbedOS
    ikarashiCAN_mk2
  )
endif()


