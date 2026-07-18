# OpenDCL WiX packaging

WiX Toolset **v3.14** sources that build installers without Visual Studio `.vdproj`.

## Deliverables

| Output | Purpose |
| --- | --- |
| **`OpenDCL.Runtime.msm`** | Merge module for third-party installers **and** for our own Runtime/Studio MSIs |
| `OpenDCL.Runtime.msi` | Thin Runtime installer (merges the MSM) |
| `OpenDCL.Studio.<LANG>.msi` | Studio + Runtime MSM (ENU, DEU, ESM, RUS, CHS, FRA, CHT) |

The **`.msm` is intentional and permanent** — other developers embed it in their own installers.

## Prerequisites

1. Successful **Release** compile (all ARX/BRX/GRX/ZRX, Runtime.Res, RxInstall, Studio, Studio.Res, CHM).
2. [WiX Toolset v3.14](https://wixtoolset.org/) (`candle.exe` / `light.exe`).

## Build

From this repository root (after a Release compile):

```powershell
.\scripts\build-wix.ps1

# MSM only (fast iterate):
.\scripts\build-wix.ps1 -SkipStudio -SkipRuntimeMsi
```

Primary outputs: **`wix\out\Release\`** only (no copies into `Runtime\Install\...` or `Studio\Localized\...\Release`).

Studio install layout (matches legacy MSI / app expectations):

```text
{ProgramFiles}\OpenDCL Consortium\OpenDCL Studio\
  OpenDCL Studio.exe          # product root
  ENU\                        # (or DEU, …)
    Studio.Res.dll
    License.htm / .txt / .rtf
    OpenDCL.chm, GNU-GPL.txt
    Samples\...
```

Override versions when cutting a release:

```powershell
.\scripts\build-wix.ps1 -ModuleVersion 10.1.1.1 -ProductVersion 10.1.101
```

## Post-package steps (manual today — automate later)

Historical release process after a successful build:

1. **`Build 9.0\!MakeNewDist.bat`** — versioned copies of MSM/MSI to the parent OpenDCL folder, plus prep/zip helpers.  
2. **`@SignAll.bat`** / **`#Sign.bat`** — Authenticode-sign `*.ms?` with the local hardware token (secrets stay off git).

A future dist script should rename from `wix\out\Release\` to versioned names, then sign, then publish (e.g. GitHub Releases).

## UI branding (Studio MSI)

Checked-in exact-size bitmaps under `wix\ui\` (package script copies them; no stretch at build time):

| File | Size | Content |
| --- | --- | --- |
| `WixUIBanner.bmp` | **493×58** | Color OpenDCL logo at **native** pixel size on the right; left clear for WixUI captions |
| `WixUIDialog.bmp` | **493×312** | CAD-themed left strip (164×312 art, brand blues); white right for UI text |

- **License** dialog embeds each language’s checked-in `Studio\Localized\<LANG>\Content\License.rtf` (WixUI ScrollableText supports RTF only, not HTML).  
  - `License.htm` remains for help/site-style viewing.  
  - `License.rtf` is the installer EULA source of truth — edit and review it like any other source; **do not** regenerate it silently during package builds.

## Design notes

- **No Detected Dependencies** — only intentional product files (modules, `Runtime.Res`, licenses, Studio app/help/samples). VC++ CRT/MFC redistributables are not bundled.
- **Module Guid** `0C4E4759-A6C2-4D0E-BAE5-7719C3BCF049` matches the historical VS MSM modularization id.
- **RxInstall** custom actions preserved (machine/user install + uninstall); user CAs impersonate.
- **ProductVersion** is 3-part MSI encoding (`10.1.101` for file version `10.1.1.1`). **ModuleVersion** is 4-part.
- **UpgradeCodes** preserved from the old Studio language packages / Runtime MSI.
- Component GUIDs are **stable** (MD5 of logical path).
- Studio shortcuts are **advertised** and nested on their target files (ICE69 / ICE43).

## Layout

```text
wix/
  Runtime/OpenDCL.Runtime.wxs       # merge module + RxInstall CAs
  Runtime/OpenDCL.Runtime.MSI.wxs   # thin Runtime MSI
  Studio/OpenDCL.Studio.wxs         # language MSI template
  out/gen/                          # generated fragments (build-time, gitignored)
  out/Release/                      # msm + msi outputs (gitignored)
scripts/build-wix.ps1
```
