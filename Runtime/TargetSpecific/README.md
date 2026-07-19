# Target-specific runtime sources and drops

CMake applies optional per-matrix-ID files under:

```text
Runtime/TargetSpecific/<RuntimeId>/
```

`<RuntimeId>` must match a matrix ID exactly (the same string as in
`cmake/OpenDCLRuntimeMatrix.cmake`).

## Extra sources

Any translation unit:

```text
Runtime/TargetSpecific/<RuntimeId>/*.{cpp,cxx,cc}
```

is compiled **only** into that runtime target (not the shared list in
`cmake/OpenDCLRuntimeSources.cmake`).

| Folder | Built with |
| --- | --- |
| `Runtime/TargetSpecific/ARX.16/` | `ARX.16` only |
| `Runtime/TargetSpecific/BRX.9/` | `BRX.9` only — classic CAcUi/CAdUi combo stubs + `drop.txt` |
| `Runtime/TargetSpecific/ARX.26.x64/` | `ARX.26.x64` only |
| `Runtime/TargetSpecific/BRX.26.x64/` | `BRX.26.x64` only |

## Shared-source drops (`drop.txt`)

Optional file:

```text
Runtime/TargetSpecific/<RuntimeId>/drop.txt
```

- **Absent or empty** → no drops (default for all IDs).
- **Present** → each non-comment line is a **basename** of a shared runtime
  source to omit from that target only (classic `ExcludedFromBuild`).
- Lines starting with `#` and blank lines are ignored.
- Paths/subdirs are not allowed (basename only, e.g. `AcadPaletteHost.cpp`).

Example (`BRX.9`):

```text
# no CAdUiPaletteSet / CAdUiDockControlBar in brx.lib
AcadDockBarHost.cpp
AcadPaletteHost.cpp
ControlBarDlg.cpp
PaletteDlg.cpp
```

## Rules

- **Shared** code stays in `Runtime/`, `Runtime/Controls/`, `Common/`, etc.
  (listed in `cmake/OpenDCLRuntimeSources.cmake`).
- **Host/version-only** add/drop policy lives in this folder so other targets
  are untouched.
- The folder is also a **private include directory** for that target (handy for
  local headers next to the `.cpp` files).
- Empty folders are ignored. New/removed `.cpp` / `drop.txt` reconfigure via
  `CONFIGURE_DEPENDS` / configure depends.

## Example (add sources only)

```text
Runtime/TargetSpecific/ARX.16/
  R16OnlyHelper.cpp
  R16OnlyHelper.h
```

No matrix or CMakeLists edit is required; reconfigure and build `ARX.16`.

## Related (not this folder)

- **R16 linetype control**: still handled by `#if (_ARXTARGET < 17)` in
  `Runtime/Controls/ArxLinetypeComboBoxCtrl.h` → `ArxR16LinetypeComboBoxCtrl.h`
  (header-only). The obsolete `ArxR16LinetypeComboBoxCtrl.cpp` was removed.
- **Flags / libs / defines**: matrix row or `cmake/overrides/<ID>.cmake`.
