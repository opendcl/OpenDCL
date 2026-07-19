# Nested Win32 (-A Win32) under an x64 Visual Studio parent.
#
# 1) Configure nest at parent generate time.
# 2) Import nest .vcxproj into the parent .sln (Solution Explorer + folders).
# 3) Build via `cmake --build <nest>` (correct -A Win32). CMake's
#    include_external_msproject targets are still driven with the *parent*
#    Platform=x64 when built through ALL_BUILD / ProjectReference, which breaks
#    nest projects (MSB8013). So imported projects are EXCLUDE_FROM_DEFAULT_BUILD.
#
# Nest cmake --build must be single-flight. Parallel parent targets each running
# `cmake --build <nest>` (OpenDCL_Win32 + OpenDCL_Res_Win32 + OpenDCL_RxInstall)
# race on the same intermediate files (LNK1104 .ilk, C1001, C1041).
# When OPENDCL_WIN32_IN_ALL: only OpenDCL_Win32 is ALL and owns the full nest
# build; Res/RxInstall keep targeted COMMAND for manual builds; product deps
# hang off OpenDCL_Win32. When not IN_ALL: Res/RxInstall own targeted builds.
#
# Call only from the x64 parent when OPENDCL_NEST_WIN32 is ON.

include_guard(GLOBAL)

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

  set(_bin "${CMAKE_BINARY_DIR}/win32")
  set(_cache_init "${_bin}/opendcl-win32-cache.cmake")
  file(MAKE_DIRECTORY "${_bin}")

  # When parent ships host-arch (x64) Res, nest must not overwrite with x86 Res.
  if(OPENDCL_RES_PE STREQUAL "host")
    set(_nest_res OFF)
  else()
    set(_nest_res "${OPENDCL_BUILD_RES_DLLS}")
  endif()

  # Init-cache avoids list-separator mangling (ENU;DEU;…).
  set(_body "")
  string(APPEND _body "set(OPENDCL_NEST_WIN32 OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body "set(OPENDCL_WIN32_IN_ALL OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_OUTPUT_ROOT [==[${OPENDCL_OUTPUT_ROOT}]==] CACHE PATH \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_BUILD_RUNTIME [==[${OPENDCL_BUILD_RUNTIME}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_BUILD_STUDIO [==[${OPENDCL_BUILD_STUDIO}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_BUILD_RXINSTALL [==[${OPENDCL_BUILD_RXINSTALL}]==] CACHE BOOL \"\" FORCE)\n")
  # CHM is arch-independent — parent owns OpenDCL_StudioHelp_*; nest must not
  # rebuild the same OpenDCL.chm or import duplicate help projects into the .sln.
  string(APPEND _body
    "set(OPENDCL_BUILD_STUDIO_HELP OFF CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_BUILD_RES_DLLS [==[${_nest_res}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_RES_PE [==[host]==] CACHE STRING \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_ENABLE_ARX [==[${OPENDCL_ENABLE_ARX}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_ENABLE_BRX [==[${OPENDCL_ENABLE_BRX}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_ENABLE_GRX [==[${OPENDCL_ENABLE_GRX}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_ENABLE_ZRX [==[${OPENDCL_ENABLE_ZRX}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_RUNTIME_AUTO [==[${OPENDCL_RUNTIME_AUTO}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_RUNTIME_REQUIRE_SELECTED [==[${OPENDCL_RUNTIME_REQUIRE_SELECTED}]==] CACHE BOOL \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_RUNTIME_TARGETS [==[${OPENDCL_RUNTIME_TARGETS}]==] CACHE STRING \"\" FORCE)\n")
  string(APPEND _body
    "set(OPENDCL_LANGS [==[${OPENDCL_LANGS}]==] CACHE STRING \"\" FORCE)\n")
  string(APPEND _body
    "set(CMAKE_CONFIGURATION_TYPES [==[Debug;FullDebug;Release]==] CACHE STRING \"\" FORCE)\n")
  string(APPEND _body
    "set(CMAKE_SYSTEM_VERSION [==[10.0.19041.0]==] CACHE STRING \"\" FORCE)\n")
  file(WRITE "${_cache_init}" "${_body}")

  # Skip nest reconfigure when init-cache is unchanged (parent ZERO_CHECK / IDE
  # reloads). Unconditional nest generate rewrites every nest .vcxproj and, with
  # GLOB CONFIGURE_DEPENDS, dirties the parent again -> regen loops and VS
  # "solution already contains an item named 'w32_...'" on reload.
  set(_nest_stamp "${_bin}/opendcl-nest-init.stamp")
  set(_need_nest_cfg TRUE)
  if(EXISTS "${_bin}/CMakeCache.txt" AND EXISTS "${_nest_stamp}")
    file(READ "${_nest_stamp}" _prev_body)
    if(_prev_body STREQUAL _body)
      set(_need_nest_cfg FALSE)
    endif()
  endif()
  if(_need_nest_cfg)
    message(STATUS "Win32 nest: configuring ${_bin} (-A Win32, shared out ${OPENDCL_OUTPUT_ROOT}) ...")
    execute_process(
      COMMAND ${CMAKE_COMMAND}
        -C "${_cache_init}"
        -S "${CMAKE_SOURCE_DIR}"
        -B "${_bin}"
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
        "Failed to configure nested Win32 under ${_bin} (exit ${_cfg_rc}).\n${_cfg_err}")
    endif()
    file(WRITE "${_nest_stamp}" "${_body}")
  else()
    message(STATUS "Win32 nest: reusing existing configure under ${_bin} (init-cache unchanged)")
  endif()

  # Do NOT use CONFIGURE_DEPENDS on nest outputs: nest generate rewrites those
  # files and would force another parent reconfigure on the next build.
  file(GLOB_RECURSE _vcxprojs "${_bin}/*.vcxproj")
  set(_w32_targets "")
  set(_w32_res_cmake_targets "") # nest CMake target names (no w32_ prefix)
  set(_w32_seen_bases "")
  set(_skip_names
    ALL_BUILD INSTALL PACKAGE RUN_TESTS
    Continuous Experimental Nightly NightlyMemoryCheck
  )
  # Parent already has OpenDCL_RxInstall umbrella (COMMAND -> nest build + IDE sources).
  get_property(_rx_via GLOBAL PROPERTY OPENDCL_RXINSTALL_VIA_WIN32_NEST)
  if(_rx_via OR TARGET OpenDCL_RxInstall)
    list(APPEND _skip_names "OpenDCL_RxInstall")
  endif()
  # Parent owns Studio + Studio.Res (x64) + Help. Nest still *builds* Win32 Studio
  # via OpenDCL_Win32; importing w32_OpenDCL_Studio causes VS reload errors
  # ("solution already contains an item named 'w32_OpenDCL_Studio'") when cmake
  # regenerates the .sln while the IDE reloads.
  list(APPEND _skip_names "OpenDCL_Studio" "OpenDCL_StudioHelp_All")
  foreach(_hl IN LISTS OPENDCL_LANGS)
    list(APPEND _skip_names
      "OpenDCL_StudioHelp_${_hl}"
      "OpenDCL_StudioRes_${_hl}"
    )
  endforeach()
  foreach(_proj IN LISTS _vcxprojs)
    if(_proj MATCHES "[/\\\\]CMakeFiles[/\\\\]")
      continue()
    endif()
    get_filename_component(_base "${_proj}" NAME_WE)
    if(_base IN_LIST _skip_names)
      continue()
    endif()
    if(_base IN_LIST _w32_seen_bases)
      continue()
    endif()
    list(APPEND _w32_seen_bases "${_base}")

    set(_t "w32_${_base}")
    if(TARGET "${_t}")
      continue()
    endif()

    # Visible in Solution Explorer (PLATFORM Win32 mapping for IDE / .sln builds).
    include_external_msproject(${_t} "${_proj}" PLATFORM "Win32")

    if(_base STREQUAL "ZERO_CHECK")
      set_target_properties(${_t} PROPERTIES FOLDER "CMake")
      set_property(TARGET ${_t} PROPERTY EXCLUDE_FROM_ALL TRUE)
      set_property(TARGET ${_t} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)
      continue()
    endif()

    # Same product folders as parent x64 peers (classic layout; not Win32/…).
    opendcl_solution_folder(_folder "Win32" "${_base}")
    set_target_properties(${_t} PROPERTIES FOLDER "${_folder}")
    # Do not hang these off ALL_BUILD ProjectReferences (wrong Platform=x64).
    set_property(TARGET ${_t} PROPERTY EXCLUDE_FROM_ALL TRUE)
    set_property(TARGET ${_t} PROPERTY EXCLUDE_FROM_DEFAULT_BUILD TRUE)

    list(APPEND _w32_targets ${_t})
    # classic_x86 OpenDCL_Res_Win32 only needs Runtime.Res (CAD). Studio.Res is
    # host-arch on the parent (and Win32 Studio.Res under out/.../Win32/).
    if(_base MATCHES "RuntimeRes_")
      list(APPEND _w32_res_cmake_targets "${_base}")
    endif()
  endforeach()

  list(LENGTH _w32_targets _n)
  if(_n EQUAL 0)
    message(WARNING
      "Win32 nest configured under ${_bin} but no external .vcxproj were imported")
  endif()

  # Full nest build with correct -A Win32 (single MSBuild of the nest tree).
  # Throttle parallelism: old Win32 toolsets (VS2010-era cl) OOM under /m+/MP
  # (C1060 afxtempl.h, C1001 ICE). Defaults in OpenDCLHelpers.cmake.
  set(_nest_cfg "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>")
  set(_nest_m "${OPENDCL_NEST_MSBUILD_MAX_CPU_COUNT}")
  set(_nest_clmp "${OPENDCL_NEST_CL_MP_COUNT}")
  if(NOT _nest_m MATCHES "^[0-9]+$" OR _nest_m STREQUAL "0")
    set(_nest_m "2")
  endif()
  if(NOT _nest_clmp MATCHES "^[0-9]+$" OR _nest_clmp STREQUAL "0")
    set(_nest_clmp "1")
  endif()
  # cmake --build ... -- <msbuild args>
  set(_nest_build_args
    --parallel "${_nest_m}"
    --
    "/m:${_nest_m}"
    "/p:CL_MPCount=${_nest_clmp}"
  )
  if(OPENDCL_WIN32_IN_ALL)
    add_custom_target(OpenDCL_Win32 ALL
      COMMAND ${CMAKE_COMMAND} --build "${_bin}" --config "${_nest_cfg}"
        ${_nest_build_args}
      COMMENT "Build nested Win32 OpenDCL (${_nest_cfg}) under ${_bin} (/m:${_nest_m} CL_MPCount=${_nest_clmp})"
      VERBATIM
    )
  else()
    add_custom_target(OpenDCL_Win32
      COMMAND ${CMAKE_COMMAND} --build "${_bin}" --config "${_nest_cfg}"
        ${_nest_build_args}
      COMMENT "Build nested Win32 OpenDCL (${_nest_cfg}) under ${_bin} (/m:${_nest_m} CL_MPCount=${_nest_clmp})"
      VERBATIM
    )
  endif()
  # Bulk nest entry (not a product PE) - sit with other CMake helpers.
  set_property(TARGET OpenDCL_Win32 PROPERTY FOLDER "CMake")
  message(STATUS
    "Win32 nest build throttle: MSBuild /m:${_nest_m} CL_MPCount=${_nest_clmp} "
    "(OPENDCL_NEST_MSBUILD_MAX_CPU_COUNT / OPENDCL_NEST_CL_MP_COUNT)")

  # RxInstall: single parent umbrella (not imported). When WIN32_IN_ALL the full
  # nest build already produces RxInstall.dll — do not also ALL+COMMAND RxInstall
  # (parallel nest cmake --build races). Targeted build still has COMMAND.
  if(TARGET OpenDCL_RxInstall)
    message(STATUS
      "RxInstall: single solution entry OpenDCL_RxInstall -> "
      "cmake --build ${_bin} --target OpenDCL_RxInstall (no w32_ import)")
  endif()

  # classic_x86 Res umbrella → nest Res targets (targeted nest build).
  # Never ALL when OpenDCL_Win32 is ALL (avoids parallel nest invocations).
  if(OPENDCL_RES_PE STREQUAL "classic_x86" AND _w32_res_cmake_targets)
    set(_res_cmd ${CMAKE_COMMAND} --build "${_bin}" --config "${_nest_cfg}")
    foreach(_rt ${_w32_res_cmake_targets})
      list(APPEND _res_cmd --target "${_rt}")
    endforeach()
    list(APPEND _res_cmd ${_nest_build_args})
    if(TARGET OpenDCL_Res_Win32)
      message(WARNING
        "OpenDCL_Res_Win32 already exists; expected full nest to own classic_x86 Res")
    else()
      # Demand-build only (Studio/runtimes depend on the nest gate below when needed).
      add_custom_target(OpenDCL_Res_Win32
        COMMAND ${_res_cmd}
        COMMENT "Build classic x86 Runtime.Res via full Win32 nest (/m:${_nest_m})"
        VERBATIM
      )
      set_target_properties(OpenDCL_Res_Win32 PROPERTIES FOLDER "Runtime/Localized Resources")
    endif()
    list(LENGTH _w32_res_cmake_targets _nr)
    message(STATUS
      "Resource DLLs: classic_x86 -> OpenDCL_Res_Win32 builds ${_nr} nest Runtime.Res target(s) "
      "(Studio.Res is host-arch on parent; not ALL when OpenDCL_Win32 is ALL)")
  endif()

  # Product → nest gate (late: runtimes/Studio were created before this function).
  # WIN32_IN_ALL: one nest flight via OpenDCL_Win32.
  # Else: Res-only gate (or nothing if host Res).
  if(OPENDCL_WIN32_IN_ALL)
    set(_nest_gate OpenDCL_Win32)
  elseif(TARGET OpenDCL_Res_Win32)
    set(_nest_gate OpenDCL_Res_Win32)
  else()
    set(_nest_gate "")
  endif()

  if(_nest_gate)
    if(TARGET OpenDCL_Studio)
      add_dependencies(OpenDCL_Studio ${_nest_gate})
    endif()
    # x64 runtimes need Runtime.Res for F5; classic_x86 Res is nest-only.
    if(OPENDCL_RES_PE STREQUAL "classic_x86" OR OPENDCL_WIN32_IN_ALL)
      foreach(_id IN LISTS OPENDCL_RT_IDS)
        string(REPLACE "." "_" _safe "${_id}")
        set(_rt "OpenDCL_Runtime_${_safe}")
        if(TARGET "${_rt}")
          add_dependencies(${_rt} ${_nest_gate})
        endif()
      endforeach()
    endif()
    message(STATUS "Win32 nest gate for Studio/runtimes: ${_nest_gate}")
  endif()

  message(STATUS
    "Win32 nest: ${_n} project(s) under Win32/... (Explorer, excluded from default build); "
    "default nest build via OpenDCL_Win32 only when IN_ALL=${OPENDCL_WIN32_IN_ALL} "
    "(no parallel OpenDCL_Res_Win32 / OpenDCL_RxInstall nest builds)")
endfunction()
