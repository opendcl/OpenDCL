#!/usr/bin/env python3
"""Fail if tracked text files mix CR/LF (or contain lone CR).

Looks at git blobs, not the working tree, so core.autocrlf / checkout
conversion cannot hide mixed endings. Does not require every file to
be LF or CRLF — only that one file uses one convention.

Usage:
  python scripts/check-eol.py              # all tracked text files
  python scripts/check-eol.py --since-ref origin/main
"""

from __future__ import annotations

import argparse
import os
import subprocess
import sys

TEXT_EXT = {
    ".c",
    ".cc",
    ".cmake",
    ".cpp",
    ".cs",
    ".css",
    ".cxx",
    ".def",
    ".filters",
    ".h",
    ".hpp",
    ".hxx",
    ".inl",
    ".js",
    ".json",
    ".md",
    ".props",
    ".ps1",
    ".py",
    ".rc",
    ".rhai",
    ".rthlog",
    ".sln",
    ".ts",
    ".txt",
    ".vcxproj",
    ".xml",
    ".yaml",
    ".yml",
}

# Path suffixes that are text even without a listed ext.
TEXT_NAMES = {
    ".gitattributes",
    ".gitignore",
    "CMakeLists.txt",
    "LICENSE",
}

SKIP_EXT = {
    ".arx",
    ".bmp",
    ".brx",
    ".cur",
    ".dll",
    ".exe",
    ".gif",
    ".ico",
    ".jpg",
    ".jpeg",
    ".lib",
    ".odc",
    ".odce",
    ".odcl",
    ".pdf",
    ".png",
    ".sld",
    ".ttf",
    ".woff",
    ".zip",
}


def git_root() -> str:
    here = os.path.dirname(os.path.abspath(__file__))
    return subprocess.check_output(
        ["git", "-C", os.path.dirname(here), "rev-parse", "--show-toplevel"],
        text=True,
    ).strip()


def git_output(args: list[str], cwd: str) -> bytes:
    return subprocess.check_output(["git", *args], cwd=cwd)


def list_files(since_ref: str | None, cwd: str) -> list[str]:
    if since_ref:
        out = git_output(
            ["diff", "--name-only", "--diff-filter=ACMRTUXB", since_ref],
            cwd,
        )
    else:
        out = git_output(["ls-files", "-z"], cwd)
        return [p.decode("utf-8", "surrogateescape") for p in out.split(b"\0") if p]
    return [
        p.decode("utf-8", "surrogateescape")
        for p in out.splitlines()
        if p
    ]


def is_text_path(path: str) -> bool:
    base = os.path.basename(path)
    if base in TEXT_NAMES:
        return True
    _, ext = os.path.splitext(path)
    ext = ext.lower()
    if ext in SKIP_EXT:
        return False
    if ext == ".filters" or path.endswith(".vcxproj.filters"):
        return True
    return ext in TEXT_EXT


def classify(data: bytes) -> tuple[int, int, int, bool]:
    crlf = lf = cr = 0
    i = 0
    n = len(data)
    binary = False
    while i < n:
        b = data[i]
        if b == 0:
            binary = True
            break
        if b == 13:
            if i + 1 < n and data[i + 1] == 10:
                crlf += 1
                i += 2
                continue
            cr += 1
        elif b == 10:
            lf += 1
        i += 1
    return crlf, lf, cr, binary


def cat_blobs(paths: list[str], cwd: str) -> dict[str, bytes]:
    if not paths:
        return {}
    payload = b"".join(
        b"HEAD:" + p.encode("utf-8", "surrogateescape") + b"\n" for p in paths
    )
    proc = subprocess.run(
        ["git", "cat-file", "--batch"],
        input=payload,
        capture_output=True,
        check=False,
        cwd=cwd,
    )
    out = proc.stdout
    result: dict[str, bytes] = {}
    i = 0
    idx = 0
    while i < len(out) and idx < len(paths):
        nl = out.find(b"\n", i)
        if nl < 0:
            break
        header = out[i:nl].decode("utf-8", "replace")
        i = nl + 1
        parts = header.split()
        if len(parts) >= 3 and parts[1] not in ("missing", "ambiguous"):
            size = int(parts[2])
            result[paths[idx]] = out[i : i + size]
            i += size
            if i < len(out) and out[i] == 10:
                i += 1
        idx += 1
    return result


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument(
        "--since-ref",
        help="only files changed vs this git ref (e.g. origin/main)",
    )
    args = ap.parse_args()

    root = git_root()
    os.chdir(root)

    files = [p.replace("\\", "/") for p in list_files(args.since_ref, root)]
    files = [p for p in files if is_text_path(p)]
    blobs = cat_blobs(files, root)

    bad: list[str] = []
    scanned = 0
    for path in files:
        data = None
        abs_path = os.path.join(root, path)
        if os.path.isfile(abs_path):
            with open(abs_path, "rb") as fh:
                data = fh.read()
        elif path in blobs:
            data = blobs[path]
        if data is None:
            continue
        crlf, lf, cr, binary = classify(data)
        if binary:
            continue
        scanned += 1
        mixed = (crlf > 0 and lf > 0) or (cr > 0 and (crlf > 0 or lf > 0))
        if mixed:
            bad.append(f"{path}  crlf={crlf} lf={lf} cr={cr}")

    scope = f"since {args.since_ref}" if args.since_ref else "all tracked"
    print(f"eol: scanned {scanned} text file(s) ({scope})")
    if bad:
        print(f"eol: {len(bad)} file(s) mix line endings:", file=sys.stderr)
        for line in bad:
            print(f"  {line}", file=sys.stderr)
        print(
            "Normalize to one ending per file (VS *.vcxproj / *.filters / *.sln: CRLF).",
            file=sys.stderr,
        )
        return 1
    print("eol: ok (no mixed CR/LF)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
