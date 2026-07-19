cmake_minimum_required(VERSION 3.24)

project(OpenDCL
  # Must match Runtime/ARX.rc (and Studio/RxInstall) FILEVERSION / PRODUCTVERSION.
  VERSION 10.1.1.1
  DESCRIPTION "OpenDCL Studio and Runtime (CMake experiment)"
  LANGUAGES CXX C RC
)

if(NOT WIN32)
  message(FATAL_ERROR "OpenDCL builds only on Windows (MFC / CAD host modules).")
endif()

# Multi-config friendly configuration list (Visual Studio / Xcode / multi-config Ninja)
get_property(_is_multi GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_is_multi)
  set(CMAKE_CONFIGURATION_TYPES "Debug;FullDebug;Release" CACHE STRING "" FORCE)
  # FullDebug is a custom config: seed flags from Debug so MSVC linker vars exist.
  foreach(_lang IN ITEMS C CXX RC Shared Module EXE Static)
    foreach(_suffix IN ITEMS "" _INIT)
      set(_dbg "CMAKE_${_lang}_FLAGS_DEBUG${_suffix}")
      set(_fd  "CMAKE_${_lang}_FLAGS_FULLDEBUG${_suffix}")
      if(DEFINED ${_dbg} AND NOT DEFINED ${_fd})
        set(${_fd} "${${_dbg}}" CACHE STRING "Flags used by the ${_lang} compiler for FullDebug builds." FORCE)
      endif()
    endforeach()
  endforeach()
  foreach(_kind IN ITEMS EXE SHARED MODULE STATIC)
    foreach(_suffix IN ITEMS "" _INIT)
      set(_dbg "CMAKE_${_kind}_LINKER_FLAGS_DEBUG${_suffix}")
      set(_fd  "CMAKE_${_kind}_LINKER_FLAGS_FULLDEBUG${_suffix}")
      if(DEFINED ${_dbg})
        set(${_fd} "${${_dbg}}" CACHE STRING "Flags used by the linker for FullDebug ${_kind} builds." FORCE)
      endif()
    endforeach()
  endforeach()
else()
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  endif()
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug FullDebug Release)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(OpenDCLHelpers)
include(OpenDCLRuntimeMatrix)
include(OpenDCLRuntimeTargets)

# MFC is required for Runtime (dynamic) and Studio (static, later).
set(CMAKE_MFC_FLAG 2) # 2 = shared/dynamic MFC

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "OpenDCL CMake experiment — push disabled on this clone")
message(STATUS "Output root: ${OPENDCL_OUTPUT_ROOT}")

add_subdirectory(Library)

if(OPENDCL_BUILD_RUNTIME)
  add_subdirectory(Runtime/Localized)
  opendcl_add_selected_runtimes()
endif()

if(OPENDCL_BUILD_RXINSTALL)
  add_subdirectory(Runtime/RxInstall)
endif()

if(OPENDCL_BUILD_STUDIO)
  add_subdirectory(Studio)
endif()

# ENU CHM is built with Studio (OpenDCL_StudioHelp_ENU). OPENDCL_BUILD_HELP is
# reserved for multi-language help packs later.
if(OPENDCL_BUILD_HELP AND NOT OPENDCL_BUILD_STUDIO)
  message(STATUS "OPENDCL_BUILD_HELP without Studio: ENU CHM is wired under Studio; enable OPENDCL_BUILD_STUDIO.")
endif()

# ---------------------------------------------------------------------------
# Nested Win32 (x86) tree under x64 VS configure — one solution builds both
# ---------------------------------------------------------------------------
# CMake's Visual Studio generator cannot emit Debug|x64 and Debug|Win32 for the
# same native targets in one configure. Nest a second binary dir (-A Win32) that
# shares OPENDCL_OUTPUT_ROOT, and drive it from OpenDCL_Win32 in this .sln.
if(OPENDCL_NEST_WIN32
    AND MSVC
    AND CMAKE_SIZEOF_VOID_P EQUAL 8
    AND CMAKE_GENERATOR MATCHES "Visual Studio")
  set(_opendcl_win32_bin "${CMAKE_BINARY_DIR}/win32")
  set(_opendcl_win32_stamp "${_opendcl_win32_bin}/.opendcl_win32_configured")

  # Forward the same product selection the parent used (langs, families, auto).
  set(_opendcl_win32_cache
    "-DOPENDCL_NEST_WIN32=OFF" # prevent recursion
    "-DOPENDCL_OUTPUT_ROOT=${OPENDCL_OUTPUT_ROOT}"
    "-DOPENDCL_BUILD_RUNTIME=${OPENDCL_BUILD_RUNTIME}"
    "-DOPENDCL_BUILD_STUDIO=${OPENDCL_BUILD_STUDIO}"
    "-DOPENDCL_BUILD_RXINSTALL=${OPENDCL_BUILD_RXINSTALL}"
    "-DOPENDCL_ENABLE_ARX=${OPENDCL_ENABLE_ARX}"
    "-DOPENDCL_ENABLE_BRX=${OPENDCL_ENABLE_BRX}"
    "-DOPENDCL_ENABLE_GRX=${OPENDCL_ENABLE_GRX}"
    "-DOPENDCL_ENABLE_ZRX=${OPENDCL_ENABLE_ZRX}"
    "-DOPENDCL_RUNTIME_AUTO=${OPENDCL_RUNTIME_AUTO}"
    "-DOPENDCL_RUNTIME_REQUIRE_SELECTED=${OPENDCL_RUNTIME_REQUIRE_SELECTED}"
    "-DOPENDCL_RUNTIME_TARGETS=${OPENDCL_RUNTIME_TARGETS}"
    "-DOPENDCL_LANGS=${OPENDCL_LANGS}"
  )

  add_custom_command(
    OUTPUT "${_opendcl_win32_stamp}"
    COMMAND ${CMAKE_COMMAND}
      -S "${CMAKE_SOURCE_DIR}"
      -B "${_opendcl_win32_bin}"
      -G "${CMAKE_GENERATOR}"
      -A Win32
      ${_opendcl_win32_cache}
    COMMAND ${CMAKE_COMMAND} -E touch "${_opendcl_win32_stamp}"
    COMMENT "Configure nested Win32 OpenDCL under ${_opendcl_win32_bin}"
    VERBATIM
  )

  if(OPENDCL_WIN32_IN_ALL)
    add_custom_target(OpenDCL_Win32 ALL
      DEPENDS "${_opendcl_win32_stamp}"
      COMMAND ${CMAKE_COMMAND}
        --build "${_opendcl_win32_bin}"
        --config "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>"
      COMMENT "Build nested Win32 OpenDCL ($<CONFIG> → Debug if FullDebug)"
      VERBATIM
    )
  else()
    add_custom_target(OpenDCL_Win32
      DEPENDS "${_opendcl_win32_stamp}"
      COMMAND ${CMAKE_COMMAND}
        --build "${_opendcl_win32_bin}"
        --config "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>"
      COMMENT "Build nested Win32 OpenDCL ($<CONFIG> → Debug if FullDebug)"
      VERBATIM
    )
  endif()
  set_property(TARGET OpenDCL_Win32 PROPERTY FOLDER "Win32")
  message(STATUS "Win32 nest: OpenDCL_Win32 target → ${_opendcl_win32_bin} (shared out: ${OPENDCL_OUTPUT_ROOT})")
elseif(OPENDCL_NEST_WIN32 AND NOT (CMAKE_SIZEOF_VOID_P EQUAL 8 AND CMAKE_GENERATOR MATCHES "Visual Studio"))
  message(STATUS "OPENDCL_NEST_WIN32 ignored (need x64 Visual Studio generator; this is ${CMAKE_GENERATOR} ptr=${CMAKE_SIZEOF_VOID_P})")
endif()
