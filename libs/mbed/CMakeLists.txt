add_library(syoch-robotics STATIC)
target_include_directories(syoch-robotics PUBLIC
  .
)
target_include_directories(syoch-robotics PRIVATE
  ../ikako_c620
  ../ikakoMDC
  ../ikarashiCAN_mk2
)
file(GLOB_RECURSE syoch-robotics_SOURCES_1 FOLLOW_SYMLINKS robotics/*.cpp robotics/*.c)
target_sources(syoch-robotics PRIVATE ${syoch-robotics_SOURCES_1})
target_link_libraries(syoch-robotics PRIVATE mbed-os ikako_c620 bno055 ikakoMDC ikarashiCAN_mk2)