find_path(syoch_robotics ...)
find_library(<package>_LIBRARY ...) # ヘッダーのみのライブラリの場合は不要
mark_as_advanced(
  syoch_robotics
  <package>_LIBRARY     # ヘッダーのみのライブラリの場合は不要
  )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(<package>
  REQUIRED_VARS
    syoch_robotics
    <package>_LIBRARY      # ヘッダーのみのライブラリの場合は不要
  )

if(<package>_FOUND AND NOT TARGET <package>::<package>)
  add_library(<package>::<package> UNKNOWN IMPORTED)
  set_target_properties(<package>::<package> PROPERTIES
    IMPORTED_LINK_INTERFACE_LANGUAGES ["C"|"CXX"]  # ヘッダーのみのライブラリの場合は不要
    IMPORTED_LOCATION "${<package>_LIBRARY}"       # ヘッダーのみのライブラリの場合は不要
    INTERFACE_INCLUDE_DIRECTORIES "${syoch_robotics}"
    )
endif()
