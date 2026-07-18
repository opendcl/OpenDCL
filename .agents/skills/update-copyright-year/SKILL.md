---
name: update-copyright-year
description: >
  Update OpenDCL copyright notice years across resource files, help content,
  and license text (packaged by WiX from those sources). Use for annual
  copyright rollover, LegalCopyright updates, or when the user runs
  /update-copyright-year. Triggers: "copyright year", "update copyright",
  "change copyright", "copyright 20xx", "LegalCopyright".
---

# Update Copyright Year

Annual (or ad-hoc) rollover of OpenDCL Consortium copyright years. Derived from
historical commits such as `46d6b4cf` (2022→2023), `c516466a` (→2024),
`e1bbd787` (→2025), and `711ac77b` (→2026).

This skill updates **year notices only**. Product `FILEVERSION` changes belong
to `/bump-version`. In practice, copyright commits in this repo almost always
also bump the product patch/build — ask whether to combine both.

**WiX packaging note:** Studio/Runtime MSIs ship license and help content from
this tree. Keep source years current in:

- `Studio/Localized/*/Content/License.txt` / `License.htm` / **`License.rtf`**
  (`License.rtf` is the checked-in WixUI EULA; do **not** regenerate it silently
  during package builds)
- `Runtime/Localized/*/Content/License.txt`
- Help HTML footers and `.rc` `LegalCopyright` as listed below

Do not hunt for copyright strings inside WiX `.wxs` templates or generated
`wix/out/gen` fragments.

## Inputs

1. **Old year** — detect from current sources (e.g. `2025`).
2. **New year** — default to the calendar year the user intends (often “this year”).
3. **Also bump version?** — historical default is yes (small build/patch bump). If
   yes, run `/bump-version` checklist in the same change set.

## Where years appear

### A. Version-info `LegalCopyright` in `.rc` files

Pattern (encoding of the copyright symbol varies by file):

```text
VALUE "LegalCopyright", "Copyright © YYYY OpenDCL Consortium.  All rights reserved."
```

Some localized resources use a plain `c` or other stand-in for ©. **Preserve the
exact character sequence** around the year; only change `YYYY`.

**Files:**

| Path | Notes |
|------|--------|
| `Runtime/ARX.rc` | Runtime module version resource |
| `Studio/Studio.rc` | Studio EXE version resource |
| `Runtime/RxInstall/RxInstall.rc` | Install helper copyright; product version must match via `/bump-version` |
| `Runtime/Localized/*/Runtime.Res/Runtime.*.rc` | CHS, CHT, DEU, ENU, ESM, FRA, RUS |
| `Studio/Localized/*/Studio.Res/Studio.*.rc` | Same languages |

### B. Studio HTML help footers

Nearly every topic under:

```text
Studio/Localized/<LANG>/Content/**/*.htm
```

Footer pattern (see `@Template.htm`):

```html
<p>Copyright © YYYY <a href="http://www.opendcl.com" ...>OpenDCL Consortium</a>.
```

Update **all** languages: CHS, CHT, DEU, ENU, ESM, FRA, RUS.

Historical pure-copyright commits touch **thousands** of `.htm` files. Prefer a
careful bulk replace limited to the copyright footer / license year patterns so
unrelated numeric years (e.g. AutoCAD product years in prose) are not damaged.

### C. License end-year ranges

Patterns:

```text
Copyright (C) 2007 - YYYY OpenDCL Consortium
```

**Files (all must stay current):**

- `Studio/Localized/*/Content/License.htm`
- `Studio/Localized/*/Content/License.txt`
- `Studio/Localized/*/Content/License.rtf` (installer EULA for WiX)
- `Runtime/Localized/*/Content/License.txt`

Keep the start year `2007` unless the user says otherwise. Only advance the end year.
Runtime and Studio license end-years must both match the new copyright year.

**Encoding:** Runtime `License.txt` files are **UTF-16 LE (BOM `FF FE`)**. Preserve
that encoding when editing; a naïve rewrite can corrupt size/encoding. Studio
`License.txt` / `License.htm` / `License.rtf` may use different encodings — detect
before writing.

### D. Do **not** change

- Third-party copyrights (LibPNG, zlib, ColourPicker “Copyright (c) 1998”, etc.)
- Repo root `LICENSE` GPLv2 boilerplate years that are part of FSF text
- CAD host product years mentioned in release notes or help prose
  (“AutoCAD 2027”, “BricsCAD V26”) unless they are clearly OpenDCL copyright lines
- Binary/obj outputs under `Release/`, `Debug/`, `.vs/`

## Workflow

### 1. Inventory old year

Search for the current copyright year in:

```text
LegalCopyright
Copyright ©
Copyright (C) 2007 -
```

Summarize hits by area (Runtime RC, Studio RC, RxInstall, help HTML, License.*).

### 2. Replace carefully

For each class of file, replace only the OpenDCL copyright year:

| Location | Old → New example |
|----------|-------------------|
| `LegalCopyright` | `2025` → `2026` inside the OpenDCL Consortium string |
| Help footers | `Copyright © 2025` → `Copyright © 2026` |
| License range | `2007 - 2025` → `2007 - 2026` |

Prefer encoding-safe edits (read file with the correct encoding, replace year tokens
in known phrases only, write back with the same encoding). Avoid blind global
replace of bare years across the whole tree.

### 3. Optional version bump

If combining with a release (historical norm):

1. Decide new product version with the user (often +1 build or +1 patch).
2. Follow `/bump-version` fully (RC resources + WiX `ProductVersion` / `ModuleVersion` defaults in `scripts/build-wix.ps1`).

### 4. Verify

1. No remaining OpenDCL `LegalCopyright` lines with the old year.
2. No remaining help footers with the old year.
3. No remaining `2007 - <old year>` license ranges under **both** Studio and Runtime
   Localized content (`License.htm`, `License.txt`, and Studio `License.rtf` for every
   language).
4. Spot-check one language pack end-to-end (ENU + one non-ENU), including file encoding.
5. Confirm third-party copyrights untouched.

### 5. Capture lessons into this skill

Before finishing, update **this** `SKILL.md` with anything newly learned while
performing the copyright rollover, for example:

- File classes or languages that still had the old year
- Encoding / © symbol pitfalls in `.rc` or `.htm` files
- Safer bulk-replace patterns that avoided false positives (host product years, etc.)
- Problems encountered and the solution that resolved them

Write durable guidance; remove or reword outdated instructions instead of leaving
contradictions. If nothing new was learned, skip edits to the skill file.

### 6. Commit message style

```text
OpenDCL A.B.C.D
- Copyright dates changed from YYYY to ZZZZ.
```

or when mixed with other work:

```text
OpenDCL A.B.C.D
- Initial support for GStarcad 2027.
- ...
- Copyright year updated to ZZZZ.
```

Only commit if the user asked.

## Skeleton / follow-ups

- [ ] Safe scripted bulk update for help `.htm` footers (all languages)
- [ ] Checklist item for any remaining binary `.rc` edge cases / code-page issues
- [ ] Whether root README or website copy lives outside this repo

## Historical reference commits

| Commit | Year change | Notes |
|--------|-------------|-------|
| `46d6b4cf` | 2022→2023 | Focused copyright + small version bump; large help tree |
| `c516466a` | →2024 | Combined with GstarCAD 2024 experimental support |
| `e1bbd787` | 2024→2025 | Combined with Acad 2026 + CHT language |
| `711ac77b` | →2026 | Combined with GstarCAD 2027 + version 9.3.3.1 |
| `ea6296b0` | →2022 | Earlier annual update pattern |
