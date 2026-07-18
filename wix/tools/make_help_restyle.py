"""Restyle Help book+? to match License multi-tone Windows blue + white highlights.

Composites the pure high-res badge from wix/ui/icons/badge/OpenDCLBadge.png.
"""
from __future__ import annotations

import io
import struct
from pathlib import Path

from PIL import Image, ImageChops, ImageDraw, ImageFilter, ImageFont

ROOT = Path(r"P:\Work\OpenDCL\Source\opendcl\wix")
OUT_ICONS = ROOT / "ui" / "icons"
BADGE_MASTER = OUT_ICONS / "badge" / "OpenDCLBadge.png"

BLUE = (0, 120, 208, 255)
BLUE_DARK = (0, 90, 180, 255)
BLUE_DEEPER = (0, 72, 160, 255)
BLUE_LIGHT = (40, 148, 224, 255)
WHITE = (255, 255, 255, 255)
PAGE = (248, 248, 248, 255)
PAGE_EDGE = (220, 228, 236, 255)
T = (0, 0, 0, 0)

_BADGE_CACHE: Image.Image | None = None


def load_badge_master() -> Image.Image:
    global _BADGE_CACHE
    if _BADGE_CACHE is None:
        if not BADGE_MASTER.is_file():
            raise SystemExit(
                f"Missing badge master: {BADGE_MASTER}\n"
                "Run: python wix/tools/make_badge.py"
            )
        _BADGE_CACHE = Image.open(BADGE_MASTER).convert("RGBA")
    return _BADGE_CACHE


def get_badge(size: int) -> Image.Image:
    return load_badge_master().resize((size, size), Image.Resampling.LANCZOS)


def draw_help_book(size: int = 256) -> Image.Image:
    im = Image.new("RGBA", (size, size), T)

    def sc0(v: float) -> int:
        return int(round(v * size / 256.0))

    left, top = sc0(28), sc0(18)
    right, bottom = sc0(228), sc0(228)
    rad = max(2, sc0(22))
    spine_w = max(2, sc0(18))
    page_h = max(2, sc0(14))

    shadow = Image.new("RGBA", (size, size), T)
    ImageDraw.Draw(shadow).rounded_rectangle(
        (left + sc0(4), top + sc0(8), right + sc0(4), bottom + sc0(6)),
        radius=rad,
        fill=(0, 0, 0, 50),
    )
    shadow = shadow.filter(ImageFilter.GaussianBlur(radius=max(1, sc0(5))))
    im = Image.alpha_composite(im, shadow)
    d = ImageDraw.Draw(im)

    page_top = bottom - page_h
    d.rounded_rectangle(
        (left + sc0(6), page_top - sc0(4), right - sc0(4), bottom + sc0(2)),
        radius=max(2, sc0(10)),
        fill=PAGE,
    )
    for i, yoff in enumerate((sc0(2), sc0(6), sc0(10))):
        y = page_top + yoff
        if y < bottom - 1:
            d.line(
                (left + sc0(14), y, right - sc0(14), y),
                fill=PAGE_EDGE if i else BLUE_DARK,
                width=max(1, sc0(2)),
            )

    cover_bottom = bottom - page_h + sc0(4)
    d.rounded_rectangle((left, top, right, cover_bottom), radius=rad, fill=BLUE)

    cover_mask = Image.new("L", (size, size), 0)
    ImageDraw.Draw(cover_mask).rounded_rectangle(
        (left, top, right, cover_bottom), radius=rad, fill=255
    )
    spine = Image.new("RGBA", (size, size), T)
    ImageDraw.Draw(spine).rounded_rectangle(
        (left, top, right, cover_bottom), radius=rad, fill=BLUE_DARK
    )
    sm = Image.new("L", (size, size), 0)
    ImageDraw.Draw(sm).rectangle(
        (left, top, left + spine_w + sc0(1), cover_bottom), fill=255
    )
    sm = ImageChops.multiply(cover_mask, sm)
    spine.putalpha(sm)
    im = Image.alpha_composite(im, spine)
    d = ImageDraw.Draw(im)

    if size >= 24:
        d.line(
            (left + spine_w, top + sc0(10), left + spine_w, cover_bottom - sc0(10)),
            fill=(255, 255, 255, 70),
            width=max(1, sc0(2)),
        )
        hi = Image.new("RGBA", (size, size), T)
        band_h = sc0(48)
        ImageDraw.Draw(hi).rounded_rectangle(
            (left, top, right, top + band_h + rad), radius=rad, fill=BLUE_LIGHT
        )
        fade = Image.new("L", (size, size), 0)
        for y in range(top, min(size, top + band_h + rad)):
            t = (y - top) / max(1, band_h + rad)
            a = int(max(0, 110 * (1.0 - t) ** 1.6))
            ImageDraw.Draw(fade).line((0, y, size, y), fill=a)
        hi_a = ImageChops.multiply(ImageChops.multiply(hi.split()[3], fade), cover_mask)
        hi.putalpha(hi_a)
        im = Image.alpha_composite(im, hi)
        d = ImageDraw.Draw(im)
        shade = Image.new("RGBA", (size, size), T)
        ImageDraw.Draw(shade).rounded_rectangle(
            (right - sc0(36), top + sc0(20), right - sc0(2), cover_bottom - sc0(8)),
            radius=max(2, sc0(8)),
            fill=BLUE_DEEPER,
        )
        sa = ImageChops.multiply(shade.split()[3].point(lambda p: int(p * 0.22)), cover_mask)
        shade.putalpha(sa)
        im = Image.alpha_composite(im, shade)
        d = ImageDraw.Draw(im)

    if size >= 32:
        for i in range(sc0(6)):
            a = 40 - i * 5
            if a > 0:
                d.line(
                    (
                        left + sc0(8),
                        cover_bottom - sc0(4) - i,
                        right - sc0(8),
                        cover_bottom - sc0(4) - i,
                    ),
                    fill=(0, 60, 130, a),
                    width=1,
                )

    q_cx = (left + right) // 2 + sc0(4)
    q_cy = top + sc0(100)
    fsz = sc0(140) if size >= 48 else max(8, int(size * 0.55))
    font = None
    for fp in (r"C:\Windows\Fonts\segoeuib.ttf", r"C:\Windows\Fonts\arialbd.ttf"):
        try:
            font = ImageFont.truetype(fp, fsz)
            break
        except OSError:
            pass
    if font is None:
        font = ImageFont.load_default()
    bbox = d.textbbox((0, 0), "?", font=font)
    tw, th = bbox[2] - bbox[0], bbox[3] - bbox[1]
    tx = q_cx - tw // 2 - bbox[0]
    ty = q_cy - th // 2 - bbox[1] - sc0(6)
    if size >= 32:
        d.text((tx + sc0(2), ty + sc0(3)), "?", font=font, fill=(0, 60, 120, 55))
    d.text((tx, ty), "?", font=font, fill=WHITE)

    if size >= 48:
        rim = Image.new("RGBA", (size, size), T)
        ImageDraw.Draw(rim).arc(
            (left + sc0(2), top + sc0(2), right - sc0(2), top + sc0(40)),
            200,
            340,
            fill=(255, 255, 255, 90),
            width=max(1, sc0(3)),
        )
        im = Image.alpha_composite(im, rim)

    if size >= 20:
        # Match License badge scale (~80px on 256 => 0.3125); slightly larger only at tiny sizes for legibility
        bs = max(8, int(round(size * (0.34 if size <= 24 else 0.3125))))
        badge = get_badge(bs)
        # License sits ~5-11px from edges on 256; use ~0.035 (~9px) for similar inset
        pad = max(1, int(round(size * 0.035)))
        bx, by = size - bs - pad, size - bs - pad
        if size >= 32:
            bsh = Image.new("RGBA", (size, size), T)
            ImageDraw.Draw(bsh).ellipse(
                (bx + 1, by + 2, bx + bs - 1, by + bs - 1), fill=(0, 0, 0, 65)
            )
            bsh = bsh.filter(ImageFilter.GaussianBlur(radius=max(1, size // 40)))
            im = Image.alpha_composite(im, bsh)
        im.paste(badge, (bx, by), badge)
    return im


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
    OUT_ICONS.mkdir(parents=True, exist_ok=True)
    sizes = [16, 24, 32, 48, 64, 128, 256]
    frames = {s: draw_help_book(s) for s in sizes}
    write_multi_ico(frames, OUT_ICONS / "OpenDCLHelp.ico")
    print("Wrote", OUT_ICONS / "OpenDCLHelp.ico")


if __name__ == "__main__":
    main()
