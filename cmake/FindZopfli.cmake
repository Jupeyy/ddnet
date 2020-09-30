if(NOT PREFER_BUNDLED_LIBS)
  set(CMAKE_MODULE_PATH ${ORIGINAL_CMAKE_MODULE_PATH})
  find_package(Zopfli)
  set(CMAKE_MODULE_PATH ${OWN_CMAKE_MODULE_PATH})
  if(ZOPFLI_FOUND)
    set(ZOPFLI_BUNDLED OFF)
    set(ZOPFLI_DEP)
  endif()
endif()

if(NOT ZOPFLI_FOUND)
  set(ZOPFLI_BUNDLED ON)
  set(ZOPFLI_SRC_DIR src/engine/external/zopfli)
  set_src(ZOPFLI_SRC GLOB ${ZOPFLI_SRC_DIR}
	blocksplitter.c
	blocksplitter.h
	cache.c
	cache.h
	deflate.c
	deflate.h
	gzip_container.c
	gzip_container.h
	hash.c
	hash.h
	katajainen.c
	katajainen.h
	lz77.c
	lz77.h
	squeeze.c
	squeeze.h
	symbols.h
	tree.c
	tree.h
	util.c
	util.h
	zlib_container.c
	zlib_container.h
	zopfli.h
	zopfli_lib.c
  )
  add_library(zopfli EXCLUDE_FROM_ALL OBJECT ${ZOPFLI_SRC})
  set(ZOPFLI_INCLUDEDIR ${ZOPFLI_SRC_DIR})
  target_include_directories(zopfli PRIVATE ${ZOPFLI_INCLUDEDIR})

  set(ZOPFLI_DEP $<TARGET_OBJECTS:zopfli>)
  set(ZOPFLI_INCLUDE_DIRS ${ZOPFLI_INCLUDEDIR})
  set(ZOPFLI_LIBRARIES)

  list(APPEND TARGETS_DEP zopfli)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(Zopfli DEFAULT_MSG ZOPFLI_INCLUDEDIR)
endif()
