"""Generate pure high-res OpenDCL crosshair badge masters (no host glyph).

Writes into wix/ui/icons/badge/ for reuse when recreating Help/License or
adding new Start Menu icons. See that folder's README.md and skill
make-package-icons.
"""
from __future__ import annotations

import io
import struct
from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(r"P:\Work\OpenDCL\Source\opendcl\wix")
BADGE_DIR = ROOT / "ui" / "icons" / "badge"

BLUE_BADGE = (0, 100, 210, 255)
WHITE = (255, 255, 255, 255)
T = (0, 0, 0, 0)


def draw_badge(size: int) -> Image.Image:
    """Pure crosshair on transparent background (4x supersampled).

    White cross bars run straight to the outer edge (no thin blue rim at the
    tips). Circle-masked so square bar ends do not stick past the disc.
    """
    scale = 4
    S = size * scale
    im = Image.new("RGBA", (S, S), T)
    d = ImageDraw.Draw(im)
    # White disc, then blue fill leaving a thin outer white ring
    d.ellipse((0, 0, S - 1, S - 1), fill=WHITE)
    ring = max(scale, round(S * 0.055))
    d.ellipse((ring, ring, S - 1 - ring, S - 1 - ring), fill=BLUE_BADGE)
    bar = max(2 * scale, round(S * 0.083))
    cx = cy = (S - 1) / 2.0
    # Full-span white bars — continuous white out through the ring
    d.rectangle([cx - bar / 2, 0, cx + bar / 2, S - 1], fill=WHITE)
    d.rectangle([0, cy - bar / 2, S - 1, cy + bar / 2], fill=WHITE)
    # Clip everything to the outer circle
    mask = Image.new("L", (S, S), 0)
    ImageDraw.Draw(mask).ellipse((0, 0, S - 1, S - 1), fill=255)
    out = Image.new("RGBA", (S, S), T)
    out.paste(im, (0, 0), mask)
    return out.resize((size, size), Image.Resampling.LANCZOS)


def write_multi_ico(frames: dict[int, Image.Image], path: Path) -> None:
    entries: list[tuple[int, int, int]] = []
    blobs: list[bytes] = []
    for size in sorted(frames.keys()):
        buf = io.BytesIO()
        frames[size].convert("RGBA").save(buf, format="PNG")
        data = buf.getvalue()
        blobs.append(data)
        w = h = 0 if size >= 256 else size
        entries.append((w, h, len(data)))
    count = len(entries)
    offset = 6 + 16 * count
    out = bytearray(struct.pack("<HHH", 0, 1, count))
    for (w, h, sz), blob in zip(entries, blobs):
        out += struct.pack("<BBBBHHII", w, h, 0, 0, 1, 32, sz, offset)
        offset += sz
    for blob in blobs:
        out += blob
    path.write_bytes(out)


def main() -> None:
    BADGE_DIR.mkdir(parents=True, exist_ok=True)
    sizes = (1024, 512, 256, 128, 96, 64, 48, 32)
    by_size: dict[int, Image.Image] = {}
    for s in sizes:
        im = draw_badge(s)
        by_size[s] = im
        p = BADGE_DIR / f"OpenDCLBadge-{s}.png"
        im.save(p, optimize=True)
        print(f"Wrote {p} ({p.stat().st_size} bytes)")

    # Canonical master name (512)
    master = BADGE_DIR / "OpenDCLBadge.png"
    by_size[512].save(master, optimize=True)
    print(f"Wrote {master} (primary master)")

    ico_sizes = [16, 24, 32, 48, 64, 128, 256]
    write_multi_ico({s: draw_badge(s) for s in ico_sizes}, BADGE_DIR / "OpenDCLBadge.ico")
    print(f"Wrote {BADGE_DIR / 'OpenDCLBadge.ico'}")


if __name__ == "__main__":
    main()
