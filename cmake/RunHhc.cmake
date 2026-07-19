# Run Microsoft HTML Help Workshop hhc.exe.
# hhc returns 1 on success and 0 on failure (opposite of normal tools).
# Usage:
#   cmake -D HHC=... -D HHP=Content/OpenDCL.hhp -D WORKDIR=... -P RunHhc.cmake

if(NOT HHC OR NOT HHP OR NOT WORKDIR)
  message(FATAL_ERROR "RunHhc.cmake requires HHC, HHP, and WORKDIR")
endif()

execute_process(
  COMMAND "${HHC}" "${HHP}"
  WORKING_DIRECTORY "${WORKDIR}"
  RESULT_VARIABLE _hhc_rc
  OUTPUT_VARIABLE _hhc_out
  ERROR_VARIABLE _hhc_err
  OUTPUT_STRIP_TRAILING_WHITESPACE
  ERROR_STRIP_TRAILING_WHITESPACE
)
if(_hhc_out)
  message(STATUS "${_hhc_out}")
endif()
if(_hhc_err)
  message(STATUS "${_hhc_err}")
endif()
# Success is exit code 1; treat 0 as failure; other codes as failure too.
if(NOT _hhc_rc EQUAL 1)
  message(FATAL_ERROR "hhc.exe failed (exit ${_hhc_rc}) for ${HHP} in ${WORKDIR}")
endif()
