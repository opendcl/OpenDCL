# OpenDCL CMake build

CMake is the **supported multi-host and dual-arch ship path** for OpenDCL
(Runtime matrix, Studio, RxInstall, WiX harvest from `build/<preset>/out`).

Classic `OpenDCL.sln` / per-host `.vcxproj` trees remain in the repo for
historical and transitional builds, but **full product dry-runs and ship
packaging use CMake** (preset **`vs2022-full`**). Private CI:
`opendcl/build-lab` package workflow with `compile_engine=cmake`.

Details for agents: **`AGENTS.md`**. Installer smoke: **`docs/SMOKE.md`**.

## Layout

| Path | Role |
| --- | --- |
| `CMakeLists.txt` | Root project, options, subdirs |
| `CMakePresets.json` | Dev, auto, **full classic-parity** (x64 + Win32) presets |
| `scripts/build-cmake-full.ps1` | Configure/build full presets + optional WiX package |
| `scripts/verify-build-outputs.ps1` | Pre-package gate (catalog / Res / Studio / RxInstall) |
| `scripts/compare-release-packages.ps1` | Diff installers vs previous release package set |
| `cmake/OpenDCLHelpers.cmake` | Options, registry, SDK detect, selection |
| `cmake/OpenDCLRuntimeMatrix.cmake` | All runtime rows (from `VI/*.props`) |
| `cmake/OpenDCLRuntimeSources.cmake` | Shared Runtime+Common `.cpp` list |
| `cmake/OpenDCLRuntimeTargets.cmake` | `opendcl_add_runtime()` factory |
| `cmake/overrides/<ID>.cmake` | Optional per-target flag/link escape hatch |
| `Runtime/TargetSpecific/<ID>/` | Optional per-target `.cpp` injection (auto) |
| `Library/CMakeLists.txt` | ZLib + LibPNG |
| `Runtime/Localized/CMakeLists.txt` | `Runtime.Res.<lang>` DLLs |

CMake Visual Studio generators write **`build/<preset>/<preset>.sln`** (e.g.
`build/vs2022-x64-dev/vs2022-x64-dev.sln`) so multiple presets stay distinguishable
in the IDE. Override with `-DOPENDCL_SOLUTION_NAME=…`. Nested Win32 under a full
preset is `build/vs2022-full/win32/vs2022-full-win32.sln`.

## Configure

### Requirements

| Requirement | Notes |
| --- | --- |
| CMake ≥ 3.24 | Multi-config VS generator + presets |
| Visual Studio 2022 (or later) | C++ desktop + **MFC** workload |
| CAD SDKs | Env vars (`ARX2027`, `BRX26`, `GRX2027`, `ZRX2025`, …) same as classic `VI/*.props` |
| HTML Help Workshop (`hhc.exe`) | Optional; builds per-language `OpenDCL.chm` for Studio packaging / F5 Help |
| Pre-VS2015 Platform Toolsets | Needed only for the full historic matrix (`TOOLSET` in `cmake/OpenDCLRuntimeMatrix.cmake`). Documented in the main **opendcl** public README (Daffodil). Modern-only presets (`v141`+) do not need them. |

Each runtime row’s toolset is in `cmake/OpenDCLRuntimeMatrix.cmake` (`TOOLSET "vNNN"`). CMake sets `VS_PLATFORM_TOOLSET` on the module target. Older toolsets link **toolset-matched** zlib/png static libs (see `Library/CMakeLists.txt` / `opendcl_ensure_runtime_png`) so UCRT objects from the VS2022 default toolset are not mixed into `v100` (and similar) modules.

**ZLib / LibPNG target names** (Solution Explorer under classic-like folders `Library/ZLib`, `Library/LibPNG`):

```text
opendcl_{zlib|png}_{x86|x64}_{md|mt}_{toolset}
```

| Example | Meaning |
| --- | --- |
| `opendcl_zlib_x64_md_v143` | ZLib, x64, `/MD`, compiled with **v143** (typical VS2022 host default) |
| `opendcl_png_x64_mt_v143` | LibPNG, x64, `/MT` (Studio), same host toolset |
| `opendcl_png_x86_md_v100` | LibPNG, x86, `/MD`, compiled with **v100** |

**Arch, `md`/`mt`, and toolset are always present** (no omitted defaults). The toolset tag is the toolset used to **compile that static lib** (host tag for the default pair; mapped tag for older multi-toolset modules). Convenience cache vars: `OPENDCL_PNG_MD_TARGET`, `OPENDCL_PNG_MT_TARGET`, etc.

**Host-arch only:** each CMake binary dir creates Library targets for **one** architecture (`CMAKE_SIZEOF_VOID_P`). Nested Win32 must not define `*_x64_*` static libs (and vice versa), or shared `OPENDCL_OUTPUT_ROOT` can be overwritten with the wrong PE machine (LNK4272). Dual-arch ship = parent x64 products + nest x86 products; not dual-arch Library targets in one configure.

**Runtime.Res** — `OPENDCL_RES_PE` (CAD CommonFiles only):

| Value | Behavior |
| --- | --- |
| **`classic_x86`** | Always **x86** Runtime.Res (public Mixed ship / classic CommonFilesFolder parity). On x64, from full Win32 nest (or dedicated `OpenDCL_Res_Win32`); skip native x64 Runtime.Res. |
| **`host`** | Runtime.Res PE matches the configure arch. When nested Win32 modules are on with host Res, the nest may skip rebuilding Res so it does not overwrite host PE. |

**Studio.exe / Studio.Res** — `OPENDCL_STUDIO_PE` (LoadLibrary pair; not the Runtime.Res flag):

| Value | Behavior |
| --- | --- |
| **`classic_x86`** | **Win32** Studio ship parity (classic vdproj). On x64 parent, skip native Studio; nest builds `out/Studio/Win32` + `Studio.Res/Win32`. Parent still owns CHM help. |
| **`host`** | Studio PE matches configure arch (x64 Studio on x64 parent / dev). |

Default cache for both is `host`. Preset **`vs2022-full`** forces **`classic_x86`** for Runtime.Res **and** Studio. **`vs2022-x64-full`** uses **`host`** for both (optional x64 Studio path). Packaging prefers Win32 Studio then x64.

```powershell
cd <OpenDCL repo root>

# Dev default: BRX.27.x64 only (requires BRX27 env / SDK)
cmake --preset vs2022-x64-dev
cmake --build --preset vs2022-x64-dev-release
# or Debug:
cmake --build --preset vs2022-x64-dev-debug

# ARX latest
cmake --preset vs2022-x64-arx-latest
cmake --build --preset vs2022-x64-arx-latest-release

# Auto-detect every installed CAD SDK (x64)
cmake --preset vs2022-x64-auto

# Full ship: one .sln with x64 + nested Win32 (imported into Solution Explorer)
cmake --preset vs2022-full --fresh
cmake --build --preset vs2022-full-release
# Shared out/: x64+Win32 modules; vs2022-full ships Win32 Studio (classic_x86)

# One-shot configure+build (no package until verify is green):
.\scripts\build-cmake-full.ps1 -Fresh
.\scripts\verify-build-outputs.ps1 -OpenDclRoot build\vs2022-full -ModuleSet Full
# Full local make-release (dist + optional -Sign) — verifies before WiX:
.\scripts\make-release.ps1 -OpenDclRoot (Resolve-Path build\vs2022-full) `
  -ProductVersion 10.1.1.1 -ModuleSet Full -Sign
# After a full set exists, compare packages to the previous release:
.\scripts\compare-release-packages.ps1 `
  -BaselinePackageDir dist\10.0.0.0 `
  -NewPackageDir dist\10.1.1.1
```

Installer smoke checklist: **[docs/SMOKE.md](docs/SMOKE.md)**.

| Preset | Role |
| --- | --- |
| **`vs2022-full`** (Mixed) | **Public full ship:** x64 `.sln` + nested Win32 modules + **`OPENDCL_RES_PE=classic_x86`** + **`OPENDCL_STUDIO_PE=classic_x86`** (x86 Runtime.Res + **Win32 Studio** / Studio.Res via nest) |
| **`vs2022-x64-full`** | Same dual-arch module nest, but **`host`** Res + **`host`** Studio → **x64 Studio** packaging path |
| **`vs2022-win32-full`** | Standalone Win32 binary dir; host Res is x86 |
| CRT (Release) | Modules/Runtime.Res **`/MD`**; Studio **`/MT`** + `*_mt` zlib/png |
| CRT (FullDebug) | Modules **`/MDd`** (all families); non-modules FullDebug→Debug outputs |

**How dual-arch (Mixed / x64-full) works:** CMake’s VS generator cannot put `Debug|x64` and `Debug|Win32` on the **same** native target. These presets configure **x64** as the main `.sln`, then at generate time configure `build/<preset>/win32` (`-A Win32`) with the **same** `OPENDCL_OUTPUT_ROOT=…/out`, and **import** nest `.vcxproj` files into the parent solution (`include_external_msproject`, `PLATFORM Win32`). Solution Explorer uses **classic-style product folders** (both arches together): `Runtime/Rx/{ARX,BRX,GRX,ZRX}`, `Runtime/Localized Resources`, `Library/{ZLib,LibPNG}`, `Studio[/<LANG>]`, `CMake` (`OpenDCL_Win32`). Nest imports are named `w32_*`. **`OpenDCL_Win32`** is the single-flight nest build when `OPENDCL_WIN32_IN_ALL`. **RxInstall** and **classic_x86 Res** come from the full nest only (no private `rxinstall-win32` / `res-win32` when nest is on). **Studio help (CHM)** is arch-independent (nest sets `OPENDCL_BUILD_STUDIO_HELP=OFF`). Packaging `-OpenDclRoot build\vs2022-full` resolves both arches under `out\`.

**Nest hard-fail / single-flight:** With `OPENDCL_WIN32_IN_ALL`, only **`OpenDCL_Win32`** is on the default build graph and runs `cmake --build` on the nest (hard-fail). **`OpenDCL_Res_Win32`** / **`OpenDCL_RxInstall`** keep targeted nest commands for manual builds but are **not** ALL (parallel nest builds race on `.ilk`/PDB). Studio and x64 runtimes depend on `OpenDCL_Win32` so Res exists before F5/post-build. Imported `w32_*` projects are Explorer-only (parent Platform=x64 → MSB8013). Nest reconfigure is skipped when init-cache is unchanged (avoids regen loops / VS “already contains `w32_…`” on reload).

### Known limitations — nested Win32 full build (`vs2022-full`)

These are **accepted for now** (document and move on; not blocking x64/dev or packaging smoke):

| Symptom | Why | Mitigation / status |
| --- | --- | --- |
| **C1060** compiler out of heap (`afxtempl.h`, old ATL) under `OpenDCL_Win32` | Nest builds many **32-bit-era toolsets** (v100/v110/…) in one MSBuild; 32-bit `cl` heap is small | Defaults: `OPENDCL_NEST_MSBUILD_MAX_CPU_COUNT=2`, `OPENDCL_NEST_CL_MP_COUNT=1`. Try `=1` / `=1`. Still may fail on a full nest Debug/Release under heavy machine load. |
| **C1001** ICE in nest modules (PCH, PaletteDlg, etc.) | Same pressure / parallel compile instability | Same throttle; rebuild single target or stand-alone `build/.../win32` with low `/m`. |
| Cancel in VS does not stop nest `cmake --build` cleanly | Nested MSBuild is a child process of a CustomBuild step | Kill stray `MSBuild`/`cl` if needed; known CustomBuild limitation. |
| Full nest green is **not** required for day-to-day | x64 Studio and Available packages work without every old host | Prefer **`vs2022-x64-dev`** for IDE work; use **`vs2022-full`** for dual-arch Full product. |
| Studio MSI includes Runtime MSM | Separate Runtime MSI not required for Studio install smoke | Install **Studio.\<LANG\>.msi** only for Studio+Runtime install tests (see **docs/SMOKE.md**). |

**Workaround when nest fails:** build the nest tree alone with low parallelism, or only the modules you need:

```powershell
cmake --build build/vs2022-full/win32 --config Release --parallel 1 -- /m:1 /p:CL_MPCount=1
# or a single nest target:
cmake --build build/vs2022-full/win32 --config Release --target OpenDCL_Runtime_ZRX_2019
```

**Parity (not host-kit hacks):** Studio static MFC+`/MT`, modules `/MD` (+ `/MDd` FullDebug for all families), multimon stubs only on `PPTooltip.cpp` — permanent classic/product policy.

**Sticky cache:** `cmake --preset …` does **not** overwrite existing
`CMakeCache.txt` entries. A prior configure with empty
`OPENDCL_RUNTIME_TARGETS` (or all families ON) leaves a solution with every
SDK target. To re-apply the preset selection after that:

```powershell
cmake --preset vs2022-x64-dev --fresh
# or force the cache keys:
cmake -S . -B build/vs2022-x64-dev `
  -DOPENDCL_RUNTIME_TARGETS=BRX.27.x64 `
  -DOPENDCL_ENABLE_ARX=OFF -DOPENDCL_ENABLE_BRX=ON `
  -DOPENDCL_ENABLE_GRX=OFF -DOPENDCL_ENABLE_ZRX=OFF `
  -DOPENDCL_RUNTIME_AUTO=OFF -DOPENDCL_RUNTIME_REQUIRE_SELECTED=ON
```

Or manually:

```powershell
cmake -S . -B build/manual -G "Visual Studio 17 2022" -A x64 `
  -DOPENDCL_RUNTIME_TARGETS=ARX.26.x64 `
  -DOPENDCL_ENABLE_BRX=OFF -DOPENDCL_ENABLE_GRX=OFF -DOPENDCL_ENABLE_ZRX=OFF

cmake --build build/manual --config Release
```

Outputs land under `build/<preset>/out/`, mirroring the classic tree so F5 debug
loading of `Runtime.Res.dll` works (`Common/Workspace.cpp` walks two folders up
from the host module, then `..\Localized\<LANG>\Runtime.Res\Debug\`).
**FullDebug → Debug** for everything except **CAD runtime modules** (and the
`/MD` zlib/png they link). Helper: `opendcl_map_fulldebug_to_debug` /
`OPENDCL_CFG_DIR` in `cmake/OpenDCLHelpers.cmake`.

```text
out/Runtime/BRX/BRX.27.x64/FullDebug/OpenDCL.x64.27.brx   # real FullDebug module
out/Runtime/BRX/BRX.27.x64/Debug/OpenDCL.x64.27.brx
out/Runtime/Localized/ENU/Runtime.Res/Debug/Runtime.Res.dll    # Debug + FullDebug
out/Runtime/Localized/ENU/Runtime.Res/Release/Runtime.Res.dll
out/Runtime/RxInstall/Debug/RxInstall.dll     # FullDebug → Debug
out/Library/x64-md/FullDebug/...              # kept for /MDd runtime link
out/Library/x64-mt/Debug/...                  # Studio /MT; FullDebug → Debug
```

## Selection options

| Variable | Meaning |
| --- | --- |
| `OPENDCL_ENABLE_{ARX,BRX,GRX,ZRX}` | Family gates |
| `OPENDCL_RUNTIME_TARGETS` | Explicit ID list; empty = all of enabled families |
| `OPENDCL_RUNTIME_AUTO` | Skip missing SDKs (default ON) |
| `OPENDCL_RUNTIME_REQUIRE_SELECTED` | Fail if a listed target cannot build |
| `OPENDCL_<SDK>_ROOT` | Override SDK path (else `ENV{SDK}`) |
| `OPENDCL_LANGS` | Resource languages (default `ENU`) |
| `OPENDCL_BUILD_RXINSTALL` | Build Win32 RxInstall CA DLL (default **ON** in presets; nested Win32 from x64, sources listed in main .sln for editing) |

## Visual Studio F5 debugger

Default for **all** runtime hosts (ARX/BRX/GRX/ZRX):

| Setting | Default |
| --- | --- |
| Command arguments | `/ld "<absolute path to built module>"` (per config) |
| Command (exe) | unset unless cache/env / `.vcxproj.user` provides it |

CMake writes `VS_DEBUGGER_COMMAND_ARGUMENTS` using `$<TARGET_FILE:…>` so each
configuration gets a **real path** in the `.vcxproj`. Bare MSBuild
`$(TargetPath)` often expands to **empty** when Command Arguments are
inherited (VS history shows `/ld ""`), so the host never loads the module.

Host **Command** (BricsCAD/AutoCAD exe) stays machine-local: set in the
project’s `.vcxproj.user`, or via `OPENDCL_*_DEBUGGER_COMMAND` / `BRX_EXE` /
`DDCAD_PATH`-style env at configure time. Keep Command in `.user` if you like;
leave Arguments on **Inherit** after reconfigure so the project-level path is used.
Do not leave an empty `LocalDebuggerCommandArguments` element in `.user` (that
overrides the project with blank).

| Variable | Role |
| --- | --- |
| `OPENDCL_DEBUGGER_COMMAND` | Default host exe for every family |
| `OPENDCL_DEBUGGER_COMMAND_ARGUMENTS` | Default args (default `/ld "$(TargetPath)"`) |
| `OPENDCL_ARX_DEBUGGER_COMMAND` | ARX host override |
| `OPENDCL_BRX_DEBUGGER_COMMAND` | BRX host override |
| `OPENDCL_GRX_DEBUGGER_COMMAND` | GRX host override |
| `OPENDCL_ZRX_DEBUGGER_COMMAND` | ZRX host override |
| `OPENDCL_*_DEBUGGER_COMMAND_ARGUMENTS` | Per-family args override (empty = global default) |

Env fallbacks when the matching cache is empty:

| Family | Env (first hit) |
| --- | --- |
| ARX | `ARX_EXE`, then `ACAD` |
| BRX | `BRX_EXE`, then `BRICSCAD` |
| GRX | `GRX_EXE` |
| ZRX | `ZRX_EXE` |

```powershell
$env:BRX_EXE = "C:\Program Files\Bricsys\BricsCAD V27 en_US\bricscad.exe"
cmake --preset vs2022-x64-dev
# F5 on OpenDCL_Runtime_BRX_27_x64 → bricscad /ld "$(TargetPath)"
```

## Per-target overrides

1. **Matrix row** in `OpenDCLRuntimeMatrix.cmake` (`DEFINES`, `LIBS`, `SDK_INC`, `WARNING_DISABLES`, `CXX_STANDARD`, …).
2. **Target-specific sources** — drop `.cpp` files in `Runtime/TargetSpecific/<ID>/`
   (see `Runtime/TargetSpecific/README.md`). Auto-picked for that runtime only.
3. **Optional file** `cmake/overrides/<ID>.cmake`:

```cmake
function(opendcl_apply_override target)
  target_compile_options(${target} PRIVATE /wd1234)
  target_link_libraries(${target} PRIVATE SomeRare.lib)
endfunction()
```

### ARX R16 linetype

- Header path: `#if (_ARXTARGET < 17)` → `ArxR16LinetypeComboBoxCtrl.h` (inline control).
- CMake excludes `ArxLinetypeComboBoxCtrl.cpp` when building `ARX` with version &lt; 17.
- Obsolete unused `ArxR16LinetypeComboBoxCtrl.cpp` was **deleted** (was never built in practice).

### Debug / FullDebug and the StdAfx “DEBUG workaround”

`Runtime/StdAfx.h` implements Autodesk’s pattern:

1. **Debug** (`_DEBUG`, no `AC_FULL_DEBUG`): temporarily `#undef _DEBUG` while including MFC / ATL / STL so those headers do not force the debug CRT; then restores `_DEBUG` for app code. CRT **`/MD`**. Links **release** host libs (`SDK_LIB`).
2. **FullDebug** (`_DEBUG` + `AC_FULL_DEBUG`): keeps `_DEBUG` through MFC/host headers. CRT **`/MDd`** for **all** families (ARX/BRX/GRX/ZRX). Intended for developers with a **local debug host** build and host debug import libs (see below). Classic ARX FullDebug was hybrid `/MD`; CMake uses `/MDd` uniformly so static deps (`*-md` FullDebug) match.
3. **Release**: `NDEBUG`, **`/MD`**, release host libs.

#### FullDebug host debug libraries (proprietary — do not scan)

Debug CAD SDK trees are **proprietary**. CMake and agents must **not** open, list, or search those folders.

BRX FullDebug link directories are **explicit only** (no automatic path under the
release SDK). Configure one of:

| Source | Role |
| --- | --- |
| **`BrxDebugLibs` env** | Absolute debug LIB directory (classic `BRX.26.x64` FullDebug + CMake) |
| `OPENDCL_BRX27_FULLDEBUG_LIBDIR` / `BRX27_FULLDEBUG_LIBDIR` | Per-SDK absolute override (any BRX major) |
| `OPENDCL_BRX26_FULLDEBUG_LIBDIR` / `BRX26_FULLDEBUG_LIBDIR` | Same for BRX 26 |

If none is set, FullDebug still builds but only release SDK libdirs are on the
link line (host debug imports will fail until a path is configured).

```powershell
$env:BrxDebugLibs = "X:\path\to\brx\debug\libs"   # proprietary; set by operator
$env:BRX27 = "S:\BRX27"                           # release SDK for Debug/Release
cmake --preset vs2022-x64-dev
cmake --build build/vs2022-x64-dev --config FullDebug
```

#### Machine overlays: `local.props` and parent `dev.props`

Each runtime’s generated `*.libdirs.props` (attached as `VS_USER_PROPS`) ends with
optional imports using **relative paths or MSBuild macros only** (no absolute drive
paths). Later imports win:

```xml
<!-- 1) Sibling of this props file / .sln (build/<preset>/local.props) -->
<Import Project="local.props"
        Condition="exists('$(MSBuildThisFileDirectory)local.props')" />

<!-- 2) $(SolutionDir)local.props if SolutionDir differs -->
<Import Project="$(SolutionDir)local.props" Condition="..." />

<!-- 3) Parent of the git checkout (e.g. ../../../dev.props from build/<preset>/) -->
<Import Project="..\..\..\dev.props"
        Condition="exists('$(MSBuildThisFileDirectory)..\..\..\dev.props')" />
```

**Import order:**

1. `$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props` (macro)
2. OpenDCL generated libdirs (per target / config)
3. **`local.props`** next to the props/sln — solution-local overlay
4. **`<repo-parent>/dev.props`** (relative) — machine overlay; **overrides local.props**

Example **parent** `dev.props` (not in the repo): keep release SDK roots and
`BrxDebugLibs` in the **process environment**; use `dev.props` only for optional
include/lib overlays:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <!-- BrxDebugLibs / BRX27 come from the environment, not this file. -->
  <ItemDefinitionGroup>
    <ClCompile>
      <!-- Optional include overrides -->
      <!-- <AdditionalIncludeDirectories>…;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories> -->
    </ClCompile>
    <Link>
      <!-- Optional lib dir overrides (after OpenDCL + local.props) -->
      <!-- <AdditionalLibraryDirectories>…;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories> -->
    </Link>
  </ItemDefinitionGroup>
</Project>
```

```powershell
# Release SDK always; FullDebug host debug libs when building FullDebug:
$env:BRX27        = "S:\BRX27"
$env:BrxDebugLibs = "X:\path\to\brx\debug\libs"
```

Example **solution** `local.props` (`build/vs2022-x64-dev/local.props`) for checkout-specific tweaks; `dev.props` can still override them.

Do not commit machine paths. `local.props` is gitignored inside the checkout; `dev.props` lives **above** the repo so it is never part of the tree.

## Product wiring (BRX.27 / RxInstall / WiX)

When adding a host runtime (see skill `add-runtime-target`):

1. **CMake matrix** — `cmake/OpenDCLRuntimeMatrix.cmake` row + `VI/` props (already done for BRX.27).
2. **RxInstall** — `Runtime/RxInstall/RxInstall.cpp`:
   - `kBRX27` / `kBricscad27x64`
   - `InstallAllTargets` → `EnumerateRegTargets(…kBricscad27x64…)`
   - `UninstallAllTargets` → `RemoveAllRegTargets(…V27x64…)`
   - Module name is automatic: `OpenDCL.x64.27.brx` via major + `x64` modifier
   - Registry app root: `SOFTWARE\Bricsys\Bricscad\V27x64`
3. **WiX inventory** — `$RuntimeModules` in `scripts/build-wix.ps1` classic path:
   `Runtime\BRX\BRX.27.x64\Release\OpenDCL.x64.27.brx`
4. **Path resolve** — `Resolve-ProductFile` searches `-OpenDclRoot` (product layout),
   then `out\<rel>`, then `win32\out\`, then the packaging repo for source assets only.
   RxInstall CA path is candle define `RxInstallDll`.

### RxInstall CMake notes

- MSI Binary CA **must be Win32 (x86)** (`DllEntry=_RxInstallMachine@4`).
- `OPENDCL_BUILD_RXINSTALL=ON` on an **x64** parent:
  - **`OPENDCL_NEST_WIN32=ON`** (full Mixed): PE from full nest `build/<preset>/win32`.
    Target `OpenDCL_RxInstall` is an umbrella under **`Win32/Packaging`** that runs
    `cmake --build …/win32 --target OpenDCL_RxInstall` (no private `rxinstall-win32`).
  - **Nest off** (x64-only presets): private nest under `build/<preset>/rxinstall-win32`.
- Outputs:
  - `out/Runtime/RxInstall/<Config>/RxInstall.dll`
  - mirrored to classic `Runtime/RxInstall/<Config>/RxInstall.dll` for WiX
- Standalone:
  ```powershell
  cmake -S Runtime/RxInstall -B build/rxinstall-win32 -G "Visual Studio 17 2022" -A Win32
  cmake --build build/rxinstall-win32 --config Release
  ```

### Packaging from CMake outputs

Two packaging modes share the same script (`scripts/build-wix.ps1`):

| Mode | When | Output identity |
| --- | --- | --- |
| **Full product** | Default (`-ModuleSet Full`) | `OpenDCL.Runtime.msm` / `.msi` with historical modularization GUID (third-party merge) |
| **Custom subset** | `-Runtimes …`, `-ModuleSet Selected\|Available`, and/or language filters | `OpenDCL.Runtime.custom.msm` / `.msi` with seed GUIDs (no ship identity clash) |

```powershell
# Dev custom: only modules/langs actually present under the cmake binary dir out\
.\scripts\build-wix.ps1 `
  -OpenDclRoot (Resolve-Path build\vs2022-full) `
  -ModuleSet Available `
  -AvailableLanguages `
  -SkipStudio

# Explicit choice (must exist)
.\scripts\build-wix.ps1 `
  -OpenDclRoot (Resolve-Path build\vs2022-x64-dev) `
  -Runtimes BRX.27.x64 `
  -Languages ENU `
  -SkipStudio

# Full product release (all catalog modules + all languages; missing files fail)
.\scripts\build-wix.ps1
# or: .\scripts\make-release.ps1 -ProductVersion 10.1.1.1
```

`-Runtimes` accepts IDs (`BRX.27.x64`), families (`BRX`), or wildcards (`BRX.2*`).
Languages: `-Languages ENU,DEU` or `-AvailableLanguages` (only packs that have `Runtime.Res`).

## Studio (CMake)

Enabled with `OPENDCL_BUILD_STUDIO=ON` (on in base presets).

| Target | Output |
| --- | --- |
| `OpenDCL_Studio` | `out/Studio/x64\|Win32/<Debug\|Release>/OpenDCL Studio.exe` (+ classic mirror) |
| `OpenDCL_StudioRes_<LANG>` | `out/Studio/Localized/<LANG>/Studio.Res/<Debug\|Release>/Studio.Res.dll` |
| `OpenDCL_StudioHelp_<LANG>` | `Studio/Localized/<LANG>/Content/OpenDCL.chm` (per `OPENDCL_LANGS`; needs `hhc.exe`) |
| `OpenDCL_StudioHelp_All` | Aggregate of all configured language CHMs |

Notes:
- **Static MFC + /MT** (classic). zlib/png use matching `*_mt` static CRT variants.
- **No FullDebug product**: solution `FullDebug` maps Studio + Studio.Res to **Debug** (policy for all non-runtime-module targets).
- **WINVER** forced to 0x0601+ for modern MFC headers; arch-correct embedded manifest (classic RC is X86-only).
- **HTML Help**: `OpenDCL_StudioHelp_<LANG>` builds each language CHM via `hhc.exe`. The default language is a Studio dependency; **post-build copies** `Studio.Res.dll` + `OpenDCL.chm` next to `Studio.exe` so classic `Workspace::FindFile` / `GetLocalResourceModule` work without special F5 path logic.
- **Multimon stubs**: Studio sets `COMPILE_MULTIMON_STUBS` on **`PPTooltip.cpp` only** (static MFC has no `_AFXDLL`; `FolderTreeCtrl.cpp` also includes `MultiMon.h` and must not compile stubs). Runtime modules still rely on `_AFXDLL` in source.

## Known gaps

- Full multi-toolset ship CI parity for the oldest hosts (e.g. ARX.16 / BRX.9 and some `v140` + latest Windows SDK edge cases still need host-specific care; toolset-matched zlib/png cover the main UCRT link gap)
- Classic `.vcxproj` rows may lag CMake for brand-new hosts (e.g. BRX.27 is in the CMake matrix + WiX/RxInstall inventory; classic project scaffolding is optional)

## Regenerating the matrix

```powershell
# See scripts/generate-runtime-matrix.ps1 when present
```

The matrix is derived from each `Runtime/*/VI/*.props` plus toolset/output
names from the corresponding classic `.vcxproj` where one exists.
