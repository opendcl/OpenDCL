# Target-specific runtime sources

CMake automatically compiles translation units placed under:

```text
Runtime/TargetSpecific/<RuntimeId>/*.{cpp,cxx,cc}
```

`<RuntimeId>` must match a matrix ID exactly (the same string as in
`cmake/OpenDCLRuntimeMatrix.cmake`), for example:

| Folder | Built with |
| --- | --- |
| `Runtime/TargetSpecific/ARX.16/` | `ARX.16` only |
| `Runtime/TargetSpecific/ARX.26.x64/` | `ARX.26.x64` only |
| `Runtime/TargetSpecific/BRX.26.x64/` | `BRX.26.x64` only |

## Rules

- **Shared** code stays in `Runtime/`, `Runtime/Controls/`, `Common/`, etc.
  (listed in `cmake/OpenDCLRuntimeSources.cmake`).
- **Host/version-only** code goes here so it is not compiled into other targets.
- The folder is also added as a **private include directory** for that target
  (handy for local headers next to the `.cpp` files).
- Empty folders are ignored. New/removed files reconfigure via `CONFIGURE_DEPENDS`.

## Example

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
