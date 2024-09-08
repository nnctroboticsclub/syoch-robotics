file(GLOB_RECURSE syoch-robotics-core FOLLOW_SYMLINKS
  robotics/*.cpp robotics/*.c
)

if (NOT DEFINED syoch-robotics-DISABLE-FEATURES)
  target_include_directories(syoch-robotics PUBLIC
    ../ikako_c620
    ../ikakoMDC
    ../ikarashiCAN_mk2
  )
  target_link_libraries(syoch-robotics PRIVATE
    mbed-os ikako_c620 bno055 ikakoMDC ikarashiCAN_mk2
  )
  file(GLOB_RECURSE syoch-robotics-features FOLLOW_SYMLINKS mbed-robotics/*.cpp mbed-robotics/*.c)
else()
  set(syoch-robotics-features "")
endif()

add_library(syoch-robotics OBJECT)
target_include_directories(syoch-robotics PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(syoch-robotics PRIVATE
  ${syoch-robotics-core}
  ${syoch-robotics-features}
)