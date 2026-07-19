cmake_minimum_required(VERSION 3.24)

# Visual Studio .sln base name should distinguish presets in the IDE.
# Default: derive from binaryDir — build/<preset> → <preset>.sln;
# nested build/<preset>/win32 → <preset>-win32.sln.
# Override: -DOPENDCL_SOLUTION_NAME=MyLabel  → MyLabel.sln
set(OPENDCL_SOLUTION_NAME "" CACHE STRING
  "Visual Studio solution base name (empty = derive from binary dir / preset)")
if(OPENDCL_SOLUTION_NAME)
  set(_opendcl_project_name "${OPENDCL_SOLUTION_NAME}")
else()
  get_filename_component(_opendcl_bin_leaf "${CMAKE_BINARY_DIR}" NAME)
  get_filename_component(_opendcl_bin_parent "${CMAKE_BINARY_DIR}" DIRECTORY)
  get_filename_component(_opendcl_parent_leaf "${_opendcl_bin_parent}" NAME)
  get_filename_component(_opendcl_grandparent "${_opendcl_bin_parent}" DIRECTORY)
  get_filename_component(_opendcl_grandparent_leaf "${_opendcl_grandparent}" NAME)
  if(_opendcl_parent_leaf STREQUAL "build")
    # .../build/vs2022-x64-dev → vs2022-x64-dev.sln
    set(_opendcl_project_name "${_opendcl_bin_leaf}")
  elseif(_opendcl_grandparent_leaf STREQUAL "build")
    # .../build/vs2022-full/win32 → vs2022-full-win32.sln
    set(_opendcl_project_name "${_opendcl_parent_leaf}-${_opendcl_bin_leaf}")
  else()
    set(_opendcl_project_name "OpenDCL")
  endif()
endif()

project(${_opendcl_project_name}
  # Must match Runtime/ARX.rc (and Studio/RxInstall) FILEVERSION / PRODUCTVERSION.
  VERSION 10.1.1.1
  DESCRIPTION "OpenDCL Studio and Runtime (CMake experiment)"
  LANGUAGES CXX C RC
)
message(STATUS "Solution / project name: ${PROJECT_NAME} -> ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.sln")
# Drop stale default OpenDCL.sln after a rename so the IDE folder isn't ambiguous.
if(NOT PROJECT_NAME STREQUAL "OpenDCL")
  set(_opendcl_stale_sln "${CMAKE_BINARY_DIR}/OpenDCL.sln")
  if(EXISTS "${_opendcl_stale_sln}")
    file(REMOVE "${_opendcl_stale_sln}")
    message(STATUS "Removed stale OpenDCL.sln (renamed to ${PROJECT_NAME}.sln)")
  endif()
endif()

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

message(STATUS "OpenDCL CMake experiment - push disabled on this clone")
message(STATUS "Output root: ${OPENDCL_OUTPUT_ROOT}")

add_subdirectory(Library)

# Runtime.Res / Studio.Res: OPENDCL_RES_PE=classic_x86 nests Win32 Res on x64;
# host builds Res natively in this tree. Nest first so F5 paths exist when needed.
include(OpenDCLResWin32)
opendcl_add_res_win32_nest()

if(OPENDCL_BUILD_RUNTIME OR OPENDCL_BUILD_RES_DLLS)
  add_subdirectory(Runtime/Localized)
endif()
if(OPENDCL_BUILD_RUNTIME)
  opendcl_add_selected_runtimes()
endif()

if(OPENDCL_BUILD_RXINSTALL)
  add_subdirectory(Runtime/RxInstall)
endif()

if(OPENDCL_BUILD_STUDIO OR OPENDCL_BUILD_RES_DLLS)
  add_subdirectory(Studio)
endif()

# ENU CHM is built with Studio (OpenDCL_StudioHelp_ENU). OPENDCL_BUILD_HELP is
# reserved for multi-language help packs later.
if(OPENDCL_BUILD_HELP AND NOT OPENDCL_BUILD_STUDIO)
  message(STATUS "OPENDCL_BUILD_HELP without Studio: ENU CHM is wired under Studio; enable OPENDCL_BUILD_STUDIO.")
endif()

# ---------------------------------------------------------------------------
# Nested Win32 (x86) under x64 VS — projects live in the parent .sln
# ---------------------------------------------------------------------------
# Second binary dir (-A Win32), shared OPENDCL_OUTPUT_ROOT. Nest is configured
# at parent generate time; each nest .vcxproj is imported with
# include_external_msproject(PLATFORM Win32) for Solution Explorer (folders
# under Win32/…). Imported projects are EXCLUDE_FROM_DEFAULT_BUILD because
# parent Platform=x64 breaks nest MSBuild (MSB8013). Build via OpenDCL_Win32,
# OpenDCL_RxInstall, or OpenDCL_Res_Win32 → cmake --build <bin>/win32.
include(OpenDCLNestWin32)
opendcl_add_win32_nest()
