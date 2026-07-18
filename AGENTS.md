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
OpenDCL.sln              Visual Studio solution
Common/                  Shared core + per-language SharedRes (<LANG>/)
Library/                 Third-party (LibPNG, ZLib)
Runtime/
  ARX|BRX|GRX|ZRX/       Host-specific module projects
  Localized/<LANG>/      Runtime.Res + License.txt
  RxInstall/             Demand-load installer DLL (used as WiX Binary CA)
Studio/
  Win32/                 OpenDCL Studio.exe
  Localized/<LANG>/      Studio.Res, Content (help + samples), HTMLHelp project
scripts/build-wix.ps1    Packaging entry point
wix/                     WiX sources, UI bitmaps/icons, tools
wix/out/                 Generated packages (gitignored)
.agents/skills/          Optional agent skills (often untracked until ready)
```

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
{ProgramFiles}\OpenDCL Consortium\OpenDCL Studio\
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

Runtime module inventory: **`$RuntimeModules`** in `build-wix.ps1`. Do not hand-edit
`wix/out/gen/*.wxs`.

### Demand-load commands (`RxInstall`)

`Runtime/RxInstall/RxInstall.cpp` registers CAD Application keys:

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
- Preserve file encodings (many localized `.rc` / `License.txt` are **UTF-16 LE**).
- Do not commit generated `wix/out/**`, CHMs, or CAD SDK binaries.
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
