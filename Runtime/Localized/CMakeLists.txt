# Per-language Runtime.Res resource DLLs.
#
# Not a CAD runtime module — FullDebug → Debug (classic has Debug|Release only).

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

foreach(_lang IN LISTS OPENDCL_LANGS)
  opendcl_add_runtime_res("${_lang}")
endforeach()
