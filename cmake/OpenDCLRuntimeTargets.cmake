# Factory: create one SHARED runtime module from registry props + shared sources.
include_guard(GLOBAL)

include(OpenDCLRuntimeSources)

function(opendcl_expand_sdk_paths root rel_list out_var)
  set(_paths)
  foreach(_rel IN LISTS rel_list)
    if(_rel)
      list(APPEND _paths "${root}/${_rel}")
    endif()
  endforeach()
  set(${out_var} "${_paths}" PARENT_SCOPE)
endfunction()

# Matrix list fields use '|' so CMake ';' list rules do not eat values.
function(opendcl_split_bar input out_var)
  set(_list)
  if(input)
    string(REPLACE "|" ";" _list "${input}")
  endif()
  set(${out_var} "${_list}" PARENT_SCOPE)
endfunction()

function(opendcl_add_runtime id)
  opendcl_rt_get("${id}" FAMILY _family)
  opendcl_rt_get("${id}" ARCH _arch)
  opendcl_rt_get("${id}" EXT _ext)
  opendcl_rt_get("${id}" OUTPUT_NAME _output)
  opendcl_rt_get("${id}" TOOLSET _toolset)
  opendcl_rt_get("${id}" CHARACTER_SET _charset)
  opendcl_rt_get("${id}" SDK_ENV _sdk_env)
  opendcl_rt_get("${id}" SDK_INC _sdk_inc)
  opendcl_rt_get("${id}" SDK_LIB _sdk_lib)
  opendcl_rt_get("${id}" SDK_LIB_FULLDEBUG _sdk_lib_fd)
  opendcl_rt_get("${id}" VI_DIR _vi_dir)
  opendcl_rt_get("${id}" DEFINES _defines)
  opendcl_rt_get("${id}" LIBS _libs)
  opendcl_rt_get("${id}" WARNING_DISABLES _wd)
  opendcl_rt_get("${id}" CXX_STANDARD _cxxstd)
  opendcl_rt_get("${id}" EXTRA_INCLUDES _extra_inc)
  opendcl_rt_get("${id}" EXTRA_LIBDIRS _extra_libdirs)
  opendcl_rt_get("${id}" COMPILE_OPTIONS _compile_opts)
  opendcl_rt_get("${id}" LINK_OPTIONS _link_opts)
  if(NOT _charset)
    set(_charset "Unicode")
  endif()
  set(_opendcl_pin_winsdk_after_libdirs FALSE)
  set(_opendcl_legacy_winsdk "")

  opendcl_resolve_sdk_root("${_sdk_env}" _sdk_root)
  if(NOT _sdk_root OR NOT EXISTS "${_sdk_root}")
    message(FATAL_ERROR "opendcl_add_runtime(${id}): SDK ${_sdk_env} not found")
  endif()

  # FullDebug host debug libdirs are MSBuild-time (*.libdirs.props + optional
  # parent dev.props / local.props), not configure-time discovery. Per-host
  # defaults live in opendcl_fulldebug_libdir_msbuild (macros or empty). Log only
  # absolute cache/env overrides — not empty or $(MACRO) defaults.
  opendcl_fulldebug_libdir_msbuild("${_family}" "${_sdk_env}" "${_toolset}" _fd_lib_ms)
  if(_fd_lib_ms AND NOT _fd_lib_ms MATCHES "^\\$\\(")
    message(STATUS "  ${id}: FullDebug lib override = ${_fd_lib_ms}")
  endif()

  string(REPLACE "." "_" _safe "${id}")
  set(_target "OpenDCL_Runtime_${_safe}")

  # Shared sources + filters + optional TargetSpecific/<id>/ injection.
  set(_sources ${OPENDCL_RUNTIME_SOURCES})
  opendcl_rt_get("${id}" VERSION _ver)

  # ARX R16: linetype control is header-only (ArxR16LinetypeComboBoxCtrl.h via #if).
  # Do not compile the AcUi-based ArxLinetypeComboBoxCtrl.cpp for _ARXTARGET < 17.
  if(_family STREQUAL "ARX" AND _ver MATCHES "^[0-9]+$" AND _ver LESS 17)
    list(FILTER _sources EXCLUDE REGEX "ArxLinetypeComboBoxCtrl\\.cpp$")
  endif()

  # Optional target-specific TUs + shared-source drops:
  #   Runtime/TargetSpecific/<id>/*.{cpp,cxx,cc}  — extra sources for this ID only
  #   Runtime/TargetSpecific/<id>/drop.txt        — basenames of shared TUs to omit
  #     (classic ExcludedFromBuild). No file / empty → no drops.
  set(_ts_dir "${CMAKE_SOURCE_DIR}/Runtime/TargetSpecific/${id}")
  set(_ts_drop "${_ts_dir}/drop.txt")
  if(EXISTS "${_ts_drop}")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_ts_drop}")
    file(STRINGS "${_ts_drop}" _drop_lines)
    list(LENGTH _sources _sources_before_drop)
    foreach(_line IN LISTS _drop_lines)
      string(STRIP "${_line}" _line)
      if(_line STREQUAL "" OR _line MATCHES "^#")
        continue()
      endif()
      # Basename only (AcadPaletteHost.cpp); reject path separators.
      if(_line MATCHES "[/\\\\]")
        message(FATAL_ERROR
          "${id}: TargetSpecific drop.txt entries must be basenames only (got '${_line}')")
      endif()
      # Match forward- or backslash-separated paths ending in this basename.
      list(FILTER _sources EXCLUDE REGEX "[/\\\\]${_line}$")
    endforeach()
    list(LENGTH _sources _sources_after_drop)
    math(EXPR _drop_count "${_sources_before_drop}-${_sources_after_drop}")
    if(_drop_count GREATER 0)
      message(STATUS "  ${id}: -${_drop_count} shared source(s) via TargetSpecific/${id}/drop.txt")
    endif()
  endif()

  set(_ts_sources)
  if(EXISTS "${_ts_dir}")
    file(GLOB _ts_sources CONFIGURE_DEPENDS
      "${_ts_dir}/*.cpp"
      "${_ts_dir}/*.cxx"
      "${_ts_dir}/*.cc"
    )
    list(SORT _ts_sources)
    list(APPEND _sources ${_ts_sources})
    list(LENGTH _ts_sources _ts_count)
    if(_ts_count GREATER 0)
      message(STATUS "  ${id}: +${_ts_count} file(s) from Runtime/TargetSpecific/${id}")
    endif()
  endif()

  add_library(${_target} SHARED
    ${_sources}
    ${OPENDCL_RUNTIME_RC}
  )

  opendcl_split_bar("${_defines}" _def_list)
  opendcl_split_bar("${_sdk_inc}" _sdk_inc_list)
  opendcl_split_bar("${_sdk_lib}" _sdk_lib_list)
  opendcl_split_bar("${_libs}" _lib_list)
  opendcl_split_bar("${_wd}" _wd_list)

  opendcl_expand_sdk_paths("${_sdk_root}" "${_sdk_inc_list}" _inc_paths)
  opendcl_expand_sdk_paths("${_sdk_root}" "${_sdk_lib_list}" _lib_paths)

  # Match historical vcxproj AdditionalIncludeDirectories (project + props).
  target_include_directories(${_target} PRIVATE
    "${CMAKE_SOURCE_DIR}/Runtime"
    "${CMAKE_SOURCE_DIR}/Runtime/Controls"
    "${CMAKE_SOURCE_DIR}/Runtime/Methods"
    "${CMAKE_SOURCE_DIR}/Common"
    "${CMAKE_SOURCE_DIR}/Common/Controls"
    "${CMAKE_SOURCE_DIR}/Library/ZLib"
    "${CMAKE_SOURCE_DIR}/Library/LibPNG"
    "${CMAKE_SOURCE_DIR}/${_vi_dir}"
    ${_inc_paths}
    ${_extra_inc}
  )
  if(EXISTS "${_ts_dir}")
    target_include_directories(${_target} PRIVATE "${_ts_dir}")
  endif()

  # Shared project-level defines from historical Runtime vcxproj ItemDefinitionGroup.
  # MFC extension DLL: _AFXDLL + _AFXEXT (do NOT define _USRDLL — that pulls dllmodul DllMain).
  #
  # Debug vs FullDebug (see Runtime/StdAfx.h "DEBUG workaround"):
  #   Debug:     _DEBUG, no AC_FULL_DEBUG → PCH #undef _DEBUG around MFC/ATL/STL; /MD
  #   FullDebug: _DEBUG + AC_FULL_DEBUG → full host/MFC debug CRT (/MDd) + host debug libs
  #   Release:   NDEBUG; /MD
  #
  # FullDebug is for developers with a local debug host build. All families use
  # /MDd (CMake policy); classic ARX FullDebug was hybrid /MD — we intentionally
  # use /MDd for every runtime module so zlib/png FullDebug (/MDd) link cleanly.
  #
  # FullDebug host debug libraries are proprietary: paths must be set explicitly
  # (see opendcl_resolve_fulldebug_libdirs). Do not scan debug SDK trees.
  set(_fulldebug_defs _DEBUG AC_FULL_DEBUG)
  if(_family STREQUAL "BRX")
    # BRX sample FullDebug also defines BRX_BCAD_DEBUG.
    list(APPEND _fulldebug_defs BRX_BCAD_DEBUG)
  endif()
  # Character set: default Unicode (classic modern modules). MultiByte only where
  # classic was MBCS — ARX.16 (ACHAR) and ZRX.2014 (ZWSoft strlen on ZTCHAR*).
  # Item 4 of 8 locked: matrix CHARACTER_SET, not a project-wide undef.
  set(_charset_defs)
  if(_charset STREQUAL "MultiByte")
    set(_vs_charset "MultiByte")
  else()
    set(_vs_charset "Unicode")
    list(APPEND _charset_defs UNICODE _UNICODE)
  endif()
  target_compile_definitions(${_target} PRIVATE
    ${_def_list}
    _WINDOWS
    _AFXDLL
    _AFXEXT
    _ACRXAPP
    ARX_MODULE
    _CRT_SECURE_NO_DEPRECATE
    _CRT_STDIO_LEGACY_WIDE_SPECIFIERS=1
    _PRERELEASE
    ${_charset_defs}
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:FullDebug>:${_fulldebug_defs}>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<STREQUAL:${_arch},x64>:_WIN64>
  )

  # CRT: Debug+Release /MD (host release). FullDebug /MDd for all families.
  set(_rt_lib "MultiThreaded$<$<CONFIG:FullDebug>:Debug>DLL")

  # Mirror classic layout so Common/Workspace.cpp F5 debug loading finds Runtime.Res:
  #   Runtime/<FAMILY>/<ID>/<Config>/OpenDCL.*.brx|arx|...
  #   Runtime/Localized/<LANG>/Runtime.Res/Debug/Runtime.Res.dll
  # (debugger path strips two folders then uses ..\Localized\...\Runtime.Res\Debug\)
  set(_out_mod "${OPENDCL_OUTPUT_ROOT}/Runtime/${_family}/${id}/$<CONFIG>")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_sol_arch "x64")
  else()
    set(_sol_arch "Win32")
  endif()
  # id e.g. ARX.18 / ARX.18.x64 → folder x64|Win32/Runtime/ARX
  opendcl_solution_folder(_sol_folder "${_sol_arch}" "OpenDCL_Runtime_${id}")

  set_target_properties(${_target} PROPERTIES
    OUTPUT_NAME "${_output}"
    PREFIX ""
    SUFFIX "${_ext}"
    RUNTIME_OUTPUT_DIRECTORY "${_out_mod}"
    LIBRARY_OUTPUT_DIRECTORY "${_out_mod}"
    ARCHIVE_OUTPUT_DIRECTORY "${_out_mod}"
    PDB_OUTPUT_DIRECTORY "${_out_mod}"
    MSVC_RUNTIME_LIBRARY "${_rt_lib}"
    VS_GLOBAL_UseOfMfc "Dynamic"
    VS_GLOBAL_CharacterSet "${_vs_charset}"
    # Classic *.vcxproj: GenerateManifest=false (CAD modules are not apps).
    # CMake default true → link /MANIFEST + mt.exe → TRK0005 on old toolsets.
    VS_GLOBAL_GenerateManifest "false"
    FOLDER "${_sol_folder}"
  )

  # F5 host CAD (optional): machine-local via .user, cache, or env (BRX_EXE, …).
  opendcl_resolve_debugger_command("${_family}" _dbg_cmd)
  if(_dbg_cmd)
    set_property(TARGET ${_target} PROPERTY VS_DEBUGGER_COMMAND "${_dbg_cmd}")
  endif()

  # F5 command *arguments*: bake the real module path with a generator expression.
  # Do NOT use MSBuild $(TargetPath) here — with Inherit / property sheets / .user
  # it often expands empty (VS history shows /ld ""), so the host never loads the
  # module. $<TARGET_FILE:…> is written per-config into the .vcxproj as a full path.
  opendcl_resolve_debugger_arguments("${_family}" _dbg_args_template)
  if(_dbg_args_template)
    # Replace $(TargetPath) / ${TargetPath} placeholders with genex; otherwise
    # treat the whole string as literal args (no path injection).
    set(_dbg_args "${_dbg_args_template}")
    string(REPLACE "$(TargetPath)" "$<TARGET_FILE:${_target}>" _dbg_args "${_dbg_args}")
    string(REPLACE "\${TargetPath}" "$<TARGET_FILE:${_target}>" _dbg_args "${_dbg_args}")
    set_property(TARGET ${_target} PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "${_dbg_args}")
  endif()

  if(_toolset)
    set_property(TARGET ${_target} PROPERTY VS_PLATFORM_TOOLSET "${_toolset}")
    # Item 3 option C (locked): keep classic toolsets (e.g. v140) on a host that
    # defaults to a newer Windows kit. Pin kit via ForceImportBeforeCppProps
    # (opendcl_vs_pin_windows_sdk) — NOT late VS_USER_PROPS. Validated ARX.21/22
    # x64 + Win32 Release with no /I or /FI UCRT shims.
    # Override kit: -DOPENDCL_LEGACY_WINDOWS_SDK=10.0.19041.0
    # If still too fragile, fall back to matrix toolset v141+ (option A).
    if(_toolset MATCHES "^v(70|80|90|100|110|120|140)")
      set(_opendcl_legacy_winsdk "10.0.19041.0")
      if(DEFINED OPENDCL_LEGACY_WINDOWS_SDK AND OPENDCL_LEGACY_WINDOWS_SDK)
        set(_opendcl_legacy_winsdk "${OPENDCL_LEGACY_WINDOWS_SDK}")
      endif()
      set(_opendcl_pin_winsdk_after_libdirs TRUE)
    endif()
  endif()

  # Classic modules never generate/embed manifests (avoids mt.exe TRK0005).
  # Applied after optional SDK pin so modern rows get nomanifest.props only.
  # (Pin path already disables manifest in *.winsdk-tools.props.)

  if(_cxxstd)
    set_target_properties(${_target} PROPERTIES
      CXX_STANDARD ${_cxxstd}
      CXX_STANDARD_REQUIRED ON
    )
  endif()

  # PCH (StdAfx)
  target_precompile_headers(${_target} PRIVATE
    "${CMAKE_SOURCE_DIR}/Runtime/StdAfx.h"
  )
  # StdAfx.cpp creates the PCH in MSVC classic workflow; with cmake PCH,
  # still compile StdAfx.cpp as a normal TU (header already in sources).

  # /FS: MSBuild /m runs multiple cl.exe per project; without /FS they race on
  # the shared vc*.pdb → C1041 (and can surface as Permission denied on .obj).
  # /FS exists VS2013+ (v120). Older toolsets (v70–v110) omit it.
  if(MSVC)
    set(_opendcl_fs TRUE)
    if(_toolset MATCHES "^v([0-9]+)")
      if(CMAKE_MATCH_1 LESS 120)
        set(_opendcl_fs FALSE)
      endif()
    elseif(_toolset MATCHES "^v110")
      set(_opendcl_fs FALSE)
    endif()
    if(_opendcl_fs)
      target_compile_options(${_target} PRIVATE "/FS")
    endif()
  endif()

  foreach(_w IN LISTS _wd_list)
    if(_w)
      target_compile_options(${_target} PRIVATE "/wd${_w}")
    endif()
  endforeach()

  if(_compile_opts)
    target_compile_options(${_target} PRIVATE ${_compile_opts})
  endif()
  if(_link_opts)
    target_link_options(${_target} PRIVATE ${_link_opts})
  endif()

  # LNK4099: missing PDB for SDK import libs (e.g. rxapi.lib / BRX imports).
  # /IGNORE:4099 is reliable on MSVC toolsets v100+ (VS2010). Older linkers
  # (v70/v80/v90) may reject the switch — skip there.
  if(_toolset MATCHES "^v([0-9]+)")
    if(CMAKE_MATCH_1 GREATER_EQUAL 100)
      target_link_options(${_target} PRIVATE "/IGNORE:4099")
    endif()
  endif()

  # Common Windows / media libs from historical vcxproj
  target_link_libraries(${_target} PRIVATE
    rpcrt4
    vfw32
    ${_lib_list}
  )

  # Library directories via generated .props (not target_link_directories).
  # The VS generator turns target_link_directories into:
  #   path;path\$(Configuration);%(AdditionalLibraryDirectories)
  # which is not classic.
  #
  # We emit: FullDebug → <host-debug if set>;<release-SDK-lib>;%(AdditionalLibraryDirectories)
  #          Debug/Release → <release-SDK-lib>;%(AdditionalLibraryDirectories)
  # Host-debug for BRX is only from BrxDebugLibs / OPENDCL_*_FULLDEBUG_LIBDIR
  # (see opendcl_fulldebug_libdir_msbuild). Do not GLOB/list proprietary debug trees.
  set(_all_release_dirs ${_lib_paths} ${_extra_libdirs})
  opendcl_vs_attach_libdir_props(${_target} "${_all_release_dirs}" "${_fd_lib_ms}")

  # After libdirs VS_USER_PROPS exists, optionally wrap with Windows SDK pin.
  if(_opendcl_pin_winsdk_after_libdirs)
    opendcl_vs_pin_windows_sdk(${_target} "${_opendcl_legacy_winsdk}")
  endif()
  # Classic CAD modules: no embed-manifest / mt.exe (pin path already sets this).
  opendcl_vs_disable_manifest(${_target})

  # No /NODEFAULTLIB CRT hacks: module, MFC (/MD vs /MDd), and static deps
  # (zlib/png *-md) must all use the same runtime library per config.

  # Arch + toolset-matched third-party static libs (see Library/CMakeLists.txt).
  # Older toolsets (≤ v140) cannot link default VS2022 UCRT-built zlib/png.
  if(_arch STREQUAL "x64")
    set(_png_arch "x64")
  else()
    set(_png_arch "x86")
  endif()
  opendcl_ensure_runtime_png("${_png_arch}" "${_toolset}" _zlib_tgt _png_tgt)
  target_link_libraries(${_target} PRIVATE ${_zlib_tgt} ${_png_tgt})

  # Default ENU runtime resources when present natively in this tree.
  # classic_x86 / full nest gate is attached later in opendcl_add_win32_nest
  # (OpenDCL_Res_Win32 / OpenDCL_Win32 do not exist yet at add_runtime time).
  if(TARGET OpenDCL_RuntimeRes_ENU)
    add_dependencies(${_target} OpenDCL_RuntimeRes_ENU)
  endif()

  # Optional override file: cmake/overrides/<id>.cmake defining opendcl_apply_override(target)
  set(_override "${CMAKE_SOURCE_DIR}/cmake/overrides/${id}.cmake")
  if(EXISTS "${_override}")
    include("${_override}")
    if(COMMAND opendcl_apply_override)
      opendcl_apply_override(${_target})
      # Avoid leaking the function name into the next target
      function(opendcl_apply_override)
      endfunction()
    endif()
  endif()

  # Friendly alias
  add_library(OpenDCL::${id} ALIAS ${_target})
endfunction()

function(opendcl_add_selected_runtimes)
  opendcl_select_runtimes(_enabled _skipped)
  opendcl_print_runtime_summary("${_enabled}" "${_skipped}")
  foreach(_id IN LISTS _enabled)
    opendcl_add_runtime("${_id}")
  endforeach()
  set(OPENDCL_ENABLED_RUNTIMES "${_enabled}" CACHE INTERNAL "" FORCE)
endfunction()
