---
name: add-runtime-target
description: >
  Add a new CAD host runtime target (AutoCAD/ObjectARX, BricsCAD/BRX, GstarCAD/GRX,
  or ZWCAD/ZRX) by cloning the previous host VI folder, adding a CMake matrix row,
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

Reference commits for "clean" adds (feature-focused, limited noise):

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

**Do not guess** ARX registry major (`R25.0`, `R26.0`, ...) or whether a new AutoCAD year shares the previous ObjectARX major. Prefer copying the previous year's mapping and adjusting from SDK docs / user guidance. History shows multiple AutoCAD years can share one ARX major with a minor discriminator in `RxInstall.cpp`.

## Workflow

### 1. Clone the previous host VI tree

Copy the latest same-platform host folder (VI props + headers only). There is
**no** checked-in `.vcxproj` / `OpenDCL.sln`; CMake generates those.

```text
Runtime/ARX/ARX.25.x64/  ->  Runtime/ARX/ARX.26.x64/
Runtime/BRX/BRX.25.x64/  ->  Runtime/BRX/BRX.26.x64/
Runtime/GRX/GRX.2026.x64/ -> Runtime/GRX/GRX.2027.x64/
Runtime/ZRX/ZRX.2023.x64/ -> Runtime/ZRX/ZRX.2025.x64/   # when years skip, clone nearest
```

Include at least:

- `VI/*.props`
- `VI/ARXVI.h`

**Do not copy** `Debug/`, `Release/`, `.tlog`, `.obj`, `.pdb`, `.vcxproj`,
`.vcxproj.filters`, `.vcxproj.user`, or other build outputs.

### 2. Retarget host identity

Inside the new folder, update:

1. **`VI/<id>.props` filename** to the new host id (`ARX.26.x64.props`, ...).
2. **`_PropertySheetDisplayName`** and any residual strings of the **old** id.
3. **Output name** used in the CMake matrix `OUTPUT_NAME`:
   - ARX/BRX: `OpenDCL.x64.<Major>` (e.g. `OpenDCL.x64.26`)
   - GRX/ZRX: `OpenDCL.x64.<Year>` (e.g. `OpenDCL.x64.2027`)

CMake `OUTPUT_NAME` must match `RxInstall`'s `GetTargetModulePath()` naming
(`OpenDCL.x64.<ver>.<ext>`).

### 3. Update `VI` property sheet and `ARXVI.h`

#### `VI/<Project>.props`

Clone-adjust fields (names vary by platform):

- `_PropertySheetDisplayName`
- `TargetExt` (`.arx` / `.brx` / `.grx` / `.zrx`)
- SDK include/lib macros (`$(ARX2027)`, `$(BRX26)`, `$(GRX2027)`, ...)
- Preprocessor defines:
  - ARX: `_ACADTARGET`, `_ARXTARGET`
  - BRX: `_ACADTARGET` (compatibility), `_BRXTARGET`
  - GRX: `_ACADTARGET` (compatibility), `_GRXTARGET`
  - ZRX: analogous platform macros
- Toolset / language / warning options if the new SDK requires them
  (example: ARX 26 added `stdcpp20` and extra `/wd` flags in `08e3efa8`)
- Link dependencies if the new SDK changes required libs

#### `VI/ARXVI.h`

- Set `#define _ACADTARGET ...` (and platform-specific command-entry macros if the
  clone already has host-specific macros - GRX/BRX differ from ARX).
- Prefer cloning the **same platform's** previous `ARXVI.h`, not an ARX header into GRX/BRX.

### 4. Add a CMake matrix row

Edit **`cmake/OpenDCLRuntimeMatrix.cmake`** (or clone-adjust then run
`pwsh scripts/generate-runtime-matrix.ps1` and restore **TOOLSET** /
**OUTPUT_NAME** / **CHARACTER_SET** from the previous same-platform row).

Required fields: `ID`, `FAMILY`, `VERSION`, `ARCH`, `EXT`, `OUTPUT_NAME`,
`TOOLSET`, `SDK_ENV`, `SDK_INC`, `SDK_LIB`, `VI_DIR`, `DEFINES`. Copy
`LIBS` / `WARNING_DISABLES` / `CXX_STANDARD` from the clone source when they
apply. Do **not** default `TOOLSET` to `v142` — take it from the previous host
or the new SDK's documented toolset. **`TOOLSET` lives in the CMake matrix, not
`VI/*.props`.**

**v145+:** `ARX.26.x64` is pinned **`v145`**. Any new row with `TOOLSET` **v145**
or newer requires the **Visual Studio 2026 v145 toolset**. Prefer **`vs2026-full`**
/ **`vs2026-dev`** for product and ARX.26 iteration; **`vs2022-full`** also works
when v145 is installed. **`BRX.27.x64` stays `v143`**.

CMake presets pick the row via SDK env detect; no solution file to edit.
Reconfigure with `--fresh` after adding the row.

### 5. Register the target in `Runtime/RxInstall/RxInstall.cpp`

This is required for install/uninstall demand-loading. Typical touch points:

1. **`MajorVersion` enum** - add `kARX...` / `kBRX...` / `kGRX...` / `kZRX...` as needed.
2. **Combined target enum** - e.g. `kAutoCAD2027x64`, `kBricscad26x64`,
   `kGstarCAD2027x64`, `kZWCAD2026x64` using architecture | platform | major | minor.
3. **`InstallAllTargets`** - `EnumerateRegTargets(TargetModule(...))` for the new id.
4. **`UninstallAllTargets`** - `RemoveAllRegTargets` with the host registry path:
   - AutoCAD: `Autodesk\\AutoCAD\\R<major>.<minor>` (verify against real install keys)
   - BricsCAD: `Bricsys\\Bricscad\\V<ver>x64` (follow existing pattern)
   - ZWCAD: `ZWSOFT\\ZWCAD\\<year>`
   - GstarCAD: `Gstarsoft\\GstarCAD\\R...` (note historical casing/path quirks)
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

1. Add the new module's **catalog path** to `$RuntimeModuleCatalogRels` in
   `scripts/build-wix.ps1` (use `{Config}` for the harvest folder), e.g.:

   ```text
   Runtime\ARX\ARX.27.x64\{Config}\OpenDCL.x64.27.arx
   Runtime\BRX\BRX.27.x64\{Config}\OpenDCL.x64.27.brx
   Runtime\GRX\GRX.2028.x64\{Config}\OpenDCL.x64.2028.grx
   Runtime\ZRX\ZRX.2026.x64\{Config}\OpenDCL.x64.2026.zrx
   ```

2. Place it with the correct platform group (ARX / BRX / GRX / ZRX) for readability.
3. **Do not** hand-edit `wix/out/gen/OpenDCL.Runtime.Files.wxs` - that fragment is
   regenerated at package build time from the selected catalog (and is gitignored).
4. Component GUIDs are **stable MD5 seeds** of the module file name; no manual GUID
   assignment is required when adding a new module.
5. Align the shipped file name with CMake `OUTPUT_NAME` + extension
   (`OpenDCL.x64.<ver>.arx|brx|grx|zrx`).
6. Packaging modes (same script):
   - **Full product** (default): all catalog modules + all languages; historical MSM GUID.
   - **Custom**: `-Runtimes` / `-ModuleSet Selected|Available` / `-Languages` ->
     `OpenDCL.Runtime.custom.msm` with seed GUIDs (dev/subset installers).

**Do not** add system DLL "detected dependencies." WiX packages only intentional
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
- `Runtime/RxInstall/RxInstall.rc` (product version - must stay in sync)
- Localized Runtime/Studio `.rc` version resources
- WiX defaults in `scripts/build-wix.ps1`
  (`ProductVersion` MSI 3-part, `ModuleVersion` 4-part)

### 8. Build and smoke-check

1. CMake configure (matching preset) and build the new `Runtime_<id>` target (Release).
   Prefer **`vs2026-*`** for **v145+** rows (including **ARX.26**).
2. Confirm output name/extension under `build/<preset>/out/`.
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
- Clone/rename pitfalls, CMake matrix TOOLSET, WiX `$RuntimeModules` path details
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
- Several AutoCAD years may share one `kARX20xx` major with different minor nibble values in the target enum - copy the latest mapping carefully.
- **`ARX.26.x64` `TOOLSET` is `v145`.** Full product prefers **`vs2026-full`**. Future ARX rows that stay on v145+ have the same VS2026 requirement.

### BricsCAD (BRX)

- Folder `BRX.<ver>.x64`, extension `.brx`, env-style `$(BRX26)` / `$(BRX27)`.
- **`BRX.27.x64` stays `TOOLSET` `v143`** on vs2026 (do not bump it to v145 with ARX.26).
- `_ACADTARGET` in props is often a compatibility value (commonly `24` in recent BRX props) while `_BRXTARGET` carries the BricsCAD major.
- RxInstall: `kBRX<ver>`, `kBricscad<ver>x64`, install enumerate, uninstall `Bricsys\Bricscad\V<ver>x64`.
- WiX: `Runtime\BRX\BRX.<ver>.x64\Release\OpenDCL.x64.<ver>.brx` in `$RuntimeModules`.
- CMake: matrix row + optional nested Win32 RxInstall; packaging resolves
  `out\` layout via `Resolve-ProductFile` in `build-wix.ps1`.
- CMake FullDebug for **all** runtime families uses **`/MDd`** + host debug libdirs
  (see `OpenDCLRuntimeTargets.cmake` / `CMAKE.md`). Do not scan proprietary debug trees.

### GstarCAD (GRX)

- Folder uses **calendar year** (`GRX.2027.x64`).
- Output `OpenDCL.x64.2027.grx`.
- `ARXVI.h` includes GRX-specific command macros and entry-point fixes - clone GRX, not ARX.
- Installer registration keys under `Gstarsoft\\GstarCAD\\...` have irregular historical forms; copy nearest year and verify.

### ZWCAD (ZRX)

- Calendar year folders; extension `.zrx`.
- Some years may be x86+x64; recent adds trend x64.
- Watch for years that reuse prior binaries (comments in `RxInstall` mention cross-loading quirks historically).

## Out of scope

- Adding a new **UI language** (see CHT addition in `e1bbd787`) - different skill later.
- Copyright year rollover - `/update-copyright-year`.
- Porting application code to fix host API breaks beyond project scaffolding
  (do fix compile blockers for the new target, but treat large refactors as explicit work).

## Skeleton / follow-ups

- [ ] Per-platform deep checklist with exact registry key examples for the next N years
- [ ] Safer automated clone script (copy VI tree, patch props, add matrix row)
- [ ] Document required machine environment variables for each SDK
- [ ] Note CMake matrix TOOLSET / OUTPUT_NAME when generate-runtime-matrix.ps1 would drop them

## Quick verification checklist

- [ ] New host folder with `VI/` only (no build artifacts, no `.vcxproj`)
- [ ] CMake matrix row: `ID`, `OUTPUT_NAME` matches RxInstall filename scheme, `TOOLSET` from previous/SDK (`v145+` => VS2026)
- [ ] If `TOOLSET` is v145+, smoke on **`vs2026-dev` / `vs2026-full`**
- [ ] Props point at the new SDK macros and defines
- [ ] `RxInstall.cpp` enum + install + uninstall paths
- [ ] New module path added to `$RuntimeModuleCatalogRels` in `scripts/build-wix.ps1`
- [ ] Version resources + WiX version defaults bumped (including `RxInstall.rc`)
- [ ] CMake Release build produces the expected module name
- [ ] Skill file updated if new lessons or fixes were discovered
