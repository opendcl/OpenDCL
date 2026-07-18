---
name: bump-version
description: >
  Bump the OpenDCL product version across Studio, Runtime, localized resources,
  and WiX packaging defaults. Use for minor or major version updates, release
  versioning, FILEVERSION/PRODUCTVERSION changes, or when the user runs
  /bump-version. Triggers: "bump version", "new version", "minor version",
  "major version", "release version", "update version numbers".
---

# Bump OpenDCL Version

Mechanical checklist for raising the product version (`major.minor.patch.build`)
across every place OpenDCL embeds it. Packaging uses WiX in this repository
(`scripts/build-wix.ps1`, `wix/`).

This skill only updates **version identity**. Do **not** use it for copyright
years (`/update-copyright-year`) or for scaffolding a new CAD host
(`/add-runtime-target`) — those often *include* a version bump as a final step.

## Version scheme

OpenDCL uses a four-part version: `Major.Minor.Patch.Build` (example: `10.1.1.1`).

| Component | Typical meaning in this repo |
|-----------|------------------------------|
| Major / Minor | Larger product lines (`9.2` → `9.3`, `9.x` → `10.1`) when shipping a significant milestone |
| Patch (3rd) | Feature releases such as a new host runtime or notable feature set; often incremented and Build reset to `1` |
| Build (4th) | Hotfix / follow-up within the same patch line (`9.2.3.1` → `9.2.3.2` → `9.2.3.3`) |

**Ask the user** for the target version if not specified. Confirm whether this is:

- **Major/minor** (`x.y.0.1` style milestone), or
- **Patch** (feature drop), or
- **Build** (small follow-up).

### Encoding forms

Keep all forms in sync for the same logical version `A.B.C.D`:

| Form | Example for `10.1.1.1` | Where used |
|------|------------------------|------------|
| Comma RC form | `10, 1, 1, 1` | `FILEVERSION` / `PRODUCTVERSION` and string `FileVersion` / `ProductVersion` in `.rc` files |
| Dot form (4-part) | `10.1.1.1` | Commit messages; WiX **ModuleVersion** (merge module) |
| WiX MSI ProductVersion (3-part) | `10.1.101` | MSI `Product/@Version` / `build-wix.ps1 -ProductVersion` |

**WiX MSI ProductVersion formula** (MSI allows at most three numeric parts for upgrade comparison):

```text
"{Major}.{Minor}.{Patch * 100 + Build}"
# 10.1.1.1  →  10.1.101
# 9.3.3.1   →  9.3.301
# 9.2.1.5   →  9.2.105
```

**ModuleVersion** stays the full four-part file version (`10.1.1.1`) for the Runtime **MSM**.

## Packaging layout (WiX)

```text
scripts/build-wix.ps1                 # packaging entry point
wix/README.md                         # packaging docs
wix/Runtime/OpenDCL.Runtime.wxs       # MSM definition
wix/Runtime/OpenDCL.Runtime.MSI.wxs   # thin Runtime MSI
wix/Studio/OpenDCL.Studio.wxs         # Studio language MSI template
wix/out/gen/                          # generated fragments (build-time, gitignored)
wix/out/Release/                      # msm + msi outputs (gitignored)
```

### Version-related packaging rules

| Concern | What to do |
|---------|------------|
| MSI / MSM version numbers | Update defaults in `scripts/build-wix.ps1` (`ProductVersion`, `ModuleVersion`) |
| Product codes | Leave alone — Runtime/Studio `Product Id="*"` regenerates each build |
| Upgrade codes | **Leave stable** (Runtime MSI + per-language Studio codes in `$StudioLangMeta`) |
| MSM modularization GUID | Leave `Package/@Id` (`0C4E4759-…`) unchanged |
| Component GUIDs | Stable MD5 seeds of logical path — do not churn |
| Module file inventory | Only when adding/removing host modules (`$RuntimeModules`) |

## Workflow

### 1. Resolve current and target versions

1. Read current version from `Runtime/ARX.rc` (`FILEVERSION` / `PRODUCTVERSION`).
2. Cross-check `Studio/Studio.rc`, `Runtime/RxInstall/RxInstall.rc`, and one localized pair.
3. Confirm target `A.B.C.D` with the user if ambiguous.
4. Compute:
   - Comma RC form
   - WiX MSI ProductVersion `A.B.(C*100+D)`
   - ModuleVersion `A.B.C.D`

### 2. Update native version resources (`.rc`)

Update **all four** of:

- `FILEVERSION A, B, C, D`
- `PRODUCTVERSION A, B, C, D`
- `VALUE "FileVersion", "A, B, C, D"`
- `VALUE "ProductVersion", "A, B, C, D"`

**Files (always):**

- `Runtime/ARX.rc`
- `Studio/Studio.rc`
- `Runtime/RxInstall/RxInstall.rc` — **must stay in sync with the product version**
- Every `Runtime/Localized/*/Runtime.Res/Runtime.*.rc` (CHS, CHT, DEU, ENU, ESM, FRA, RUS)
- Every `Studio/Localized/*/Studio.Res/Studio.*.rc` (same language set)

**RxInstall notes:**

- `RxInstall.rc` is part of the product version surface area and must match product `A.B.C.D`.
- Use the same version forms as `Runtime/ARX.rc` / `Studio/Studio.rc`.

#### Encoding (critical)

Detect encoding **per file** before read/write; never assume one encoding for all `.rc` files:

| Encoding | Typical files |
|----------|----------------|
| ANSI / Windows-1252 (no BOM) | `Runtime/ARX.rc`, `Studio/Studio.rc`, `Runtime/RxInstall/RxInstall.rc`, some CHT `.rc` |
| UTF-16 LE (`FF FE`) | Most `Runtime/Localized/*/Runtime.Res/*.rc` and `Studio/Localized/*/Studio.Res/*.rc` |

After writing, re-read and confirm the new version is present and the old version is gone. Size should change when digit count changes (e.g. `9, 3, 3, 1` → `10, 1, 1, 1` adds one character per occurrence).

Preserve encoding and copyright symbol (©) exactly as found in each file; only change the year when running the copyright skill.

### 3. Update WiX packaging defaults

Edit **`scripts/build-wix.ps1`**:

1. Default parameter **`ProductVersion`** → MSI 3-part form (e.g. `"10.1.101"`).
2. Default parameter **`ModuleVersion`** → 4-part form (e.g. `"10.1.1.1"`).
3. Keep any comment that documents the encoding rule in sync if present.

Optional but recommended: align example versions in `wix/README.md` with the current release.

**Do not** change UpgradeCodes, MSM modularization GUID, or component GUID seeds when only bumping version.

**At package build time** (if defaults are already updated, plain build is enough):

```powershell
.\scripts\build-wix.ps1 `
  -ProductVersion "A.B.<C*100+D>" `
  -ModuleVersion "A.B.C.D"
```

### 4. Consistency verification

In the product tree (excluding build outputs `**/Release/**`, `**/Debug/**`, `**/.vs/**`):

- No leftover old comma form in `.rc`
- `RxInstall.rc` matches product version
- `scripts/build-wix.ps1` defaults match ProductVersion / ModuleVersion
- README examples not stale (if you edited them)

Also spot-check UTF-16 localized `.rc` files still start with BOM `FF FE`.

### 5. Capture lessons into this skill

Before finishing, update **this** `SKILL.md` with anything newly learned while performing the bump. Write durable guidance; remove outdated instructions. If nothing new was learned, skip edits.

### 6. Commit message style (historical)

```text
OpenDCL A.B.C.D
- <bullet describing why the version moved>
- <optional additional bullets>
```

Examples:

```text
OpenDCL 9.3.2.1
- Initial support for Acad 2027.
- Modernized C++ ('override' for virtual functions)
```

```text
OpenDCL 10.1.1.1
- Bump product version from 9.3.3.1 to 10.1.1.1 across Studio, Runtime, RxInstall, localized resources, and installers.
```

Only create a git commit if the user asked for one.

## Out of scope / common pitfalls

- **Not a substitute for adding a host runtime.** New ARX/BRX/GRX/ZRX projects use `/add-runtime-target` first; that skill ends by calling this checklist.
- **Do not edit binary build artifacts** under `Release/`, `Debug/`, or `.vs/`.
- **Do not invent version policy.** If major vs patch is unclear, ask.
- **Help HTML footers** carry copyright years, not the four-part product version — leave them alone here.
- **LICENSE** at repo root is GPLv2 text, not the product version resource.
- **Do not churn UpgradeCodes or MSM Package Id** when only bumping version.

## Skeleton / follow-ups

- [ ] Encoding-safe RC bulk-replace helper
- [ ] Single release helper that bumps RC + updates `build-wix.ps1` defaults together
- [ ] Document dist rename/sign steps after `build-wix.ps1` (see `wix/README.md`)

## Historical reference commits

| Commit | Version | Notes |
|--------|---------|-------|
| `b4593bdd` | 9.2.2.1 | Clean runtime-add + version resource pattern |
| `46d6b4cf` | 9.2.1.3 | Version bump combined with copyright year |
| `e1bbd787` | 9.3.0.1 | Minor milestone + large feature set |
| `08e3efa8` | 9.3.2.1 | Acad 2027 + version |
| `711ac77b` | 9.3.3.1 | GstarCAD 2027 + version + copyright |
| `5044832f` | 10.1.1.1 | Product version bump (WiX packaging era) |
