---
name: code-sign-release
description: >
  Authenticode-sign OpenDCL installer packages (YubiKey / SSL.com + jsign),
  assemble versioned dist folders, and publish GitHub Releases. Use for
  jsign, code signing, sign msi/msm, or when the user runs
  /code-sign-release. Triggers: "code sign", "jsign", "signtool", "YubiKey",
  "sign installers", "Authenticode", "SSL.com", "release assets".
---

# Code Sign and Release

Public product skill. **Operator machine details** (YubiKey setup notes, local
site paths) live in private **`opendcl/build-lab`** skill `code-sign-operator`
— do not put personal paths, PINs, or certificate fingerprints in this public
tree.

## Production signing (summary)

**SSL.com code signing certificate on a YubiKey** + **jsign** (`--storetype YUBIKEY`).

| Item | Value |
|------|--------|
| Tool | `jsign` (Chocolatey / PATH; requires Java + Yubico PIV Tool ykcs11) |
| Store type | `YUBIKEY` (override with `SIGN_STORE_TYPE`) |
| Private key | On YubiKey (not exportable) |
| Timestamp | RFC3161 `http://ts.ssl.com` (override with `SIGN_TIMESTAMP_URL`) |
| Description URL | `https://www.opendcl.com` (override with `SIGN_DESCRIPTION_URL`) |
| PIN | **Env only** — `SIGN_STORE_PASSWORD` (also `SIGN_PIN`, `YUBIKEY_PIN`) |

**PIN:** Set in the process environment; `sign-files.ps1` passes
`--storepass env:VARNAME` so the PIN never appears on the command line.
Never put the PIN in scripts, files committed to git, or CI logs.

If the token has multiple certificates, set `SIGN_CERT_ALIAS` / `-Alias`
(jsign lists aliases when required).

### Sign packages

```powershell
# Preferred: PIN in process env (user scope — not committed)
$env:SIGN_STORE_PASSWORD = "<yubikey-pin>"

.\scripts\sign-files.ps1 -Path .\dist\10.1.1.1

# Optional alias when multiple certs on the token:
.\scripts\sign-files.ps1 -Path .\dist\10.1.1.1 `
  -Alias "X.509 Certificate for Digital Signature" `
  -TimestampUrl "http://ts.ssl.com"
```

`sign-files.ps1` defaults:

- Tool: jsign (not signtool)
- Timestamp: `http://ts.ssl.com` (RFC3161)
- Description: `https://www.opendcl.com`
- Directory scan: `*.msi`, `*.msm` only (`-IncludeBinaries` for exe/dll)
- Verifies each file with `Get-AuthenticodeSignature` unless `-SkipVerify`

### Full release folder (installers + localization zips + optional sign)

```powershell
$env:SIGN_STORE_PASSWORD = "<yubikey-pin>"
.\scripts\make-release.ps1 -ProductVersion 10.1.1.1 -Sign
```

Localization `.zip` files are **not** Authenticode-signed (only MSI/MSM).

Then publish (public ship only — not dry-run):

```powershell
# Product tag: installers only. Do not attach OpenDCL.<LANG>.zip here.
gh release create "v10.1.1.1" .\dist\10.1.1.1\*.msi .\dist\10.1.1.1\*.msm `
  --title "OpenDCL 10.1.1.1"
# Translator packs: rolling tag only (see localization-packs.yml / Make release)
# gh release create localization-packs .\dist\10.1.1.1\OpenDCL.*.zip `
#   --title "Localization packs (latest)" --latest=false
```

Or run the **Make release** workflow (self-hosted) with `sign=true`. PIN must be
available as host env `SIGN_STORE_PASSWORD` on the runner (not in this skill file).
Product `v*` assets are MSI/MSM only; language zips refresh **localization-packs**.

## What to sign

**Full sign** (`make-release.ps1 -Sign` or build-lab package `sign=true`):

1. **Ship PE before WiX** — under product `out\` (CMake) or `Runtime`/`Studio` (classic):  
   `*.exe`, `*.dll`, host modules `*.arx` / `*.brx` / `*.grx` / `*.zrx` / `*.dbx`  
   (`sign-files.ps1 -IncludeBinaries -Recurse`).
2. **Installer containers after package** — `*.msi`, `*.msm` in `dist\` (and WiX out).

Localization `.zip` files are not Authenticode-signed.

Installer-only escape hatch: `make-release.ps1 -Sign -SkipBinarySign`.

## Scripts map

| Script | Role |
|--------|------|
| `scripts/sign-files.ps1` | YubiKey / PKCS#12 signing via jsign (MSI/MSM and/or PE) |
| `scripts/make-dist.ps1` | Versioned MSI/MSM names |
| `scripts/make-localization-zips.ps1` | Translator zips |
| `scripts/make-release.ps1` | verify → [sign PE] → WiX → dist → loc → [sign MSI] |
| `scripts/build-wix.ps1` | WiX MSM/MSI build |
| `scripts/update-site-versions.ps1` | After public Release: Pages `version/*.txt` + `versions.js` |

## GitHub Actions

| Workflow | Role |
|----------|------|
| `.github/workflows/package.yml` | WiX + dist + localization artifacts |
| `.github/workflows/release.yml` (**Make release**) | Full pipeline + sign + `gh release` |
| `.github/workflows/localization-packs.yml` | Loc zips only (rolling tag) |

**Runner:** self-hosted Windows with the signing token available when signing.
GitHub-hosted runners are not suitable for interactive YubiKey PIN signing.

**Secret / env:** `SIGN_STORE_PASSWORD` (YubiKey PIN) on the self-hosted
runner process — not in the public repo. Prefer interactive or machine-local
env over GitHub secrets for the PIN.

## eSigner (not default)

SSL.com eSigner / Cloud Key Adapter can automate signing without a PIN prompt.
**Not** the current production path unless the user explicitly switches.

## Historical mapping

| Legacy | Replacement |
|--------|-------------|
| `#Sign.bat` (SafeNet eToken + Sectigo) | `sign-files.ps1` + jsign + YubiKey PIN env |
| `@SignAll.bat` / signtool + thumbprint | `sign-files.ps1` (jsign `--storetype YUBIKEY`) |
| `!MakeNewDist.bat` | `make-dist.ps1` / `make-release.ps1` |
| `!MakeLocalizationZips.bat` | `make-localization-zips.ps1` |

## Manual post-release: website update-check versions (**do not skip**)

Update-check is **not** covered by WiX, `make-release`, or GitHub Releases.
After every new **stable** or **development** ship, update plain-text version
files (and the download JS) in **`opendcl/opendcl.github.io`** — custom domain
`opendcl.com` / `www.opendcl.com` points at that Pages site.

### Clients (current)

| Client | Method | URL / path | Status on GitHub Pages |
|--------|--------|------------|-------------------------|
| Runtime `UpdateCheck.cpp` (new builds) | HTTPS **GET** + local compare | `/version/version.txt` or `version_dev.txt` | **Works** (static plain `A.B.C.D`; HTTP 2xx required) |
| Studio sample `*ODCL:UpdateCheck` | HTTPS **GET** | same | **Works** |
| Studio sample **Update** (`open_downloads_page`) | Browser | `/go?studio…` → GitHub Release MSI | **Works** (JS redirect; meta refresh → `/download/` only) |
| **Legacy** Runtime (pre-client-GET builds) | HTTP **POST** | `/version/vercheck.php` | **Broken** on Pages (no PHP) |

| Build flavor | `productName` | Version file |
|--------------|---------------|--------------|
| **Release** (shipping) | `OpenDCL Runtime` | `version/version.txt` |
| **Dev / non-release** | `OpenDCL Runtime Dev` | `version/version_dev.txt` |

Tray notification **Action** opens `https://www.opendcl.com/download/`.

### Site versions (after public Release — not dry-run)

Use product script **`scripts/update-site-versions.ps1`** against sibling
`../opendcl.github.io` (or `-SiteRoot`). It keeps the three files in sync:

| Path | Field |
|------|--------|
| `version/version.txt` | stable |
| `version/version_dev.txt` | current / dev |
| `assets/versions.js` | `stable` / `current` |

**Normal public ship** (new build is almost always **dev/current** only):

```powershell
# After gh release create v10.1.1.1 (or Make release)
.\scripts\update-site-versions.ps1 -DevVersion 10.1.1.1 -RequireRelease -GitCommit
# then: git -C ..\opendcl.github.io push
```

**Promote that shipped dev to stable** (usual stable release — same version, no new MSI):

```powershell
.\scripts\update-site-versions.ps1 -PromoteToStable -RequireRelease -GitCommit
```

**Exceptions** (rare): `-StableVersion` and/or both channels explicitly.

Never run this during private dry-run. Operator notes: private build-lab
`code-sign-operator`.

### Release checklist (append)

- [ ] Installers built, signed, GitHub Release `vA.B.C.D` published  
- [ ] Localization packs refreshed if needed  
- [ ] **Site versions:** `-DevVersion` for normal ship, or `-PromoteToStable` when promoting  
- [ ] **`opendcl.github.io` committed and pushed;** GET `version/*.txt` is bare `A.B.C.D`  

## Security checklist

- [ ] PIN only in process env (`SIGN_STORE_PASSWORD`), never in git or scripts  
- [ ] jsign + Yubico PIV Tool available on the signing machine  
- [ ] `Get-AuthenticodeSignature` Status=Valid on every shipped MSI/MSM  
- [ ] Ship PE under `out\` also Valid when full `-Sign` was used  
- [ ] Timestamp present  
- [ ] **opendcl.github.io `version/*.txt` + `assets/versions.js` updated for this release**  

## Capture lessons

If the signing cert is rekeyed/renewed, update **machine env / alias notes**
and the private build-lab operator skill — not the PIN, and not hard-coded
values in this public skill.
