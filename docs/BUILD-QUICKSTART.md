# Build quickstart

Step-by-step CMake generate and build for day-to-day work on Studio or a
small set of CAD runtimes. This is the path for unfamiliar developers.

Framework details (matrix, options, dual-arch ship, debugger overrides):
**[CMAKE.md](../CMAKE.md)**. Agent / packaging layout notes: **[AGENTS.md](../AGENTS.md)**.

## Prerequisites

| Requirement | Notes |
| --- | --- |
| Windows | Native MSVC build only |
| [CMake](https://cmake.org/) ≥ 3.24 | On `PATH` (`cmake --version`) |
| Visual Studio 2022 | **Desktop development with C++** and **MFC** |
| CAD SDK(s) | Only if you build Runtime modules (not needed for Studio-only) |
| HTML Help Workshop (`hhc.exe`) | Optional; builds `OpenDCL.chm` for Studio Help / F5 |

CAD SDKs are **not** in this repository. Point an environment variable at the
SDK root (same names as classic `VI/*.props`). Common modern examples:

| Matrix ID (example) | Env var | Points at |
| --- | --- | --- |
| `ARX.26.x64` | `ARX2027` | ObjectARX 2027 / AutoCAD 2027 SDK root |
| `BRX.27.x64` | `BRX27` | BricsCAD V27 BRX SDK root |

Override with a CMake cache path if you prefer: `-DOPENDCL_<ENV>_ROOT=…`
(for example `OPENDCL_BRX27_ROOT`). Full ID ↔ env map:
`cmake/OpenDCLRuntimeMatrix.cmake`.

The **dev** preset only considers Platform Toolsets **≥ v141** (VS2017+). Older
toolsets and Daffodil are **not** needed for the paths below. See the main
[README](../README.md) only if you later build historic matrix rows.

## Path A — Studio only (no CAD SDK)

### A1. Dedicated Win32 Studio tree

Win32 Studio + Studio.Res; no Runtime modules, Runtime.Res, or RxInstall.

```powershell
cd <OpenDCL repo root>

cmake --preset vs2022-x86-studio
cmake --build --preset vs2022-x86-studio-debug
# or Release:
cmake --build --preset vs2022-x86-studio-release
```

Open: `build/vs2022-x86-studio/vs2022-x86-studio.sln`.

Configs are **Debug** and **Release** only (no FullDebug without runtimes).

### A2. Dev preset with no CAD SDKs

If you use **`vs2022-x64-dev`** and no modern CAD SDK env vars are set, configure
still succeeds: **no runtime modules** are selected, and Studio still builds
(x64). Prefer A1 when you only care about Studio.

## Path B — Runtime + Studio (typical)

### B1. Dev default (recommended)

Preset **`vs2022-x64-dev`**:

- Enables ARX / BRX / GRX / ZRX families
- Skips missing SDKs (`OPENDCL_RUNTIME_AUTO`)
- Keeps only Platform Toolset **≥ v141**
- Enables at most **one** runtime per family (highest matrix version with an SDK)

Set env vars for the SDKs you have. Families without a matching SDK are omitted.
After installing a new SDK, reconfigure with `--fresh` so selection updates.

```powershell
cd <OpenDCL repo root>

# Examples only — use the roots on your machine (any subset is fine):
$env:ARX2027 = "C:\ObjectARX 2027"
$env:BRX27   = "C:\Bricsys\BricsCAD V27\API\brx"

cmake --preset vs2022-x64-dev
# or after changing SDK env vars:
# cmake --preset vs2022-x64-dev --fresh

cmake --build --preset vs2022-x64-dev-debug
# or Release:
cmake --build --preset vs2022-x64-dev-release
```

Configure log lines `ENABLE  <id>  SDK=…` show which modules were selected.

Open: `build/vs2022-x64-dev/vs2022-x64-dev.sln`.

Set a runtime module (or Studio) as the startup project, then **F5**. CMake fills
`/ld "<path-to-module>"` and tries to discover the CAD host from the registry
(override details in [CMAKE.md](../CMAKE.md) — Visual Studio F5 debugger).

### B2. Pin a single host version

```powershell
$env:ARX2027 = "C:\ObjectARX 2027"
cmake --preset vs2022-x64-arx-latest
cmake --build --preset vs2022-x64-arx-latest-release

# BricsCAD V27 only:
# cmake --preset vs2022-x64-brx-latest
```

These **require** the pinned SDK (`ARX.26.x64` / `BRX.27.x64`).

### B3. Every SDK you have installed (x64)

No per-family cap and no toolset floor — can be a large solution.

```powershell
cmake --preset vs2022-x64-auto
cmake --build build/vs2022-x64-auto --config Debug
```

## Outputs

Binaries land under `build/<preset>/out/`, for example:

```text
out/Runtime/BRX/BRX.27.x64/Debug/OpenDCL.x64.27.brx
out/Runtime/ARX/ARX.26.x64/Debug/OpenDCL.x64.26.arx
out/Runtime/Localized/ENU/Runtime.Res/Debug/Runtime.Res.dll
out/Studio/x64/Debug/Studio.exe
```

Exact layout and FullDebug mapping: [CMAKE.md](../CMAKE.md).

## Common problems

| Symptom | What to try |
| --- | --- |
| No runtime modules with `vs2022-x64-dev` | Set SDK env vars for modern hosts (toolset ≥ v141); check configure `ENABLE` / `SKIP` lines; use `--fresh` after changing env |
| Configure errors about missing SDK / selected target | Pinned presets (`*-arx-latest`, `*-brx-latest`) require that SDK; or use `vs2022-x64-dev` / auto / Studio-only |
| Wrong / stale runtimes after changing SDK env or presets | Sticky cache: `cmake --preset vs2022-x64-dev --fresh` |
| MFC / `afx….h` not found | Install the VS **MFC and ATL** workload components and reconfigure |
| F5 starts CAD but does not load OpenDCL | Leave debugger **Command Arguments** on Inherit; see CMAKE.md debugger section (empty `.user` args override `/ld`) |
| No CHM / Help target fails | Install HTML Help Workshop, or ignore Help if you are only iterating code |

## Other presets

| Preset | When to use |
| --- | --- |
| `vs2022-x64-arx-modern` | Up to **3** latest modern ARX SDKs (`PER_FAMILY_MAX=3`, toolset ≥ v141) |
| `vs2022-full` | Full dual-arch product / ship layout — not needed for normal contribution. Details: [CMAKE.md](../CMAKE.md), packaging smoke: [SMOKE.md](SMOKE.md) |

All configure/build preset names: `CMakePresets.json`.

## Classic solution

`OpenDCL.sln` remains for historical / transitional builds. For new work,
prefer the CMake presets above.
