# OpenDCL CMake experiment (`opendcl-cmake`)

This tree is a **local checkout** for evaluating a CMake-generated build that
replaces the hand-maintained per-CAD-version `.vcxproj` matrix.

## Push is disabled

This clone refuses pushes:

- `git remote` push URL is `DISABLED_DO_NOT_PUSH`
- `.git/hooks/pre-push` exits with an error

Fetch from the local `opendcl` clone (or re-point `origin` fetch) is fine.
Do **not** enable push until the experiment is deliberately promoted.

## Layout

| Path | Role |
| --- | --- |
| `CMakeLists.txt` | Root project, options, subdirs |
| `CMakePresets.json` | Dev / ARX-latest / ARX-modern presets |
| `cmake/OpenDCLHelpers.cmake` | Options, registry, SDK detect, selection |
| `cmake/OpenDCLRuntimeMatrix.cmake` | All runtime rows (from `VI/*.props`) |
| `cmake/OpenDCLRuntimeSources.cmake` | Shared Runtime+Common `.cpp` list |
| `cmake/OpenDCLRuntimeTargets.cmake` | `opendcl_add_runtime()` factory |
| `cmake/overrides/<ID>.cmake` | Optional per-target flag/link escape hatch |
| `Runtime/TargetSpecific/<ID>/` | Optional per-target `.cpp` injection (auto) |
| `Library/CMakeLists.txt` | ZLib + LibPNG |
| `Runtime/Localized/CMakeLists.txt` | `Runtime.Res.<lang>` DLLs |

Existing `OpenDCL.sln` / `.vcxproj` files are **unchanged** and still buildable.

## Configure

Requirements: CMake ≥ 3.24, Visual Studio 2022 (or later) with C++ MFC, CAD SDKs
as env vars (`ARX2027`, `BRX26`, …) same as the classic props sheets.

```powershell
cd P:\Work\OpenDCL\Source\opendcl-cmake

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
```

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

BRX FullDebug matches classic OpenDCL:

```text
$(BRX_PATH)\lib\vc143x64\Debug;%(AdditionalLibraryDirectories)
```

(`vcNNNx64` follows the target toolset, e.g. v143 → `vc143x64`.)

| Source | Role |
| --- | --- |
| **`BRX_PATH` env** | Classic (preferred). Used as MSBuild `$(BRX_PATH)` — no reconfigure needed when it changes |
| `BrxDebugLibs` | Absolute override (BRX samples) |
| `OPENDCL_BRX27_FULLDEBUG_LIBDIR` / `BRX27_FULLDEBUG_LIBDIR` | Absolute override |

```powershell
$env:BRX_PATH = "X:\dev\bricscad\brx"   # classic
$env:BRX27 = "S:\BRX27"                 # release SDK for Debug/Release
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

Example **parent** `dev.props` (not in the repo): do **not** redefine `BRX_PATH` —
FullDebug already expands `$(BRX_PATH)` from the **process environment**. Use
`dev.props` only for optional include/lib overlays:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <!-- BRX_PATH / BRX27 come from the environment, not this file. -->
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
# Required for FullDebug (and CMake release SDK root):
$env:BRX_PATH = "X:\dev\bricscad\brx"
$env:BRX27    = "S:\BRX27"
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
4. **Path resolve** — `Resolve-ProductFile` tries classic then `out\<classic>` so either
   a full classic Release tree or a CMake binary dir (`-OpenDclRoot build\…`) works.
   RxInstall CA path is passed as candle define `RxInstallDll`.

### RxInstall CMake notes

- MSI Binary CA **must be Win32 (x86)** (`DllEntry=_RxInstallMachine@4`).
- `OPENDCL_BUILD_RXINSTALL=ON` on an **x64** parent creates nested Win32 configure under
  `build/<preset>/rxinstall-win32` and target `OpenDCL_RxInstall`.
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
# Dev custom: whatever is built (e.g. BRX.27 + ENU only)
.\scripts\build-wix.ps1 `
  -OpenDclRoot (Resolve-Path build\vs2022-x64-dev) `
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
| `OpenDCL_StudioHelp_ENU` | `Studio/Localized/ENU/Content/OpenDCL.chm` (Studio dependency; needs `hhc.exe`) |

Notes:
- **Static MFC + /MT** (classic). zlib/png use matching `*_mt` static CRT variants.
- **No FullDebug product**: solution `FullDebug` maps Studio + Studio.Res to **Debug** (policy for all non-runtime-module targets).
- **WINVER** forced to 0x0601+ for modern MFC headers; arch-correct embedded manifest (classic RC is X86-only).
- **ENU HTML Help**: `OpenDCL_StudioHelp_ENU` builds `Studio/Localized/ENU/Content/OpenDCL.chm` via `hhc.exe`. Studio depends on it and **post-build copies** `Studio.Res.dll` + `OpenDCL.chm` next to `Studio.exe` so classic `Workspace::FindFile` / `GetLocalResourceModule` work without special F5 path logic.
- **Multimon stubs**: Studio sets `COMPILE_MULTIMON_STUBS` on **`PPTooltip.cpp` only** (static MFC has no `_AFXDLL`; `FolderTreeCtrl.cpp` also includes `MultiMon.h` and must not compile stubs). Runtime modules still rely on `_AFXDLL` in source.

## Not yet ported

- Multi-language CHM packs in CMake (classic `HTMLHelp.*.vcxproj` per lang)
- Full multi-toolset ship CI parity (older SDKs fail linking modern UCRT zlib/png from VS2022)

## Regenerating the matrix

From this tree (PowerShell):

```powershell
# See scripts/generate-runtime-matrix.ps1 when present
```

The matrix was generated from each `Runtime/*/VI/*.props` plus toolset/output
from the sibling `.vcxproj`.
