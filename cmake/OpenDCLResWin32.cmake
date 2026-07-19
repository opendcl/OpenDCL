# Nested Win32 build of Runtime.Res + Studio.Res for classic x86 packaging parity.
#
# Used only when OPENDCL_RES_PE=classic_x86 on an x64 configure (public Mixed ship).
# OPENDCL_RES_PE=host builds Res natively in the parent (x64 Res on x64) and does
# not create this nest — path open for future packaging that accepts host-arch Res.
#
# Nested tree flags: OPENDCL_BUILD_RES_DLLS=ON only (no modules / Studio.exe / RxInstall).
# Lang list is written via -C init-cache.cmake so MSBuild does not mangle ';'.

include_guard(GLOBAL)

function(opendcl_add_res_win32_nest)
  opendcl_res_need_win32_nest(_need)
  if(NOT _need)
    if(OPENDCL_NEST_WIN32 AND OPENDCL_RES_PE STREQUAL "classic_x86")
      message(STATUS
        "Resource DLLs: classic_x86 via full Win32 nest (no private res-win32; "
        "OpenDCL_Res_Win32 umbrella created after nest import)")
    endif()
    return()
  endif()
  if(TARGET OpenDCL_Res_Win32)
    return()
  endif()

  set(_res_bin "${CMAKE_BINARY_DIR}/res-win32")
  set(_res_stamp "${_res_bin}/.opendcl_res_win32_configured")
  set(_res_cache "${_res_bin}/opendcl-res-win32-cache.cmake")

  # Semicolon lists cannot safely pass through VS custom-build command lines.
  file(MAKE_DIRECTORY "${_res_bin}")
  set(_cache_body "")
  string(APPEND _cache_body "set(OPENDCL_NEST_WIN32 OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _cache_body "set(OPENDCL_BUILD_RES_DLLS ON CACHE BOOL \"\" FORCE)\n")
  # Nest is Win32: host Res PE is x86 (same as classic_x86 ship).
  string(APPEND _cache_body "set(OPENDCL_RES_PE [==[host]==] CACHE STRING \"\" FORCE)\n")
  string(APPEND _cache_body "set(OPENDCL_BUILD_RUNTIME OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _cache_body "set(OPENDCL_BUILD_STUDIO OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _cache_body "set(OPENDCL_BUILD_RXINSTALL OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _cache_body "set(OPENDCL_BUILD_HELP OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _cache_body
    "set(OPENDCL_OUTPUT_ROOT [==[${OPENDCL_OUTPUT_ROOT}]==] CACHE PATH \"\" FORCE)\n")
  string(APPEND _cache_body
    "set(OPENDCL_LANGS [==[${OPENDCL_LANGS}]==] CACHE STRING \"\" FORCE)\n")
  string(APPEND _cache_body
    "set(CMAKE_SYSTEM_VERSION [==[10.0.19041.0]==] CACHE STRING \"\" FORCE)\n")
  file(WRITE "${_res_cache}" "${_cache_body}")

  add_custom_command(
    OUTPUT "${_res_stamp}"
    COMMAND ${CMAKE_COMMAND}
      -C "${_res_cache}"
      -S "${CMAKE_SOURCE_DIR}"
      -B "${_res_bin}"
      -G "${CMAKE_GENERATOR}"
      -A Win32
    COMMAND ${CMAKE_COMMAND} -E touch "${_res_stamp}"
    DEPENDS "${_res_cache}"
    COMMENT "Configure nested Win32 resource DLLs under ${_res_bin}"
    VERBATIM
  )

  add_custom_target(OpenDCL_Res_Win32 ALL
    DEPENDS "${_res_stamp}"
    COMMAND ${CMAKE_COMMAND}
      --build "${_res_bin}"
      --config "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>"
    COMMENT "Build nested Win32 Runtime.Res + Studio.Res (classic x86 ship PE)"
    VERBATIM
  )
  set_property(TARGET OpenDCL_Res_Win32 PROPERTY FOLDER "Runtime/Localized Resources")
  message(STATUS
    "Resource DLLs: OPENDCL_RES_PE=classic_x86 -> OpenDCL_Res_Win32 under ${_res_bin} "
    "(x86 PE into ${OPENDCL_OUTPUT_ROOT}; langs=${OPENDCL_LANGS})")
endfunction()
