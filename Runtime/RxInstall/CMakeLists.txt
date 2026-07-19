# RxInstall — WiX / MSI demand-load custom-action DLL.
#
# Must be **Win32 (x86)**: OpenDCL.Runtime.wxs DllEntry uses stdcall decoration
# (_RxInstallMachine@4, …). An x64 build exports undecorated names and will not
# load as a Binary CA for the x86 merge module.
#
# Usage:
#   - Parent OpenDCL tree: add_subdirectory when OPENDCL_BUILD_RXINSTALL=ON
#     (x64 parent schedules a nested Win32 configure/build).
#   - Standalone: cmake -S Runtime/RxInstall -B build/rx -G "Visual Studio 17 2022" -A Win32

# Detect whether this file is the top-level CMakeLists (nested Win32 build).
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  cmake_minimum_required(VERSION 3.24)
  project(OpenDCL_RxInstall LANGUAGES CXX RC)
  if(NOT DEFINED OPENDCL_OUTPUT_ROOT)
    set(OPENDCL_OUTPUT_ROOT "${CMAKE_BINARY_DIR}/out" CACHE PATH
      "Root directory for built binaries")
  endif()
  set(_opendcl_rx_classic_dir "${CMAKE_SOURCE_DIR}")
  set(_opendcl_rx_is_standalone TRUE)
  # Nested/standalone does not include OpenDCLHelpers — provide FullDebug map.
  if(NOT DEFINED OPENDCL_CFG_DIR)
    set(OPENDCL_CFG_DIR "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>")
  endif()
  if(NOT COMMAND opendcl_map_fulldebug_to_debug)
    function(opendcl_map_fulldebug_to_debug target out_base)
      foreach(_kind IN ITEMS RUNTIME LIBRARY ARCHIVE PDB)
        set_target_properties(${target} PROPERTIES
          ${_kind}_OUTPUT_DIRECTORY_DEBUG "${out_base}/Debug"
          ${_kind}_OUTPUT_DIRECTORY_FULLDEBUG "${out_base}/Debug"
          ${_kind}_OUTPUT_DIRECTORY_RELEASE "${out_base}/Release"
        )
      endforeach()
    endfunction()
  endif()
else()
  set(_opendcl_rx_classic_dir "${CMAKE_SOURCE_DIR}/Runtime/RxInstall")
  set(_opendcl_rx_is_standalone FALSE)
endif()

set(_RX_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/RegIO.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/RxInstall.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/RxInstall.rc"
)

function(opendcl_rxinstall_add_native classic_dir)
  add_library(OpenDCL_RxInstall SHARED ${_RX_SOURCES})
  target_include_directories(OpenDCL_RxInstall PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
  target_compile_definitions(OpenDCL_RxInstall PRIVATE
    WIN32
    _WINDOWS
    _USRDLL
    $<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:_DEBUG>
    $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>,$<CONFIG:RelWithDebInfo>>:NDEBUG>
  )
  # Match classic RxInstall.vcxproj: static CRT, multi-byte (not Unicode).
  # FullDebug → Debug (not a CAD runtime module).
  set(_rx_out "${OPENDCL_OUTPUT_ROOT}/Runtime/RxInstall")
  set_target_properties(OpenDCL_RxInstall PROPERTIES
    OUTPUT_NAME "RxInstall"
    PREFIX ""
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:Debug>"
  )
  opendcl_map_fulldebug_to_debug(OpenDCL_RxInstall "${_rx_out}")
  # Mirror classic path for build-wix.ps1 (Runtime\RxInstall\Release\RxInstall.dll).
  add_custom_command(TARGET OpenDCL_RxInstall POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${classic_dir}/${OPENDCL_CFG_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:OpenDCL_RxInstall>"
      "${classic_dir}/${OPENDCL_CFG_DIR}/$<TARGET_FILE_NAME:OpenDCL_RxInstall>"
    COMMENT "Mirror RxInstall.dll to classic ${classic_dir}/<Debug|Release>/"
  )
  target_link_libraries(OpenDCL_RxInstall PRIVATE msi advapi32)
endfunction()

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  opendcl_rxinstall_add_native("${_opendcl_rx_classic_dir}")
  message(STATUS "RxInstall: native Win32 target OpenDCL_RxInstall")
elseif(_opendcl_rx_is_standalone)
  message(FATAL_ERROR
    "RxInstall must be configured as Win32 (x86). Re-run with -A Win32 "
    "(MSI Binary CA exports use stdcall decoration _Name@4).")
else()
  # Nested Win32 build from an x64 parent so packaging still gets a correct CA DLL.
  set(_rx_bin "${CMAKE_BINARY_DIR}/rxinstall-win32")
  set(_rx_stamp "${_rx_bin}/.configured")

  add_custom_command(
    OUTPUT "${_rx_stamp}"
    COMMAND ${CMAKE_COMMAND}
      -S "${CMAKE_CURRENT_SOURCE_DIR}"
      -B "${_rx_bin}"
      -G "${CMAKE_GENERATOR}"
      -A Win32
      "-DOPENDCL_OUTPUT_ROOT=${OPENDCL_OUTPUT_ROOT}"
      "-DCMAKE_CONFIGURATION_TYPES=Debug;Release"
    COMMAND ${CMAKE_COMMAND} -E touch "${_rx_stamp}"
    COMMENT "Configure nested Win32 RxInstall under ${_rx_bin}"
    VERBATIM
  )

  # Map FullDebug → Debug for the nested Win32 project (no FullDebug there).
  # SOURCES: list RxInstall.cpp etc. in the main x64 solution for editing only;
  # they are not compiled here (Win32 nested build does that).
  add_custom_target(OpenDCL_RxInstall ALL
    DEPENDS "${_rx_stamp}"
    COMMAND ${CMAKE_COMMAND}
      --build "${_rx_bin}"
      --config "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>"
    COMMENT "Build nested Win32 RxInstall"
    SOURCES ${_RX_SOURCES}
      "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/RegIO.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/resource.h"
    VERBATIM
  )
  # VS: show as "does not participate in build" content rather than fake ClCompile.
  set_source_files_properties(${_RX_SOURCES}
    "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/RegIO.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/resource.h"
    PROPERTIES HEADER_FILE_ONLY TRUE
  )
  message(STATUS "RxInstall: nested Win32 build target OpenDCL_RxInstall (MSI CA is x86); sources listed for IDE")
endif()
