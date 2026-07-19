# Generated from VI/*.props for opendcl-cmake experiments.
# List fields use | instead of ; (CMake list separator).
# Regenerate: pwsh scripts/generate-runtime-matrix.ps1
include_guard(GLOBAL)

opendcl_register_runtime(
  ID "ARX.16"
  FAMILY ARX
  VERSION "16"
  ARCH x86
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.16"
  TOOLSET "v70"
  SDK_ENV "ARX2004"
  SDK_INC "Inc"
  SDK_LIB "Lib"
  VI_DIR "Runtime/ARX/ARX.16/VI"
  DEFINES "_ACADTARGET=16|_ARXTARGET=16"
)

opendcl_register_runtime(
  ID "ARX.17.x64"
  FAMILY ARX
  VERSION "17"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.17"
  TOOLSET "v80"
  SDK_ENV "ARX2008X64"
  SDK_INC "Inc"
  SDK_LIB "Lib"
  VI_DIR "Runtime/ARX/ARX.17.x64/VI"
  DEFINES "_ACADTARGET=17|_ARXTARGET=17"
  WARNING_DISABLES "4481"
)

opendcl_register_runtime(
  ID "ARX.17"
  FAMILY ARX
  VERSION "17"
  ARCH x86
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.17"
  TOOLSET "v80"
  SDK_ENV "ARX2007"
  SDK_INC "Inc"
  SDK_LIB "Lib"
  VI_DIR "Runtime/ARX/ARX.17/VI"
  DEFINES "_ACADTARGET=17|_ARXTARGET=17"
  WARNING_DISABLES "4481"
)

opendcl_register_runtime(
  ID "ARX.18.x64"
  FAMILY ARX
  VERSION "18"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.18"
  TOOLSET "v90"
  SDK_ENV "ARX2010"
  SDK_INC "Inc-x64|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ARX/ARX.18.x64/VI"
  DEFINES "_ACADTARGET=18|_ARXTARGET=18"
  WARNING_DISABLES "4265|4481"
)

opendcl_register_runtime(
  ID "ARX.18"
  FAMILY ARX
  VERSION "18"
  ARCH x86
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.18"
  TOOLSET "v90"
  SDK_ENV "ARX2010"
  SDK_INC "Inc-Win32|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ARX/ARX.18/VI"
  DEFINES "_ACADTARGET=18|_ARXTARGET=18"
  WARNING_DISABLES "4481"
)

opendcl_register_runtime(
  ID "ARX.19.x64"
  FAMILY ARX
  VERSION "19"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.19"
  TOOLSET "v100"
  SDK_ENV "ARX2013"
  SDK_INC "Inc-x64|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ARX/ARX.19.x64/VI"
  DEFINES "_ACADTARGET=19|_ARXTARGET=19|WINVER=0x0500"
  WARNING_DISABLES "4986|4987|4265|4481"
)

opendcl_register_runtime(
  ID "ARX.19"
  FAMILY ARX
  VERSION "19"
  ARCH x86
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.19"
  TOOLSET "v100"
  SDK_ENV "ARX2013"
  SDK_INC "Inc-Win32|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ARX/ARX.19/VI"
  DEFINES "_ACADTARGET=19|_ARXTARGET=19|WINVER=0x0500"
  WARNING_DISABLES "4481"
)

opendcl_register_runtime(
  ID "ARX.20.x64"
  FAMILY ARX
  VERSION "20"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.20"
  TOOLSET "v110_xp"
  SDK_ENV "ARX2015"
  SDK_INC "Inc-x64|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ARX/ARX.20.x64/VI"
  DEFINES "_ACADTARGET=20|_ARXTARGET=20|WINVER=0x0500"
  LIBS "AcDrawBridge.lib"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "ARX.20"
  FAMILY ARX
  VERSION "20"
  ARCH x86
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.20"
  TOOLSET "v110_xp"
  SDK_ENV "ARX2015"
  SDK_INC "Inc-Win32|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ARX/ARX.20/VI"
  DEFINES "_ACADTARGET=20|_ARXTARGET=20|WINVER=0x0500"
  LIBS "AcDrawBridge.lib"
)

opendcl_register_runtime(
  ID "ARX.21.x64"
  FAMILY ARX
  VERSION "21"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.21"
  TOOLSET "v140"
  SDK_ENV "ARX2017"
  SDK_INC "Inc-x64|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ARX/ARX.21.x64/VI"
  DEFINES "_ACADTARGET=21|_ARXTARGET=21|WINVER=0x0600|_WIN32_IE=0x0600|_WIN32_WINNT=0x0600"
  LIBS "AcDrawBridge.lib"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "ARX.21"
  FAMILY ARX
  VERSION "21"
  ARCH x86
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.21"
  TOOLSET "v140"
  SDK_ENV "ARX2017"
  SDK_INC "Inc-Win32|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ARX/ARX.21/VI"
  DEFINES "_ACADTARGET=21|_ARXTARGET=21|WINVER=0x0600|_WIN32_IE=0x0600|_WIN32_WINNT=0x0600"
  LIBS "AcDrawBridge.lib"
)

opendcl_register_runtime(
  ID "ARX.22.x64"
  FAMILY ARX
  VERSION "22"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.22"
  TOOLSET "v140"
  SDK_ENV "ARX2018"
  SDK_INC "Inc-x64|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ARX/ARX.22.x64/VI"
  DEFINES "_ACADTARGET=22|_ARXTARGET=22|WINVER=0x0600|_WIN32_IE=0x0600|_WIN32_WINNT=0x0600"
  LIBS "AcDrawBridge.lib"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "ARX.22"
  FAMILY ARX
  VERSION "22"
  ARCH x86
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.22"
  TOOLSET "v140"
  SDK_ENV "ARX2018"
  SDK_INC "Inc-Win32|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ARX/ARX.22/VI"
  DEFINES "_ACADTARGET=22|_ARXTARGET=22|WINVER=0x0600|_WIN32_IE=0x0600|_WIN32_WINNT=0x0600"
  LIBS "AcDrawBridge.lib"
)

opendcl_register_runtime(
  ID "ARX.23.x64"
  FAMILY ARX
  VERSION "23"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.23"
  TOOLSET "v141"
  SDK_ENV "ARX2019"
  SDK_INC "Inc-x64|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ARX/ARX.23.x64/VI"
  DEFINES "_ACADTARGET=23|_ARXTARGET=23|WINVER=0x0600|_WIN32_IE=0x0600|_WIN32_WINNT=0x0600"
  LIBS "AcDrawBridge.lib"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "ARX.23"
  FAMILY ARX
  VERSION "23"
  ARCH x86
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.23"
  TOOLSET "v141"
  SDK_ENV "ARX2019"
  SDK_INC "Inc-Win32|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ARX/ARX.23/VI"
  DEFINES "_ACADTARGET=23|_ARXTARGET=23|WINVER=0x0600|_WIN32_IE=0x0600|_WIN32_WINNT=0x0600"
  LIBS "AcDrawBridge.lib"
)

opendcl_register_runtime(
  ID "ARX.24.x64"
  FAMILY ARX
  VERSION "24"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.24"
  TOOLSET "v142"
  SDK_ENV "ARX2021"
  SDK_INC "Inc-x64|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ARX/ARX.24.x64/VI"
  DEFINES "_ACADTARGET=24|_ARXTARGET=24|WINVER=0x0600|_WIN32_IE=0x0600|_WIN32_WINNT=0x0600"
  LIBS "AcDrawBridge.lib"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "ARX.25.x64"
  FAMILY ARX
  VERSION "25"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.25"
  TOOLSET "v142"
  SDK_ENV "ARX2025"
  SDK_INC "Inc-x64|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ARX/ARX.25.x64/VI"
  DEFINES "_ACADTARGET=25|_ARXTARGET=25|WINVER=0x0600|_WIN32_IE=0x0600|_WIN32_WINNT=0x0600"
  LIBS "AcPal.lib|AcDrawBridge.lib"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "ARX.26.x64"
  FAMILY ARX
  VERSION "26"
  ARCH x64
  EXT ".arx"
  OUTPUT_NAME "OpenDCL.x64.26"
  TOOLSET "v143"
  SDK_ENV "ARX2027"
  SDK_INC "Inc-x64|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ARX/ARX.26.x64/VI"
  DEFINES "_ACADTARGET=26|_ARXTARGET=26|WINVER=0x0600|_WIN32_IE=0x0600|_WIN32_WINNT=0x0600"
  LIBS "AcPal.lib|AcDrawBridge.lib"
  WARNING_DISABLES "4986|4987|4265|5262|5266|5267"
  CXX_STANDARD 20
)

opendcl_register_runtime(
  ID "BRX.10"
  FAMILY BRX
  VERSION "10"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.10"
  TOOLSET "v80"
  SDK_ENV "BRX10"
  SDK_INC "Inc"
  SDK_LIB "Lib"
  VI_DIR "Runtime/BRX/BRX.10/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=10"
)

opendcl_register_runtime(
  ID "BRX.11"
  FAMILY BRX
  VERSION "11"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.11"
  TOOLSET "v80"
  SDK_ENV "BRX11"
  SDK_INC "Inc"
  SDK_LIB "Lib"
  VI_DIR "Runtime/BRX/BRX.11/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=11"
)

opendcl_register_runtime(
  ID "BRX.12"
  FAMILY BRX
  VERSION "12"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.12"
  TOOLSET "v80"
  SDK_ENV "BRX12"
  SDK_INC "Inc"
  SDK_LIB "Lib"
  VI_DIR "Runtime/BRX/BRX.12/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=12"
)

opendcl_register_runtime(
  ID "BRX.13.x64"
  FAMILY BRX
  VERSION "13"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.13"
  TOOLSET "v100"
  SDK_ENV "BRX13"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.13.x64/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=13|WINVER=0x0500"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.13"
  FAMILY BRX
  VERSION "13"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.13"
  TOOLSET "v100"
  SDK_ENV "BRX13"
  SDK_INC "Inc32|Inc"
  SDK_LIB "Lib32"
  VI_DIR "Runtime/BRX/BRX.13/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=13|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "BRX.14.x64"
  FAMILY BRX
  VERSION "14"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.14"
  TOOLSET "v100"
  SDK_ENV "BRX14"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.14.x64/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=14|WINVER=0x0500"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.14"
  FAMILY BRX
  VERSION "14"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.14"
  TOOLSET "v100"
  SDK_ENV "BRX14"
  SDK_INC "Inc32|Inc"
  SDK_LIB "Lib32"
  VI_DIR "Runtime/BRX/BRX.14/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=14|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "BRX.15.x64"
  FAMILY BRX
  VERSION "15"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.15"
  TOOLSET "v100"
  SDK_ENV "BRX15"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.15.x64/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=15|WINVER=0x0500"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.15"
  FAMILY BRX
  VERSION "15"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.15"
  TOOLSET "v100"
  SDK_ENV "BRX15"
  SDK_INC "Inc32|Inc"
  SDK_LIB "Lib32"
  VI_DIR "Runtime/BRX/BRX.15/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=15|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "BRX.16.x64"
  FAMILY BRX
  VERSION "16"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.16"
  TOOLSET "v120"
  SDK_ENV "BRX16"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.16.x64/VI"
  DEFINES "_ACADTARGET=19|_BRXTARGET=16|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.16"
  FAMILY BRX
  VERSION "16"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.16"
  TOOLSET "v120"
  SDK_ENV "BRX16"
  SDK_INC "Inc32|Inc"
  SDK_LIB "Lib32"
  VI_DIR "Runtime/BRX/BRX.16/VI"
  DEFINES "_ACADTARGET=19|_BRXTARGET=16|WINVER=0x0501|_WIN32_WINNT=0x0501"
)

opendcl_register_runtime(
  ID "BRX.17.x64"
  FAMILY BRX
  VERSION "17"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.17"
  TOOLSET "v120"
  SDK_ENV "BRX17"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.17.x64/VI"
  DEFINES "_ACADTARGET=20|_BRXTARGET=17|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.17"
  FAMILY BRX
  VERSION "17"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.17"
  TOOLSET "v120"
  SDK_ENV "BRX17"
  SDK_INC "Inc32|Inc"
  SDK_LIB "Lib32"
  VI_DIR "Runtime/BRX/BRX.17/VI"
  DEFINES "_ACADTARGET=20|_BRXTARGET=17|WINVER=0x0501|_WIN32_WINNT=0x0501"
)

opendcl_register_runtime(
  ID "BRX.18.x64"
  FAMILY BRX
  VERSION "18"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.18"
  TOOLSET "v120"
  SDK_ENV "BRX18"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.18.x64/VI"
  DEFINES "_ACADTARGET=20|_BRXTARGET=18|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.18"
  FAMILY BRX
  VERSION "18"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.18"
  TOOLSET "v120"
  SDK_ENV "BRX18"
  SDK_INC "Inc32|Inc"
  SDK_LIB "Lib32"
  VI_DIR "Runtime/BRX/BRX.18/VI"
  DEFINES "_ACADTARGET=20|_BRXTARGET=18|WINVER=0x0501|_WIN32_WINNT=0x0501"
)

opendcl_register_runtime(
  ID "BRX.19.x64"
  FAMILY BRX
  VERSION "19"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.19"
  TOOLSET "v141"
  SDK_ENV "BRX19"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.19.x64/VI"
  DEFINES "_ACADTARGET=20|_BRXTARGET=19|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.19"
  FAMILY BRX
  VERSION "19"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.19"
  TOOLSET "v141"
  SDK_ENV "BRX19"
  SDK_INC "Inc32|Inc"
  SDK_LIB "Lib32"
  VI_DIR "Runtime/BRX/BRX.19/VI"
  DEFINES "_ACADTARGET=20|_BRXTARGET=19|WINVER=0x0501|_WIN32_WINNT=0x0501"
)

opendcl_register_runtime(
  ID "BRX.20.x64"
  FAMILY BRX
  VERSION "20"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.20"
  TOOLSET "v141"
  SDK_ENV "BRX20"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.20.x64/VI"
  DEFINES "_ACADTARGET=21|_BRXTARGET=20|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.20"
  FAMILY BRX
  VERSION "20"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.20"
  TOOLSET "v141"
  SDK_ENV "BRX20"
  SDK_INC "Inc32|Inc"
  SDK_LIB "Lib32"
  VI_DIR "Runtime/BRX/BRX.20/VI"
  DEFINES "_ACADTARGET=21|_BRXTARGET=20|WINVER=0x0501|_WIN32_WINNT=0x0501"
)

opendcl_register_runtime(
  ID "BRX.21.x64"
  FAMILY BRX
  VERSION "21"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.21"
  TOOLSET "v141"
  SDK_ENV "BRX21"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.21.x64/VI"
  DEFINES "_ACADTARGET=21|_BRXTARGET=21|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.22.x64"
  FAMILY BRX
  VERSION "22"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.22"
  TOOLSET "v142"
  SDK_ENV "BRX22"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.22.x64/VI"
  DEFINES "_ACADTARGET=23|_BRXTARGET=22|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.23.x64"
  FAMILY BRX
  VERSION "23"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.23"
  TOOLSET "v142"
  SDK_ENV "BRX23"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.23.x64/VI"
  DEFINES "_ACADTARGET=23|_BRXTARGET=23|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.24.x64"
  FAMILY BRX
  VERSION "24"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.24"
  TOOLSET "v142"
  SDK_ENV "BRX24"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.24.x64/VI"
  DEFINES "_ACADTARGET=23|_BRXTARGET=24|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.25.x64"
  FAMILY BRX
  VERSION "25"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.25"
  TOOLSET "v142"
  SDK_ENV "BRX25"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.25.x64/VI"
  DEFINES "_ACADTARGET=23|_BRXTARGET=25|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.26.x64"
  FAMILY BRX
  VERSION "26"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.26"
  TOOLSET "v142"
  SDK_ENV "BRX26"
  SDK_INC "Inc64|Inc"
  SDK_LIB "Lib64"
  VI_DIR "Runtime/BRX/BRX.26.x64/VI"
  DEFINES "_ACADTARGET=24|_BRXTARGET=26|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.27.x64"
  FAMILY BRX
  VERSION "27"
  ARCH x64
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.x64.27"
  TOOLSET "v143"
  SDK_ENV "BRX27"
  SDK_INC "inc64|inc"
  SDK_LIB "lib64"
  # FullDebug uses $(BRX_PATH)\lib\vc143x64\Debug (see opendcl_fulldebug_libdir_msbuild).
  VI_DIR "Runtime/BRX/BRX.27.x64/VI"
  DEFINES "_ACADTARGET=24|_BRXTARGET=27|WINVER=0x0501|_WIN32_WINNT=0x0501"
  WARNING_DISABLES "4986|4987|4265"
)

opendcl_register_runtime(
  ID "BRX.9"
  FAMILY BRX
  VERSION "9"
  ARCH x86
  EXT ".brx"
  OUTPUT_NAME "OpenDCL.9"
  TOOLSET "v80"
  SDK_ENV "BRX9"
  SDK_INC "Inc"
  SDK_LIB "Lib"
  VI_DIR "Runtime/BRX/BRX.9/VI"
  DEFINES "_ACADTARGET=17|_BRXTARGET=9"
)

opendcl_register_runtime(
  ID "GRX.2015.x64"
  FAMILY GRX
  VERSION "2015"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2015"
  TOOLSET "v100"
  SDK_ENV "GRX2015"
  SDK_INC "inc-x64|inc/arx|inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/GRX/GRX.2015.x64/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2015|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2015"
  FAMILY GRX
  VERSION "2015"
  ARCH x86
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.2015"
  TOOLSET "v100"
  SDK_ENV "GRX2015"
  SDK_INC "inc-x86|inc/arx|inc"
  SDK_LIB "Lib-x86"
  VI_DIR "Runtime/GRX/GRX.2015/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2015|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2016.x64"
  FAMILY GRX
  VERSION "2016"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2016"
  TOOLSET "v100"
  SDK_ENV "GRX2016"
  SDK_INC "inc-x64|inc/arx|inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/GRX/GRX.2016.x64/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2016|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2016"
  FAMILY GRX
  VERSION "2016"
  ARCH x86
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.2016"
  TOOLSET "v100"
  SDK_ENV "GRX2016"
  SDK_INC "inc-x86|inc/arx|inc"
  SDK_LIB "Lib-x86"
  VI_DIR "Runtime/GRX/GRX.2016/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2016|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2017.x64"
  FAMILY GRX
  VERSION "2017"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2017"
  TOOLSET "v100"
  SDK_ENV "GRX2017"
  SDK_INC "inc-x64|inc/arx|inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/GRX/GRX.2017.x64/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2017|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2017"
  FAMILY GRX
  VERSION "2017"
  ARCH x86
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.2017"
  TOOLSET "v100"
  SDK_ENV "GRX2017"
  SDK_INC "inc-x86|inc/arx|inc"
  SDK_LIB "Lib-x86"
  VI_DIR "Runtime/GRX/GRX.2017/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2017|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2018.x64"
  FAMILY GRX
  VERSION "2018"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2018"
  TOOLSET "v100"
  SDK_ENV "GRX2018"
  SDK_INC "inc-x64|inc/arx|inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/GRX/GRX.2018.x64/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2018|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2018"
  FAMILY GRX
  VERSION "2018"
  ARCH x86
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.2018"
  TOOLSET "v100"
  SDK_ENV "GRX2018"
  SDK_INC "inc-x86|inc/arx|inc"
  SDK_LIB "Lib-x86"
  VI_DIR "Runtime/GRX/GRX.2018/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2018|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2019.x64"
  FAMILY GRX
  VERSION "2019"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2019"
  TOOLSET "v100"
  SDK_ENV "GRX2019"
  SDK_INC "inc-x64|inc/arx|inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/GRX/GRX.2019.x64/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2019|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2019"
  FAMILY GRX
  VERSION "2019"
  ARCH x86
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.2019"
  TOOLSET "v100"
  SDK_ENV "GRX2019"
  SDK_INC "inc-x86|inc/arx|inc"
  SDK_LIB "Lib-x86"
  VI_DIR "Runtime/GRX/GRX.2019/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2019|WINVER=0x0500"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2020.x64"
  FAMILY GRX
  VERSION "2020"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2020"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2020"
  SDK_INC "inc-x64|inc/arx|inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/GRX/GRX.2020.x64/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2020|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2020"
  FAMILY GRX
  VERSION "2020"
  ARCH x86
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.2020"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2020"
  SDK_INC "inc-x86|inc/arx|inc"
  SDK_LIB "Lib-x86"
  VI_DIR "Runtime/GRX/GRX.2020/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2020|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2021.x64"
  FAMILY GRX
  VERSION "2021"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2021"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2021"
  SDK_INC "inc-x64|inc/arx|inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/GRX/GRX.2021.x64/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2021|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2021"
  FAMILY GRX
  VERSION "2021"
  ARCH x86
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.2021"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2021"
  SDK_INC "inc-x86|inc/arx|inc"
  SDK_LIB "Lib-x86"
  VI_DIR "Runtime/GRX/GRX.2021/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2021|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2022.x64"
  FAMILY GRX
  VERSION "2022"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2022"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2022"
  SDK_INC "inc-x64|inc/arx|inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/GRX/GRX.2022.x64/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2022|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2022"
  FAMILY GRX
  VERSION "2022"
  ARCH x86
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.2022"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2022"
  SDK_INC "inc-x86|inc/arx|inc"
  SDK_LIB "Lib-x86"
  VI_DIR "Runtime/GRX/GRX.2022/VI"
  DEFINES "_ACADTARGET=19|_GRXTARGET=2022|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "grxport.lib|gcad.lib|gcap.lib|gcax.lib|gcdb.lib|gced.lib|gcgs.lib|gcui.lib|gcut.lib|gplt.lib|TD_Root.lib|TD_DBRoot.lib|TD_Db.lib|TD_Ge.lib|TD_Gi.lib|TD_Gs.lib"
)

opendcl_register_runtime(
  ID "GRX.2023.x64"
  FAMILY GRX
  VERSION "2023"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2023"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2023"
  SDK_INC "inc-x64|arx/inc|inc"
  SDK_LIB "lib-x64"
  VI_DIR "Runtime/GRX/GRX.2023.x64/VI"
  DEFINES "_ACADTARGET=22|_GRXTARGET=2023|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "gcad.lib|gcax.lib|gcbase.lib|gccore.lib|gcdb.lib|gcgs.lib|gplot.lib"
)

opendcl_register_runtime(
  ID "GRX.2024.x64"
  FAMILY GRX
  VERSION "2024"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2024"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2024"
  SDK_INC "inc-x64|arx/inc|inc"
  SDK_LIB "lib-x64"
  VI_DIR "Runtime/GRX/GRX.2024.x64/VI"
  DEFINES "_ACADTARGET=23|_GRXTARGET=2024|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "gcad.lib|gcax.lib|gcbase.lib|gccore.lib|gcdb.lib|gcgs.lib|gplot.lib"
)

opendcl_register_runtime(
  ID "GRX.2025.x64"
  FAMILY GRX
  VERSION "2025"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2025"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2025"
  SDK_INC "inc-x64|arx/inc|inc"
  SDK_LIB "lib-x64"
  VI_DIR "Runtime/GRX/GRX.2025.x64/VI"
  DEFINES "_ACADTARGET=23|_GRXTARGET=2025|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "gcad.lib|gcax.lib|gcbase.lib|gccore.lib|gcdb.lib|gcgs.lib|gplot.lib"
)

opendcl_register_runtime(
  ID "GRX.2026.x64"
  FAMILY GRX
  VERSION "2026"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2026"
  TOOLSET "v141_xp"
  SDK_ENV "GRX2026"
  SDK_INC "inc-x64|arx/inc|inc"
  SDK_LIB "lib-x64"
  VI_DIR "Runtime/GRX/GRX.2026.x64/VI"
  DEFINES "_ACADTARGET=24|_GRXTARGET=2026|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "gcad.lib|gcax.lib|gcbase.lib|gccore.lib|gcdb.lib|gcgs.lib|gplot.lib"
)

opendcl_register_runtime(
  ID "GRX.2027.x64"
  FAMILY GRX
  VERSION "2027"
  ARCH x64
  EXT ".grx"
  OUTPUT_NAME "OpenDCL.x64.2027"
  TOOLSET "v142"
  SDK_ENV "GRX2027"
  SDK_INC "inc-x64|arx/inc|inc"
  SDK_LIB "lib-x64"
  VI_DIR "Runtime/GRX/GRX.2027.x64/VI"
  DEFINES "_ACADTARGET=24|_GRXTARGET=2027|WINVER=0x0501|_WIN32_WINNT=0x501"
  LIBS "gcad.lib|gcax.lib|gcbase.lib|gccore.lib|gcdb.lib|gcgs.lib|gplot.lib"
)

opendcl_register_runtime(
  ID "ZRX.2014"
  FAMILY ZRX
  VERSION "2014"
  ARCH x86
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.2014"
  TOOLSET "v100"
  SDK_ENV "ZRX2014"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib"
  VI_DIR "Runtime/ZRX/ZRX.2014/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2014|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2015"
  FAMILY ZRX
  VERSION "2015"
  ARCH x86
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.2015"
  TOOLSET "v100"
  SDK_ENV "ZRX2015"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib"
  VI_DIR "Runtime/ZRX/ZRX.2015/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2015|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2017.x64"
  FAMILY ZRX
  VERSION "2017"
  ARCH x64
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.x64.2017"
  TOOLSET "v100"
  SDK_ENV "ZRX2017"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ZRX/ZRX.2017.x64/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2017|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2017"
  FAMILY ZRX
  VERSION "2017"
  ARCH x86
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.2017"
  TOOLSET "v100"
  SDK_ENV "ZRX2017"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ZRX/ZRX.2017/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2017|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2018.x64"
  FAMILY ZRX
  VERSION "2018"
  ARCH x64
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.x64.2018"
  TOOLSET "v100"
  SDK_ENV "ZRX2018"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ZRX/ZRX.2018.x64/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2018|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2018"
  FAMILY ZRX
  VERSION "2018"
  ARCH x86
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.2018"
  TOOLSET "v100"
  SDK_ENV "ZRX2018"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ZRX/ZRX.2018/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2018|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2019.x64"
  FAMILY ZRX
  VERSION "2019"
  ARCH x64
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.x64.2019"
  TOOLSET "v100"
  SDK_ENV "ZRX2019"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ZRX/ZRX.2019.x64/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2019|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2019"
  FAMILY ZRX
  VERSION "2019"
  ARCH x86
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.2019"
  TOOLSET "v100"
  SDK_ENV "ZRX2019"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ZRX/ZRX.2019/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2019|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2020.x64"
  FAMILY ZRX
  VERSION "2020"
  ARCH x64
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.x64.2020"
  TOOLSET "v100"
  SDK_ENV "ZRX2020"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ZRX/ZRX.2020.x64/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2020|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2020"
  FAMILY ZRX
  VERSION "2020"
  ARCH x86
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.2020"
  TOOLSET "v100"
  SDK_ENV "ZRX2020"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ZRX/ZRX.2020/VI"
  DEFINES "_ACADTARGET=17|_ZRXTARGET=2020|WINVER=0x0500"
)

opendcl_register_runtime(
  ID "ZRX.2021.x64"
  FAMILY ZRX
  VERSION "2021"
  ARCH x64
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.x64.2021"
  TOOLSET "v141"
  SDK_ENV "ZRX2021"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ZRX/ZRX.2021.x64/VI"
  DEFINES "_ACADTARGET=21|_ZRXTARGET=2021|WINVER=0x0501|_WIN32_WINNT=0x0501"
  LIBS "ZwGs.lib"
)

opendcl_register_runtime(
  ID "ZRX.2021"
  FAMILY ZRX
  VERSION "2021"
  ARCH x86
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.2021"
  TOOLSET "v141"
  SDK_ENV "ZRX2021"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ZRX/ZRX.2021/VI"
  DEFINES "_ACADTARGET=21|_ZRXTARGET=2021|WINVER=0x0501|_WIN32_WINNT=0x0501"
  LIBS "ZwGs.lib"
)

opendcl_register_runtime(
  ID "ZRX.2022.x64"
  FAMILY ZRX
  VERSION "2022"
  ARCH x64
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.x64.2022"
  TOOLSET "v141"
  SDK_ENV "ZRX2022"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ZRX/ZRX.2022.x64/VI"
  DEFINES "_ACADTARGET=21|_ZRXTARGET=2022|WINVER=0x0501|_WIN32_WINNT=0x0501"
  LIBS "ZwGs.lib"
)

opendcl_register_runtime(
  ID "ZRX.2022"
  FAMILY ZRX
  VERSION "2022"
  ARCH x86
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.2022"
  TOOLSET "v141"
  SDK_ENV "ZRX2022"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ZRX/ZRX.2022/VI"
  DEFINES "_ACADTARGET=21|_ZRXTARGET=2022|WINVER=0x0501|_WIN32_WINNT=0x0501"
  LIBS "ZwGs.lib"
)

opendcl_register_runtime(
  ID "ZRX.2023.x64"
  FAMILY ZRX
  VERSION "2023"
  ARCH x64
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.x64.2023"
  TOOLSET "v141"
  SDK_ENV "ZRX2023"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-x64"
  VI_DIR "Runtime/ZRX/ZRX.2023.x64/VI"
  DEFINES "_ACADTARGET=22|_ZRXTARGET=2023|WINVER=0x0501|_WIN32_WINNT=0x0501"
  LIBS "ZwGs.lib"
)

opendcl_register_runtime(
  ID "ZRX.2023"
  FAMILY ZRX
  VERSION "2023"
  ARCH x86
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.2023"
  TOOLSET "v141"
  SDK_ENV "ZRX2023"
  SDK_INC "arxport|Inc"
  SDK_LIB "Lib-Win32"
  VI_DIR "Runtime/ZRX/ZRX.2023/VI"
  DEFINES "_ACADTARGET=22|_ZRXTARGET=2023|WINVER=0x0501|_WIN32_WINNT=0x0501"
  LIBS "ZwGs.lib"
)

opendcl_register_runtime(
  ID "ZRX.2025.x64"
  FAMILY ZRX
  VERSION "2025"
  ARCH x64
  EXT ".zrx"
  OUTPUT_NAME "OpenDCL.x64.2025"
  TOOLSET "v141"
  SDK_ENV "ZRX2025"
  SDK_INC "arxport/inc|inc"
  SDK_LIB "lib-x64"
  VI_DIR "Runtime/ZRX/ZRX.2025.x64/VI"
  DEFINES "_ACADTARGET=23|_ZRXTARGET=2025|WINVER=0x0501|_WIN32_WINNT=0x0501"
  LIBS "ZwGs.lib"
)

