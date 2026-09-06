# Microsoft Edge WebView2 (Evergreen loader)

Vendored from NuGet `Microsoft.Web.WebView2` **1.0.4191.47** (BSD-3-Clause, `LICENSE.txt`).
`include/EventToken.h` is a Win8 SDK shim (one copy) so older CAD toolsets
that still use the Win7/XP SDK can compile `WebView2.h`.

The Html control delay-loads the arch-matching loader
(`WebView2Loader.x64.dll` / `WebView2Loader.x86.dll`) from the module directory
(Runtime: `Common Files\OpenDCL`; Studio: install dir). Vendored NuGet files keep
the stock `WebView2Loader.dll` name under `x64/` and `x86/`; WiX/CMake rename on
copy so both arches share one folder. The control uses the machine Evergreen
WebView2 Runtime. If the loader or runtime is missing, it falls back to IE
`CLSID_WebBrowser` hosted in a separate child `CHtmlView`.
