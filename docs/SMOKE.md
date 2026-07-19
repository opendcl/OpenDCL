# OpenDCL installer smoke requirements

Use this checklist for local dry-run / release candidates. Prefer a **clean build
tree**, then package only after **verified** outputs. Do **not** start WiX while
a compile is still running.

## Preconditions

| Check | Requirement |
| --- | --- |
| Product tree | CMake preset **`vs2022-full`** (dual-arch ship) |
| Config | **Release** for ship smoke |
| SDKs | Host SDKs present for every catalog runtime you expect in a **Full** package |
| WiX | Toolset **v3.14** (`candle` / `light`) |
| Signing (optional) | YubiKey + PIN; never store PIN in scripts |
| No races | Close VS builds / stray MSBuild on the same tree before packaging |

## Build → verify → package order

```text
1. Clean (optional): remove build/, wix/out/, dist/<ver>/ as needed
2. Configure:  cmake --preset vs2022-full --fresh
3. Build:      cmake --build --preset vs2022-full-release
               (or: scripts/build-cmake-full.ps1 -Fresh)
4. Verify:     scripts/verify-build-outputs.ps1 -OpenDclRoot build\vs2022-full
               → fail if Full catalog / Studio / Res / RxInstall incomplete
5. Package:    scripts/make-release.ps1 -OpenDclRoot build\vs2022-full -ProductVersion A.B.C.D [-Sign]
6. Smoke:      Runtime MSI alone, then Studio ENU alone (or vice versa on clean machine)
7. Compare:    new dist packages vs the previous release package set (see below)
```

**Rule:** Packaging must not run until step 4 exits 0. `make-release.ps1` verifies
before WiX by default (`-SkipVerify` only when deliberately reusing a known-good tree).

### Path resolve

`build-wix.ps1` `Resolve-ProductFile` only uses:

1. `-OpenDclRoot\<rel>`
2. `-OpenDclRoot\out\<rel>` (CMake)
3. `-OpenDclRoot\win32\out\<rel>`
4. Packaging **repo** (source assets: icons, Content, samples)

Missing modules fail **Full** or are skipped under **Available**. Packaging never
fills gaps from outside the product/repo tree.

## Smoke A — Runtime MSI only

Goal: install Runtime without Studio; confirm modules and RxInstall registration.

1. List products: `build-lab\scripts\uninstall-opendcl.ps1` (list only).
2. Uninstall existing OpenDCL if needed (user consent / elevation).
3. Install `dist\<ver>\OpenDCL.Runtime.<ver>.msi` (or unversioned `wix\out\Release\…`).
4. Checks:
   - [ ] ARP: **OpenDCL Runtime** (or Runtime custom for Available packages)
   - [ ] `Common Files\OpenDCL\` module count matches package inventory
   - [ ] Module sizes match verified build **`out\`** (same files that were packaged)
   - [ ] PE: Win32 modules x86; `OpenDCL.x64.*` modules x64
   - [ ] `Runtime.Res` languages present as packaged
   - [ ] Optional: demand-load in one CAD host (`OPENDCL`)
5. Optional uninstall clean.

## Smoke B — Studio language MSI only

Goal: Studio ENU (or one lang) embeds Runtime MSM; Studio.Res PE matches Studio.

1. Clean slate or uninstall Runtime first if testing Studio-only path.
2. Install `OpenDCL.Studio.ENU.<ver>.msi` only (do **not** also install Runtime MSI unless testing both products together).
3. Checks:
   - [ ] Studio under **Program Files** (x64 Studio) or **Program Files (x86)** (x86 Studio)
   - [ ] `Studio.Res.dll` next to layout under `<LANG>\`; **PE machine == Studio.exe**
   - [ ] Help: `OpenDCL.chm` under `<LANG>\`; Help shortcut works
   - [ ] License / Start Menu icons
   - [ ] Runtime modules also under Common Files (MSM)
   - [ ] Launch Studio: no MFC assert / wrong-arch Res load failure
4. Optional: second language MSI upgrade/side-by-side policy as needed.

## Smoke C — Full local make-release

```powershell
# From the packaging repo root
$ver  = "10.1.1.1"
$prev = "10.0.0.0"   # last shipped / last accepted dry-run version

# Clean empty tree (example)
Remove-Item "build\vs2022-full","wix\out","dist\$ver" -Recurse -Force -ErrorAction SilentlyContinue

.\scripts\build-cmake-full.ps1 -Fresh -Preset vs2022-full
.\scripts\verify-build-outputs.ps1 -OpenDclRoot build\vs2022-full -ModuleSet Full
.\scripts\make-release.ps1 `
  -OpenDclRoot (Resolve-Path build\vs2022-full) `
  -ProductVersion $ver `
  -ModuleSet Full `
  -Sign   # PIN prompt; omit for unsigned dry-run
```

Then run **Smoke A** and **Smoke B** on `dist\$ver\`.

## Comparison (after successful make-release)

Once a full verified (and preferably signed) package set exists, **compare to the
previous release packages** — not to legacy source trees. Typical baselines:

- Prior `dist\<previous-version>\` from this repo, or  
- Unpacked / archived assets from the previous GitHub Release  

```powershell
.\scripts\compare-release-packages.ps1 `
  -BaselinePackageDir dist\$prev `
  -NewPackageDir      dist\$ver
```

(Alias: `compare-cmake-classic.ps1` with the same parameters.)

Report under `wix\out\compare\<timestamp>\`. Review presence gaps, large size
deltas, and PE arch mismatches. Intentional product changes should be explained
in release notes; unexpected drops or arch flips are blockers.

## Pass / fail summary

| Gate | Pass criteria |
| --- | --- |
| Build | cmake build exit 0 |
| Verify | `verify-build-outputs.ps1` exit 0 for intended ModuleSet |
| Package | make-release / build-wix exit 0; ship names for Full (`OpenDCL.Runtime.msi` not only `.custom`) |
| Runtime smoke | modules match verified `out\` |
| Studio smoke | PE match Res; CHM; launches |
| Sign (if used) | `signtool verify /pa` on MSI/MSM |
| Compare | report vs **previous release packages**; known gaps documented |

## Related

- [CMAKE.md](../CMAKE.md) — presets, nest, packaging modes  
- [AGENTS.md](../AGENTS.md) — agent conventions  
- [wix/README.md](../wix/README.md) — WiX deliverables  
- build-lab dry-run skill — private CI packaging without public ship  
