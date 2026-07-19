# OpenDCL CMake helpers — options, SDK detection, registry, selection.
include_guard(GLOBAL)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(OPENDCL_BUILD_RUNTIME "Build CAD runtime modules" ON)
option(OPENDCL_BUILD_STUDIO "Build OpenDCL Studio.exe + Studio.Res (static MFC)" OFF)
option(OPENDCL_BUILD_RXINSTALL "Build RxInstall CA (Win32 nested from x64; sources in main .sln)" ON)
option(OPENDCL_BUILD_HELP "Build HTML Help projects" OFF)

# VS generators are one platform per configure. On an x64 configure, optionally
# nest a Win32 binary dir and expose OpenDCL_Win32 so one .sln builds both
# (same idea as nested RxInstall). Shared OPENDCL_OUTPUT_ROOT so packaging sees
# out/Studio/Win32 and out/Runtime/<x86 ids> next to x64 outputs.
option(OPENDCL_NEST_WIN32
  "When configuring x64 with the VS generator, also configure/build Win32 under <bin>/win32" OFF)
option(OPENDCL_WIN32_IN_ALL
  "Make nested Win32 part of the default ALL_BUILD (full ship); otherwise build OpenDCL_Win32 explicitly" OFF)

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
    ID FAMILY VERSION ARCH EXT OUTPUT_NAME TOOLSET
    SDK_ENV SDK_INC SDK_LIB SDK_LIB_FULLDEBUG VI_DIR DEFINES LIBS
    WARNING_DISABLES CXX_STANDARD)
  set(multiValueArgs PROFILES EXTRA_INCLUDES EXTRA_LIBDIRS COMPILE_OPTIONS LINK_OPTIONS)
  cmake_parse_arguments(RT "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if(NOT RT_ID)
    message(FATAL_ERROR "opendcl_register_runtime: ID is required")
  endif()

  set(_ids ${OPENDCL_RT_IDS})
  list(APPEND _ids "${RT_ID}")
  list(REMOVE_DUPLICATES _ids)
  set(OPENDCL_RT_IDS "${_ids}" CACHE INTERNAL "Registered runtime target IDs" FORCE)

  foreach(_f IN ITEMS
      FAMILY VERSION ARCH EXT OUTPUT_NAME TOOLSET
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
# Classic BRX FullDebug (BRX.26.x64.vcxproj):
#   $(BRX_PATH)\lib\vc143x64\Debug;%(AdditionalLibraryDirectories)
#
# Do not invent BRX27\lib\vc…\Debug under the release SDK root, and do not scan
# proprietary debug trees. Prefer MSBuild $(BRX_PATH) so the env can change
# without reconfigure.
function(opendcl_fulldebug_libdir_msbuild family sdk_env toolset out_var)
  set(_prefix "")

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
  elseif(family STREQUAL "BRX")
    # Match classic OpenDCL BRX FullDebug exactly (toolset → vcNNNx64).
    set(_vc "vc143x64")
    if(toolset MATCHES "^v([0-9]+)")
      set(_vc "vc${CMAKE_MATCH_1}x64")
    endif()
    set(_prefix "$(BRX_PATH)\\lib\\${_vc}\\Debug")
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
