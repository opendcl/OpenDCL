# Microsoft Edge WebView2 (Evergreen loader)

Vendored from NuGet `Microsoft.Web.WebView2` **1.0.4191.47** (BSD-3-Clause, `LICENSE.txt`).

The Html control delay-loads `WebView2Loader.dll` from the module directory (or
`WebView2\x64` / `WebView2\x86` beside the runtime install folder) and uses the
machine Evergreen WebView2 Runtime. If the loader or runtime is missing, the
control falls back to IE `CLSID_WebBrowser` hosted in a separate child `CHtmlView`.
