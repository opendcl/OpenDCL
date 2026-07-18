---
name: add-runtime-target
description: >
  Add a new CAD host runtime target (AutoCAD/ObjectARX, BricsCAD/BRX, GstarCAD/GRX,
  or ZWCAD/ZRX) by cloning the previous version project, wiring the solution,
  RxInstall registration, WiX module inventory, and product version. Use when
  adding support for a new host year/version, or when the user runs
  /add-runtime-target. Triggers: "add runtime", "new runtime target",
  "support AutoCAD", "support BricsCAD", "support GstarCAD", "support ZWCAD",
  "ARX.xx", "BRX.xx", "GRX.20xx", "ZRX.20xx".
---

# Add Runtime Target

Scaffold and wire a new host-specific OpenDCL Runtime module. Based on repeated
historical patterns:

| Host | Folder family | Example project | Output name pattern | Extension |
|------|---------------|-----------------|---------------------|-----------|
| AutoCAD | `Runtime/ARX/` | `ARX.26.x64` | `OpenDCL.x64.26` | `.arx` |
| BricsCAD | `Runtime/BRX/` | `BRX.26.x64` | `OpenDCL.x64.26` | `.brx` |
| GstarCAD | `Runtime/GRX/` | `GRX.2027.x64` | `OpenDCL.x64.2027` | `.grx` |
| ZWCAD | `Runtime/ZRX/` | `ZRX.2025.x64` | `OpenDCL.x64.2025` | `.zrx` |

Reference commits for “clean” adds (feature-focused, limited noise):

- AutoCAD: `b4593bdd` (2025), `e1bbd787` (2026), `08e3efa8` (2027)
- BricsCAD + GstarCAD: `2b8f67f6` (BRX 26 / GRX 2026)
- GstarCAD: `711ac77b` (GRX 2027), `5a4d3bb0` (GRX 2025)
- ZWCAD: `31506608` (ZRX 2025)
- Missing module follow-up: `f4b93724` (added GRX 2024 after experimental pass)

## Inputs to collect

Before editing, confirm with the user:

1. **Platform**: AutoCAD (ARX), BricsCAD (BRX), GstarCAD (GRX), ZWCAD (ZRX)
2. **Host product year / version** (e.g. AutoCAD 2028, BricsCAD V27, GstarCAD 2028)
3. **Architecture**: modern targets are **x64 only**; older hosts may still have x86 trees
4. **SDK location / env var** expected by props (e.g. `$(ARX2027)`, `$(BRX26)`, `$(GRX2027)`, `$(ZRX2025)`)
5. **Clone source**: which existing project to copy (default: latest same-platform target)
6. **Product version** for the release commit (usually bump patch, reset build to `1`)

Also determine host-specific version numbering used **inside OpenDCL**:

| Platform | Project folder number | Meaning |
|----------|----------------------|---------|
| ARX | ObjectARX major (e.g. `26` for Acad 2027 in recent trees) | `_ACADTARGET` / `_ARXTARGET` |
| BRX | BricsCAD major (e.g. `26` for V26) | `_BRXTARGET` |
| GRX | Calendar year (`2027`) | `_GRXTARGET` (and often a fixed compatibility `_ACADTARGET`) |
| ZRX | Calendar year (`2025`) | `_ZRXTARGET` / related |

**Do not guess** ARX registry major (`R25.0`, `R26.0`, …) or whether a new AutoCAD year shares the previous ObjectARX major. Prefer copying the previous year’s mapping and adjusting from SDK docs / user guidance. History shows multiple AutoCAD years can share one ARX major with a minor discriminator in `RxInstall.cpp`.

## Workflow

### 1. Clone the previous target project tree

Copy the latest same-platform project directory, e.g.:

```text
Runtime/ARX/ARX.25.x64/  →  Runtime/ARX/ARX.26.x64/
Runtime/BRX/BRX.25.x64/  →  Runtime/BRX/BRX.26.x64/
Runtime/GRX/GRX.2026.x64/ → Runtime/GRX/GRX.2027.x64/
Runtime/ZRX/ZRX.2023.x64/ → Runtime/ZRX/ZRX.2025.x64/   # when years skip, clone nearest
```

Include at least:

- `*.vcxproj`
- `*.vcxproj.filters`
- optional `*.vcxproj.user` (machine-local; often omit from intentional commits)
- `VI/*.props`
- `VI/ARXVI.h`

**Do not copy** `Debug/`, `Release/`, `.tlog`, `.obj`, `.pdb`, or other build outputs.

### 2. Retarget project identity

Inside the new folder, update:

1. **Filenames** to the new project name (`ARX.26.x64.vcxproj`, …).
2. **`ProjectGuid`** — generate a new GUID (never reuse the clone’s GUID).
3. **`RootNamespace` / output name**:
   - ARX/BRX: `OpenDCL.x64.<Major>` (e.g. `OpenDCL.x64.26`)
   - GRX/ZRX: `OpenDCL.x64.<Year>` (e.g. `OpenDCL.x64.2027`)
4. **Property sheet import** paths (`VI\ARX.26.x64.props`, etc.).
5. **`.filters`** file name references if present.
6. Any residual strings of the **old** project name inside the vcxproj/filters.

`TargetName` should remain `$(RootNameSpace)` so the built module matches
`RxInstall`’s `GetTargetModulePath()` naming (`OpenDCL.x64.<ver>.<ext>`).

### 3. Update `VI` property sheet and `ARXVI.h`

#### `VI/<Project>.props`

Clone-adjust fields (names vary by platform):

- `_PropertySheetDisplayName`
- `TargetExt` (`.arx` / `.brx` / `.grx` / `.zrx`)
- SDK include/lib macros (`$(ARX2027)`, `$(BRX26)`, `$(GRX2027)`, …)
- Preprocessor defines:
  - ARX: `_ACADTARGET`, `_ARXTARGET`
  - BRX: `_ACADTARGET` (compatibility), `_BRXTARGET`
  - GRX: `_ACADTARGET` (compatibility), `_GRXTARGET`
  - ZRX: analogous platform macros
- Toolset / language / warning options if the new SDK requires them
  (example: ARX 26 added `stdcpp20` and extra `/wd` flags in `08e3efa8`)
- Link dependencies if the new SDK changes required libs

#### `VI/ARXVI.h`

- Set `#define _ACADTARGET …` (and platform-specific command-entry macros if the
  clone already has host-specific macros — GRX/BRX differ from ARX).
- Prefer cloning the **same platform’s** previous `ARXVI.h`, not an ARX header into GRX/BRX.

### 4. Add project to the solution

#### `OpenDCL.sln`

1. Add a `Project(...)` entry with the new GUID and relative path.
2. Add full configuration lines for existing solution configs
   (`Debug|Mixed Platforms`, `Debug|Win32`, `Debug|x64`, `FullDebug|…`, `Release|…`)
   mirroring the previous same-platform x64 project.
3. Nest under the correct solution folder GUID (ARX / BRX / GRX / ZRX folder).

#### `OpenDCL.Compile.slnf`

Add the new `vcxproj` path to the compile filter list so batch builds include it.

### 5. Register the target in `Runtime/RxInstall/RxInstall.cpp`

This is required for install/uninstall demand-loading. Typical touch points:

1. **`MajorVersion` enum** — add `kARX…` / `kBRX…` / `kGRX…` / `kZRX…` as needed.
2. **Combined target enum** — e.g. `kAutoCAD2027x64`, `kBricscad26x64`,
   `kGstarCAD2027x64`, `kZWCAD2026x64` using architecture | platform | major | minor.
3. **`InstallAllTargets`** — `EnumerateRegTargets(TargetModule(...))` for the new id.
4. **`UninstallAllTargets`** — `RemoveAllRegTargets` with the host registry path:
   - AutoCAD: `Autodesk\\AutoCAD\\R<major>.<minor>` (verify against real install keys)
   - BricsCAD: `Bricsys\\Bricscad\\V<ver>x64` (follow existing pattern)
   - ZWCAD: `ZWSOFT\\ZWCAD\\<year>`
   - GstarCAD: `Gstarsoft\\GstarCAD\\R…` (note historical casing/path quirks)
5. Any platform-specific path logic in `GetRootKey` / `GetTargetModulePath` if the
   new host breaks previous assumptions.

Module filename construction (do not invent alternate names):

- AutoCAD: `OpenDCL.x64.<ARX major>.arx`
- BricsCAD: `OpenDCL.x64.<BRX major>.brx`
- ZWCAD: `OpenDCL.x64.<year>.zrx`
- GstarCAD: `OpenDCL.x64.<year>.grx`

### 6. WiX packaging inventory

Installer packaging is **WiX Toolset v3** in this repo (`scripts/build-wix.ps1`,
`wix/`). The Runtime MSM file list comes from an explicit inventory in
`scripts/build-wix.ps1`:

1. Add the new module’s **Release output path** to the `$RuntimeModules` array
   (same relative shape as existing entries), e.g.:

   ```text
   Runtime\ARX\ARX.27.x64\Release\OpenDCL.x64.27.arx
   Runtime\BRX\BRX.27.x64\Release\OpenDCL.x64.27.brx
   Runtime\GRX\GRX.2028.x64\Release\OpenDCL.x64.2028.grx
   Runtime\ZRX\ZRX.2026.x64\Release\OpenDCL.x64.2026.zrx
   ```

2. Place it with the correct platform group (ARX / BRX / GRX / ZRX) for readability.
3. **Do not** hand-edit `wix/out/gen/OpenDCL.Runtime.Files.wxs` — that fragment is
   regenerated at package build time from `$RuntimeModules` (and is gitignored).
4. Component GUIDs are **stable MD5 seeds** of the module file name; no manual GUID
   assignment is required when adding a new module.
5. Align the shipped file name with `RootNamespace` + extension
   (`OpenDCL.x64.<ver>.arx|brx|grx|zrx`).

**Do not** add system DLL “detected dependencies.” WiX packages only intentional
product files (modules, `Runtime.Res`, licenses, Studio app/help/samples).

After a successful OpenDCL Release compile, package with:

```powershell
.\scripts\build-wix.ps1 `
  -ProductVersion "<MSI 3-part>" -ModuleVersion "<A.B.C.D>"
```

See `wix/README.md` for prerequisites and outputs
(`OpenDCL.Runtime.msm`, `OpenDCL.Runtime.msi`, `OpenDCL.Studio.<LANG>.msi` under
`wix/out/Release/`).

### 7. Product version bump

New runtime support almost always ships as a versioned release. Run the
`/bump-version` checklist so all of these match:

- `Runtime/ARX.rc`, `Studio/Studio.rc`
- `Runtime/RxInstall/RxInstall.rc` (product version — must stay in sync)
- Localized Runtime/Studio `.rc` version resources
- WiX defaults in `scripts/build-wix.ps1`
  (`ProductVersion` MSI 3-part, `ModuleVersion` 4-part)

### 8. Build and smoke-check

1. Build the new project (Release|x64) with the correct SDK env vars set.
2. Confirm output name/extension under the project `Release` folder.
3. Build `RxInstall` if registration code changed.
4. Note any new compiler errors that require shared-code fixes (API removals,
  stricter C++, host header changes). Large shared-code modernizations sometimes
  land in the same commit as a new ARX target (`08e3efa8`); keep those deliberate
  and separate in the summary when possible.

### 9. Capture lessons into this skill

Before finishing, update **this** `SKILL.md` with anything newly learned while
adding the runtime target, for example:

- Host-specific SDK paths, macros, toolset, or `ARXVI.h` differences
- Correct registry keys / major-version mapping discovered for the new host
- Clone/rename pitfalls, solution-folder GUIDs, WiX `$RuntimeModules` path details
- Compile fixes that future targets will likely need
- Problems encountered and the solution that resolved them

Write durable platform notes and checklist items; remove or reword outdated
guidance instead of leaving contradictions. If nothing new was learned, skip
edits to the skill file.

### 10. Commit message style

```text
OpenDCL A.B.C.D
- Added initial support for <Host> <Version>.
```

or

```text
OpenDCL A.B.C.D
- Initial support for Acad 2027.
```

Only commit if the user asked.

## Platform-specific notes

### AutoCAD (ARX)

- Newer hosts are x64-only folders (`ARX.<n>.x64`).
- ObjectARX major does not always equal marketing year (e.g. ARX `26` used for Acad 2027 in tree).
- Several AutoCAD years may share one `kARX20xx` major with different minor nibble values in the target enum — copy the latest mapping carefully.

### BricsCAD (BRX)

- Folder `BRX.<ver>.x64`, extension `.brx`, env-style `$(BRX26)`.
- `_ACADTARGET` in props is often a compatibility value (commonly `24` in recent BRX props) while `_BRXTARGET` carries the BricsCAD major.

### GstarCAD (GRX)

- Folder uses **calendar year** (`GRX.2027.x64`).
- Output `OpenDCL.x64.2027.grx`.
- `ARXVI.h` includes GRX-specific command macros and entry-point fixes — clone GRX, not ARX.
- Installer registration keys under `Gstarsoft\\GstarCAD\\…` have irregular historical forms; copy nearest year and verify.

### ZWCAD (ZRX)

- Calendar year folders; extension `.zrx`.
- Some years may be x86+x64; recent adds trend x64.
- Watch for years that reuse prior binaries (comments in `RxInstall` mention cross-loading quirks historically).

## Out of scope

- Adding a new **UI language** (see CHT addition in `e1bbd787`) — different skill later.
- Copyright year rollover — `/update-copyright-year`.
- Porting application code to fix host API breaks beyond project scaffolding
  (do fix compile blockers for the new target, but treat large refactors as explicit work).

## Skeleton / follow-ups

- [ ] Per-platform deep checklist with exact registry key examples for the next N years
- [ ] Safer automated clone script (copy tree, re-GUID, rename, patch props)
- [ ] Document required machine environment variables for each SDK
- [ ] Note when `OpenDCL.Compile.slnf` must stay ordered/complete for CI-style builds

## Quick verification checklist

- [ ] New project folder without build artifacts
- [ ] Unique `ProjectGuid`
- [ ] `RootNamespace` matches RxInstall filename scheme
- [ ] Props point at the new SDK macros and defines
- [ ] `OpenDCL.sln` project + configs + solution folder
- [ ] `OpenDCL.Compile.slnf` entry (local/generated filter if used)
- [ ] `RxInstall.cpp` enum + install + uninstall paths
- [ ] New module path added to `$RuntimeModules` in `scripts/build-wix.ps1`
- [ ] Version resources + WiX version defaults bumped (including `RxInstall.rc`)
- [ ] Release build produces the expected module name
- [ ] Skill file updated if new lessons or fixes were discovered
