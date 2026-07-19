---
name: add-language
description: >
  Add a new UI/help language pack to OpenDCL (Common SharedRes, Runtime.Res,
  Studio.Res, HTML help Content, WiX Studio MSI meta, solution wiring). Use when
  adding a locale, cloning ENU/CHS into a new folder code, or when the user runs
  /add-language. Triggers: "add language", "new language", "new locale",
  "localize", "CHT", "Studio language MSI", "language pack".
---

# Add Language

Scaffold a full OpenDCL language pack by **cloning an existing language** and
rewiring identity (folder code, project names, GUIDs, LCID, WiX meta). Based on
the Traditional Chinese (CHT) addition in commit `e1bbd787` and the current
in-repo WiX packaging layout.

This skill does **not** perform translation for you. It creates a buildable pack
(usually cloned from ENU or a related language) ready for localization.

Website online-help publishing is separate — after help HTML is ready, use
`/sync-help-to-website`.

## Existing language codes

| Code | Typical meaning | Win32 LCID (Studio MSI) |
|------|-----------------|-------------------------|
| ENU | English (US) | 1033 |
| DEU | German | 1031 |
| ESM | Spanish (modern sort) | 3082 |
| FRA | French | 1036 |
| RUS | Russian | 1049 |
| CHS | Chinese Simplified | 2052 |
| CHT | Chinese Traditional | 1028 |

Confirm **3-letter code**, **display name**, and **LCID** with the user before
editing. Prefer standard Windows language IDs.

## Trees that must gain a language folder

Clone from a source language (default **ENU**, or **CHS** when adding a Chinese
variant). Do **not** copy build outputs (`Debug/`, `Release/`, `.tlog`, `.obj`,
`.chm` if regenerable, etc.).

| Area | Path pattern | Purpose |
|------|--------------|---------|
| Shared resources | `Common/<LANG>/` (`Res/`, `SharedRes.<LANG>.rc`) | Shared bitmaps/icons strings |
| Runtime resources | `Runtime/Localized/<LANG>/Runtime.Res/` | `Runtime.Res.<LANG>.vcxproj`, `.rc`, `Res/` |
| Runtime content | `Runtime/Localized/<LANG>/Content/` | `License.txt`, `GNU-GPL.txt` |
| Studio resources | `Studio/Localized/<LANG>/Studio.Res/` | `Studio.Res.<LANG>.vcxproj`, `.rc`, `Res/` |
| Studio help | `Studio/Localized/<LANG>/Content/` | HTML topics, `OpenDCL.hhp` / `.hhc`, licenses, Samples |
| HTML Help project | `Studio/Localized/<LANG>/HTMLHelp.<LANG>.vcxproj` (+ `.filters`) | Builds `Content/OpenDCL.chm` via `Studio/Localized/BuildCHM.mak` |
| Optional | `Studio/Localized/<LANG>/Setup.reg` | Legacy reg sample; WiX owns real file-assoc registry now |

Also wire:

- `OpenDCL.sln` — project entries, configs, solution folder for the new language
- `OpenDCL.Compile.slnf` if the local compile filter is used
- `scripts/build-wix.ps1` — `$RuntimeLangs` + `$StudioLangMeta`
- Product version often bumps when shipping the language (`/bump-version`)

## Inputs to collect

1. **New language code** (3 letters, e.g. `ITA`, `JPN`) — must match folder names.
2. **Clone source** (default `ENU`).
3. **English display name** for ARP comments (e.g. `OpenDCL Studio (Italian)`).
4. **LCID** for WiX `ProductLanguage` / MSI language.
5. **UpgradeCode policy**:
   - Prefer a **new** stable UpgradeCode GUID for the language MSI.
   - Exception: historical CHT shares UpgradeCode with CHS — only reuse when the
     user explicitly wants upgrade continuity with an existing package.
6. **ProductCode**: generate a new GUID (historical Studio packages used fixed
   ProductCodes in `$StudioLangMeta`; WiX Studio `Product Id="*"` still auto-
   generates build ProductCodes, but meta table keeps the historical codes for
   documentation/continuity — follow existing pattern when adding a row).
7. Whether to **bump product version** for the release that introduces the language.

## Workflow

### 1. Clone folder trees

Copy (PowerShell examples):

```powershell
$src = "ENU"; $dst = "ITA"   # example
Copy-Item -Recurse "Common\$src" "Common\$dst"
Copy-Item -Recurse "Runtime\Localized\$src" "Runtime\Localized\$dst"
Copy-Item -Recurse "Studio\Localized\$src" "Studio\Localized\$dst"
```

Then **rename** project files inside the clone:

- `SharedRes.ENU.rc` → `SharedRes.<LANG>.rc`
- `Runtime.Res.ENU.vcxproj` → `Runtime.Res.<LANG>.vcxproj` (+ filters)
- `Runtime.ENU.rc` → `Runtime.<LANG>.rc` (if so named)
- `Studio.Res.ENU.vcxproj` → `Studio.Res.<LANG>.vcxproj` (+ filters)
- `Studio.ENU.rc` → `Studio.<LANG>.rc`
- `HTMLHelp.ENU.vcxproj` → `HTMLHelp.<LANG>.vcxproj` (+ filters)

Strip accidental build artifacts under the new trees.

### 2. Retarget project identity

In every new `.vcxproj` / `.rc` / `.filters`:

1. Replace source language code with the new code in names, paths, and preprocessor
   includes (careful with whole-word replaces so you do not damage unrelated strings).
2. Assign **new `ProjectGuid`** values (never reuse the clone’s GUIDs).
3. Update `RootNamespace` / output names to the new language suffix.
4. Ensure `.rc` version resources match current product version and copyright year
   (or run `/bump-version` + `/update-copyright-year` as part of the release).
5. Encoding: many localized `.rc` files are **UTF-16 LE**; preserve encoding when
   editing (same rules as `/bump-version`).

### 3. Solution wiring (`OpenDCL.sln`)

1. Add a solution folder for the language if other langs use one (see CHT folder).
2. Add projects:
   - SharedRes (if present as a project entry pattern for that language)
   - `Runtime.Res.<LANG>`
   - `Studio.Res.<LANG>`
   - `HTMLHelp.<LANG>`
3. Copy configuration lines from the source language’s projects for all solution
   configs (`Debug|…`, `Release|…`, `FullDebug|…`, platforms).
4. Nest projects under the correct solution folders.

Update `OpenDCL.Compile.slnf` when that filter is part of the build workflow.

### 4. Help content and CHM

1. `Studio/Localized/<LANG>/Content/` holds the HTML help set.
2. Start from the clone; translators edit `.htm` (and samples if localized).
3. Keep structural parity with ENU (topic paths, `OpenDCL.hhc` TOC, `OpenDCL.hhp`).
4. Ensure installer license sources exist:
   - `License.txt`, `License.htm`, **`License.rtf`** (WiX EULA)
   - `GNU-GPL.txt`
5. Build CHM via the HTMLHelp project (`BuildCHM.mak` / HTML Help Workshop toolchain).
   Output: `Studio/Localized/<LANG>/Content/OpenDCL.chm` (gitignored `*.chm`).
   CMake dev build wires **ENU only** today (`OpenDCL_StudioHelp_ENU` + Studio
   dependency). Other languages still use classic `HTMLHelp.<LANG>.vcxproj` until
   multi-lang help is ported; add `OPENDCL_LANGS` / packaging when shipping.

### 5. WiX packaging

Edit **`scripts/build-wix.ps1`**:

1. Append the code to `$RuntimeLangs` (order with existing languages).
2. Add `$StudioLangMeta['LANG']` entry:

   ```powershell
   ITA = @{
     ProductCode = "<new-guid>"
     UpgradeCode = "<new-stable-guid>"
     LangId = <LCID>
     Comments = "OpenDCL Studio (Italian)"
   }
   ```

3. Runtime language components are generated from `$RuntimeLangs` + Content paths —
   no hand-edit of `wix/out/gen/*`.
4. Studio files fragment picks up `Studio/Localized/<LANG>/…` when that language is
   built (`-Languages` / default full list).

Confirm `wix/README.md` language list still accurate if it enumerates languages.

### 6. Application / registry expectations

WiX Studio MSIs write per-language `Software\OpenDCL` settings (`Language`,
`SamplesFolder` under `[INSTALLDIR]<LANG>\Samples\`, etc.) via
`New-StudioFilesFragment`. No `.vdproj` work.

Demand-load **OPENDCLDEMO** depends on `HKCR\OpenDCL.Project` from Studio install
(file association), not on language code.

### 7. Build and smoke-check

1. Build `Runtime.Res.<LANG>` and `Studio.Res.<LANG>` (Release).
2. Build `HTMLHelp.<LANG>` → `OpenDCL.chm` present.
3. Package:

   ```powershell
   .\scripts\build-wix.ps1 -Languages <LANG> -SkipMsm -SkipRuntimeMsi
   # or full package when cutting a release
   .\scripts\build-wix.ps1
   ```

4. Install the new `OpenDCL.Studio.<LANG>.msi` elevated; verify Start Menu, help CHM,
   language folder under install dir, and `Software\OpenDCL` language value.

### 8. Website / downloads (follow-ups)

1. **Download page** (`opendcl.github.io`): add Studio MSI language entry if missing
   (`download/` + `assets/versions.js` patterns — see existing CHT `currentOnly` flag).
2. **Online help**: only some languages are published under `HelpFiles/` today
   (ENU, DEU, RUS). When help HTML is ready, run **`/sync-help-to-website`**.
3. **Localization packs**: after the language lands on `main`, the
   `localization-packs` workflow rebuilds translator zips; localizers can also
   use https://opendcl.github.io/localization/ (“Build from main”).
4. Do not assume every Studio language automatically gets a HelpFiles folder.

### 9. Capture lessons into this skill

Update this file with LCID choices, UpgradeCode decisions, clone pitfalls, or
tooling requirements (HTML Help Workshop, code pages). Remove outdated guidance.

### 10. Commit message style

```text
OpenDCL A.B.C.D
- Added <Language> (<CODE>) language pack.
```

Only commit if the user asked. Website repo commits are separate.

## Out of scope

- Full professional translation (content work; this skill scaffolds structure).
- Adding a CAD host runtime (`/add-runtime-target`).
- Copyright-only or version-only bumps (use those skills; often combined at release).
- Pushing installers into `opendcl.github.io` (releases stay on GitHub Releases).

## Quick verification checklist

- [ ] `Common/<LANG>/`, Runtime + Studio Localized trees present without build junk
- [ ] Unique project GUIDs; names use new language code
- [ ] `OpenDCL.sln` (+ compile filter if used) includes new projects
- [ ] Help Content builds to `OpenDCL.chm`
- [ ] `License.rtf` / `License.txt` / `License.htm` present for Studio
- [ ] `$RuntimeLangs` + `$StudioLangMeta` updated in `scripts/build-wix.ps1`
- [ ] Studio MSI builds and installs for the new language
- [ ] Download/help website follow-ups noted or done
- [ ] Skill updated if new lessons were learned
