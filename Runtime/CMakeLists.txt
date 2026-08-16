# CAD runtime modules (Runtime_*).
# Defined here (not the root CMakeLists) so VS generator writes .vcxproj next to
# other Runtime products under build/<preset>/Runtime/ instead of the binary root.
#
# Localized/ and RxInstall/ remain separate add_subdirectory calls from the root
# (order: Res nest → Localized → modules → RxInstall).

if(NOT OPENDCL_BUILD_RUNTIME)
  return()
endif()

opendcl_add_selected_runtimes()

# Pre-move layouts put Runtime_* next to the .sln. CMake does not delete
# orphaned VS files on reconfigure — drop stale root copies so only Runtime/ remains.
file(GLOB _opendcl_stale_rt_root
  "${CMAKE_BINARY_DIR}/Runtime_*.vcxproj"
  "${CMAKE_BINARY_DIR}/Runtime_*.vcxproj.*"
  "${CMAKE_BINARY_DIR}/Runtime_*.props"
  "${CMAKE_BINARY_DIR}/Runtime_*.targets"
  "${CMAKE_BINARY_DIR}/OpenDCL_Runtime_*.vcxproj"
  "${CMAKE_BINARY_DIR}/OpenDCL_Runtime_*.vcxproj.*"
  "${CMAKE_BINARY_DIR}/OpenDCL_Runtime_*.props"
  "${CMAKE_BINARY_DIR}/OpenDCL_Runtime_*.targets"
)
if(_opendcl_stale_rt_root)
  file(REMOVE ${_opendcl_stale_rt_root})
  message(STATUS "Removed stale binary-root Runtime_* project files (now under Runtime/)")
endif()
file(GLOB _opendcl_stale_rt_dirs
  "${CMAKE_BINARY_DIR}/Runtime_*.dir"
  "${CMAKE_BINARY_DIR}/OpenDCL_Runtime_*.dir")
if(_opendcl_stale_rt_dirs)
  file(REMOVE_RECURSE ${_opendcl_stale_rt_dirs})
  message(STATUS "Removed stale binary-root Runtime_*.dir intermediate dirs")
endif()
