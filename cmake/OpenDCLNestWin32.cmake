# Nested Win32 under an x64 Visual Studio parent.
#
#   win32-lib/<toolset>-<crt>/  zlib+png for one toolset/CRT
#   win32-rt/<id>/              one CAD Runtime (IMPORTED libs)
#   win32-common/               Res + Studio + RxInstall
#
# Nest_Libs builds every win32-lib tree in one CustomBuild (serial COMMAND list).
# Runtime nests and Nest_Win32_Common follow; imported w32_* are Explorer-only.
# Nest MSBuild: MSBUILDDISABLENODEREUSE + /nodeReuse:false.

include_guard(GLOBAL)

function(opendcl_nest_msbuild_args out_var)
  set(_m "${OPENDCL_NEST_MSBUILD_MAX_CPU_COUNT}")
  set(_clmp "${OPENDCL_NEST_CL_MP_COUNT}")
  if(NOT _m MATCHES "^[0-9]+$" OR _m STREQUAL "0")
    set(_m "1")
  endif()
  if(NOT _clmp MATCHES "^[0-9]+$" OR _clmp STREQUAL "0")
    set(_clmp "1")
  endif()
  set(${out_var}
    --parallel "${_m}"
    --
    "/m:${_m}"
    "/nodeReuse:false"
    "/p:CL_MPCount=${_clmp}"
    PARENT_SCOPE)
  set(OPENDCL_NEST_EFFECTIVE_M "${_m}" PARENT_SCOPE)
  set(OPENDCL_NEST_EFFECTIVE_CLMP "${_clmp}" PARENT_SCOPE)
endfunction()

function(opendcl_nest_build_cmd out_var nest_bin)
  set(_cfg "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>")
  opendcl_nest_msbuild_args(_args)
  set(_cmd
    ${CMAKE_COMMAND} -E env MSBUILDDISABLENODEREUSE=1 --
    ${CMAKE_COMMAND} --build "${nest_bin}" --config "${_cfg}"
  )
  foreach(_a IN LISTS ARGN)
    list(APPEND _cmd "${_a}")
  endforeach()
  list(APPEND _cmd ${_args})
  set(${out_var} "${_cmd}" PARENT_SCOPE)
endfunction()

# Write init-cache + configure nest if stamp changed. Extra cache lines via ARGN.
function(opendcl_nest_configure nest_bin)
  file(MAKE_DIRECTORY "${nest_bin}")
  set(_cache_init "${nest_bin}/opendcl-nest-cache.cmake")
  set(_body "")
  string(APPEND _body "set(OPENDCL_NEST_WIN32 OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body "set(OPENDCL_WIN32_IN_ALL OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_OUTPUT_ROOT [==[${OPENDCL_OUTPUT_ROOT}]==] CACHE PATH \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_ENABLE_ARX [==[${OPENDCL_ENABLE_ARX}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_ENABLE_BRX [==[${OPENDCL_ENABLE_BRX}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_ENABLE_GRX [==[${OPENDCL_ENABLE_GRX}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_ENABLE_ZRX [==[${OPENDCL_ENABLE_ZRX}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_LANGS [==[${OPENDCL_LANGS}]==] CACHE STRING \"\" FORCE)\n")
  string(APPEND _body
    "set(CMAKE_CONFIGURATION_TYPES [==[Debug;FullDebug;Release]==] CACHE STRING \"\" FORCE)\n")
  string(APPEND _body
    "set(CMAKE_SYSTEM_VERSION [==[10.0.19041.0]==] CACHE STRING \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_BUILD_STUDIO_HELP OFF CACHE BOOL \"\" FORCE)\n")
  foreach(_line IN LISTS ARGN)
    string(APPEND _body "${_line}\n")
  endforeach()
  file(WRITE "${_cache_init}" "${_body}")

  set(_nest_stamp "${nest_bin}/opendcl-nest-init.stamp")
  set(_need TRUE)
  if(EXISTS "${nest_bin}/CMakeCache.txt" AND EXISTS "${_nest_stamp}")
    file(READ "${_nest_stamp}" _prev)
    if(_prev STREQUAL _body)
      set(_need FALSE)
    endif()
  endif()
  if(_need)
    message(STATUS "Win32 nest: configuring ${nest_bin} (-A Win32) ...")
    execute_process(
      COMMAND ${CMAKE_COMMAND}
        -C "${_cache_init}"
        -S "${CMAKE_SOURCE_DIR}"
        -B "${nest_bin}"
        -G "${CMAKE_GENERATOR}"
        -A Win32
      RESULT_VARIABLE _cfg_rc
      OUTPUT_VARIABLE _cfg_out
      ERROR_VARIABLE _cfg_err
      ECHO_OUTPUT_VARIABLE
      ECHO_ERROR_VARIABLE
    )
    if(NOT _cfg_rc EQUAL 0)
      message(FATAL_ERROR
        "Failed to configure nest under ${nest_bin} (exit ${_cfg_rc}).\n${_cfg_err}")
    endif()
    file(WRITE "${_nest_stamp}" "${_body}")
  else()
    message(STATUS "Win32 nest: reusing ${nest_bin} (init-cache unchanged)")
  endif()
endfunction()

function(opendcl_nest_import_vcxprojs nest_bin name_prefix skip_names out_bases out_res_targets)
  file(GLOB_RECURSE _vcxprojs "${nest_bin}/*.vcxproj")
  set(_bases "")
  set(_res "")
  set(_seen "")
  set(_skip ${skip_names})
  list(APPEND _skip
    ALL_BUILD INSTALL PACKAGE RUN_TESTS
    Continuous Experimental Nightly NightlyMemoryCheck ZERO_CHECK
  )
  foreach(_proj IN LISTS _vcxprojs)
    if(_proj MATCHES "[/\\\\]CMakeFiles[/\\\\]")
      continue()
    endif()
    get_filename_component(_base "${_proj}" NAME_WE)
    if(_base IN_LIST _skip)
      continue()
    endif()
    if(_base IN_LIST _seen)
      continue()
    endif()
    list(APPEND _seen "${_base}")
    set(_t "${name_prefix}${_base}")
    if(TARGET "${_t}")
      continue()
    endif()
    include_external_msproject(${_t} "${_proj}" PLATFORM "Win32")
    opendcl_solution_folder(_folder "Win32" "${_base}")
    set_target_properties(${_t} PROPERTIES FOLDER "${_folder}")
    set_property(TARGET ${_t} PROPERTY EXCLUDE_FROM_ALL TRUE)
    set_property(TARGET ${_t} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
    list(APPEND _bases "${_base}")
    if(_base MATCHES "RuntimeRes_")
      list(APPEND _res "${_base}")
    endif()
  endforeach()
  set(${out_bases} "${_bases}" PARENT_SCOPE)
  set(${out_res_targets} "${_res}" PARENT_SCOPE)
endfunction()

function(opendcl_add_win32_nest)
  if(NOT (OPENDCL_NEST_WIN32 AND MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8
          AND CMAKE_GENERATOR MATCHES "Visual Studio"))
    if(OPENDCL_NEST_WIN32)
      message(STATUS
        "OPENDCL_NEST_WIN32 ignored (need x64 Visual Studio generator; this is "
        "${CMAKE_GENERATOR} ptr=${CMAKE_SIZEOF_VOID_P})")
    endif()
    return()
  endif()

  opendcl_nest_msbuild_args(_unused_args)
  set(_nest_m "${OPENDCL_NEST_EFFECTIVE_M}")
  set(_nest_clmp "${OPENDCL_NEST_EFFECTIVE_CLMP}")
  message(STATUS
    "Win32 nest build throttle: MSBuild /m:${_nest_m} CL_MPCount=${_nest_clmp} "
    "nodeReuse=false")

  # --- Discover Win32 runtime IDs (same filters as a Win32 configure) ---
  opendcl_select_runtimes(_w32_ids _w32_skipped x86)
  list(LENGTH _w32_ids _nw32)
  message(STATUS "Win32 nest runtimes: ${_nw32} (split lib/rt/common nests)")

  set(_have_fulldebug FALSE)
  if(CMAKE_CONFIGURATION_TYPES AND "FullDebug" IN_LIST CMAKE_CONFIGURATION_TYPES)
    set(_have_fulldebug TRUE)
  endif()


  # Lib nests: configure each win32-lib tree; build them all via Nest_Libs.
  set(_lib_keys "")
  foreach(_id IN LISTS _w32_ids)
    opendcl_rt_get("${_id}" TOOLSET _ts)
    opendcl_png_lib_toolset("${_ts}" _lib_ts)
    string(REGEX REPLACE "[^A-Za-z0-9]" "_" _lib_safe "${_lib_ts}")
    list(APPEND _lib_keys "${_lib_safe}|md")
    if(_have_fulldebug)
      list(APPEND _lib_keys "${_lib_safe}|mdd")
    endif()
  endforeach()
  string(REGEX REPLACE "[^A-Za-z0-9]" "_" _host_safe "${OPENDCL_HOST_TOOLSET_TAG}")
  list(APPEND _lib_keys "${_host_safe}|mt")
  if(_have_fulldebug)
    list(APPEND _lib_keys "${_host_safe}|md")
    list(APPEND _lib_keys "${_host_safe}|mdd")
  else()
    list(APPEND _lib_keys "${_host_safe}|md")
  endif()
  list(REMOVE_DUPLICATES _lib_keys)
  list(SORT _lib_keys)

  set(_nest_libs_args "")
  foreach(_key IN LISTS _lib_keys)
    string(REPLACE "|" ";" _kv "${_key}")
    list(GET _kv 0 _ts_safe)
    list(GET _kv 1 _crt)
    set(_ts_raw "${_ts_safe}")
    if(_ts_safe STREQUAL _host_safe)
      set(_ts_raw "${OPENDCL_HOST_TOOLSET_TAG}")
    endif()
    set(_lib_bin "${CMAKE_BINARY_DIR}/win32-lib/${_ts_safe}-${_crt}")
    opendcl_nest_configure("${_lib_bin}"
      "set(OPENDCL_BUILD_RUNTIME OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_BUILD_STUDIO OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_BUILD_RXINSTALL OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_BUILD_RES_DLLS OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_LIBRARY_IMPORTED OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_LIBRARY_ONLY_TOOLSET [==[${_ts_raw}]==] CACHE STRING \"\" FORCE)"
      "set(OPENDCL_LIBRARY_ONLY_CRT [==[${_crt}]==] CACHE STRING \"\" FORCE)"
      "set(OPENDCL_RUNTIME_AUTO OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_RUNTIME_TARGETS [==[]==] CACHE STRING \"\" FORCE)"
    )
    opendcl_nest_build_cmd(_lib_cmd "${_lib_bin}")
    list(APPEND _nest_libs_args COMMAND ${_lib_cmd})
  endforeach()

  if(NOT TARGET Nest_Libs)
    add_custom_target(Nest_Libs ${_nest_libs_args}
      COMMENT "Win32 lib nests"
      VERBATIM
    )
    set_property(TARGET Nest_Libs PROPERTY FOLDER "CMake")
  endif()

  # --- Per-runtime nests ---
  set(_all_rt_targets "")
  set(_skip_rt
    Studio StudioHelp_All RxInstall
    zlib_x86_md_${_host_safe} png_x86_md_${_host_safe}
    zlib_x86_mdd_${_host_safe} png_x86_mdd_${_host_safe}
    zlib_x86_mt_${_host_safe} png_x86_mt_${_host_safe}
  )
  foreach(_hl IN LISTS OPENDCL_LANGS)
    list(APPEND _skip_rt "StudioHelp_${_hl}" "StudioRes_${_hl}" "RuntimeRes_${_hl}")
  endforeach()

  foreach(_id IN LISTS _w32_ids)
    opendcl_rt_get("${_id}" TOOLSET _ts)
    opendcl_png_lib_toolset("${_ts}" _lib_ts)
    string(REGEX REPLACE "[^A-Za-z0-9]" "_" _lib_safe "${_lib_ts}")
    string(REPLACE "." "_" _id_safe "${_id}")
    set(_rt_bin "${CMAKE_BINARY_DIR}/win32-rt/${_id}")
    set(_rt_tgt "Nest_Win32_${_id_safe}")
    set(_rt_cmake "Runtime_${_id_safe}")
    opendcl_nest_configure("${_rt_bin}"
      "set(OPENDCL_BUILD_RUNTIME ON CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_BUILD_STUDIO OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_BUILD_RXINSTALL OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_BUILD_RES_DLLS OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_LIBRARY_IMPORTED ON CACHE BOOL \"\" FORCE)"
      "set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_LIBRARY_ONLY_TOOLSET [==[]==] CACHE STRING \"\" FORCE)"
      "set(OPENDCL_LIBRARY_ONLY_CRT [==[]==] CACHE STRING \"\" FORCE)"
      "set(OPENDCL_RUNTIME_AUTO OFF CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_RUNTIME_REQUIRE_SELECTED ON CACHE BOOL \"\" FORCE)"
      "set(OPENDCL_RUNTIME_TARGETS [==[${_id}]==] CACHE STRING \"\" FORCE)"
      "set(OPENDCL_RUNTIME_PER_FAMILY_MAX [==[0]==] CACHE STRING \"\" FORCE)"
      "set(OPENDCL_RUNTIME_MIN_TOOLSET [==[]==] CACHE STRING \"\" FORCE)"
      "set(OPENDCL_RES_PE [==[host]==] CACHE STRING \"\" FORCE)"
      "set(OPENDCL_STUDIO_PE [==[host]==] CACHE STRING \"\" FORCE)"
    )
    opendcl_nest_import_vcxprojs("${_rt_bin}" "w32_" "${_skip_rt}" _bases _res_unused)
    opendcl_nest_build_cmd(_rt_cmd "${_rt_bin}" --target "${_rt_cmake}")
    if(NOT TARGET ${_rt_tgt})
      add_custom_target(${_rt_tgt}
        COMMAND ${_rt_cmd}
        COMMENT "Win32 runtime nest ${_id} (/m:${_nest_m})"
        VERBATIM
      )
      set_property(TARGET ${_rt_tgt} PROPERTY FOLDER "CMake/Win32 Runtime")
    endif()
    add_dependencies(${_rt_tgt} Nest_Libs)
    list(APPEND _all_rt_targets ${_rt_tgt})
  endforeach()

  # --- Common nest: Res + Studio + RxInstall; IMPORTED host mt (+ md for safety) ---
  set(_common_bin "${CMAKE_BINARY_DIR}/win32-common")
  set(_common_res OFF)
  if(OPENDCL_RES_PE STREQUAL "classic_x86" AND OPENDCL_BUILD_RES_DLLS)
    set(_common_res ON)
  elseif(OPENDCL_BUILD_RES_DLLS AND OPENDCL_RES_PE STREQUAL "host")
    # Parent host Res: do not build x86 Res into shared out from common.
    set(_common_res OFF)
  endif()
  # When classic_x86, common always builds nest Res (x86 PE).
  if(OPENDCL_RES_PE STREQUAL "classic_x86")
    set(_common_res "${OPENDCL_BUILD_RES_DLLS}")
  endif()

  set(_common_studio OFF)
  if(OPENDCL_BUILD_STUDIO)
    set(_common_studio ON)
  endif()
  set(_common_rx OFF)
  if(OPENDCL_BUILD_RXINSTALL)
    set(_common_rx ON)
  endif()

  set(_res_exclude OFF)
  if(_common_res)
    set(_res_exclude ON)
  endif()

  opendcl_nest_configure("${_common_bin}"
    "set(OPENDCL_BUILD_RUNTIME OFF CACHE BOOL \"\" FORCE)"
    "set(OPENDCL_BUILD_STUDIO [==[${_common_studio}]==] CACHE BOOL \"\" FORCE)"
    "set(OPENDCL_BUILD_RXINSTALL [==[${_common_rx}]==] CACHE BOOL \"\" FORCE)"
    "set(OPENDCL_BUILD_RES_DLLS [==[${_common_res}]==] CACHE BOOL \"\" FORCE)"
    "set(OPENDCL_LIBRARY_IMPORTED ON CACHE BOOL \"\" FORCE)"
    "set(CMAKE_NO_SYSTEM_FROM_IMPORTED ON CACHE BOOL \"\" FORCE)"
    "set(OPENDCL_LIBRARY_ONLY_TOOLSET [==[]==] CACHE STRING \"\" FORCE)"
    "set(OPENDCL_LIBRARY_ONLY_CRT [==[]==] CACHE STRING \"\" FORCE)"
    "set(OPENDCL_RES_PE [==[host]==] CACHE STRING \"\" FORCE)"
    "set(OPENDCL_STUDIO_PE [==[host]==] CACHE STRING \"\" FORCE)"
    "set(OPENDCL_RUNTIME_RES_EXCLUDE_FROM_ALL [==[${_res_exclude}]==] CACHE BOOL \"\" FORCE)"
    "set(OPENDCL_RUNTIME_AUTO OFF CACHE BOOL \"\" FORCE)"
    "set(OPENDCL_RUNTIME_TARGETS [==[]==] CACHE STRING \"\" FORCE)"
  )

  # Import RuntimeRes for Explorer; Studio/RxInstall stay parent umbrellas.
  set(_skip_common_import
    ALL_BUILD INSTALL PACKAGE RUN_TESTS
    Continuous Experimental Nightly NightlyMemoryCheck ZERO_CHECK
    Studio StudioHelp_All RxInstall
  )
  foreach(_hl IN LISTS OPENDCL_LANGS)
    list(APPEND _skip_common_import "StudioHelp_${_hl}" "StudioRes_${_hl}")
  endforeach()
  opendcl_nest_import_vcxprojs("${_common_bin}" "w32_" "${_skip_common_import}"
    _common_bases _w32_res_targets)

  opendcl_nest_build_cmd(_common_cmd "${_common_bin}")
  if(NOT TARGET Nest_Win32_Common)
    if(OPENDCL_WIN32_IN_ALL)
      add_custom_target(Nest_Win32_Common ALL
        COMMAND ${_common_cmd}
        COMMENT "Win32 common nest (Res/Studio/RxInstall) (/m:${_nest_m})"
        VERBATIM
      )
    else()
      add_custom_target(Nest_Win32_Common
        COMMAND ${_common_cmd}
        COMMENT "Win32 common nest (Res/Studio/RxInstall) (/m:${_nest_m})"
        VERBATIM
      )
    endif()
    set_property(TARGET Nest_Win32_Common PROPERTY FOLDER "CMake")
  endif()
  add_dependencies(Nest_Win32_Common Nest_Libs)

  # Res_Win32: Res targets only (build-once).
  if(_common_res AND _w32_res_targets)
    set(_res_extra)
    foreach(_rt ${_w32_res_targets})
      list(APPEND _res_extra --target "${_rt}")
    endforeach()
    opendcl_nest_build_cmd(_res_cmd "${_common_bin}" ${_res_extra})
    if(NOT TARGET Res_Win32)
      add_custom_target(Res_Win32
        COMMAND ${_res_cmd}
        COMMENT "Build classic x86 Runtime.Res via win32-common (/m:${_nest_m})"
        VERBATIM
      )
      set_target_properties(Res_Win32 PROPERTIES FOLDER "Runtime/Localized Resources")
    endif()
    add_dependencies(Nest_Win32_Common Res_Win32)
    list(LENGTH _w32_res_targets _nr)
    message(STATUS
      "Resource DLLs: classic_x86 -> Res_Win32 (${_nr} langs) via win32-common")
  endif()

  # Store common bin for RxInstall umbrella (may already exist).
  set(OPENDCL_WIN32_COMMON_BIN "${_common_bin}" CACHE INTERNAL "" FORCE)

  if(TARGET RxInstall AND TARGET Nest_Win32_Common)
    # Retarget RxInstall COMMAND to common nest if still pointing at old path —
    # RxInstall is created earlier; update via dependency only. COMMAND was set
    # with win32 path at creation — fix in RxInstall.cmake to use common.
    if(OPENDCL_WIN32_IN_ALL)
      add_dependencies(RxInstall Nest_Win32_Common)
    endif()
  endif()

  # Product gates: classic_x86 Res for F5.
  if(TARGET Res_Win32 AND OPENDCL_RES_PE STREQUAL "classic_x86")
    if(TARGET Studio)
      add_dependencies(Studio Res_Win32)
    endif()
    foreach(_id IN LISTS OPENDCL_RT_IDS)
      string(REPLACE "." "_" _safe "${_id}")
      set(_rt "Runtime_${_safe}")
      if(TARGET "${_rt}")
        add_dependencies(${_rt} Res_Win32)
      endif()
    endforeach()
    message(STATUS "Win32 Res gate for Studio/runtimes: Res_Win32")
  endif()

  # Nest_Win32: Nest_Libs -> runtime nests -> common (serial via deps).
  if(NOT TARGET Nest_Win32)
    if(OPENDCL_WIN32_IN_ALL)
      add_custom_target(Nest_Win32 ALL
        COMMENT "Win32 nests"
        VERBATIM
      )
    else()
      add_custom_target(Nest_Win32
        COMMENT "Win32 nests"
        VERBATIM
      )
    endif()
    set_property(TARGET Nest_Win32 PROPERTY FOLDER "CMake")
  endif()
  add_dependencies(Nest_Win32 Nest_Libs)
  foreach(_t IN LISTS _all_rt_targets)
    add_dependencies(Nest_Win32 ${_t})
  endforeach()
  add_dependencies(Nest_Win32 Nest_Win32_Common)

  set(_nest_serial Nest_Libs)
  foreach(_t IN LISTS _all_rt_targets)
    add_dependencies(${_t} ${_nest_serial})
    set(_nest_serial "${_t}")
  endforeach()
  add_dependencies(Nest_Win32_Common ${_nest_serial})

  list(LENGTH _lib_keys _nlib)
  list(LENGTH _all_rt_targets _nrt)
  message(STATUS
    "Win32 nests: Nest_Libs (${_nlib} trees), ${_nrt} runtime, 1 common; "
    "IN_ALL=${OPENDCL_WIN32_IN_ALL}")
endfunction()
