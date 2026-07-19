# OpenDCL Studio application (static MFC + static CRT, matching classic) +
# per-language Studio.Res DLLs.
# HTML Help / CHM remains classic for now (optional later OPENDCL_BUILD_HELP).
#
# Not CAD runtime modules — FullDebug → Debug (opendcl_map_fulldebug_to_debug).

include("${CMAKE_SOURCE_DIR}/cmake/OpenDCLStudioSources.cmake")

# ---------------------------------------------------------------------------
# Studio.Res.<lang> resource DLLs (noentry, like Runtime.Res)
# ---------------------------------------------------------------------------
function(opendcl_add_studio_res lang)
  set(_dir "${CMAKE_CURRENT_SOURCE_DIR}/Localized/${lang}/Studio.Res")
  set(_rc "${_dir}/Studio.${lang}.rc")
  if(NOT EXISTS "${_rc}")
    message(STATUS "Studio.Res.${lang}: missing ${_rc}, skip")
    return()
  endif()

  set(_target "OpenDCL_StudioRes_${lang}")
  add_library(${_target} SHARED)
  target_sources(${_target} PRIVATE "${_rc}")

  # Classic layout: Studio/Localized/<LANG>/Studio.Res/<Debug|Release>/Studio.Res.dll
  set(_out_res_base "${OPENDCL_OUTPUT_ROOT}/Studio/Localized/${lang}/Studio.Res")
  set_target_properties(${_target} PROPERTIES
    OUTPUT_NAME "Studio.Res"
    PREFIX ""
    SUFFIX ".dll"
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:Debug>DLL"
    LINKER_LANGUAGE CXX
  )
  opendcl_map_fulldebug_to_debug(${_target} "${_out_res_base}")

  target_include_directories(${_target} PRIVATE
    "${CMAKE_SOURCE_DIR}/Common"
    "${CMAKE_SOURCE_DIR}/Common/${lang}"
    "${CMAKE_SOURCE_DIR}/Studio"
    "${_dir}"
  )
  target_compile_definitions(${_target} PRIVATE
    WIN32
    _WINDOWS
    _USRDLL
    UNICODE
    _UNICODE
  )
  if(MSVC)
    target_link_options(${_target} PRIVATE "/NOENTRY")
  endif()

  add_custom_command(TARGET ${_target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
      "${CMAKE_SOURCE_DIR}/Studio/Localized/${lang}/Studio.Res/${OPENDCL_CFG_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:${_target}>"
      "${CMAKE_SOURCE_DIR}/Studio/Localized/${lang}/Studio.Res/${OPENDCL_CFG_DIR}/$<TARGET_FILE_NAME:${_target}>"
    COMMENT "Mirror Studio.Res.dll to classic Studio/Localized/${lang}/Studio.Res/<Debug|Release>/"
  )
endfunction()

foreach(_lang IN LISTS OPENDCL_LANGS)
  opendcl_add_studio_res("${_lang}")
endforeach()

# Ensure F5 builds language resource DLLs before Studio.exe
set(_studio_res_deps "")
foreach(_lang IN LISTS OPENDCL_LANGS)
  if(TARGET "OpenDCL_StudioRes_${_lang}")
    list(APPEND _studio_res_deps "OpenDCL_StudioRes_${_lang}")
  endif()
endforeach()

# ---------------------------------------------------------------------------
# OpenDCL Studio.exe — static MFC + static CRT (classic)
# ---------------------------------------------------------------------------
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(_studio_plat "x64")
  set(_studio_manifest_arch "amd64")
  set(_studio_png opendcl_png_x64_mt)
else()
  set(_studio_plat "Win32")
  set(_studio_manifest_arch "x86")
  set(_studio_png opendcl_png_x86_mt)
endif()

# Arch-correct application manifest (classic Res\…manifest is X86-only and
# causes 0xC000007B "application failed to start" on x64).
set(_studio_manifest "${CMAKE_CURRENT_BINARY_DIR}/OpenDCL_Studio.manifest")
file(WRITE "${_studio_manifest}" "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>
<assembly xmlns=\"urn:schemas-microsoft-com:asm.v1\" manifestVersion=\"1.0\">
  <assemblyIdentity version=\"1.0.0.0\" processorArchitecture=\"${_studio_manifest_arch}\"
    name=\"OpenDCL.OpenDCL Studio\" type=\"win32\" />
  <description>OpenDCL Studio</description>
  <dependency>
    <dependentAssembly>
      <assemblyIdentity type=\"win32\" name=\"Microsoft.Windows.Common-Controls\"
        version=\"6.0.0.0\" processorArchitecture=\"${_studio_manifest_arch}\"
        publicKeyToken=\"6595b64144ccf1df\" language=\"*\" />
    </dependentAssembly>
  </dependency>
  <trustInfo xmlns=\"urn:schemas-microsoft-com:asm.v3\">
    <security>
      <requestedPrivileges>
        <requestedExecutionLevel level=\"asInvoker\" uiAccess=\"false\" />
      </requestedPrivileges>
    </security>
  </trustInfo>
</assembly>
")

set(_studio_manifest_rc "${CMAKE_CURRENT_BINARY_DIR}/OpenDCL_Studio_manifest.rc")
file(WRITE "${_studio_manifest_rc}"
  "1 RT_MANIFEST \"${_studio_manifest}\"\n")
# Escape backslashes for RC
string(REPLACE "\\" "\\\\" _studio_manifest_esc "${_studio_manifest}")
file(WRITE "${_studio_manifest_rc}"
  "1 RT_MANIFEST \"${_studio_manifest_esc}\"\n")

# Static MFC requires CMAKE_MFC_FLAG=1 and no _AFXDLL.
set(CMAKE_MFC_FLAG 1)

add_executable(OpenDCL_Studio WIN32
  ${OPENDCL_STUDIO_SOURCES}
  ${OPENDCL_STUDIO_RC}
  "${_studio_manifest_rc}"
)

# Skip classic X86 RT_MANIFEST inside Studio.rc when building under CMake.
set_property(SOURCE ${OPENDCL_STUDIO_RC} APPEND PROPERTY COMPILE_DEFINITIONS
  OPENDCL_SKIP_EMBEDDED_MANIFEST)

target_include_directories(OpenDCL_Studio PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_SOURCE_DIR}/Common"
  "${CMAKE_SOURCE_DIR}/Common/Controls"
  "${CMAKE_SOURCE_DIR}/Library/ZLib"
  "${CMAKE_SOURCE_DIR}/Library/LibPNG"
)

target_compile_definitions(OpenDCL_Studio PRIVATE
  _WINDOWS
  _CRT_SECURE_NO_DEPRECATE
  EDITOR
  _PRERELEASE
  UNICODE
  _UNICODE
  # Modern MFC requires WINVER >= 0x0501 (classic StdAfx defaults are 0x0400).
  WINVER=0x0601
  _WIN32_WINNT=0x0601
  _WIN32_WINDOWS=0x0601
  _WIN32_IE=0x0800
  # FullDebug uses the same _DEBUG settings as Debug (no AC_FULL_DEBUG for Studio).
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:_DEBUG>
  $<$<CONFIG:Release>:NDEBUG>
  $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>:_WIN64>
  $<$<EQUAL:${CMAKE_SIZEOF_VOID_P},4>:WIN32>
)

# Static MFC has no _AFXDLL; classic PPTooltip only #defines COMPILE_MULTIMON_STUBS
# under _AFXDLL. FolderTreeCtrl also #includes MultiMon.h without stubs — so this
# must be per-source (not target-wide) or both TUs define the same symbols (LNK2005).
set_property(SOURCE "${CMAKE_SOURCE_DIR}/Common/PPTooltip.cpp" APPEND PROPERTY
  COMPILE_DEFINITIONS COMPILE_MULTIMON_STUBS)


set(_out_studio_base "${OPENDCL_OUTPUT_ROOT}/Studio/${_studio_plat}")
set_target_properties(OpenDCL_Studio PROPERTIES
  OUTPUT_NAME "OpenDCL Studio"
  # Classic: static CRT (/MT, /MTd)
  MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:Debug>"
  VS_GLOBAL_UseOfMfc "Static"
  VS_GLOBAL_CharacterSet "Unicode"
  # Manifest comes from generated RT_MANIFEST only.
  VS_GLOBAL_EmbedManifest "false"
  VS_GLOBAL_GenerateManifest "false"
)
opendcl_map_fulldebug_to_debug(OpenDCL_Studio "${_out_studio_base}")

target_link_libraries(OpenDCL_Studio PRIVATE
  rpcrt4
  vfw32
)

if(TARGET ${_studio_png})
  target_link_libraries(OpenDCL_Studio PRIVATE ${_studio_png})
endif()

if(_studio_res_deps)
  add_dependencies(OpenDCL_Studio ${_studio_res_deps})
endif()

# ---------------------------------------------------------------------------
# ENU HTML Help (OpenDCL.chm) — classic Content + hhc.exe
# Dev F5 Help: Studio depends on ENU CHM (FindFile → Localized\ENU\Content\).
# hhc returns 1 on success; cmake/RunHhc.cmake inverts that for CMake.
# ---------------------------------------------------------------------------
set(_enu_help_dir "${CMAKE_SOURCE_DIR}/Studio/Localized/ENU")
set(_enu_content "${_enu_help_dir}/Content")
set(_enu_chm "${_enu_content}/OpenDCL.chm")
set(_enu_hhp "${_enu_content}/OpenDCL.hhp")
set(_run_hhc "${CMAKE_SOURCE_DIR}/cmake/RunHhc.cmake")

find_program(OPENDCL_HHC_EXECUTABLE
  NAMES hhc hhc.exe
  PATHS
    "C:/Program Files (x86)/HTML Help Workshop"
    "C:/Program Files/HTML Help Workshop"
  DOC "Microsoft HTML Help Workshop compiler (hhc.exe)"
)

if(EXISTS "${_enu_hhp}" AND OPENDCL_HHC_EXECUTABLE)
  # Track help sources so the CHM rebuilds when Content changes.
  file(GLOB_RECURSE _enu_help_sources CONFIGURE_DEPENDS
    "${_enu_content}/*")
  list(FILTER _enu_help_sources EXCLUDE REGEX "\\.[Cc][Hh][Mm]$")

  add_custom_command(
    OUTPUT "${_enu_chm}"
    COMMAND ${CMAKE_COMMAND}
      "-DHHC=${OPENDCL_HHC_EXECUTABLE}"
      "-DHHP=Content/OpenDCL.hhp"
      "-DWORKDIR=${_enu_help_dir}"
      -P "${_run_hhc}"
    DEPENDS ${_enu_help_sources} "${_run_hhc}"
    COMMENT "Build Studio ENU OpenDCL.chm (hhc)"
    VERBATIM
  )

  add_custom_target(OpenDCL_StudioHelp_ENU
    DEPENDS "${_enu_chm}"
    SOURCES
      "${_enu_hhp}"
      "${_enu_content}/OpenDCL.hhc"
      "${CMAKE_SOURCE_DIR}/Studio/Localized/BuildCHM.mak"
      "${CMAKE_SOURCE_DIR}/Studio/Localized/BuildCHM.bat"
  )
  set_target_properties(OpenDCL_StudioHelp_ENU PROPERTIES
    FOLDER "Studio"
  )
  add_dependencies(OpenDCL_Studio OpenDCL_StudioHelp_ENU)
  message(STATUS "Studio help: OpenDCL_StudioHelp_ENU → ${_enu_chm}")
else()
  if(NOT EXISTS "${_enu_hhp}")
    message(STATUS "Studio help: skip ENU CHM (missing ${_enu_hhp})")
  else()
    message(WARNING
      "Studio help: hhc.exe not found (install HTML Help Workshop). "
      "OpenDCL.chm will not be built; F5 Help shows a missing-file dialog.")
  endif()
endif()

if(MSVC)
  target_link_options(OpenDCL_Studio PRIVATE
    "/IGNORE:4217"
    "/IGNORE:4099"
    "/MANIFEST:NO"
  )
  target_compile_options(OpenDCL_Studio PRIVATE
    /wd4100 /wd4244 /wd4267 /wd4018 /wd4389 /wd4996 /wd4312
  )
endif()

target_precompile_headers(OpenDCL_Studio PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/StdAfx.h"
)

# Side-by-side layout next to Studio.exe so classic Workspace FindFile /
# GetLocalResourceModule work without special F5 path logic:
#   Studio.Res.dll, OpenDCL.chm (ENU), and the classic tree mirror.
set(_studio_classic_out "${CMAKE_SOURCE_DIR}/Studio/${_studio_plat}/${OPENDCL_CFG_DIR}")
set(_studio_post_cmds
  COMMAND ${CMAKE_COMMAND} -E make_directory "${_studio_classic_out}"
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "$<TARGET_FILE:OpenDCL_Studio>"
    "${_studio_classic_out}/$<TARGET_FILE_NAME:OpenDCL_Studio>"
)
# Prefer first configured language (dev presets use ENU) for side-by-side res/help.
list(GET OPENDCL_LANGS 0 _studio_default_lang)
if(TARGET "OpenDCL_StudioRes_${_studio_default_lang}")
  list(APPEND _studio_post_cmds
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:OpenDCL_StudioRes_${_studio_default_lang}>"
      "$<TARGET_FILE_DIR:OpenDCL_Studio>/$<TARGET_FILE_NAME:OpenDCL_StudioRes_${_studio_default_lang}>"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "$<TARGET_FILE:OpenDCL_StudioRes_${_studio_default_lang}>"
      "${_studio_classic_out}/$<TARGET_FILE_NAME:OpenDCL_StudioRes_${_studio_default_lang}>"
  )
endif()
# CHM is produced by OpenDCL_StudioHelp_* before Studio links (add_dependencies).
if(TARGET "OpenDCL_StudioHelp_${_studio_default_lang}")
  set(_studio_chm "${CMAKE_SOURCE_DIR}/Studio/Localized/${_studio_default_lang}/Content/OpenDCL.chm")
  list(APPEND _studio_post_cmds
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_studio_chm}"
      "$<TARGET_FILE_DIR:OpenDCL_Studio>/OpenDCL.chm"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${_studio_chm}"
      "${_studio_classic_out}/OpenDCL.chm"
  )
endif()

add_custom_command(TARGET OpenDCL_Studio POST_BUILD
  ${_studio_post_cmds}
  COMMENT "Mirror Studio.exe + side-by-side Studio.Res/CHM for classic F5 paths"
)

message(STATUS "Studio: OpenDCL_Studio (${_studio_plat}, static MFC+/MT; FullDebug→Debug) + Studio.Res for: ${OPENDCL_LANGS}")
