# Per-language Runtime.Res resource DLLs.
#
# Not a CAD runtime module — FullDebug → Debug (classic has Debug|Release only).
#
# PE policy OPENDCL_RES_PE (see OpenDCLHelpers.cmake):
#   classic_x86 — public Mixed: native only on Win32; x64 uses OpenDCL_Res_Win32.
#   host        — Res PE matches this configure (x64 Res on x64).

function(opendcl_add_runtime_res lang)
  set(_dir "${CMAKE_CURRENT_SOURCE_DIR}/${lang}/Runtime.Res")
  set(_rc "${_dir}/Runtime.${lang}.rc")
  # SharedRes.<lang>.rc is #include'd from Runtime.<lang>.rc (do not compile twice).

  if(NOT EXISTS "${_rc}")
    message(STATUS "Runtime.Res.${lang}: missing ${_rc}, skip")
    return()
  endif()

  set(_target "OpenDCL_RuntimeRes_${lang}")
  add_library(${_target} SHARED)
  target_sources(${_target} PRIVATE "${_rc}")

  # Classic layout: Runtime/Localized/<LANG>/Runtime.Res/<Debug|Release>/Runtime.Res.dll
  set(_out_res_base "${OPENDCL_OUTPUT_ROOT}/Runtime/Localized/${lang}/Runtime.Res")
  set_target_properties(${_target} PROPERTIES
    OUTPUT_NAME "Runtime.Res"
    PREFIX ""
    SUFFIX ".dll"
    # FullDebug uses Debug CRT settings (same folder as Debug; pure resource /NOENTRY).
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:FullDebug>>:Debug>DLL"
    LINKER_LANGUAGE CXX
  )
  opendcl_map_fulldebug_to_debug(${_target} "${_out_res_base}")
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_rr_arch "x64")
  else()
    set(_rr_arch "Win32")
  endif()
  opendcl_solution_folder(_rr_folder "${_rr_arch}" "OpenDCL_RuntimeRes_${lang}")
  set_target_properties(${_target} PROPERTIES FOLDER "${_rr_folder}")

  target_include_directories(${_target} PRIVATE
    "${CMAKE_SOURCE_DIR}/Common"
    "${CMAKE_SOURCE_DIR}/Common/${lang}"
    "${CMAKE_SOURCE_DIR}/Runtime"
    "${_dir}"
  )

  target_compile_definitions(${_target} PRIVATE
    _WINDOWS
    _USRDLL
    UNICODE
    _UNICODE
  )

  if(MSVC)
    target_link_options(${_target} PRIVATE "/NOENTRY")
  endif()
endfunction()

opendcl_runtime_res_build_native(_res_native)
if(_res_native)
  foreach(_lang IN LISTS OPENDCL_LANGS)
    opendcl_add_runtime_res("${_lang}")
  endforeach()
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Runtime.Res: native x64 (OPENDCL_RES_PE=host) for: ${OPENDCL_LANGS}")
  else()
    message(STATUS "Runtime.Res: native Win32 (x86) for: ${OPENDCL_LANGS}")
  endif()
else()
  if(OPENDCL_NEST_WIN32)
    message(STATUS
      "Runtime.Res: skip native targets (classic_x86 via full Win32 nest / OpenDCL_Res_Win32)")
  else()
    message(STATUS
      "Runtime.Res: skip native targets (OPENDCL_RES_PE=classic_x86; use OpenDCL_Res_Win32)")
  endif()
endif()
