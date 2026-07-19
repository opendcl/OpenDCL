# AGENTS.md — OpenDCL

Durable guidance for coding agents working in this repository. Prefer this file
plus the skills under `.agents/skills/` over reverse-engineering session history.

## Product

OpenDCL is a dialog/UI toolkit for AutoLISP on AutoCAD-compatible hosts
(AutoCAD/ObjectARX, BricsCAD/BRX, GstarCAD/GRX, ZWCAD/ZRX). It includes:

- **Runtime** — host modules (`.arx` / `.brx` / `.grx` / `.zrx`) + `RxInstall` demand-load registration
- **Studio** — dialog editor, localized help (HTML → CHM), samples
- **Packaging** — WiX Toolset v3 (`scripts/build-wix.ps1`, `wix/`)

License: **GPLv2+** (`LICENSE`). Copyright OpenDCL Consortium.

Current product version is defined in native resources (e.g. `Runtime/ARX.rc`
`FILEVERSION` / `PRODUCTVERSION`). Do not trust stale version strings in docs
without checking those resources.

## Repository layout

```text
OpenDCL.sln              Classic Visual Studio solution
CMakeLists.txt           CMake experiment (multi-runtime + Studio + RxInstall)
cmake/                   Matrix, targets, helpers (see CMAKE.md)
Common/                  Shared core + per-language SharedRes (<LANG>/)
Library/                 Third-party (LibPNG, ZLib); CMake builds /MD and /MT variants
Runtime/
  ARX|BRX|GRX|ZRX/       Host-specific module projects (+ VI props)
  Localized/<LANG>/      Runtime.Res + License.txt
  RxInstall/             Demand-load installer DLL (used as WiX Binary CA)
Studio/
  Studio.vcxproj         Classic Studio (static MFC + /MT)
  Localized/<LANG>/      Studio.Res, Content (help + samples), HTMLHelp project
scripts/build-wix.ps1    Packaging entry point
wix/                     WiX sources, UI bitmaps/icons, tools
wix/out/                 Generated packages (gitignored)
.agents/skills/          Optional agent skills (often untracked until ready)
```

### CMake experiment (preferred for multi-host matrix + Studio F5)

Details: **`CMAKE.md`**, presets in `CMakePresets.json` (dev default `vs2022-x64-dev`).

| Area | Notes |
|------|--------|
| Configs | `Debug`, `FullDebug`, `Release` |
| CAD runtime modules | Real FullDebug product (`AC_FULL_DEBUG`, **`/MDd`**, host debug libs). **Do not scan proprietary debug SDK trees.** |
| Everything else | **FullDebug → Debug** on-disk outputs (`opendcl_map_fulldebug_to_debug` / `OPENDCL_CFG_DIR`): Studio, Studio.Res, Runtime.Res, RxInstall, Studio `/MT` zlib/png. Exception: `/MD` zlib/png keep FullDebug for `/MDd` module link. |
| Studio | Static MFC + `/MT`; `COMPILE_MULTIMON_STUBS` on **`PPTooltip.cpp` only** (not project-wide — `FolderTreeCtrl.cpp` also includes `MultiMon.h`). Post-build copies `Studio.Res.dll` + ENU `OpenDCL.chm` **next to** Studio.exe so classic `Workspace` path logic works. |
| ENU CHM | Target `OpenDCL_StudioHelp_ENU` (depends from Studio); needs HTML Help Workshop `hhc.exe`. Output `Studio/Localized/ENU/Content/OpenDCL.chm` (gitignored). |
| Packaging paths | `build-wix.ps1` `Resolve-ProductFile`: classic first, then CMake `out\…`. |
| Studio.rc encoding | **Windows-1252** (no BOM); copyright is single-byte `0xA9` (©). Do not re-save as UTF-8. |

Sibling repos (typical under the same `Source/` parent):

| Path | Role |
|------|------|
| `opendcl.github.io` | GitHub Pages site + online `HelpFiles/` |
| Historical trees (`OpenDCL 9.0`, etc.) | Archives only — not the packaging path |

## Languages

Folder codes: **ENU, DEU, ESM, FRA, RUS, CHS, CHT**.

Each language generally needs:

- `Common/<LANG>/`
- `Runtime/Localized/<LANG>/`
- `Studio/Localized/<LANG>/` (including `Content/`, `Studio.Res/`, `HTMLHelp.<LANG>.vcxproj`)
- WiX: `$RuntimeLangs` + `$StudioLangMeta` in `scripts/build-wix.ps1` (LCID, UpgradeCode, ARP comments)

Installer EULA for Studio is checked-in **`License.rtf`** (WixUI); also keep
`License.txt` / `License.htm` in sync for content/copyright. **`OpenDCL.chm`**
is built from Content and is **gitignored** (`*.chm`).

Adding a language: skill **`add-language`** (`.agents/skills/add-language/`).

## Packaging (WiX — not Visual Studio Installer)

`.vdproj` projects were **removed**. Do not reintroduce them.

| Output | Location |
|--------|----------|
| `OpenDCL.Runtime.msm` | Merge module (third parties embed this) |
| `OpenDCL.Runtime.msi` | Thin Runtime MSI |
| `OpenDCL.Studio.<LANG>.msi` | Studio + merged Runtime MSM |

```powershell
# After a successful Release compile of product + localized Res + CHM:
.\scripts\build-wix.ps1
.\scripts\build-wix.ps1 -Languages ENU -SkipMsm -SkipRuntimeMsi   # iterate Studio only
```

Outputs: **`wix/out/Release/`** only (gitignored). Details: `wix/README.md`.

### Studio install layout

```text
{ProgramFiles}\OpenDCL Studio\
  OpenDCL Studio.exe
  <LANG>\
    Studio.Res.dll, License.*, OpenDCL.chm, Samples\...
```

### Version encoding for WiX

| Form | Example (`10.1.1.1`) | Where |
|------|----------------------|--------|
| RC comma | `10, 1, 1, 1` | `.rc` FILEVERSION/PRODUCTVERSION |
| ModuleVersion | `10.1.1.1` | MSM `Module/@Version` |
| ProductVersion | `10.1.101` | MSI (`Patch*100+Build`) |

Defaults live in `scripts/build-wix.ps1` (`-ProductVersion`, `-ModuleVersion`).
Leave **UpgradeCodes** and MSM modularization GUID (`0C4E4759-…`) stable.
Component GUIDs are stable MD5 seeds of logical paths.

Runtime module **catalog** lives in `scripts/build-wix.ps1` (`$RuntimeModuleCatalogRels`).
Do not hand-edit `wix/out/gen/*.wxs`. Paths resolve classic first, then CMake
`out\<classic-rel>` via `Resolve-ProductFile`. RxInstall Binary CA uses
candle define `RxInstallDll`.

Packaging modes:
- **Full product** (default): all catalog modules + all langs → `OpenDCL.Runtime.msm`
  with historical modularization GUID.
- **Custom**: `-Runtimes` / `-ModuleSet Selected|Available` / language filters →
  `OpenDCL.Runtime.custom.msm` with seed GUIDs (safe for local/dev installers).

Newest BRX ship row: `Runtime\BRX\BRX.27.x64\Release\OpenDCL.x64.27.brx`
(registry `Bricsys\Bricscad\V27x64`).

### Demand-load commands (`RxInstall`)

`Runtime/RxInstall/RxInstall.cpp` registers CAD Application keys for each known
host target **only when the matching module file exists** under the install
folder (`Common Files\OpenDCL\OpenDCL*.arx|brx|grx|zrx`). Full product packages
include every module; custom WiX subsets therefore register only what they ship.

- **Runtime-only:** Commands = `OPENDCL`
- **Studio installed:** Commands = `OPENDCL` + **`OPENDCLDEMO`**

Studio detection: **`HKCR\OpenDCL.Project`** must exist (file association for
`.odcl`). WiX Studio MSI writes this via registry components in
`New-StudioFilesFragment` (same role as old vdproj registry). If that ProgID is
missing, installs look like Runtime-only and **OPENDCLDEMO is not registered**.

Deferred CA `RxInstallMachine` runs after `StartServices` (after
`WriteRegistryValues`), so association keys must be authored in the MSI.

Per-machine Studio install requires elevation (silent non-admin → 1925/1603).

## Icons and WixUI art

Checked-in production assets only under `wix/ui/`:

| Asset | Path |
|-------|------|
| Banner / dialog | `wix/ui/WixUIBanner.bmp` (493×58), `WixUIDialog.bmp` (493×312) |
| Help / License shortcuts | `wix/ui/icons/OpenDCLHelp.ico`, `OpenDCLLicense.ico` |
| Crosshair badge master | `wix/ui/icons/badge/OpenDCLBadge.png` (+ size set / ICO) |
| Generators | `wix/tools/make_badge.py`, `make_help_restyle.py` |

Studio app icon: extracted at package time from **`Studio.Res.dll` GROUP_ICON #10**
(multi-res). Icon table Ids must end in **`.exe` / `.ico`** (ICE50 / Start Menu).
Advertised shortcuts use Icon table streams — rebuild MSI to refresh Start Menu icons.

Style: multi-tone Windows blue `(0,120,208)`, white glyphs; badge ~31% of 256px
canvas, full-span white cross (no blue tip border). Skill: **`make-package-icons`**.

## Website help

Sibling repo **`opendcl.github.io`**:

- Online help: `HelpFiles/<LANG>/` (published set has historically been ENU, DEU, RUS)
- Source topics: `Studio/Localized/<LANG>/Content/`
- Sync: copy HTML/css/js/images; **omit** Samples, CHM, hhp/hhc, License.txt/rtf, GNU-GPL, `@Template.htm`
- **Preserve** website chrome: `TOC.html`, `Top.html`
- Frame allowlist in `HelpFiles/index.html` must list online languages

Skill: **`sync-help-to-website`**. Downloads use GitHub Releases + `assets/versions.js`
(not large MSIs in the Pages repo).

## Agent skills

Optional project skills under **`.agents/skills/<name>/SKILL.md`**:

| Skill | Use for |
|-------|---------|
| `add-runtime-target` | New ARX/BRX/GRX/ZRX host module |
| `add-language` | New UI/help language pack |
| `bump-version` | Product version + WiX version defaults |
| `update-copyright-year` | Copyright years in rc/help/licenses |
| `make-package-icons` | Start Menu / WiX icons + badge |
| `sync-help-to-website` | Push help HTML to opendcl.github.io |
| `code-sign-release` | Authenticode sign + GitHub Release assets |

These may remain **untracked** until polished — that is intentional. Do not assume
they are absent if the folder exists on disk. Invoke with `/skill-name` when present.

## Gitignore (relevant)

- `wix/out/` — packages and gen fragments
- `*.chm` — compiled help
- `OpenDCL.Compile.slnf` — local compile filter
- Build outputs: `Release/`, `Debug/`, `*.dll`, `*.exe`, etc. (see `.gitignore`)
- **Do not ignore `.agents/`** merely because skills are uncommitted; leave them
  visible as untracked until the team chooses to commit them

## Coding and change norms

- Prefer small, task-scoped diffs; do not drive-by refactors unrelated to the request.
- Preserve file encodings (many localized `.rc` / `License.txt` are **UTF-16 LE**;
  `Studio/Studio.rc` and `Runtime/ARX.rc` are often **Windows-1252** with `©` as `0xA9`).
- Do not commit generated `wix/out/**`, CHMs, or CAD SDK binaries.
- Prefer CMake side-by-side copies / classic `Workspace` paths over hard-coding
  F5 layout parsers in `Common/Workspace.cpp`.
- Host SDKs (ObjectARX, BRX, …) are external; projects expect local env/props macros.
- Release-style commits historically look like:

  ```text
  OpenDCL A.B.C.D
  - Bullet describing the change.
  ```

## Release pipeline (post-build)

Historical desktop steps and replacements:

| Legacy | Replacement |
|--------|-------------|
| `!BuildRelease.bat` | Self-hosted compile (VS/MSBuild; private build-lab workflows optional) |
| vdproj / early WiX package | `scripts/build-wix.ps1`, `.github/workflows/package.yml` |
| Full post-package release | `scripts/make-release.ps1` + **Make release** workflow |
| `!MakeNewDist.bat` | `scripts/make-dist.ps1` (also called by make-release) |
| `@SignAll.bat` / `#Sign.bat` | `scripts/sign-files.ps1` (`make-release -Sign`) |
| `!MakeLocalizationZips.bat` | `scripts/make-localization-zips.ps1` (also in make-release) |
| Manual download upload | **Make release** / `gh release create` |
| Online help SVN refresh | `/sync-help-to-website` → `opendcl.github.io` |

**Code signing (production):** SSL.com cert on **YubiKey** → Windows Personal store →
`scripts/sign-files.ps1 -CertThumbprint …` (or `SIGN_CERT_THUMBPRINT`). Enter **PIN**
when prompted. Timestamp default `http://ts.ssl.com`. Self-hosted runner only.
Never commit PINs/PFX. See `/code-sign-release`.

**Manual after every release (not automated yet):** Runtime “check for updates”
POSTs to `http://opendcl.com/version/vercheck.php`, which reads plain-text
`version/version.txt` (stable product name `OpenDCL Runtime`) or
`version/version_dev.txt` (dev builds `OpenDCL Runtime Dev`). Update those files
on the **opendcl.com** host to the new `A.B.C.D` so clients learn about the
release. Details in `/code-sign-release`.

**Pre-ship dry run (private):** compile/package/test installers without publishing
lives in private **`opendcl/build-lab`** (`RELEASE.md`, skill `dry-run-release`) —
not in this public tree.

## Quick command map

| Goal | Start here |
|------|------------|
| New CAD year/host | `/add-runtime-target` → `/bump-version` |
| New language | `/add-language` → package → optional `/sync-help-to-website` |
| Version only | `/bump-version` |
| Copyright year | `/update-copyright-year` |
| Installers | Release build, then `.\scripts\build-wix.ps1` |
| Full release folder | `.\scripts\make-release.ps1 -ProductVersion A.B.C.D` |
| Versioned dist only | `.\scripts\make-dist.ps1 -ProductVersion A.B.C.D` |
| Localization zips only | `.\scripts\make-localization-zips.ps1` |
| Code sign / GitHub Release | `/code-sign-release`, **Make release** workflow |
| Start Menu icons | `/make-package-icons`, assets under `wix/ui/` |
| Online help | Edit Studio Content, then `/sync-help-to-website` |

## When uncertain

1. Read the relevant skill under `.agents/skills/` if present.
2. Read `wix/README.md` for packaging.
3. Prefer copying the nearest existing host/language project over inventing layout.
4. Ask before force-pushing, signing binaries, or publishing releases.

## CAD SDK debug trees

FullDebug links against **host debug** import libraries. Those directories are proprietary.
Do **not** open, list, search, or copy contents of CAD SDK debug folders.
Configure paths only via `BrxDebugLibs`, `OPENDCL_*_FULLDEBUG_LIBDIR`, or documented
`SDK_LIB_FULLDEBUG` relative paths — never by discovering files inside debug trees.

## Machine MSBuild overlays

- `local.props` — optional, next to generated .sln (gitignored if under the repo).
- `<parent-of-checkout>/dev.props` — optional shared machine file (outside the repo).
  Imported **after** local.props so it can override include/lib paths and `BRX_PATH`.
  Do not open or scan proprietary CAD debug trees when editing these files.
