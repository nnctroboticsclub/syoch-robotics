add_subdirectory(dcan)
add_subdirectory(ssp)
add_subdirectory(froute)
add_subdirectory(sync-stream)
add_subdirectory(mbed-uart)
add_subdirectory(simple-can)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "ESP")
  add_subdirectory(idf-simple-can)
endif()

add_subdirectory(fep)
add_subdirectory(rep)