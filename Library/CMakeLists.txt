# Bundled third-party static libraries (ZLib, LibPNG).

set(_ZLIB_SOURCES
  ZLib/adler32.cpp
  ZLib/compress.cpp
  ZLib/crc32.cpp
  ZLib/deflate.cpp
  ZLib/gzio.cpp
  ZLib/infback.cpp
  ZLib/inffast.cpp
  ZLib/inflate.cpp
  ZLib/inftrees.cpp
  ZLib/trees.cpp
  ZLib/uncompr.cpp
  ZLib/zutil.cpp
)

set(_PNG_SOURCES
  LibPNG/png.c
  LibPNG/pngerror.c
  LibPNG/pngget.c
  LibPNG/pngmem.c
  LibPNG/pngpread.c
  LibPNG/pngread.c
  LibPNG/pngrio.c
  LibPNG/pngrtran.c
  LibPNG/pngrutil.c
  LibPNG/pngset.c
  LibPNG/pngtrans.c
  LibPNG/pngwio.c
  LibPNG/pngwrite.c
  LibPNG/pngwtran.c
  LibPNG/pngwutil.c
)

# runtime_lib: "md"  → /MD  (CAD runtime modules; keep FullDebug for /MDd)
#              "mt"  → /MT  (Studio; FullDebug → Debug)
function(opendcl_add_static_lib name arch sources runtime_lib)
  add_library(${name} STATIC ${sources})
  target_include_directories(${name} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/ZLib"
    "${CMAKE_CURRENT_SOURCE_DIR}/LibPNG"
  )
  set(_out_base "${OPENDCL_OUTPUT_ROOT}/Library/${arch}-${runtime_lib}")
  if(runtime_lib STREQUAL "mt")
    # Studio-only: no FullDebug product.
    set(_rtl "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:Debug>")
    opendcl_map_fulldebug_to_debug(${name} "${_out_base}")
  else()
    # Linked into CAD runtime modules — keep FullDebug (/MDd) distinct.
    set(_rtl "MultiThreaded$<$<CONFIG:FullDebug>:Debug>DLL")
    set_target_properties(${name} PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${_out_base}/Debug"
      ARCHIVE_OUTPUT_DIRECTORY_FULLDEBUG "${_out_base}/FullDebug"
      ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${_out_base}/Release"
    )
  endif()
  set_target_properties(${name} PROPERTIES
    MSVC_RUNTIME_LIBRARY "${_rtl}"
  )
  # ZLib sources are .cpp in this tree; LibPNG is C.
  if(name MATCHES "png")
    set_target_properties(${name} PROPERTIES LINKER_LANGUAGE C)
  endif()
endfunction()

# /MD pair — CAD runtime modules (FullDebug kept)
opendcl_add_static_lib(opendcl_zlib_x64 x64 "${_ZLIB_SOURCES}" md)
opendcl_add_static_lib(opendcl_png_x64 x64 "${_PNG_SOURCES}" md)
target_link_libraries(opendcl_png_x64 PUBLIC opendcl_zlib_x64)

opendcl_add_static_lib(opendcl_zlib_x86 x86 "${_ZLIB_SOURCES}" md)
opendcl_add_static_lib(opendcl_png_x86 x86 "${_PNG_SOURCES}" md)
target_link_libraries(opendcl_png_x86 PUBLIC opendcl_zlib_x86)

# /MT pair — Studio (FullDebug → Debug)
opendcl_add_static_lib(opendcl_zlib_x64_mt x64 "${_ZLIB_SOURCES}" mt)
opendcl_add_static_lib(opendcl_png_x64_mt x64 "${_PNG_SOURCES}" mt)
target_link_libraries(opendcl_png_x64_mt PUBLIC opendcl_zlib_x64_mt)

opendcl_add_static_lib(opendcl_zlib_x86_mt x86 "${_ZLIB_SOURCES}" mt)
opendcl_add_static_lib(opendcl_png_x86_mt x86 "${_PNG_SOURCES}" mt)
target_link_libraries(opendcl_png_x86_mt PUBLIC opendcl_zlib_x86_mt)
