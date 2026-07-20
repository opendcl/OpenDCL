# OpenDCL CMake helpers — options, SDK detection, registry, selection.
include_guard(GLOBAL)

# Enable target FOLDER → Visual Studio solution folders (Library/ZLib, …).
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(OPENDCL_BUILD_RUNTIME "Build CAD runtime modules" ON)
option(OPENDCL_BUILD_STUDIO "Build OpenDCL Studio.exe + Studio.Res (static MFC)" OFF)
option(OPENDCL_BUILD_RXINSTALL "Build RxInstall CA (Win32 nested from x64; sources in main .sln)" ON)
option(OPENDCL_BUILD_HELP "Build HTML Help projects" OFF)
# CHM is arch-independent (hhc → Studio/Localized/<lang>/Content/OpenDCL.chm).
# Full Mixed nest forces this OFF so only the parent .sln owns help targets.
option(OPENDCL_BUILD_STUDIO_HELP
  "Build Studio OpenDCL.chm targets via hhc (arch-independent; one tree only)" ON)

# Classic Runtime.Res / Studio.Res ship as Win32 (x86) PE under CommonFilesFolder.
# Public Mixed release forces classic x86; host-arch Res remains available for
# single-arch / future packaging (item 5: lock classic_x86 for public, path open).
option(OPENDCL_BUILD_RES_DLLS
  "Build Runtime.Res + Studio.Res language DLLs (native and/or nested)" ON)

# Resource DLL PE policy:
#   classic_x86 — Runtime.Res is always x86 (public Mixed / vs2022-full). On x64,
#                 nest OpenDCL_Res_Win32 for Runtime.Res only. Studio.Res always
#                 matches Studio PE (not this flag).
#   host        — Runtime.Res PE matches this configure (x64 on x64, x86 on Win32).
set(OPENDCL_RES_PE "host" CACHE STRING
  "Resource DLL PE: classic_x86 (always x86 ship) or host (match configure arch)")
set_property(CACHE OPENDCL_RES_PE PROPERTY STRINGS classic_x86 host)
if(NOT OPENDCL_RES_PE STREQUAL "classic_x86" AND NOT OPENDCL_RES_PE STREQUAL "host")
  message(FATAL_ERROR
    "OPENDCL_RES_PE must be classic_x86 or host (got '${OPENDCL_RES_PE}')")
endif()

# VS generators are one platform per configure. On an x64 configure, optionally
# nest a Win32 binary dir, import its .vcxproj into the parent .sln (Explorer +
# normal MSBuild), and share OPENDCL_OUTPUT_ROOT (see cmake/OpenDCLNestWin32.cmake).
option(OPENDCL_NEST_WIN32
  "When configuring x64 with the VS generator, also nest Win32 under <bin>/win32 and import projects into the parent .sln" OFF)
option(OPENDCL_WIN32_IN_ALL
  "Include nested Win32 projects in ALL_BUILD / default solution build (full ship); OFF = Explorer only unless you build OpenDCL_Win32 or a Win32 project" OFF)
# Full dual-arch nest compiles many old toolsets (v100/v110/...) under one MSBuild.
# Unbounded /m + /MP OOMs 32-bit cl (C1060 heap) and triggers C1001 ICEs.
set(OPENDCL_NEST_MSBUILD_MAX_CPU_COUNT "2" CACHE STRING
  "MSBuild /m for cmake --build of the Win32 nest (OpenDCL_Win32 / Res / RxInstall); raise carefully")
set(OPENDCL_NEST_CL_MP_COUNT "1" CACHE STRING
  "CL_MPCount for nest builds (cl /MP within each project); 1 is safest for old toolsets")

# Solution Explorer folder for a product target (classic-style, not arch-first).
#
# Dual-arch Mixed / x64-full puts x64 and Win32 (w32_*) peers in the *same*
# product folders — similar to classic OpenDCL.sln:
#   Runtime/Rx/{ARX,BRX,GRX,ZRX}
#   Runtime/Localized Resources
#   Runtime                          (RxInstall)
#   Studio[/LANG]                    (exe, Studio.Res, Help by language)
#   Library/{ZLib,LibPNG}
#   CMake                            (OpenDCL_Win32 nest bulk, ZERO_CHECK helpers)
#
# arch_label is kept for call-site compatibility but does not affect the path
# (arch is already in target names: …_x64, w32_*, opendcl_zlib_x86_…).
function(opendcl_solution_folder out_var arch_label base)
  if(base MATCHES "OpenDCL_Runtime_ARX" OR base MATCHES "_Runtime_ARX_")
    set(_f "Runtime/Rx/ARX")
  elseif(base MATCHES "OpenDCL_Runtime_BRX" OR base MATCHES "_Runtime_BRX_")
    set(_f "Runtime/Rx/BRX")
  elseif(base MATCHES "OpenDCL_Runtime_GRX" OR base MATCHES "_Runtime_GRX_")
    set(_f "Runtime/Rx/GRX")
  elseif(base MATCHES "OpenDCL_Runtime_ZRX" OR base MATCHES "_Runtime_ZRX_")
    set(_f "Runtime/Rx/ZRX")
  elseif(base MATCHES "opendcl_zlib" OR base MATCHES "/ZLib" OR base STREQUAL "ZLib")
    set(_f "Library/ZLib")
  elseif(base MATCHES "opendcl_png" OR base MATCHES "LibPNG")
    set(_f "Library/LibPNG")
  elseif(base MATCHES "RuntimeRes")
    set(_f "Runtime/Localized Resources")
  elseif(base MATCHES "Res_Win32" OR base STREQUAL "OpenDCL_Res_Win32")
    set(_f "Runtime/Localized Resources")
  elseif(base MATCHES "StudioRes_([A-Za-z][A-Za-z0-9]+)")
    # Classic: Studio.Res.ENU under Studio/ENU
    set(_f "Studio/${CMAKE_MATCH_1}")
  elseif(base MATCHES "StudioRes")
    set(_f "Studio")
  elseif(base MATCHES "StudioHelp_([A-Za-z][A-Za-z0-9]+)")
    # Classic HTMLHelp.<LANG> under Studio/<LANG>
    set(_f "Studio/${CMAKE_MATCH_1}")
  elseif(base MATCHES "StudioHelp")
    set(_f "Studio")
  elseif(base MATCHES "Studio" OR base STREQUAL "OpenDCL_Studio")
    set(_f "Studio")
  elseif(base MATCHES "RxInstall")
    set(_f "Runtime")
  elseif(base STREQUAL "OpenDCL_Win32" OR base MATCHES "^OpenDCL_Win32")
    set(_f "CMake")
  elseif(base MATCHES "ZERO_CHECK" OR base MATCHES "/CMake" OR base STREQUAL "CMake")
    set(_f "CMake")
  else()
    set(_f "")
  endif()
  set(${out_var} "${_f}" PARENT_SCOPE)
endfunction()

# True when this binary dir should create native Runtime.Res targets.
# classic_x86 on x64: Runtime.Res is x86 (OpenDCL_Res_Win32 / full nest) for CAD
# CommonFiles; Studio.Res is separate (always matches Studio PE — see below).
function(opendcl_runtime_res_build_native out_var)
  if(NOT OPENDCL_BUILD_RES_DLLS AND NOT OPENDCL_BUILD_RUNTIME AND NOT OPENDCL_BUILD_STUDIO)
    set(${out_var} FALSE PARENT_SCOPE)
    return()
  endif()
  if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    set(${out_var} TRUE PARENT_SCOPE)
  elseif(OPENDCL_RES_PE STREQUAL "host")
    set(${out_var} TRUE PARENT_SCOPE)
  else()
    # classic_x86 on x64: nest x86 Runtime.Res only.
    set(${out_var} FALSE PARENT_SCOPE)
  endif()
endfunction()

# Studio.Res is loaded into Studio.exe (LoadLibrary) — PE must match Studio.
# Always build natively for this configure (x64 Studio → x64 Studio.Res).
# Win32 nest uses a Win32 subfolder under out so it does not overwrite x64 PE.
function(opendcl_studio_res_build_native out_var)
  if(NOT OPENDCL_BUILD_RES_DLLS AND NOT OPENDCL_BUILD_STUDIO)
    set(${out_var} FALSE PARENT_SCOPE)
  else()
    set(${out_var} TRUE PARENT_SCOPE)
  endif()
endfunction()

# Back-compat name: Runtime.Res policy (not Studio.Res).
function(opendcl_res_build_native out_var)
  opendcl_runtime_res_build_native(_v)
  set(${out_var} "${_v}" PARENT_SCOPE)
endfunction()

# True when x64 parent must use a *dedicated* res-win32 nest for classic x86 Res.
# When OPENDCL_NEST_WIN32 is ON, Res come from the full Win32 nest instead.
function(opendcl_res_need_win32_nest out_var)
  if(OPENDCL_NEST_WIN32)
    set(${out_var} FALSE PARENT_SCOPE)
    return()
  endif()
  if(CMAKE_SIZEOF_VOID_P EQUAL 8
      AND OPENDCL_BUILD_RES_DLLS
      AND OPENDCL_RES_PE STREQUAL "classic_x86"
      AND MSVC
      AND CMAKE_GENERATOR MATCHES "Visual Studio")
    set(${out_var} TRUE PARENT_SCOPE)
  else()
    set(${out_var} FALSE PARENT_SCOPE)
  endif()
endfunction()

option(OPENDCL_ENABLE_ARX "Consider AutoCAD (ARX) runtime targets" ON)
option(OPENDCL_ENABLE_BRX "Consider BricsCAD (BRX) runtime targets" ON)
option(OPENDCL_ENABLE_GRX "Consider GstarCAD (GRX) runtime targets" ON)
option(OPENDCL_ENABLE_ZRX "Consider ZWCAD (ZRX) runtime targets" ON)

set(OPENDCL_RUNTIME_TARGETS "" CACHE STRING
  "Semicolon-separated runtime IDs to build (empty = all enabled families). Example: ARX.26.x64;BRX.26.x64")

option(OPENDCL_RUNTIME_AUTO
  "Skip runtime targets whose CAD SDK root is not available" ON)
option(OPENDCL_RUNTIME_REQUIRE_SELECTED
  "Fail configure if a selected runtime target cannot be built" OFF)

# FullDebug host *debug* import libraries.
#
# FullDebug uses AC_FULL_DEBUG + /MDd (all families) and must link host debug libs.
# Those trees are proprietary — do NOT auto-scan or "discover" them.
# Configure an explicit path only:
#   - OPENDCL_<SDK_ENV>_FULLDEBUG_LIBDIR  (cache)
#   - ENV{<SDK_ENV>_FULLDEBUG_LIBDIR}
#   - ENV{BrxDebugLibs}                   (BRX samples / OpenDCL BRX FullDebug)
#   - optional matrix SDK_LIB_FULLDEBUG relative to release SDK root
#     (documented convention only, e.g. lib/vc143x64/Debug from BRX.26.vcxproj)
#
# Never GLOB/list proprietary debug folders in CMake or tooling.

set(OPENDCL_LANGS "ENU" CACHE STRING
  "Semicolon-separated language codes for resource DLLs (ENU;DEU;...)")

set(OPENDCL_OUTPUT_ROOT "${CMAKE_BINARY_DIR}/out" CACHE PATH
  "Root directory for built binaries (package layout)")

# ---------------------------------------------------------------------------
# FullDebug → Debug layout (everything except CAD runtime modules)
# ---------------------------------------------------------------------------
# Solution FullDebug exists for host plugins (ARX/BRX/GRX/ZRX): AC_FULL_DEBUG,
# /MDd, host debug libs. All other targets (Studio, Res DLLs, RxInstall,
# Studio-only /MT libs) only have classic Debug|Release products — map FullDebug
# outputs into the Debug folder so F5 never looks for a separate FullDebug artifact.
# Generator expression for post-build / mirror paths:
set(OPENDCL_CFG_DIR "$<IF:$<CONFIG:FullDebug>,Debug,$<CONFIG>>")

# Map RUNTIME/LIBRARY/ARCHIVE/PDB output dirs: FullDebug writes beside Debug.
function(opendcl_map_fulldebug_to_debug target out_base)
  foreach(_kind IN ITEMS RUNTIME LIBRARY ARCHIVE PDB)
    set_target_properties(${target} PROPERTIES
      ${_kind}_OUTPUT_DIRECTORY_DEBUG "${out_base}/Debug"
      ${_kind}_OUTPUT_DIRECTORY_FULLDEBUG "${out_base}/Debug"
      ${_kind}_OUTPUT_DIRECTORY_RELEASE "${out_base}/Release"
    )
  endforeach()
endfunction()

# Visual Studio F5: host CAD + load built module (classic /ld "$(TargetPath)").
# ARX and BRX use the same load switch; GRX/ZRX default to the same and can override.
set(OPENDCL_DEBUGGER_COMMAND "" CACHE FILEPATH
  "Default host CAD executable for F5 (empty = try per-family env, else leave unset)")
# Use $(TargetPath) as a *placeholder* only: CMake rewrites it to
# $<TARGET_FILE:…> so the .vcxproj gets a real per-config path (VS often
# expands bare $(TargetPath) to empty for inherited debugger args).
set(OPENDCL_DEBUGGER_COMMAND_ARGUMENTS "/ld \"$(TargetPath)\"" CACHE STRING
  "Default F5 args; $(TargetPath) is replaced with the built module path at generate time")

set(OPENDCL_ARX_DEBUGGER_COMMAND "" CACHE FILEPATH "Override F5 host for ARX (default: OPENDCL_DEBUGGER_COMMAND or ENV{ARX_EXE}/ENV{ACAD})")
set(OPENDCL_BRX_DEBUGGER_COMMAND "" CACHE FILEPATH "Override F5 host for BRX (default: OPENDCL_DEBUGGER_COMMAND or ENV{BRX_EXE}/ENV{BRICSCAD})")
set(OPENDCL_GRX_DEBUGGER_COMMAND "" CACHE FILEPATH "Override F5 host for GRX (default: OPENDCL_DEBUGGER_COMMAND or ENV{GRX_EXE})")
set(OPENDCL_ZRX_DEBUGGER_COMMAND "" CACHE FILEPATH "Override F5 host for ZRX (default: OPENDCL_DEBUGGER_COMMAND or ENV{ZRX_EXE})")

set(OPENDCL_ARX_DEBUGGER_COMMAND_ARGUMENTS "" CACHE STRING "Override F5 args for ARX (empty = OPENDCL_DEBUGGER_COMMAND_ARGUMENTS)")
set(OPENDCL_BRX_DEBUGGER_COMMAND_ARGUMENTS "" CACHE STRING "Override F5 args for BRX (empty = OPENDCL_DEBUGGER_COMMAND_ARGUMENTS)")
set(OPENDCL_GRX_DEBUGGER_COMMAND_ARGUMENTS "" CACHE STRING "Override F5 args for GRX (empty = OPENDCL_DEBUGGER_COMMAND_ARGUMENTS)")
set(OPENDCL_ZRX_DEBUGGER_COMMAND_ARGUMENTS "" CACHE STRING "Override F5 args for ZRX (empty = OPENDCL_DEBUGGER_COMMAND_ARGUMENTS)")

# ---------------------------------------------------------------------------
# Registry storage
# ---------------------------------------------------------------------------
# Each registered runtime is stored as OPENDCL_RT_<id>_<FIELD>
# and listed in OPENDCL_RT_IDS.

set(OPENDCL_RT_IDS "" CACHE INTERNAL "Registered runtime target IDs")

function(opendcl_register_runtime)
  set(options)
  set(oneValueArgs
    ID FAMILY VERSION ARCH EXT OUTPUT_NAME TOOLSET CHARACTER_SET
    SDK_ENV SDK_INC SDK_LIB SDK_LIB_FULLDEBUG VI_DIR DEFINES LIBS
    WARNING_DISABLES CXX_STANDARD)
  set(multiValueArgs PROFILES EXTRA_INCLUDES EXTRA_LIBDIRS COMPILE_OPTIONS LINK_OPTIONS)
  cmake_parse_arguments(RT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT RT_ID)
    message(FATAL_ERROR "opendcl_register_runtime: ID is required")
  endif()

  # Default CharacterSet matches classic Unicode modules.
  # MultiByte only via matrix (ARX.16, ZRX.2014) — item 4 locked.
  if(NOT RT_CHARACTER_SET)
    set(RT_CHARACTER_SET "Unicode")
  endif()

  set(_ids ${OPENDCL_RT_IDS})
  list(APPEND _ids "${RT_ID}")
  list(REMOVE_DUPLICATES _ids)
  set(OPENDCL_RT_IDS "${_ids}" CACHE INTERNAL "Registered runtime target IDs" FORCE)

  foreach(_f IN ITEMS
      FAMILY VERSION ARCH EXT OUTPUT_NAME TOOLSET CHARACTER_SET
      SDK_ENV SDK_INC SDK_LIB SDK_LIB_FULLDEBUG VI_DIR DEFINES LIBS
      WARNING_DISABLES CXX_STANDARD)
    set(OPENDCL_RT_${RT_ID}_${_f} "${RT_${_f}}" CACHE INTERNAL "" FORCE)
  endforeach()

  foreach(_f IN ITEMS PROFILES EXTRA_INCLUDES EXTRA_LIBDIRS COMPILE_OPTIONS LINK_OPTIONS)
    set(OPENDCL_RT_${RT_ID}_${_f} "${RT_${_f}}" CACHE INTERNAL "" FORCE)
  endforeach()
endfunction()

# FullDebug host-debug lib dir as an MSBuild AdditionalLibraryDirectories *prefix*
# (the part before ";%(AdditionalLibraryDirectories)").
#
# BRX FullDebug (explicit only — proprietary debug trees, never auto-discovered):
#   OPENDCL_<SDK_ENV>_FULLDEBUG_LIBDIR  (CMake cache)
#   ENV{<SDK_ENV>_FULLDEBUG_LIBDIR}
#   ENV{BrxDebugLibs}                   (shared BRX debug lib dir)
#
# Do not invent BRXnn\lib\vc…\Debug under the release SDK root and do not fall
# back to any other env name. Empty prefix → FullDebug links release SDK libs only
# until the operator sets one of the paths above.
function(opendcl_fulldebug_libdir_msbuild family sdk_env toolset out_var)
  set(_prefix "")
  # ${toolset} kept for API stability (callers pass matrix toolset).

  # Explicit absolute override (cache / env) — never auto-discovered.
  set(_cache_var "OPENDCL_${sdk_env}_FULLDEBUG_LIBDIR")
  if(NOT DEFINED ${_cache_var})
    set(${_cache_var} "" CACHE PATH
      "FullDebug host debug LIB directory for ${sdk_env} (proprietary; set explicitly)")
  endif()
  if(${_cache_var})
    file(TO_NATIVE_PATH "${${_cache_var}}" _prefix)
  elseif(DEFINED ENV{${sdk_env}_FULLDEBUG_LIBDIR} AND NOT "$ENV{${sdk_env}_FULLDEBUG_LIBDIR}" STREQUAL "")
    file(TO_NATIVE_PATH "$ENV{${sdk_env}_FULLDEBUG_LIBDIR}" _prefix)
  elseif(family STREQUAL "BRX" AND DEFINED ENV{BrxDebugLibs} AND NOT "$ENV{BrxDebugLibs}" STREQUAL "")
    file(TO_NATIVE_PATH "$ENV{BrxDebugLibs}" _prefix)
  endif()

  set(${out_var} "${_prefix}" PARENT_SCOPE)
endfunction()

# Join a CMake path list into a single MSBuild AdditionalLibraryDirectories value
# (native slashes, no path/$(Configuration) quirk from target_link_directories).
function(opendcl_msbuild_join_libdirs out_var)
  set(_parts)
  foreach(_d IN LISTS ARGN)
    if(_d)
      file(TO_NATIVE_PATH "${_d}" _n)
      list(APPEND _parts "${_n}")
    endif()
  endforeach()
  list(JOIN _parts ";" _joined)
  set(${out_var} "${_joined}" PARENT_SCOPE)
endfunction()

# Emit a VS .props attached via VS_USER_PROPS (PropertySheets / LocalAppDataPlatform).
# Clean AdditionalLibraryDirectories only (no path\$(Configuration) quirk).
# Optional trailing imports: local.props, parent-of-repo dev.props.
#
# F5 debugger *arguments* are set on the target with VS_DEBUGGER_COMMAND_ARGUMENTS
# and $<TARGET_FILE:…> — not here. Early sheets + $(TargetPath) often yield /ld "".
function(opendcl_vs_attach_libdir_props target release_dirs fulldebug_prefix)
  if(NOT CMAKE_GENERATOR MATCHES "Visual Studio")
    if(fulldebug_prefix AND NOT fulldebug_prefix MATCHES "^\\$\\(")
      target_link_directories(${target} BEFORE PRIVATE
        "$<$<CONFIG:FullDebug>:${fulldebug_prefix}>")
    endif()
    if(release_dirs)
      target_link_directories(${target} PRIVATE ${release_dirs})
    endif()
    return()
  endif()

  opendcl_msbuild_join_libdirs(_rel_ms ${release_dirs})
  set(_fd_ms "${fulldebug_prefix}")

  get_filename_component(_opendcl_repo_parent "${CMAKE_SOURCE_DIR}" DIRECTORY)
  set(_dev_props_abs "${_opendcl_repo_parent}/dev.props")
  file(RELATIVE_PATH _dev_props_rel "${CMAKE_CURRENT_BINARY_DIR}" "${_dev_props_abs}")
  string(REPLACE "/" "\\" _dev_props_rel_ms "${_dev_props_rel}")
  set(_local_props_rel "local.props")

  set(_props "${CMAKE_CURRENT_BINARY_DIR}/${target}.libdirs.props")
  set(_xml "")
  string(APPEND _xml "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
  string(APPEND _xml "<Project xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
  string(APPEND _xml "  <!-- Generated by OpenDCL CMake: AdditionalLibraryDirectories only. -->\n")
  string(APPEND _xml "  <!-- Import order: Microsoft.Cpp.*.user.props → this file → local.props → dev.props. -->\n")
  string(APPEND _xml "  <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\"\n")
  string(APPEND _xml "         Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" />\n")

  if(_rel_ms)
    foreach(_cfg IN ITEMS Debug Release)
      string(APPEND _xml "  <ItemDefinitionGroup Condition=\"'$(Configuration)'=='${_cfg}'\">\n")
      string(APPEND _xml "    <Link>\n")
      string(APPEND _xml "      <AdditionalLibraryDirectories>${_rel_ms};%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\n")
      string(APPEND _xml "    </Link>\n")
      string(APPEND _xml "  </ItemDefinitionGroup>\n")
    endforeach()
  endif()

  if(_fd_ms OR _rel_ms)
    set(_fd_joined "")
    if(_fd_ms)
      set(_fd_joined "${_fd_ms}")
    endif()
    if(_rel_ms)
      if(_fd_joined)
        set(_fd_joined "${_fd_joined};${_rel_ms}")
      else()
        set(_fd_joined "${_rel_ms}")
      endif()
    endif()
    string(APPEND _xml "  <ItemDefinitionGroup Condition=\"'$(Configuration)'=='FullDebug'\">\n")
    string(APPEND _xml "    <Link>\n")
    string(APPEND _xml "      <AdditionalLibraryDirectories>${_fd_joined};%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>\n")
    string(APPEND _xml "    </Link>\n")
    string(APPEND _xml "  </ItemDefinitionGroup>\n")
  endif()

  string(APPEND _xml "  <Import Project=\"${_local_props_rel}\"\n")
  string(APPEND _xml "         Condition=\"exists('$(MSBuildThisFileDirectory)${_local_props_rel}')\" />\n")
  string(APPEND _xml "  <Import Project=\"$(SolutionDir)local.props\"\n")
  string(APPEND _xml "         Condition=\"'$(SolutionDir)' != '' and exists('$(SolutionDir)local.props') and '$(SolutionDir)' != '$(MSBuildThisFileDirectory)'\" />\n")
  string(APPEND _xml "  <Import Project=\"${_dev_props_rel_ms}\"\n")
  string(APPEND _xml "         Condition=\"exists('$(MSBuildThisFileDirectory)${_dev_props_rel_ms}')\" />\n")

  string(APPEND _xml "</Project>\n")
  file(WRITE "${_props}" "${_xml}")

  set_target_properties(${target} PROPERTIES VS_USER_PROPS "${_props}")
endfunction()

# Pin WindowsTargetPlatformVersion for a target so old Platform Toolsets (e.g. v140)
# do not pick up a host kit (e.g. 10.0.26100) whose UCRT needs newer intrinsics.
#
# IMPORTANT: VS_USER_PROPS is imported *after* Microsoft.Cpp.props (too late for
# kit include resolution). Use ForceImportBeforeCppProps so the pin runs before
# Cpp.props selects UCRT paths (option C locked / pre-Cpp props sheet).
#
# Kit pin uses TWO generated props sheets:
#   1) ForceImportBeforeCppProps — WindowsTargetPlatformVersion only (UCRT paths).
#   2) ForceImportAfterCppProps  — WindowsSDK_ExecutablePath + APPEND kit bin to
#      ExecutablePath so Tracker finds mt.exe (TRK0005) WITHOUT clobbering VC's
#      CL.exe path (early ExecutablePath freezes an incomplete PATH → mass TRK0005).
#
# Requires the chosen kit to be installed under the Windows Kits root.
# Optional: OPENDCL_LEGACY_WINDOWS_SDK cache/env override (default 10.0.19041.0).
function(opendcl_vs_pin_windows_sdk target version)
  if(NOT target OR NOT version)
    message(FATAL_ERROR "opendcl_vs_pin_windows_sdk(${target} ${version}): need target and version")
  endif()

  set(_kits_root "")
  if(DEFINED ENV{WindowsSdkDir} AND EXISTS "$ENV{WindowsSdkDir}")
    file(TO_CMAKE_PATH "$ENV{WindowsSdkDir}" _kits_root)
  else()
    set(_kits_root "C:/Program Files (x86)/Windows Kits/10")
  endif()
  string(REGEX REPLACE "/$" "" _kits_root "${_kits_root}")

  set(_kit_ucrt "${_kits_root}/Include/${version}/ucrt")
  if(NOT EXISTS "${_kit_ucrt}")
    message(WARNING
      "opendcl_vs_pin_windows_sdk(${target}): Windows SDK ${version} not found under "
      "Windows Kits Include\\${version}\\ucrt — pin may be ignored at build time")
  endif()

  # Versioned kit bin (preferred). Fall back to unversioned bin\x86|x64.
  set(_bin_x86 "${_kits_root}/bin/${version}/x86")
  set(_bin_x64 "${_kits_root}/bin/${version}/x64")
  if(NOT EXISTS "${_bin_x86}/mt.exe" AND NOT EXISTS "${_bin_x64}/mt.exe")
    set(_bin_x86 "${_kits_root}/bin/x86")
    set(_bin_x64 "${_kits_root}/bin/x64")
  endif()
  if(NOT EXISTS "${_bin_x86}/mt.exe" AND NOT EXISTS "${_bin_x64}/mt.exe")
    message(WARNING
      "opendcl_vs_pin_windows_sdk(${target}): mt.exe not found under Windows Kits "
      "bin\\${version} or bin\\{x86,x64} — TRK0005 may still occur")
  endif()
  file(TO_NATIVE_PATH "${_bin_x86}" _bin_x86_n)
  file(TO_NATIVE_PATH "${_bin_x64}" _bin_x64_n)

  set(_sdk_props_early "${CMAKE_CURRENT_BINARY_DIR}/${target}.winsdk.props")
  set(_sdk_props_late "${CMAKE_CURRENT_BINARY_DIR}/${target}.winsdk-tools.props")
  file(TO_NATIVE_PATH "${_sdk_props_early}" _sdk_props_early_n)
  file(TO_NATIVE_PATH "${_sdk_props_late}" _sdk_props_late_n)

  # --- Early: kit version for include/lib resolution ---
  set(_xml "")
  string(APPEND _xml "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
  string(APPEND _xml "<Project xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
  string(APPEND _xml "  <!-- OpenDCL: pin Windows SDK version BEFORE Microsoft.Cpp.props. -->\n")
  string(APPEND _xml "  <PropertyGroup>\n")
  string(APPEND _xml "    <WindowsTargetPlatformVersion>${version}</WindowsTargetPlatformVersion>\n")
  string(APPEND _xml "    <WindowsTargetPlatformMinVersion>${version}</WindowsTargetPlatformMinVersion>\n")
  string(APPEND _xml "  </PropertyGroup>\n")
  string(APPEND _xml "</Project>\n")
  file(WRITE "${_sdk_props_early}" "${_xml}")

  # --- Late: after Cpp.props (append kit tools; classic CAD modules: no manifest) ---
  set(_xml "")
  string(APPEND _xml "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
  string(APPEND _xml "<Project xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
  string(APPEND _xml "  <!-- OpenDCL: AFTER Microsoft.Cpp.props. -->\n")
  string(APPEND _xml "  <PropertyGroup>\n")
  string(APPEND _xml "    <WindowsSDK_ExecutablePath_x86>${_bin_x86_n}</WindowsSDK_ExecutablePath_x86>\n")
  string(APPEND _xml "    <WindowsSDK_ExecutablePath_x64>${_bin_x64_n}</WindowsSDK_ExecutablePath_x64>\n")
  # Classic runtime vcxproj: GenerateManifest=false. CMake emits true per-config;
  # PropertyGroup after Cpp.props overrides it and avoids TRK0005 mt.exe.
  string(APPEND _xml "    <GenerateManifest>false</GenerateManifest>\n")
  string(APPEND _xml "    <EmbedManifest>false</EmbedManifest>\n")
  string(APPEND _xml "  </PropertyGroup>\n")
  string(APPEND _xml "  <PropertyGroup Condition=\"'$(Platform)'=='Win32' or '$(Platform)'=='x86'\">\n")
  string(APPEND _xml "    <WindowsSDK_ExecutablePath>$(WindowsSDK_ExecutablePath_x86)</WindowsSDK_ExecutablePath>\n")
  string(APPEND _xml "    <ExecutablePath>$(ExecutablePath);$(WindowsSDK_ExecutablePath_x86)</ExecutablePath>\n")
  string(APPEND _xml "  </PropertyGroup>\n")
  string(APPEND _xml "  <PropertyGroup Condition=\"'$(Platform)'=='x64'\">\n")
  string(APPEND _xml "    <WindowsSDK_ExecutablePath>$(WindowsSDK_ExecutablePath_x64)</WindowsSDK_ExecutablePath>\n")
  string(APPEND _xml "    <ExecutablePath>$(ExecutablePath);$(WindowsSDK_ExecutablePath_x64)</ExecutablePath>\n")
  string(APPEND _xml "  </PropertyGroup>\n")
  string(APPEND _xml "  <ItemDefinitionGroup>\n")
  string(APPEND _xml "    <Link>\n")
  string(APPEND _xml "      <GenerateManifest>false</GenerateManifest>\n")
  string(APPEND _xml "    </Link>\n")
  string(APPEND _xml "  </ItemDefinitionGroup>\n")
  string(APPEND _xml "</Project>\n")
  file(WRITE "${_sdk_props_late}" "${_xml}")

  # CMake Globals + early/late MSBuild hooks.
  set_property(TARGET ${target} PROPERTY VS_WINDOWS_TARGET_PLATFORM_VERSION "${version}")
  set_property(TARGET ${target} PROPERTY VS_GLOBAL_WindowsTargetPlatformVersion "${version}")
  set_property(TARGET ${target} PROPERTY
    VS_GLOBAL_ForceImportBeforeCppProps "${_sdk_props_early_n}")
  set_property(TARGET ${target} PROPERTY
    VS_GLOBAL_ForceImportAfterCppProps "${_sdk_props_late_n}")
endfunction()

# Classic CAD modules: GenerateManifest=false (match classic vcxproj; avoid mt.exe).
#
# CMake still writes per-config <GenerateManifest>true</GenerateManifest> in the
# .vcxproj *after* imported .props, so PropertyGroup-only fixes lose. Inject a
# .targets via ForceImportAfterCppTargets that clears the flags BeforeTargets=Link.
function(opendcl_vs_disable_manifest target)
  if(NOT target)
    message(FATAL_ERROR "opendcl_vs_disable_manifest: need target")
  endif()
  set(_tgts "${CMAKE_CURRENT_BINARY_DIR}/${target}.nomanifest.targets")
  file(TO_NATIVE_PATH "${_tgts}" _tgts_n)
  set(_xml "")
  string(APPEND _xml "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
  string(APPEND _xml "<Project xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n")
  string(APPEND _xml "  <!-- OpenDCL: classic runtime modules do not embed manifests (no mt.exe). -->\n")
  string(APPEND _xml "  <Target Name=\"OpenDCL_DisableManifest_${target}\"\n")
  string(APPEND _xml "          BeforeTargets=\"BeforeLink;Link;Manifest;EmbedManifest;ManifestResourceCompile;_Link\">\n")
  string(APPEND _xml "    <PropertyGroup>\n")
  string(APPEND _xml "      <GenerateManifest>false</GenerateManifest>\n")
  string(APPEND _xml "      <EmbedManifest>false</EmbedManifest>\n")
  string(APPEND _xml "    </PropertyGroup>\n")
  string(APPEND _xml "  </Target>\n")
  string(APPEND _xml "</Project>\n")
  file(WRITE "${_tgts}" "${_xml}")
  set_property(TARGET ${target} PROPERTY
    VS_GLOBAL_ForceImportAfterCppTargets "${_tgts_n}")
  # Belt-and-suspenders on the link line.
  if(MSVC)
    target_link_options(${target} PRIVATE "/MANIFEST:NO")
  endif()
endfunction()

function(opendcl_rt_get id field out_var)
  set(${out_var} "${OPENDCL_RT_${id}_${field}}" PARENT_SCOPE)
endfunction()

# Resolve VS F5 host executable for a runtime family.
# Priority: OPENDCL_<FAMILY>_DEBUGGER_COMMAND → OPENDCL_DEBUGGER_COMMAND → env fallbacks.
function(opendcl_resolve_debugger_command family out_var)
  string(TOUPPER "${family}" _fam)
  set(_fam_cache "OPENDCL_${_fam}_DEBUGGER_COMMAND")
  if(DEFINED ${_fam_cache} AND NOT "${${_fam_cache}}" STREQUAL "")
    set(${out_var} "${${_fam_cache}}" PARENT_SCOPE)
    return()
  endif()
  if(OPENDCL_DEBUGGER_COMMAND)
    set(${out_var} "${OPENDCL_DEBUGGER_COMMAND}" PARENT_SCOPE)
    return()
  endif()

  set(_cmd "")
  if(family STREQUAL "ARX")
    if(DEFINED ENV{ARX_EXE} AND NOT "$ENV{ARX_EXE}" STREQUAL "")
      set(_cmd "$ENV{ARX_EXE}")
    elseif(DEFINED ENV{ACAD} AND NOT "$ENV{ACAD}" STREQUAL "")
      set(_cmd "$ENV{ACAD}")
    endif()
  elseif(family STREQUAL "BRX")
    if(DEFINED ENV{BRX_EXE} AND NOT "$ENV{BRX_EXE}" STREQUAL "")
      set(_cmd "$ENV{BRX_EXE}")
    elseif(DEFINED ENV{BRICSCAD} AND NOT "$ENV{BRICSCAD}" STREQUAL "")
      set(_cmd "$ENV{BRICSCAD}")
    endif()
  elseif(family STREQUAL "GRX")
    if(DEFINED ENV{GRX_EXE} AND NOT "$ENV{GRX_EXE}" STREQUAL "")
      set(_cmd "$ENV{GRX_EXE}")
    endif()
  elseif(family STREQUAL "ZRX")
    if(DEFINED ENV{ZRX_EXE} AND NOT "$ENV{ZRX_EXE}" STREQUAL "")
      set(_cmd "$ENV{ZRX_EXE}")
    endif()
  endif()
  set(${out_var} "${_cmd}" PARENT_SCOPE)
endfunction()

# Resolve VS F5 command arguments for a runtime family.
# Priority: OPENDCL_<FAMILY>_DEBUGGER_COMMAND_ARGUMENTS → OPENDCL_DEBUGGER_COMMAND_ARGUMENTS.
# Default is /ld "$(TargetPath)" (AutoCAD and BricsCAD module load).
function(opendcl_resolve_debugger_arguments family out_var)
  string(TOUPPER "${family}" _fam)
  set(_fam_cache "OPENDCL_${_fam}_DEBUGGER_COMMAND_ARGUMENTS")
  if(DEFINED ${_fam_cache} AND NOT "${${_fam_cache}}" STREQUAL "")
    set(${out_var} "${${_fam_cache}}" PARENT_SCOPE)
    return()
  endif()
  set(${out_var} "${OPENDCL_DEBUGGER_COMMAND_ARGUMENTS}" PARENT_SCOPE)
endfunction()

# ---------------------------------------------------------------------------
# SDK root resolution: cache path OPENDCL_<ENV>_ROOT, else ENV{ENV}
# ---------------------------------------------------------------------------
function(opendcl_resolve_sdk_root sdk_env out_var)
  if(NOT sdk_env)
    set(${out_var} "" PARENT_SCOPE)
    return()
  endif()

  set(_cache_var "OPENDCL_${sdk_env}_ROOT")
  if(NOT DEFINED ${_cache_var})
    set(${_cache_var} "" CACHE PATH "Root of CAD SDK ${sdk_env} (default from ENV{${sdk_env}})")
  endif()

  set(_root "${${_cache_var}}")
  if(NOT _root AND DEFINED ENV{${sdk_env}})
    set(_root "$ENV{${sdk_env}}")
  endif()

  if(_root)
    file(TO_CMAKE_PATH "${_root}" _root)
  endif()
  set(${out_var} "${_root}" PARENT_SCOPE)
endfunction()

function(opendcl_sdk_available sdk_env out_var)
  opendcl_resolve_sdk_root("${sdk_env}" _root)
  if(_root AND EXISTS "${_root}")
    set(${out_var} TRUE PARENT_SCOPE)
  else()
    set(${out_var} FALSE PARENT_SCOPE)
  endif()
endfunction()

# ---------------------------------------------------------------------------
# Selection: family flags ∩ optional ID list ∩ SDK availability
# ---------------------------------------------------------------------------
function(opendcl_host_arch out_var)
  # Map generator platform / pointer size to matrix ARCH (x64|x86).
  set(_arch "")
  if(CMAKE_GENERATOR_PLATFORM)
    string(TOUPPER "${CMAKE_GENERATOR_PLATFORM}" _plat)
    if(_plat MATCHES "ARM64")
      set(_arch "arm64")
    elseif(_plat MATCHES "X64|WIN64|AMD64")
      set(_arch "x64")
    elseif(_plat MATCHES "WIN32|X86")
      set(_arch "x86")
    endif()
  endif()
  if(NOT _arch)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(_arch "x64")
    else()
      set(_arch "x86")
    endif()
  endif()
  set(${out_var} "${_arch}" PARENT_SCOPE)
endfunction()

function(opendcl_select_runtimes out_enabled out_skipped)
  set(_enabled)
  set(_skipped)
  opendcl_host_arch(_host_arch)

  foreach(_id IN LISTS OPENDCL_RT_IDS)
    opendcl_rt_get("${_id}" FAMILY _family)
    opendcl_rt_get("${_id}" SDK_ENV _sdk_env)
    opendcl_rt_get("${_id}" ARCH _rt_arch)

    set(_family_on FALSE)
    if(_family STREQUAL "ARX" AND OPENDCL_ENABLE_ARX)
      set(_family_on TRUE)
    elseif(_family STREQUAL "BRX" AND OPENDCL_ENABLE_BRX)
      set(_family_on TRUE)
    elseif(_family STREQUAL "GRX" AND OPENDCL_ENABLE_GRX)
      set(_family_on TRUE)
    elseif(_family STREQUAL "ZRX" AND OPENDCL_ENABLE_ZRX)
      set(_family_on TRUE)
    endif()

    if(NOT _family_on)
      list(APPEND _skipped "${_id}|family_disabled")
      continue()
    endif()

    if(NOT _rt_arch STREQUAL _host_arch)
      list(APPEND _skipped "${_id}|arch_mismatch:${_rt_arch}!=${_host_arch}")
      continue()
    endif()

    if(OPENDCL_RUNTIME_TARGETS)
      list(FIND OPENDCL_RUNTIME_TARGETS "${_id}" _idx)
      if(_idx LESS 0)
        list(APPEND _skipped "${_id}|not_in_OPENDCL_RUNTIME_TARGETS")
        continue()
      endif()
    endif()

    opendcl_sdk_available("${_sdk_env}" _ok)
    if(OPENDCL_RUNTIME_AUTO AND NOT _ok)
      list(APPEND _skipped "${_id}|sdk_missing:${_sdk_env}")
      continue()
    endif()

    if(NOT OPENDCL_RUNTIME_AUTO AND NOT _ok)
      if(OPENDCL_RUNTIME_REQUIRE_SELECTED)
        message(FATAL_ERROR
          "Runtime ${_id} selected but SDK ${_sdk_env} is not available. "
          "Set OPENDCL_${_sdk_env}_ROOT or env ${_sdk_env}.")
      endif()
      list(APPEND _skipped "${_id}|sdk_missing:${_sdk_env}")
      continue()
    endif()

    list(APPEND _enabled "${_id}")
  endforeach()

  if(OPENDCL_RUNTIME_REQUIRE_SELECTED AND OPENDCL_RUNTIME_TARGETS)
    foreach(_want IN LISTS OPENDCL_RUNTIME_TARGETS)
      list(FIND _enabled "${_want}" _idx)
      if(_idx LESS 0)
        message(FATAL_ERROR "Required runtime target '${_want}' was not enabled.")
      endif()
    endforeach()
  endif()

  set(${out_enabled} "${_enabled}" PARENT_SCOPE)
  set(${out_skipped} "${_skipped}" PARENT_SCOPE)
endfunction()

function(opendcl_print_runtime_summary enabled skipped)
  list(LENGTH enabled _ne)
  list(LENGTH skipped _ns)
  message(STATUS "OpenDCL runtimes: enabled ${_ne}, skipped ${_ns}")
  foreach(_id IN LISTS enabled)
    opendcl_rt_get("${_id}" SDK_ENV _env)
    opendcl_rt_get("${_id}" TOOLSET _ts)
    opendcl_resolve_sdk_root("${_env}" _root)
    message(STATUS "  ENABLE  ${_id}  SDK=${_root}  toolset=${_ts}")
  endforeach()
  foreach(_s IN LISTS skipped)
    message(STATUS "  SKIP    ${_s}")
  endforeach()
  # Host-neutral FullDebug note (once). Host debug libdirs are MSBuild-time
  # (generated *.libdirs.props + optional parent dev.props / local.props), not
  # configure-time discovery — avoid per-target "prefix unset" spam.
  if(_ne GREATER 0)
    message(STATUS "FullDebug libdirs: release SDK always; host debug dirs via generated *.libdirs.props and optional parent dev.props / local.props (MSBuild-time)")
  endif()
endfunction()
