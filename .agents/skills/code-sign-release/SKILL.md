---
name: code-sign-release
description: >
  Authenticode-sign OpenDCL installer packages (YubiKey / SSL.com + signtool),
  assemble versioned dist folders, and publish GitHub Releases. Use for
  signtool, code signing, sign msi/msm, or when the user runs
  /code-sign-release. Triggers: "code sign", "signtool", "YubiKey", "sign
  installers", "Authenticode", "SSL.com", "release assets".
---

# Code Sign and Release

Public product skill. **Operator machine details** (cert thumbprint, YubiKey
setup notes, local site paths) live in private **`opendcl/build-lab`** skill
`code-sign-operator` — do not put personal paths or certificate fingerprints
in this public tree.

## Production signing (summary)

**SSL.com code signing certificate on a YubiKey** + Windows cert store + `signtool`.

| Item | Value |
|------|--------|
| Store | Current User (or Local Machine) → Personal |
| Private key | On YubiKey via Smart Card KSP (not exportable) |
| Timestamp | RFC3161 `http://ts.ssl.com` (override with `SIGN_TIMESTAMP_URL`) |
| Description URL | `https://www.opendcl.com` (override with `SIGN_DESCRIPTION_URL`) |
| Thumbprint | **Env / secret only** — `SIGN_CERT_THUMBPRINT` or `-CertThumbprint` |

**PIN:** Enter the YubiKey PIN when Windows prompts. Never put the PIN in
scripts, env files committed to git, or CI logs.

If multiple code-signing certs are present, always pass an explicit thumbprint
so the wrong cert is not selected. Prefer auto-detect only when a single
SSL.com code-signing cert with a private key is in the store
(`sign-files.ps1` does this when thumbprint is omitted).

### Sign packages

```powershell
# Preferred: thumbprint from environment (user/machine scope — not committed)
$env:SIGN_CERT_THUMBPRINT = "<sha1-thumbprint-from-cert-store>"

.\scripts\sign-files.ps1 -Path .\dist\10.1.1.1

# Or explicit:
.\scripts\sign-files.ps1 -Path .\dist\10.1.1.1 `
  -CertThumbprint "<sha1-thumbprint>" `
  -TimestampUrl "http://ts.ssl.com"
```

`sign-files.ps1` defaults:

- Timestamp: `http://ts.ssl.com`
- Description: `https://www.opendcl.com`
- Directory scan: `*.msi`, `*.msm` only (`-IncludeBinaries` for exe/dll)
- Verifies each file with `signtool verify /pa` unless `-SkipVerify`

### Full release folder (installers + localization zips + optional sign)

```powershell
.\scripts\make-release.ps1 -ProductVersion 10.1.1.1 -Sign
# Optional: -CertThumbprint "<sha1>" if not using SIGN_CERT_THUMBPRINT
```

Localization `.zip` files are **not** Authenticode-signed (only MSI/MSM).

Then publish (public ship only — not dry-run):

```powershell
gh release create "v10.1.1.1" .\dist\10.1.1.1\* --title "OpenDCL 10.1.1.1"
```

Or run the **Make release** workflow (self-hosted), with secret
`SIGN_CERT_THUMBPRINT` set on the runner/repo (not in this skill file).

## What to sign

Historical practice: **installer packages only** (`*.msi`, `*.msm`).

Optional: sign PE modules before WiX if the user asks (`-IncludeBinaries`).

## Scripts map

| Script | Role |
|--------|------|
| `scripts/sign-files.ps1` | Thumbprint / store signing via signtool |
| `scripts/make-dist.ps1` | Versioned MSI/MSM names |
| `scripts/make-localization-zips.ps1` | Translator zips |
| `scripts/make-release.ps1` | package + dist + loc zips [+ sign] |
| `scripts/build-wix.ps1` | WiX MSM/MSI build |

## GitHub Actions

| Workflow | Role |
|----------|------|
| `.github/workflows/package.yml` | WiX + dist + localization artifacts |
| `.github/workflows/release.yml` (**Make release**) | Full pipeline + sign + `gh release` |
| `.github/workflows/localization-packs.yml` | Loc zips only (rolling tag) |

**Runner:** self-hosted Windows with the signing token available when signing.
GitHub-hosted runners are not suitable for interactive YubiKey PIN signing.

**Secret:** `SIGN_CERT_THUMBPRINT` (SHA1 of the code-signing cert).  
Do **not** store the YubiKey PIN in GitHub secrets unless using a future
eSigner automated flow.

## eSigner (not default)

SSL.com eSigner / Cloud Key Adapter can automate signing without a PIN prompt.
**Not** the current production path unless the user explicitly switches.

## Historical mapping

| Legacy | Replacement |
|--------|-------------|
| `#Sign.bat` (SafeNet eToken + Sectigo) | `sign-files.ps1` + YubiKey + interactive PIN |
| `@SignAll.bat` | `sign-files.ps1 -Path <dist-folder>` |
| `!MakeNewDist.bat` | `make-dist.ps1` / `make-release.ps1` |
| `!MakeLocalizationZips.bat` | `make-localization-zips.ps1` |

## Manual post-release: website update-check versions (**do not skip**)

The Runtime **check for updates** feature is **not** covered by WiX,
`make-release`, or GitHub Releases. After every new **stable** or
**development** ship, update the plain-text version files on **www.opendcl.com**.

### How the Runtime calls home

From `Runtime/UpdateCheck.cpp`:

- HTTP **POST** to host `opendcl.com`, path `/version/vercheck.php`
- Form fields: `productName`, `userVersion` (`A.B.C.D`), `language` (e.g. `ENU`)

Product names from `Runtime/acrxEntryPoint.cpp`:

| Build flavor | `productName` posted | Server file read by `vercheck.php` |
|--------------|----------------------|-------------------------------------|
| **Release** (shipping) | `OpenDCL Runtime` | `version/version.txt` |
| **Dev / non-release** | `OpenDCL Runtime Dev` | `version/version_dev.txt` |

### Manual update (until automated)

1. After the product release is published (installers signed + GitHub Release live).
2. On the **opendcl.com** web host (or the deploy mirror), edit:

   | File (under site `version/`) | When to update | Contents |
   |------------------------------|----------------|----------|
   | **`version.txt`** | Every **stable** release | Four-part version only, e.g. `10.1.1.1` |
   | **`version_dev.txt`** | Every **development** build clients should see as newest dev | Same format |

3. Deploy so these are reachable as:
   - `http://opendcl.com/version/version.txt`
   - `http://opendcl.com/version/version_dev.txt`

**Note:** `vercheck.php` may still reference legacy download URLs in the
notification Action. Updating **version numbers** is the required per-release
step; fixing Action URLs can be a separate website change.

Operator-local deploy paths for the site tree are documented only in private
**build-lab** `code-sign-operator` (not here).

### Release checklist (append)

- [ ] Installers built, signed, GitHub Release `vA.B.C.D` published  
- [ ] Localization packs refreshed if needed  
- [ ] **`version.txt` and/or `version_dev.txt` updated on opendcl.com**  
- [ ] Optional: `opendcl.github.io` `assets/versions.js` for the download page  

## Security checklist

- [ ] PIN never in git or scripts  
- [ ] Thumbprint only via env/secret (not hard-coded in public docs)  
- [ ] Broken old store entries not used for release  
- [ ] `signtool verify /pa` succeeds on every shipped MSI/MSM  
- [ ] Timestamp present  
- [ ] **opendcl.com `version.txt` / `version_dev.txt` updated for this release**  

## Capture lessons

If the signing cert is rekeyed/renewed, update **machine env / GitHub secrets**
and the private build-lab operator skill — not the PIN, and not hard-coded
values in this public skill.
