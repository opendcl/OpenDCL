---
name: sync-help-to-website
description: >
  Propagate Studio localized help HTML into the OpenDCL GitHub Pages site
  (opendcl.github.io HelpFiles). Use when updating online help after Content
  edits, publishing a new help language, or when the user runs
  /sync-help-to-website. Triggers: "sync help", "update website help",
  "HelpFiles", "opendcl.github.io", "online help", "publish help".
---

# Sync Help Content to Website

Copy **Studio HTML help topics** from the OpenDCL product tree into the sibling
**GitHub Pages** repo so https://opendcl.github.io/HelpFiles/ stays current.

| Role | Path |
|------|------|
| **Source** | `<opendcl>/Studio/Localized/<LANG>/Content/` |
| **Destination** | `<opendcl.github.io>/HelpFiles/<LANG>/` |
| **Site repo** | Sibling of the product repo: `../opendcl.github.io` (any workspace root) |

Product packaging (CHM / WiX) is unchanged by this skill. Installer help remains
`OpenDCL.chm` built from the same Content tree.

## What the site hosts today

Per `opendcl.github.io/README.md`, online help folders currently include at least:

- `HelpFiles/ENU`
- `HelpFiles/DEU`
- `HelpFiles/RUS`

Studio may have additional languages (CHS, CHT, ESM, FRA, …) that are **not**
automatically on the website. Adding a new HelpFiles language is allowed when
the user wants it online — update the frame language allowlist (below).

Download MSI language links are separate (`download/`, `assets/versions.js`).

## What to copy vs omit

### Copy from Studio `Content/` into `HelpFiles/<LANG>/`

- All topic HTML under `Intro/`, `Concepts/`, `Usage/`, `Advanced/`, `Reference/`
- Root help pages used online: `Index.htm`, `License.htm` (and any other root
  `.htm` that is part of the doc set)
- `Utility.js`, `WinHelp.css`
- `Reference/images/` and other topic-referenced assets

Preserve relative paths so in-help links keep working.

### Do **not** copy (product/installer only)

| Omit | Why |
|------|-----|
| `Samples/**` | Large binaries / sample projects; not part of online help |
| `OpenDCL.chm` | Compiled help; gitignored in product tree |
| `OpenDCL.hhp`, `OpenDCL.hhc` | HTML Help Workshop project/TOC for CHM |
| `License.txt`, `License.rtf` | Installer / plain license; site uses `License.htm` |
| `GNU-GPL.txt` | Not used by the framed help UI |
| `@Template.htm` | Authoring template, not a published topic |

### Preserve website-only chrome (do not delete)

Each published language folder has site-specific files **not** present in Studio:

| File | Role |
|------|------|
| `TOC.html` | Framed table of contents for the web UI |
| `Top.html` | Top banner / language flags for the web UI |

When replacing tree contents, **keep** these files (or regenerate deliberately).
A safe pattern: sync into a temp dir, then copy over while excluding chrome, or
restore `TOC.html` / `Top.html` from git after a bulk copy.

Shared frame assets live in `HelpFiles/` root (not per-language):

- `index.html` (language + page query string; default ENU/DEU/RUS)
- `Framer.js`, `TOC.js`, `TOC.css`, `Top.css`
- `flags/*`

## Workflow

### 1. Locate repos

```text
<workspace>/opendcl/                 # product (this repo)
<workspace>/opendcl.github.io/       # GitHub Pages (sibling clone)
```

Resolve with `../opendcl.github.io` from the product root when using the usual
sibling layout. If the website path differs, ask the user. Work in a clean git
state on the website repo when possible (`git status`).

### 2. Choose languages

- Default: languages that already exist under `HelpFiles/` **and** have Studio
  Content (ENU, DEU, RUS).
- Or explicit list from the user (e.g. only ENU after an English doc edit).
- For a **new** online language: create `HelpFiles/<LANG>/`, seed `TOC.html` /
  `Top.html` from ENU (adjust `Content-Language` / flags), then sync topics.

### 3. Sync files

Example (one language, PowerShell):

```powershell
# Sibling layout (adjust if your clones differ)
$srcRoot = (Resolve-Path ".").Path                       # product repo root
$webRoot = (Resolve-Path "..\opendcl.github.io").Path    # GitHub Pages sibling
$lang = "ENU"
$src  = Join-Path $srcRoot "Studio\Localized\$lang\Content"
$dst  = Join-Path $webRoot "HelpFiles\$lang"

# Backup website chrome
Copy-Item "$dst\TOC.html","$dst\Top.html" $env:TEMP -Force

# Mirror HTML/css/js/images; exclude installer/CHM/samples
# (robocopy exit codes 0–7 are success)
robocopy $src $dst /MIR /XD Samples /XF `
  OpenDCL.chm OpenDCL.hhp OpenDCL.hhc `
  License.txt License.rtf GNU-GPL.txt `
  @Template.htm `
  TOC.html Top.html

# Restore chrome if robocopy removed them
Copy-Item "$env:TEMP\TOC.html","$env:TEMP\Top.html" $dst -Force
```

`/MIR` deletes dest files removed from source — that is usually desired for
topics, which is why chrome must be excluded or restored.

Prefer encoding-preserving tools; do not re-save all HTML through an editor that
changes UTF-8/BOM or line endings site-wide without need.

### 4. Update frame language routing (when set of online langs changes)

`HelpFiles/index.html`:

- `getClientLanguage()` maps browser language → folder code.
- Hard-coded allowlist (today): only `ENU`, `DEU`, `RUS` are accepted; anything
  else falls back to ENU.

When publishing a new HelpFiles language, extend:

1. `getClientLanguage()` if automatic detection should apply.
2. The allowlist check (`language !== "ENU" && …`).
3. Optional: `Top.html` flag strip / `changeLang(...)` targets for that language
   (folder codes like `ENU`/`DEU`/`RUS` vs Google-style codes in older flag links).

### 5. Sanity checks

1. Spot-check a few topics in a browser via `HelpFiles/index.html?lang=ENU&page=…`
   or open `HelpFiles/<LANG>/Index.htm`.
2. Confirm `TOC.html` / `Top.html` still present.
3. Confirm `Samples` was not published.
4. `git status` in `opendcl.github.io` — review diff size (often many HTML files).
5. No product-tree secrets or installer binaries staged.

### 6. Commit (website repo only)

Only if the user asked to commit:

```text
Update online help (<LANG list>) from Studio Content.
```

or

```text
Sync ENU help topics to HelpFiles.
```

Product repo and website repo are **separate** git histories — do not mix commits.

### 7. Capture lessons into this skill

Record new languages on the site, robocopy edge cases, or TOC generation notes.
Remove outdated allowlists if the site’s published set changes.

## Out of scope

- Editing Studio help content itself (do that in `Studio/Localized/.../Content`).
- Building `OpenDCL.chm` or running WiX (`scripts/build-wix.ps1`).
- Adding a full product language pack (`/add-language`).
- Changing download MSI version tables unless the user also asks.
- Force-pushing or deploying beyond normal commits to `opendcl.github.io`
  (Pages usually publishes from the default branch).

## Quick verification checklist

- [ ] Correct source `Studio/Localized/<LANG>/Content` and dest `HelpFiles/<LANG>`
- [ ] Topics/assets copied; Samples/CHM/hhp/license extras omitted
- [ ] `TOC.html` and `Top.html` preserved
- [ ] `index.html` allowlist updated if a new online language was added
- [ ] Local preview looks correct
- [ ] Website git diff reviewed; commit only if requested
