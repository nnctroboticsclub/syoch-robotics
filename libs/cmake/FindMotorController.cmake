include(FetchContent)

FetchContent_Populate(MotorController
  GIT_REPOSITORY git@github.com:nnctroboticsclub/MotorController.git
  GIT_TAG aba526174f61a2b4e126b0bfa83311e292849e0e
  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/MotorController/src
  BINARY_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/MotorController/build
  SUBBUILD_DIR ${CMAKE_CURRENT_LIST_DIR}/.projects/MotorController/subbuild
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MotorController
  REQUIRED_VARS
    motorcontroller_SOURCE_DIR
)

find_package(StaticMbedOS REQUIRED)

if(MotorController_FOUND AND NOT TARGET MotorController)
  add_library(MotorController STATIC)
  target_include_directories(MotorController PUBLIC
    ${motorcontroller_SOURCE_DIR}
    ${motorcontroller_SOURCE_DIR}/Ikako_PID
    ${motorcontroller_SOURCE_DIR}/DisturbanceObserver
    ${motorcontroller_SOURCE_DIR}/DisturbanceObserver/LowPassFilter
  )
  target_sources(MotorController PUBLIC
    ${motorcontroller_SOURCE_DIR}/MotorController.cpp
    ${motorcontroller_SOURCE_DIR}/Ikako_PID/ikako_PID.cpp
    ${motorcontroller_SOURCE_DIR}/DisturbanceObserver/DOB.cpp
    ${motorcontroller_SOURCE_DIR}/DisturbanceObserver/LowPassFilter/LowPassFilter.cpp
  )
  target_link_libraries(MotorController PUBLIC
    StaticMbedOS
    ikarashiCAN_mk2
  )
endif()


