# RxInstall — WiX / MSI demand-load custom-action DLL.
#
# Must be **Win32 (x86)**: OpenDCL.Runtime.wxs DllEntry uses stdcall decoration
# (_RxInstallMachine@4, …). An x64 build exports undecorated names and will not
# load as a Binary CA for the x86 merge module.
#
# Usage:
#   - Parent OpenDCL tree: add_subdirectory when OPENDCL_BUILD_RXINSTALL=ON
#     * With OPENDCL_NEST_WIN32: PE from full Win32 nest (`build/<preset>/win32`);
#       OpenDCL_RxInstall is an umbrella that runs cmake --build on the nest.
#     * Without full nest: private nest under rxinstall-win32 (x64-only presets).
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
set(_RX_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/stdafx.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/RegIO.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/resource.h"
)

function(opendcl_rxinstall_set_folder target)
  # Classic: RxInstall under Runtime (arch is in the PE, not the folder).
  if(COMMAND opendcl_solution_folder)
    opendcl_solution_folder(_rx_folder "Win32" "OpenDCL_RxInstall")
    set_target_properties(${target} PROPERTIES FOLDER "${_rx_folder}")
  else()
    set_target_properties(${target} PROPERTIES FOLDER "Runtime")
  endif()
endfunction()

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
  opendcl_rxinstall_set_folder(OpenDCL_RxInstall)
  # Mirror classic path for build-wix.ps1 (Runtime\RxInstall\Release\RxInstall.dll).
  add_custom_command(TARGET OpenDCL_RxInstall POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${classic_dir}/${OPENDCL_CFG_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:OpenDCL_RxInstall>"
      "${classic_dir}/${OPENDCL_CFG_DIR}/$<TARGET_FILE_NAME:OpenDCL_RxInstall>"
    COMMENT "Mirror RxInstall.dll to classic ${classic_dir}/<Debug|Release>/"
  )
  target_link_libraries(OpenDCL_RxInstall PRIVATE msi advapi32)
  if(MSVC)
    target_compile_options(OpenDCL_RxInstall PRIVATE "/FS")
  endif()
endfunction()

# Umbrella on x64 parent when OPENDCL_NEST_WIN32: one PE from full nest.
# COMMAND runs at build time; nest is configured later in the same generate
# (opendcl_add_win32_nest → build/<preset>/win32).
function(opendcl_rxinstall_add_via_win32_nest classic_dir)
  if(TARGET OpenDCL_RxInstall)
    return()
  endif()
  set(_rx_nest_bin "${CMAKE_BINARY_DIR}/win32")
  set(_nest_cfg "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>")
  # Never ALL when OPENDCL_WIN32_IN_ALL: OpenDCL_Win32 already builds the full
  # nest (including RxInstall). A second concurrent cmake --build races (.ilk/PDB).
  # Targeted `cmake --build --target OpenDCL_RxInstall` still works via COMMAND.
  # When nest is on but not IN_ALL, keep ALL so packaging/default still gets the CA.
  # Match nest throttle defaults (old toolset OOM safety); override via cache.
  set(_rx_m "2")
  set(_rx_clmp "1")
  if(DEFINED OPENDCL_NEST_MSBUILD_MAX_CPU_COUNT AND OPENDCL_NEST_MSBUILD_MAX_CPU_COUNT MATCHES "^[1-9][0-9]*$")
    set(_rx_m "${OPENDCL_NEST_MSBUILD_MAX_CPU_COUNT}")
  endif()
  if(DEFINED OPENDCL_NEST_CL_MP_COUNT AND OPENDCL_NEST_CL_MP_COUNT MATCHES "^[1-9][0-9]*$")
    set(_rx_clmp "${OPENDCL_NEST_CL_MP_COUNT}")
  endif()
  if(OPENDCL_WIN32_IN_ALL)
    add_custom_target(OpenDCL_RxInstall
      COMMAND ${CMAKE_COMMAND}
        --build "${_rx_nest_bin}"
        --config "${_nest_cfg}"
        --target OpenDCL_RxInstall
        --parallel "${_rx_m}"
        -- "/m:${_rx_m}" "/p:CL_MPCount=${_rx_clmp}"
      COMMENT "Build Win32 RxInstall CA via full nest (${_rx_nest_bin})"
      SOURCES ${_RX_SOURCES} ${_RX_HEADERS}
      VERBATIM
    )
  else()
    add_custom_target(OpenDCL_RxInstall ALL
      COMMAND ${CMAKE_COMMAND}
        --build "${_rx_nest_bin}"
        --config "${_nest_cfg}"
        --target OpenDCL_RxInstall
        --parallel "${_rx_m}"
        -- "/m:${_rx_m}" "/p:CL_MPCount=${_rx_clmp}"
      COMMENT "Build Win32 RxInstall CA via full nest (${_rx_nest_bin})"
      SOURCES ${_RX_SOURCES} ${_RX_HEADERS}
      VERBATIM
    )
  endif()
  set_source_files_properties(${_RX_SOURCES} ${_RX_HEADERS}
    PROPERTIES HEADER_FILE_ONLY TRUE
  )
  opendcl_rxinstall_set_folder(OpenDCL_RxInstall)
  set_property(GLOBAL PROPERTY OPENDCL_RXINSTALL_VIA_WIN32_NEST TRUE)
  message(STATUS
    "RxInstall: via full Win32 nest (${_rx_nest_bin}); "
    "umbrella COMMAND (ALL only when not OPENDCL_WIN32_IN_ALL)")
endfunction()

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  opendcl_rxinstall_add_native("${_opendcl_rx_classic_dir}")
  message(STATUS "RxInstall: native Win32 target OpenDCL_RxInstall")
elseif(_opendcl_rx_is_standalone)
  message(FATAL_ERROR
    "RxInstall must be configured as Win32 (x86). Re-run with -A Win32 "
    "(MSI Binary CA exports use stdcall decoration _Name@4).")
elseif(OPENDCL_NEST_WIN32)
  # Full dual-arch nest owns the x86 CA; no private rxinstall-win32 tree.
  opendcl_rxinstall_add_via_win32_nest("${_opendcl_rx_classic_dir}")
else()
  # Private nest when full OPENDCL_NEST_WIN32 is off (x64-only presets).
  set(_rx_bin "${CMAKE_BINARY_DIR}/rxinstall-win32")
  set(_rx_cache "${_rx_bin}/CMakeCache.txt")

  add_custom_command(
    OUTPUT "${_rx_cache}"
    COMMAND ${CMAKE_COMMAND}
      -S "${CMAKE_CURRENT_SOURCE_DIR}"
      -B "${_rx_bin}"
      -G "${CMAKE_GENERATOR}"
      -A Win32
      "-DOPENDCL_OUTPUT_ROOT=${OPENDCL_OUTPUT_ROOT}"
      "-DCMAKE_CONFIGURATION_TYPES=Debug;Release"
    DEPENDS
      "${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.txt"
      ${_RX_SOURCES}
    COMMENT "Configure nested Win32 RxInstall under ${_rx_bin}"
    VERBATIM
  )

  add_custom_target(OpenDCL_RxInstall ALL
    DEPENDS "${_rx_cache}"
    COMMAND ${CMAKE_COMMAND}
      --build "${_rx_bin}"
      --config "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>"
    COMMENT "Build nested Win32 RxInstall"
    SOURCES ${_RX_SOURCES} ${_RX_HEADERS}
    VERBATIM
  )
  set_source_files_properties(${_RX_SOURCES} ${_RX_HEADERS}
    PROPERTIES HEADER_FILE_ONLY TRUE
  )
  opendcl_rxinstall_set_folder(OpenDCL_RxInstall)
  message(STATUS "RxInstall: private nest under ${_rx_bin} (MSI CA is x86); sources listed for IDE")
endif()
