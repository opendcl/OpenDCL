# WixUI bitmap assets

Exact pixel sizes required by WiX Toolset v3 `WixUI_InstallDir`:

| File | Pixels | Notes |
| --- | --- | --- |
| `WixUIBanner.bmp` | 493×58 | Logo on the **right** at native size (no upscaling). Left remains blank for captions. |
| `WixUIDialog.bmp` | 493×312 | Left ~164px CAD-themed art; right pure white for text. |

Start Menu icons live under `icons/` (see `icons/badge/README.md` for the shared crosshair master).

When replacing art, keep the exact BMP dimensions above so the installer never stretches UI chrome.
