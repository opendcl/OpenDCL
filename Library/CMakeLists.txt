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

function(opendcl_add_static_lib name arch sources)
  add_library(${name} STATIC ${sources})
  target_include_directories(${name} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/ZLib"
    "${CMAKE_CURRENT_SOURCE_DIR}/LibPNG"
  )
  # Debug/Release: /MD. FullDebug: /MDd so we can link into BRX FullDebug runtimes.
  set_target_properties(${name} PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:FullDebug>:Debug>DLL"
    ARCHIVE_OUTPUT_DIRECTORY "${OPENDCL_OUTPUT_ROOT}/Library/${arch}/$<CONFIG>"
  )
  if(arch STREQUAL "x86")
    # Ensure 32-bit when using multi-config VS Win32 platform.
  endif()
  # ZLib sources are .cpp in this tree; LibPNG is C.
  if(name MATCHES "png")
    set_target_properties(${name} PROPERTIES LINKER_LANGUAGE C)
  endif()
endfunction()

# Build both arches only when the generator platform may need them.
# For x64-only configures we still define both targets so matrix x86 hosts
# can be enabled later without restructure; x86 objects need Win32 platform.
opendcl_add_static_lib(opendcl_zlib_x64 x64 "${_ZLIB_SOURCES}")
opendcl_add_static_lib(opendcl_png_x64 x64 "${_PNG_SOURCES}")
target_link_libraries(opendcl_png_x64 PUBLIC opendcl_zlib_x64)

# Win32 variants (used when a selected runtime has ARCH x86)
opendcl_add_static_lib(opendcl_zlib_x86 x86 "${_ZLIB_SOURCES}")
opendcl_add_static_lib(opendcl_png_x86 x86 "${_PNG_SOURCES}")
target_link_libraries(opendcl_png_x86 PUBLIC opendcl_zlib_x86)
