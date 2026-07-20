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

1. Prior **completed** compile of the modules/languages you intend to ship:
   - **Full product release:** all ARX/BRX/GRX/ZRX, all `Runtime.Res`, RxInstall, Studio, Studio.Res, CHM.
   - **Custom installer:** only the chosen runtimes + language packs (+ RxInstall; Studio optional via `-SkipStudio`).
2. Run **`scripts/verify-build-outputs.ps1`** (or let `make-release.ps1` run it). **Do not package mid-build.**
3. [WiX Toolset v3.14](https://wixtoolset.org/) (`candle.exe` / `light.exe`).

Path resolve: OpenDclRoot / `out\` / packaging repo only.  
Smoke: **[docs/SMOKE.md](../docs/SMOKE.md)**. Package diffs: previous release set via
`scripts/compare-release-packages.ps1`.

## Build

From this repository root (after a Release compile):

```powershell
# Preferred full local release (verify → WiX → dist → loc zips [→ sign]):
.\scripts\make-release.ps1 -OpenDclRoot (Resolve-Path build\vs2022-full) `
  -ProductVersion 10.1.1.1 -ModuleSet Full

# Full product WiX only:
.\scripts\build-wix.ps1 -OpenDclRoot (Resolve-Path build\vs2022-full)

# MSM only (fast iterate on full inventory):
.\scripts\build-wix.ps1 -OpenDclRoot (Resolve-Path build\vs2022-full) -SkipStudio -SkipRuntimeMsi

# Custom subset — explicit runtimes + language(s)
.\scripts\build-wix.ps1 -OpenDclRoot (Resolve-Path build\vs2022-full) `
  -Runtimes BRX.27.x64 -Languages ENU -SkipStudio

# Custom subset — package whatever modules/langs are present under -OpenDclRoot/out
.\scripts\build-wix.ps1 -ModuleSet Available -AvailableLanguages -SkipStudio `
  -OpenDclRoot (Resolve-Path build\vs2022-full)
```

| Switch | Role |
| --- | --- |
| `-ModuleSet Full` | Default. All catalog modules; missing files fail. Ship MSM identity. |
| `-ModuleSet Selected` | Only `-Runtimes` (required). Missing files fail. |
| `-ModuleSet Available` | Catalog (optionally filtered by `-Runtimes`) where files exist under OpenDclRoot/`out`. |
| `-Runtimes` | IDs (`BRX.27.x64`), families (`BRX`), wildcards (`*.27.x64`). Implies Selected if set under Full. |
| `-Languages` | Subset of ENU DEU ESM RUS CHS FRA CHT. Empty = all (or all present with `-AvailableLanguages`). |
| `-AvailableLanguages` | Drop languages without `Runtime.Res`. |

Custom packages write **`OpenDCL.Runtime.custom.msm` / `.msi`** with seed-based modularization/upgrade GUIDs so they never overwrite or collide with the historical full-product MSM GUID used by third parties.

Primary outputs: **`wix\out\Release\`** only (no copies into `Runtime\Install\...` or `Studio\Localized\...\Release`).

Studio install layout (no hard-coded `Program Files (x86)` paths in product code —
registry and shortcuts use MSI `[INSTALLDIR]`):

```text
# x64 Studio.exe  →  [ProgramFiles64Folder]\OpenDCL Studio\   (C:\Program Files\…)
# x86 Studio.exe  →  [ProgramFilesFolder]\OpenDCL Studio\     (C:\Program Files (x86)\…)
{INSTALLDIR}\
  OpenDCL Studio.exe
  ENU\                        # (or DEU, …)
    Studio.Res.dll
    License.htm / .txt / .rtf
    OpenDCL.chm, GNU-GPL.txt
    Samples\...
```

Runtime MSM remains under **Common Files\OpenDCL** (historical x86 MSM /
`CommonFilesFolder`; modules are still found via RxInstall install-dir custom action).

Override versions when cutting a release:

```powershell
.\scripts\build-wix.ps1 -ModuleVersion 10.1.1.1 -ProductVersion 10.1.101
```

## Post-package steps (WiX → dist → localization → sign → release)

After a successful `.\scripts\build-wix.ps1` (outputs in `wix\out\Release\`), prefer
the **combined** helper:

```powershell
# One-shot: package + versioned MSI/MSM + localization zips [+ sign]
.\scripts\make-release.ps1 -ProductVersion 10.1.1.1 -Sign
```

| Step | Script / workflow | Replaces |
|------|-------------------|----------|
| Full pipeline | `scripts/make-release.ps1` / **Make release** workflow | `!MakeNewDist` + localization zips + `@SignAll` |
| Versioned installers only | `scripts/make-dist.ps1` | `!MakeNewDist.bat` COPY lines |
| Localization zips | `scripts/make-localization-zips.ps1` | `!MakeLocalizationZips.bat` |
| Authenticode sign | `scripts/sign-files.ps1` | `@SignAll.bat` / `#Sign.bat` |
| Product GitHub Release | `.github/workflows/release.yml` (**Make release**) | Manual upload |
| Package + dist (CI) | `.github/workflows/package.yml` | Ad-hoc package on build PC |
| Localization-only refresh | `.github/workflows/localization-packs.yml` | Standalone translator packs |

`make-release.ps1` writes everything under `dist\<ver>\`:

```text
OpenDCL.Runtime.<ver>.msi / .msm
OpenDCL.Studio.<LANG>.<ver>.msi
OpenDCL.<LANG>.zip                 # localization packs (same folder)
```

Example step-by-step (same result as `make-release.ps1` without `-Sign`):

```powershell
.\scripts\build-wix.ps1 -ModuleVersion 10.1.1.1 -ProductVersion 10.1.101
.\scripts\make-dist.ps1 -ProductVersion 10.1.1.1
.\scripts\make-localization-zips.ps1 -OutDir .\dist\10.1.1.1
.\scripts\sign-files.ps1 -Path .\dist\10.1.1.1 -CertThumbprint "<store-thumbprint>"
gh release create "v10.1.1.1" .\dist\10.1.1.1\* --title "OpenDCL 10.1.1.1"
```

**Signing (YubiKey + SSL.com):** insert YubiKey, set `SIGN_CERT_THUMBPRINT` (or pass
`-CertThumbprint`), run `sign-files.ps1` / `make-release.ps1 -Sign`, enter PIN when
prompted. Default timestamp is `http://ts.ssl.com`. Never commit the PIN. See skill
**`code-sign-release`**.

Asset naming expected by [opendcl.github.io](https://opendcl.github.io/) /
`assets/versions.js`:

```text
OpenDCL.Runtime.<ver>.msi / .msm
OpenDCL.Studio.<LANG>.<ver>.msi
```

Tag: `v<ver>` (e.g. `v10.1.1.1`).

**Localization packs** (translator zips, not installers):  
`.\scripts\make-localization-zips.ps1` and workflow
`.github/workflows/localization-packs.yml` (rolling GitHub Release
`localization-packs`). Public UI: https://opendcl.github.io/localization/

**Online help** refresh: skill `/sync-help-to-website` (Studio Content →
`opendcl.github.io/HelpFiles/`).

**Runtime update-check (manual):** after publishing a release, update
`opendcl.com/version/version.txt` (stable) and/or `version_dev.txt` (dev) to the
new `A.B.C.D` string. The Runtime POSTs to `/version/vercheck.php` — see skill
`code-sign-release`.
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
- Studio shortcuts are **advertised** and nested on their target files
  (`Advertise="yes"` on the same component as the file):
  - **ICE69** — advertised shortcut must share the component with its target file
  - **ICE43 / ICE57** — non-advertised shortcuts require an **HKCU** registry keypath;
    using a per-machine file keypath fails light (do not flip Help/License to
    `Advertise="no"` without adding HKCU keypaths on those components)

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
