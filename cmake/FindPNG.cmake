if(NOT PREFER_BUNDLED_LIBS)
  if(NOT CMAKE_CROSSCOMPILING)
    find_package(PkgConfig QUIET)
    pkg_check_modules(PC_PNG PNG)
  endif()

  find_library(PNG_LIBRARY
    NAMES png16
    HINTS ${HINTS_PNG_LIBDIR} ${PC_PNG_LIBDIR} ${PC_PNG_LIBRARY_DIRS}
    ${CROSSCOMPILING_NO_CMAKE_SYSTEM_PATH}
  )
  find_path(PNG_INCLUDEDIR
    NAMES png.h
    HINTS ${HINTS_PNG_INCLUDEDIR} ${PC_PNG_INCLUDEDIR} ${PC_PNG_INCLUDE_DIRS}
    ${CROSSCOMPILING_NO_CMAKE_SYSTEM_PATH}
  )

  mark_as_advanced(PNG_LIBRARY PNG_INCLUDEDIR)

  if(PNG_LIBRARY AND PNG_INCLUDEDIR)
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(PNG DEFAULT_MSG PNG_LIBRARY PNG_INCLUDEDIR)

    set(PNG_LIBRARIES ${PNG_LIBRARY})
    set(PNG_INCLUDE_DIRS ${PNG_INCLUDEDIR})
    set(PNG_BUNDLED OFF)
    set(PNG_DEP)
  endif()
endif()

if(NOT PNG_FOUND)
  set(PNG_SRC_DIR src/engine/external/png)
  set_src(PNG_SRC GLOB ${PNG_SRC_DIR}
    png.c
    png.h
    pngconf.h
    pngdebug.h
    pngerror.c
    pngget.c
    pnginfo.h
    pnglibconf.h
    pngmem.c
    pngpread.c
    pngpriv.h
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngstruct.h
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
  )
  add_library(png EXCLUDE_FROM_ALL OBJECT ${PNG_SRC})
  target_include_directories(png PRIVATE ${ZLIB_INCLUDE_DIRS})
  set(PNG_DEP $<TARGET_OBJECTS:png>)
  set(PNG_INCLUDEDIR ${PNG_SRC_DIR})
  set(PNG_INCLUDE_DIRS ${PNG_INCLUDEDIR})

  list(APPEND TARGETS_DEP png)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(PNG DEFAULT_MSG PNG_INCLUDEDIR)
  set(PNG_BUNDLED ON)
endif()
