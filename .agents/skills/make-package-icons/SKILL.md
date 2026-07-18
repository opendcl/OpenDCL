---
name: make-package-icons
description: >
  Create or refresh OpenDCL Start Menu / WiX package icons (Studio app, Help,
  License, and the shared crosshair badge). Use when adding a new shortcut icon,
  restyling Help/License, regenerating multi-res ICOs, fixing drab or missing
  Start Menu icons, or when the user runs /make-package-icons. Triggers:
  "package icons", "start menu icons", "help icon", "license icon", "crosshair
  badge", "OpenDCLHelp.ico", "OpenDCLLicense.ico", "WiX Icon table".
---

# Make OpenDCL Package Icons

Procedure for **Start Menu and ARP icons** packaged by WiX. Derived from the
WiX packaging polish (multi-res ICO extraction, Fluent-style bases, shared
OpenDCL crosshair badge).

This skill covers **icon art + WiX Icon table wiring**. It does not change
product version (`/bump-version`) or copyright years (`/update-copyright-year`).

## Where things live

Repo root = this OpenDCL tree (`OpenDCL.sln` / `scripts/build-wix.ps1`).

| Asset | Path | Role |
|-------|------|------|
| **Badge master (pure)** | `wix/ui/icons/badge/OpenDCLBadge.png` | **512×512** transparent crosshair — **canonical** for composites |
| Badge size set | `wix/ui/icons/badge/OpenDCLBadge-*.png` | 32…1024 pre-renders |
| Badge ICO | `wix/ui/icons/badge/OpenDCLBadge.ico` | Multi-res badge alone (rarely used alone) |
| Badge README | `wix/ui/icons/badge/README.md` | Geometry + palette |
| Help ICO | `wix/ui/icons/OpenDCLHelp.ico` | Start Menu **OpenDCL Help** |
| License ICO | `wix/ui/icons/OpenDCLLicense.ico` | Start Menu **OpenDCL License** |
| WixUI bitmaps | `wix/ui/WixUIBanner.bmp`, `WixUIDialog.bmp` | Installer dialog chrome (exact sizes) |
| Studio app icon | Extracted at package time from `Studio.Res.dll` **GROUP_ICON #10** | Start Menu **OpenDCL Studio** + ARP |
| WiX Icon Ids | `wix/Studio/OpenDCL.Studio.wxs` | `OpenDCLStudio.exe`, `OpenDCLHelp.ico`, `OpenDCLLicense.ico` |
| Badge generator | `wix/tools/make_badge.py` | Regenerates pure badge masters |
| Help restyle generator | `wix/tools/make_help_restyle.py` | Book+? multi-tone + badge → Help ICO |
| Local package output | `wix/out/` (gitignored) | Generated MSI/MSM, extracted app icons, scratch |

**Checked-in product icon sources** for Help/License are under `wix/ui/icons/`
(ICO + badge masters). Generators live in `wix/tools/`. Do not treat `wix/out/`
scratch as source of truth.

## Design rules

### Shared style

- **Windows-like multi-tone blue**, not flat navy. Body blue matches License:
  primary **`(0, 120, 208)`** / `#0078D0`.
- White glyphs/highlights (header bar, text lines, `?`, page edges).
- Darker blue for spine / dog-ear / depth **`(0, 90, 180)`**.
- Soft top highlight / lighter blue **`(40, 148, 224)`** where helpful.
- Transparent background (no checker baked into the ICO).

### Crosshair badge (always from pure master)

- **Source:** `wix/ui/icons/badge/OpenDCLBadge.png` (or sized variant).
- **Never** crop the badge from Help/License composites for new icons (shadow +
  host glyph bleed).
- On a 256 canvas: badge ≈ **31%** of width (~80px), bottom-right, ~3–4% padding
  (match License).
- Optional soft elliptical shadow under the badge at ≥32 px.
- Badge palette: blue **`(0, 100, 210)`**, white ring/cross; ring ~5.5% diameter,
  bar ~8.3% diameter; **white bars extend full radius** (no thin blue border at
  the tips — see badge README).

### Glyph bases (current)

| Shortcut | Base concept | Notes |
|----------|--------------|--------|
| Studio | Product app icon from resources | Full multi-res group **#10** from `Studio.Res.dll` — do not invent a new Studio mark unless asked |
| Help | Book + white `?` | Fluent-like book-question; **multi-tone** blue to match License |
| License | Document header (white header bar + lines + dog-ear) | Multi-tone blue; reference look for Help |

Same base shape can be kept when restyling; **palette/lighting** is usually
enough. Only switch base glyph if the user asks for a different metaphor.

## WiX / Windows constraints (do not skip)

1. **Icon table Id** must end in `.exe` or `.ico` or ICE50 / Start Menu advertising
   misbehaves (`OpenDCLStudio.exe`, `OpenDCLHelp.ico`, `OpenDCLLicense.ico`).
2. **Advertised shortcuts** use the Icon table stream (not only the target file’s
   embedded icon). Updating on-disk CHM/TXT icons alone does **not** refresh the
   Start Menu; the MSI Icon streams must be rebuilt.
3. **Multi-resolution ICO** required (at least 16, 32, 48, 256). Prefer PNG-compressed
   frames inside the ICO (Vista+). Typical set: **16, 24, 32, 48, 64, 128, 256**.
4. **Never** feed WiX a whole `Studio.Res.dll` (or other PE) as `Icon/@SourceFile`
   hoping multi-res survives — Windows often shows a single drab size. Extract a
   real `.ico` (app) or ship a standalone multi-res `.ico` (Help/License).
5. Studio app icon: `Export-StudioAppIcon` in `scripts/build-wix.ps1` extracts
   **GROUP_ICON #10** into `wix/out/gen/icons/<LANG>/StudioRes.App.ico` at package
   time. Help/License are **checked-in** under `wix/ui/icons/`.

## Workflows

### A. Regenerate pure badge only

```powershell
python wix\tools\make_badge.py
```

Confirm `wix/ui/icons/badge/OpenDCLBadge.png` exists (512×512, transparent).

### B. Recreate / restyle Help (or similar badged icon)

1. Ensure badge master exists (workflow A).
2. Draw or load the base glyph at each needed size (or draw at 256 and carefully
   downscale; for ≤32 prefer redrawing for crispness).
3. Composite badge from `badge/OpenDCLBadge.png` (resize with LANCZOS).
4. Write multi-res ICO into `wix/ui/icons/`.
5. For Help specifically:

```powershell
python wix\tools\make_help_restyle.py
```

6. Visually compare Help vs License side-by-side (same blue family, matching badge).

### C. Recreate License

License art is the multi-tone document-header reference. If regenerating:

1. Keep white header bar, three horizontal lines, dog-ear darker blue.
2. Composite the **same** badge master bottom-right (same scale as Help).
3. Overwrite `wix/ui/icons/OpenDCLLicense.ico`.

### D. New Start Menu shortcut icon

1. Choose a clear glyph (prefer simple Fluent-like filled shapes).
2. Apply multi-tone blue / white highlights so it sits next to Help/License.
3. Composite `badge/OpenDCLBadge.png`.
4. Add checked-in multi-res ICO under `wix/ui/icons/`.
5. Wire WiX:
   - New `<Icon Id="Something.ico" SourceFile="..."/>` in `OpenDCL.Studio.wxs`
     (Id **must** end in `.ico` or `.exe`).
   - Shortcut `Icon=` / `IconIndex="0"` in `New-StudioFilesFragment` inside
     `scripts/build-wix.ps1`.
6. Rebuild Studio MSI and verify Start Menu.

### E. Package and verify

```powershell
# ENU-only rebuild (reuse MSM)
.\scripts\build-wix.ps1 -Languages ENU -SkipMsm -SkipRuntimeMsi
```

Install **elevated** (per-machine MSI; silent non-admin → Error 1925 / 1603):

```powershell
Start-Process msiexec.exe -Verb RunAs -Wait -ArgumentList @(
  '/i', (Resolve-Path 'wix\out\Release\OpenDCL.Studio.ENU.msi'),
  '/qn'
)
```

Verify:

1. Build log line `icons: app=… help=… license=…` — Help/License sizes match the
   new files on disk.
2. Start Menu shortcuts under
   `%ProgramData%\Microsoft\Windows\Start Menu\Programs\OpenDCL Studio\`
   → `IconLocation` under `C:\Windows\Installer\{ProductCode}\…`.
3. New ProductCode each build (`Product Id="*"`); latest Installer cache folder
   holds the new streams.
4. Optional: `dark.exe -x <dir> OpenDCL.Studio.ENU.msi` and open `Icon\OpenDCLHelp.ico`.

## Multi-res ICO packing (Python)

Use PNG frames inside ICO (reliable on modern Windows). Helpers: `write_multi_ico`
in `wix/tools/make_badge.py` / `make_help_restyle.py`. Do **not** rely on Pillow’s
default ICO save alone without checking frame count / file size (easy to write a
single 16×16 by mistake).

## Palette quick reference

| Token | RGBA | Use |
|-------|------|-----|
| Body blue | `(0, 120, 208, 255)` | Cover / document body |
| Dark blue | `(0, 90, 180, 255)` | Spine, dog-ear |
| Deep blue | `(0, 72, 160, 255)` | Soft shade |
| Light blue | `(40, 148, 224, 255)` | Top highlight band |
| Badge blue | `(0, 100, 210, 255)` | Crosshair quadrants |
| White | `(255, 255, 255, 255)` | Glyphs, cross, ring |
| Page | `(248, 248, 248, 255)` | Book page edge |

## Do / don’t

| Do | Don’t |
|----|--------|
| Keep pure badge under `wix/ui/icons/badge/` | Crop badge from composite Help/License icons |
| Ship multi-res standalone ICOs for Help/License | Point Icon table at whole PE/DLL files |
| Match Help blue to License multi-tone | Leave Help as flat navy next to bright License |
| End Icon Ids with `.ico` / `.exe` | Use Ids like `HelpIcon` without extension |
| Elevated reinstall to refresh advertised icons | Expect shortcut icons to update without MSI Icon streams |
| Prefer regenerating scripts for durable art | Hand-edit only one size and forget smaller sizes |

## Capture lessons

When this procedure changes (new badge geometry, new shortcut, WiX path move),
update **this** skill and `wix/ui/icons/badge/README.md`.
