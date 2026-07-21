# Bundled third-party static libraries (ZLib, LibPNG).
#
# CAD modules use older PlatformToolsets (v70–v140) that cannot link objects
# compiled with the VS2022 default toolset (UCRT helpers such as
# __libm_sse2_pow_precise / __acrt_iob_func). Runtime modules therefore link
# toolset-matched zlib/png variants via opendcl_ensure_runtime_png().
#
# Target naming (arch, CRT, and toolset are ALWAYS present):
#   opendcl_{zlib|png}_{x86|x64}_{md|mt}_{toolset}
# Examples:
#   opendcl_zlib_x64_md_v143     host toolset, /MD (runtime default on VS2022)
#   opendcl_png_x64_mt_v143      host toolset, /MT (Studio)
#   opendcl_png_x86_md_v100      compiled with v100, /MD
#
# Solution Explorer folders (classic-like):
#   Library/ZLib
#   Library/LibPNG

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

# Absolute paths so toolset variants created from other directories still compile.
set(OPENDCL_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "Library/ source root")
set(_zlib_abs)
foreach(_s IN LISTS _ZLIB_SOURCES)
  list(APPEND _zlib_abs "${CMAKE_CURRENT_SOURCE_DIR}/${_s}")
endforeach()
set(_png_abs)
foreach(_s IN LISTS _PNG_SOURCES)
  list(APPEND _png_abs "${CMAKE_CURRENT_SOURCE_DIR}/${_s}")
endforeach()
set(OPENDCL_ZLIB_SOURCES "${_zlib_abs}" CACHE INTERNAL "ZLib sources for toolset variants")
set(OPENDCL_PNG_SOURCES "${_png_abs}" CACHE INTERNAL "LibPNG sources for toolset variants")

# Host platform toolset tag used in default target names (e.g. v143).
if(CMAKE_VS_PLATFORM_TOOLSET)
  set(_opendcl_host_toolset "${CMAKE_VS_PLATFORM_TOOLSET}")
elseif(DEFINED MSVC_TOOLSET_VERSION AND MSVC_TOOLSET_VERSION)
  set(_opendcl_host_toolset "v${MSVC_TOOLSET_VERSION}")
else()
  set(_opendcl_host_toolset "v143")
endif()
string(REGEX REPLACE "[^A-Za-z0-9]" "_" OPENDCL_HOST_TOOLSET_TAG "${_opendcl_host_toolset}")
set(OPENDCL_HOST_TOOLSET_TAG "${OPENDCL_HOST_TOOLSET_TAG}" CACHE INTERNAL
  "Sanitized host VS toolset tag for Library target names (e.g. v143)")

# name: opendcl_{zlib|png}_{x86|x64}_{md|mt}_{toolset}
# runtime_lib: "md" | "mt"
# toolset_tag (optional 5th arg): out leaf under Library/ts/<tag>/…
function(opendcl_add_static_lib name arch sources runtime_lib)
  set(_toolset_tag "")
  if(ARGC GREATER_EQUAL 5)
    set(_toolset_tag "${ARGV4}")
  endif()
  add_library(${name} STATIC ${sources})
  target_include_directories(${name} PUBLIC
    "${OPENDCL_LIBRARY_DIR}/ZLib"
    "${OPENDCL_LIBRARY_DIR}/LibPNG"
  )
  if(_toolset_tag)
    set(_out_base "${OPENDCL_OUTPUT_ROOT}/Library/ts/${_toolset_tag}/${arch}-${runtime_lib}")
  else()
    set(_out_base "${OPENDCL_OUTPUT_ROOT}/Library/${arch}-${runtime_lib}")
  endif()
  if(runtime_lib STREQUAL "mt")
    set(_rtl "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:Debug>")
    opendcl_map_fulldebug_to_debug(${name} "${_out_base}")
  else()
    # /MD modules use FullDebug == Debug CRT by default; map FullDebug zlib/png
    # beside Debug so ARX can build FullDebug without /MDd host debug libs.
    # fulldebug.<family>.props may switch a module to /MDd — then rebuild zlib
    # FullDebug with /MDd or keep hybrid /MD (classic ARX-style).
    set(_rtl "MultiThreadedDLL")
    opendcl_map_fulldebug_to_debug(${name} "${_out_base}")
  endif()
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_lib_arch "x64")
  else()
    set(_lib_arch "Win32")
  endif()
  if(name MATCHES "zlib")
    opendcl_solution_folder(_folder "${_lib_arch}" "opendcl_zlib")
  else()
    opendcl_solution_folder(_folder "${_lib_arch}" "opendcl_png")
  endif()
  set_target_properties(${name} PROPERTIES
    MSVC_RUNTIME_LIBRARY "${_rtl}"
    FOLDER "${_folder}"
    # Global CMAKE_MFC_FLAG=2 would otherwise make static libs "MFC projects".
    # MultiByte + MFC ⇒ MSB8031 on older toolsets (v120+). Keep Unicode / no MFC.
    VS_GLOBAL_UseOfMfc "false"
    VS_GLOBAL_CharacterSet "Unicode"
  )
  target_compile_definitions(${name} PRIVATE UNICODE _UNICODE)
  # /FS when toolset is VS2013+ (host defaults are modern). Applied again below
  # for explicit toolset-matched libs after VS_PLATFORM_TOOLSET is set.
  if(MSVC)
    if(NOT _toolset_tag OR NOT _toolset_tag MATCHES "^v(70|80|90|100|110)")
      target_compile_options(${name} PRIVATE "/FS")
    endif()
  endif()
  if(name MATCHES "png")
    set_target_properties(${name} PROPERTIES LINKER_LANGUAGE C)
  endif()
endfunction()

function(opendcl_png_toolset_is_modern toolset out_var)
  if(NOT toolset OR toolset STREQUAL "")
    set(${out_var} TRUE PARENT_SCOPE)
    return()
  endif()
  if(toolset MATCHES "^v14[1-9]" OR toolset MATCHES "^v1[5-9][0-9]")
    set(${out_var} TRUE PARENT_SCOPE)
  else()
    set(${out_var} FALSE PARENT_SCOPE)
  endif()
endfunction()

# Map a CAD module toolset → toolset used to *compile* zlib/png.
# Prefer exact match for legacy rows so CRT helpers (__iob_func vs UCRT) align.
# Modern rows (v141+) share the host-built default pair.
function(opendcl_png_lib_toolset toolset out_var)
  if(NOT toolset OR toolset STREQUAL "")
    set(${out_var} "${OPENDCL_HOST_TOOLSET_TAG}" PARENT_SCOPE)
    return()
  endif()
  opendcl_png_toolset_is_modern("${toolset}" _modern)
  if(_modern)
    set(${out_var} "${OPENDCL_HOST_TOOLSET_TAG}" PARENT_SCOPE)
  else()
    set(${out_var} "${toolset}" PARENT_SCOPE)
  endif()
endfunction()

# Ensure /MD zlib+png exist for (arch, module_toolset). Always full names with toolset.
function(opendcl_ensure_runtime_png arch toolset out_zlib out_png)
  if(NOT arch STREQUAL "x64" AND NOT arch STREQUAL "x86")
    message(FATAL_ERROR "opendcl_ensure_runtime_png: arch must be x64 or x86 (got '${arch}')")
  endif()

  opendcl_png_lib_toolset("${toolset}" _lib_ts)
  string(REGEX REPLACE "[^A-Za-z0-9]" "_" _ts_safe "${_lib_ts}")
  set(_z "opendcl_zlib_${arch}_md_${_ts_safe}")
  set(_p "opendcl_png_${arch}_md_${_ts_safe}")

  # Default host pair is created at the bottom of this file; only create extras here.
  if(NOT TARGET ${_z})
    opendcl_add_static_lib(${_z} ${arch} "${OPENDCL_ZLIB_SOURCES}" md "${_ts_safe}")
    set_property(TARGET ${_z} PROPERTY VS_PLATFORM_TOOLSET "${_lib_ts}")
    if(_lib_ts MATCHES "^v(70|80|90|100|110)")
      set_target_properties(${_z} PROPERTIES
        CXX_STANDARD 98
        CXX_STANDARD_REQUIRED OFF
        CXX_EXTENSIONS OFF
      )
    elseif(MSVC)
      # v120+: parallel cl needs /FS for shared vc*.pdb (C1041).
      target_compile_options(${_z} PRIVATE "/FS")
    endif()
    # Old toolsets under VS2022: pin kit + mt.exe path (same as runtime modules).
    if(_lib_ts MATCHES "^v(70|80|90|100|110|120|140)")
      set(_lib_winsdk "10.0.19041.0")
      if(DEFINED OPENDCL_LEGACY_WINDOWS_SDK AND OPENDCL_LEGACY_WINDOWS_SDK)
        set(_lib_winsdk "${OPENDCL_LEGACY_WINDOWS_SDK}")
      endif()
      opendcl_vs_pin_windows_sdk(${_z} "${_lib_winsdk}")
    endif()
    message(STATUS "Library: ${_z} (toolset ${_lib_ts}, /MD)")
  endif()

  if(NOT TARGET ${_p})
    opendcl_add_static_lib(${_p} ${arch} "${OPENDCL_PNG_SOURCES}" md "${_ts_safe}")
    set_property(TARGET ${_p} PROPERTY VS_PLATFORM_TOOLSET "${_lib_ts}")
    if(_lib_ts MATCHES "^v(70|80|90|100|110)")
      # no /FS on pre-v120
    elseif(MSVC)
      target_compile_options(${_p} PRIVATE "/FS")
    endif()
    if(_lib_ts MATCHES "^v(70|80|90|100|110|120|140)")
      set(_lib_winsdk "10.0.19041.0")
      if(DEFINED OPENDCL_LEGACY_WINDOWS_SDK AND OPENDCL_LEGACY_WINDOWS_SDK)
        set(_lib_winsdk "${OPENDCL_LEGACY_WINDOWS_SDK}")
      endif()
      opendcl_vs_pin_windows_sdk(${_p} "${_lib_winsdk}")
    endif()
    target_link_libraries(${_p} PUBLIC ${_z})
    message(STATUS "Library: ${_p} (toolset ${_lib_ts}, /MD)")
  endif()

  set(${out_zlib} "${_z}" PARENT_SCOPE)
  set(${out_png} "${_p}" PARENT_SCOPE)
endfunction()

# Host-arch only (locked): one configure → one arch of Library targets.
# Nested Win32 must not emit *_x64_* libs into shared OPENDCL_OUTPUT_ROOT.
set(_hts "${OPENDCL_HOST_TOOLSET_TAG}")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(_ha "x64")
else()
  set(_ha "x86")
endif()

opendcl_add_static_lib(opendcl_zlib_${_ha}_md_${_hts} ${_ha} "${_ZLIB_SOURCES}" md)
opendcl_add_static_lib(opendcl_png_${_ha}_md_${_hts} ${_ha} "${_PNG_SOURCES}" md)
target_link_libraries(opendcl_png_${_ha}_md_${_hts} PUBLIC opendcl_zlib_${_ha}_md_${_hts})

opendcl_add_static_lib(opendcl_zlib_${_ha}_mt_${_hts} ${_ha} "${_ZLIB_SOURCES}" mt)
opendcl_add_static_lib(opendcl_png_${_ha}_mt_${_hts} ${_ha} "${_PNG_SOURCES}" mt)
target_link_libraries(opendcl_png_${_ha}_mt_${_hts} PUBLIC opendcl_zlib_${_ha}_mt_${_hts})

# Convenience names for Studio / docs (always fully qualified).
set(OPENDCL_ZLIB_MD_TARGET "opendcl_zlib_${_ha}_md_${_hts}" CACHE INTERNAL "")
set(OPENDCL_PNG_MD_TARGET  "opendcl_png_${_ha}_md_${_hts}"  CACHE INTERNAL "")
set(OPENDCL_ZLIB_MT_TARGET "opendcl_zlib_${_ha}_mt_${_hts}" CACHE INTERNAL "")
set(OPENDCL_PNG_MT_TARGET  "opendcl_png_${_ha}_mt_${_hts}"  CACHE INTERNAL "")

message(STATUS
  "Library: host ${_ha} toolset ${_hts}  "
  "md=${OPENDCL_PNG_MD_TARGET}  mt=${OPENDCL_PNG_MT_TARGET}")
