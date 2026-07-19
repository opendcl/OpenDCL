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
