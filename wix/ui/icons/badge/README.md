# OpenDCL crosshair badge

Canonical **pure** badge art for Start Menu / package icons. Transparent background,
no host glyph (book, document, etc.) — only the circular crosshair used as a corner
overlay on Help, License, and any future shortcut icons.

## Use this file

| Path | Purpose |
|------|---------|
| **`OpenDCLBadge.png`** | Primary high-res master (**512×512**). Prefer this when compositing or resizing. |
| `OpenDCLBadge-1024.png` | Ultra-high master for print / large marketing if ever needed |
| `OpenDCLBadge-256.png` … `OpenDCLBadge-32.png` | Pre-sized PNGs for quick paste at known sizes |
| `OpenDCLBadge.ico` | Multi-res ICO of the badge alone (rarely used alone in the product) |

**Do not** crop the badge out of `OpenDCLHelp.png` / `OpenDCLLicense.png` for new work —
those composites include soft shadow and may clip document pixels into the crop.

## Palette / geometry (keep in sync)

| Element | Value |
|---------|--------|
| Badge blue | RGB `(0, 100, 210)` — close to License body `#0078D0` |
| Cross / ring | Pure white `(255, 255, 255)` |
| Background | Fully transparent |
| Ring thickness | ~5.5% of diameter |
| Cross bar width | ~8.3% of diameter |
| Cross end inset | **none** — white bars run straight to the outer edge (no blue tip border) |

Regenerate with:

```powershell
python wix\tools\make_badge.py
```

## Compositing onto a base icon

1. Draw or load the base glyph at 256×256 (or larger, then downscale).
2. Load `OpenDCLBadge.png` (or a sized variant).
3. Scale badge to ~**31%** of canvas (≈80 px on 256) — same as License.
4. Place bottom-right with ~3–4% padding; optional soft elliptical shadow under the badge.
5. Export multi-resolution ICO (16, 24, 32, 48, 64, 128, 256) with **PNG-compressed** frames.

See skill **`make-package-icons`** for the full Start Menu / WiX procedure.
