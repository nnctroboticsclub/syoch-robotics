add_subdirectory(dcan)
add_subdirectory(ssp)
add_subdirectory(froute)
add_subdirectory(sync-stream)

if ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "Mbed")
  add_subdirectory(mbed-uart)
  add_subdirectory(mbed-simple-can)
elseif ("${SYOCH_ROBOTICS_TARGET}" STREQUAL "ESP")
  add_subdirectory(idf-simple-can)
endif()

if (NOT ${NON_THREAD})
  add_subdirectory(fep)
  endif()

if (${SROBO1_RANDOM_PROVIDED})
  add_subdirectory(rep)
endif()