# Bundled third-party static libraries (ZLib, LibPNG).
#
# CAD modules use older PlatformToolsets (v70–v140) that cannot link objects
# compiled with the VS2022 default toolset (UCRT helpers such as
# __libm_sse2_pow_precise / __acrt_iob_func). Runtime modules therefore link
# toolset-matched zlib/png variants via opendcl_ensure_runtime_png().
#
# Target naming (arch, CRT, and toolset are ALWAYS present):
#   opendcl_{zlib|png}_{x86|x64}_{md|mdd|mt}_{toolset}
# Examples:
#   opendcl_zlib_x64_md_v143     host toolset, /MD  (Debug + fake FullDebug + Release)
#   opendcl_zlib_x64_mdd_v143    host toolset, /MDd (real host-debug FullDebug)
#   opendcl_png_x64_mt_v143      host toolset, /MT  (Studio)
#   opendcl_png_x86_md_v100      compiled with v100, /MD
#
# CRT pairs for CAD modules under solution FullDebug:
#   *_md_*   — always /MD; FullDebug outputs map beside Debug ("fake" FullDebug)
#   *_mdd_*  — /MDd for Debug + FullDebug; separate FullDebug outs ("real" FullDebug)
# Modules with fulldebug.<family>.props link mdd; everyone else links md.
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

# name: opendcl_{zlib|png}_{x86|x64}_{md|mdd|mt}_{toolset}
# runtime_lib: "md" | "mdd" | "mt"
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
    # Studio /MT: FullDebug maps beside Debug (no separate FullDebug product).
    set(_rtl "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:Debug>")
    opendcl_map_fulldebug_to_debug(${name} "${_out_base}")
  elseif(runtime_lib STREQUAL "mdd")
    # Real host-debug FullDebug: /MDd for Debug + FullDebug; keep FullDebug outs.
    set(_rtl "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:Debug>DLL")
    set_target_properties(${name} PROPERTIES
      ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${_out_base}/Debug"
      ARCHIVE_OUTPUT_DIRECTORY_FULLDEBUG "${_out_base}/FullDebug"
      ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${_out_base}/Release"
    )
  else()
    # /MD for all configs (Debug, fake FullDebug, Release). FullDebug → Debug outs.
    if(NOT runtime_lib STREQUAL "md")
      message(FATAL_ERROR "opendcl_add_static_lib: runtime_lib must be md, mdd, or mt (got '${runtime_lib}')")
    endif()
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

# Apply toolset / SDK pin to a zlib or png static lib created on demand.
function(opendcl_configure_runtime_static_lib tgt lib_ts crt_label)
  set_property(TARGET ${tgt} PROPERTY VS_PLATFORM_TOOLSET "${lib_ts}")
  if(lib_ts MATCHES "^v(70|80|90|100|110)")
    set_target_properties(${tgt} PROPERTIES
      CXX_STANDARD 98
      CXX_STANDARD_REQUIRED OFF
      CXX_EXTENSIONS OFF
    )
  elseif(MSVC)
    # v120+: parallel cl needs /FS for shared vc*.pdb (C1041).
    target_compile_options(${tgt} PRIVATE "/FS")
  endif()
  # Old toolsets under VS2022: pin kit + mt.exe path (same as runtime modules).
  if(lib_ts MATCHES "^v(70|80|90|100|110|120|140)")
    set(_lib_winsdk "10.0.19041.0")
    if(DEFINED OPENDCL_LEGACY_WINDOWS_SDK AND OPENDCL_LEGACY_WINDOWS_SDK)
      set(_lib_winsdk "${OPENDCL_LEGACY_WINDOWS_SDK}")
    endif()
    opendcl_vs_pin_windows_sdk(${tgt} "${_lib_winsdk}")
  endif()
  message(STATUS "Library: ${tgt} (toolset ${lib_ts}, ${crt_label})")
endfunction()

# Ensure zlib+png exist for (arch, module_toolset, CRT).
# runtime_crt: "md" (fake FullDebug / always /MD) or "mdd" (real FullDebug /MDd)
function(opendcl_ensure_runtime_png arch toolset runtime_crt out_zlib out_png)
  if(NOT arch STREQUAL "x64" AND NOT arch STREQUAL "x86")
    message(FATAL_ERROR "opendcl_ensure_runtime_png: arch must be x64 or x86 (got '${arch}')")
  endif()
  if(NOT runtime_crt STREQUAL "md" AND NOT runtime_crt STREQUAL "mdd")
    message(FATAL_ERROR "opendcl_ensure_runtime_png: runtime_crt must be md or mdd (got '${runtime_crt}')")
  endif()

  opendcl_png_lib_toolset("${toolset}" _lib_ts)
  string(REGEX REPLACE "[^A-Za-z0-9]" "_" _ts_safe "${_lib_ts}")
  set(_z "opendcl_zlib_${arch}_${runtime_crt}_${_ts_safe}")
  set(_p "opendcl_png_${arch}_${runtime_crt}_${_ts_safe}")

  if(runtime_crt STREQUAL "mdd")
    set(_crt_label "/MDd")
  else()
    set(_crt_label "/MD")
  endif()

  # Default host pair is created at the bottom of this file; only create extras here.
  if(NOT TARGET ${_z})
    opendcl_add_static_lib(${_z} ${arch} "${OPENDCL_ZLIB_SOURCES}" ${runtime_crt} "${_ts_safe}")
    opendcl_configure_runtime_static_lib(${_z} "${_lib_ts}" "${_crt_label}")
  endif()

  if(NOT TARGET ${_p})
    opendcl_add_static_lib(${_p} ${arch} "${OPENDCL_PNG_SOURCES}" ${runtime_crt} "${_ts_safe}")
    opendcl_configure_runtime_static_lib(${_p} "${_lib_ts}" "${_crt_label}")
    target_link_libraries(${_p} PUBLIC ${_z})
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

# /MD pair — Debug, fake FullDebug (maps to Debug), Release.
opendcl_add_static_lib(opendcl_zlib_${_ha}_md_${_hts} ${_ha} "${_ZLIB_SOURCES}" md)
opendcl_add_static_lib(opendcl_png_${_ha}_md_${_hts} ${_ha} "${_PNG_SOURCES}" md)
target_link_libraries(opendcl_png_${_ha}_md_${_hts} PUBLIC opendcl_zlib_${_ha}_md_${_hts})

# /MDd pair — real host-debug FullDebug (only when FullDebug is a solution config).
set(_opendcl_have_fulldebug FALSE)
if(CMAKE_CONFIGURATION_TYPES)
  if("FullDebug" IN_LIST CMAKE_CONFIGURATION_TYPES)
    set(_opendcl_have_fulldebug TRUE)
  endif()
elseif(CMAKE_BUILD_TYPE STREQUAL "FullDebug")
  set(_opendcl_have_fulldebug TRUE)
endif()
if(_opendcl_have_fulldebug)
  opendcl_add_static_lib(opendcl_zlib_${_ha}_mdd_${_hts} ${_ha} "${_ZLIB_SOURCES}" mdd)
  opendcl_add_static_lib(opendcl_png_${_ha}_mdd_${_hts} ${_ha} "${_PNG_SOURCES}" mdd)
  target_link_libraries(opendcl_png_${_ha}_mdd_${_hts} PUBLIC opendcl_zlib_${_ha}_mdd_${_hts})
  set(OPENDCL_ZLIB_MDD_TARGET "opendcl_zlib_${_ha}_mdd_${_hts}" CACHE INTERNAL "")
  set(OPENDCL_PNG_MDD_TARGET  "opendcl_png_${_ha}_mdd_${_hts}"  CACHE INTERNAL "")
else()
  unset(OPENDCL_ZLIB_MDD_TARGET CACHE)
  unset(OPENDCL_PNG_MDD_TARGET CACHE)
endif()

opendcl_add_static_lib(opendcl_zlib_${_ha}_mt_${_hts} ${_ha} "${_ZLIB_SOURCES}" mt)
opendcl_add_static_lib(opendcl_png_${_ha}_mt_${_hts} ${_ha} "${_PNG_SOURCES}" mt)
target_link_libraries(opendcl_png_${_ha}_mt_${_hts} PUBLIC opendcl_zlib_${_ha}_mt_${_hts})

# Convenience names for Studio / docs (always fully qualified).
set(OPENDCL_ZLIB_MD_TARGET "opendcl_zlib_${_ha}_md_${_hts}" CACHE INTERNAL "")
set(OPENDCL_PNG_MD_TARGET  "opendcl_png_${_ha}_md_${_hts}"  CACHE INTERNAL "")
set(OPENDCL_ZLIB_MT_TARGET "opendcl_zlib_${_ha}_mt_${_hts}" CACHE INTERNAL "")
set(OPENDCL_PNG_MT_TARGET  "opendcl_png_${_ha}_mt_${_hts}"  CACHE INTERNAL "")

if(_opendcl_have_fulldebug)
  message(STATUS
    "Library: host ${_ha} toolset ${_hts}  "
    "md=${OPENDCL_PNG_MD_TARGET}  mdd=${OPENDCL_PNG_MDD_TARGET}  mt=${OPENDCL_PNG_MT_TARGET}")
else()
  message(STATUS
    "Library: host ${_ha} toolset ${_hts}  "
    "md=${OPENDCL_PNG_MD_TARGET}  mt=${OPENDCL_PNG_MT_TARGET}")
endif()
