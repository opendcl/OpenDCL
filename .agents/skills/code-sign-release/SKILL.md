---
name: code-sign-release
description: >
  Authenticode-sign OpenDCL installer packages with SSL.com code signing on a
  YubiKey (PIN when prompted), version dist folders, and GitHub Releases. Use for
  signtool, YubiKey, code signing, sign msi/msm, or when the user runs
  /code-sign-release. Triggers: "code sign", "signtool", "YubiKey", "sign
  installers", "Authenticode", "SSL.com", "release assets".
---

# Code Sign and Release

## Production signing (current)

**SSL.com code signing certificate on a YubiKey** + Windows cert store + `signtool`.

| Item | Value |
|------|--------|
| Subject (example) | `CN=MANUSOFT, OU=Engineering, O=MANUSOFT, …` |
| Issuer | SSL.com Code Signing Intermediate CA ECC R2 |
| Thumbprint (SHA1) | `535892C8273A64940E5DDB321965EB255241DA57` |
| Store | Current User → Personal (`Cert:\CurrentUser\My`) |
| Private key | On YubiKey via **Microsoft Smart Card Key Storage Provider** (not exportable) |
| Timestamp | RFC3161 `http://ts.ssl.com` |
| Description URL | `https://www.opendcl.com` |

**PIN:** Enter the YubiKey PIN when Windows prompts (often once per session; sometimes
per file). Never put the PIN in scripts, env files committed to git, or CI logs.

**Do not** use the old Sectigo / SafeNet eToken cert if it still appears in the
store (may show `public key does NOT match private key`). Always pass the SSL.com
**thumbprint** so the wrong cert is not selected.

### One-time machine setup (already done on Owen’s workstation)

1. Install YubiKey Minidriver + SSL Manager.
2. Cert visible under SSL Manager **Certificates on YubiKey Device**.
3. Import **public** `.crt`/`.der` into Current User Personal (file dialog is normal;
   private key is **not** exported — CA rules forbid that).
4. If `HasPrivateKey` is False, with key inserted:
   ```powershell
   certutil -user -repairstore My 535892C8273A64940E5DDB321965EB255241DA57
   ```
5. Confirm:
   ```powershell
   Get-ChildItem Cert:\CurrentUser\My |
     Where-Object Thumbprint -eq '535892C8273A64940E5DDB321965EB255241DA57' |
     Format-List Subject, Issuer, HasPrivateKey, Thumbprint
   ```

### Sign packages

```powershell
# Preferred: env thumbprint (user or machine scope — not committed)
$env:SIGN_CERT_THUMBPRINT = "535892C8273A64940E5DDB321965EB255241DA57"

# Entire versioned dist folder (*.msi / *.msm only by default)
.\scripts\sign-files.ps1 -Path .\dist\10.1.1.1

# Or explicit:
.\scripts\sign-files.ps1 -Path .\dist\10.1.1.1 `
  -CertThumbprint "535892C8273A64940E5DDB321965EB255241DA57" `
  -TimestampUrl "http://ts.ssl.com"
```

If thumbprint is omitted, `sign-files.ps1` **auto-detects** a single SSL.com code
signing cert with a private key in the store.

`sign-files.ps1` defaults:

- Timestamp: `http://ts.ssl.com` (override with `-TimestampUrl` / `SIGN_TIMESTAMP_URL`)
- Description: `https://www.opendcl.com`
- Directory scan: `*.msi`, `*.msm` only (`-IncludeBinaries` for exe/dll)
- Verifies each file with `signtool verify /pa` unless `-SkipVerify`

### Full release folder (installers + localization zips + optional sign)

```powershell
.\scripts\make-release.ps1 -ProductVersion 10.1.1.1 -Sign `
  -CertThumbprint "535892C8273A64940E5DDB321965EB255241DA57"
```

Localization `.zip` files are **not** Authenticode-signed (only MSI/MSM).

Then publish:

```powershell
gh release create "v10.1.1.1" .\dist\10.1.1.1\* --title "OpenDCL 10.1.1.1"
```

Or run the **Make release** workflow (self-hosted), with secret
`SIGN_CERT_THUMBPRINT` set on the runner/repo.

## What to sign

Historical practice: **installer packages only** (`*.msi`, `*.msm`) — matches
`@SignAll.bat *.ms?`.

Optional: sign PE modules before WiX if the user asks (`-IncludeBinaries`).

## Scripts map

| Script | Role |
|--------|------|
| `scripts/sign-files.ps1` | YubiKey / thumbprint signing via signtool |
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

**Runner:** `[self-hosted, Windows, opendcl-build]` — YubiKey must be available to
that machine when signing; GitHub-hosted runners are not suitable for this path.

**Secret:** `SIGN_CERT_THUMBPRINT` = `535892C8273A64940E5DDB321965EB255241DA57`  
(Do not store the YubiKey PIN in GitHub secrets unless using a future eSigner
automated flow.)

## eSigner (not default)

SSL.com eSigner / Cloud Key Adapter can automate signing without a PIN prompt
(TOTP secret in CI). **Not** the current production path. Stay on YubiKey + PIN
unless the user explicitly switches.

## Historical mapping

| Legacy | Replacement |
|--------|-------------|
| `#Sign.bat` (SafeNet eToken + Sectigo, PIN in bat) | `sign-files.ps1` + YubiKey thumbprint + interactive PIN |
| `@SignAll.bat` (`#sign.bat *.ms?`) | `sign-files.ps1 -Path <dist-folder>` |
| `!MakeNewDist.bat` | `make-dist.ps1` / `make-release.ps1` |
| `!MakeLocalizationZips.bat` | `make-localization-zips.ps1` |

## Manual post-release: website update-check versions (**do not skip**)

The Runtime **check for updates** feature is **not** covered by WiX, `make-release`,
or GitHub Releases. After every new **stable** or **development** ship, update the
plain-text version files on **www.opendcl.com** (hosting for `opendcl.com`).

### How the Runtime calls home

From `Runtime/UpdateCheck.cpp`:

- HTTP **POST** to host `opendcl.com`, path `/version/vercheck.php`
- Form fields: `productName`, `userVersion` (`A.B.C.D`), `language` (e.g. `ENU`)

Product names from `Runtime/acrxEntryPoint.cpp`:

| Build flavor | `productName` posted | Server file read by `vercheck.php` |
|--------------|----------------------|-------------------------------------|
| **Release** (shipping) | `OpenDCL Runtime` | `version/version.txt` |
| **Dev / non-release** | `OpenDCL Runtime Dev` | `version/version_dev.txt` |

Server script (site tree, not this git repo): `OpenDCL.com/version/vercheck.php`  
compares `userVersion` to the string in the matching text file. If the site version
is newer, it returns a notification XML pointing users at the download location.

### Manual update (until automated)

1. After the product release is published (installers signed + GitHub Release live).
2. On the **opendcl.com** web host (or the local mirror used to deploy it), edit:

   | File (under site `version/`) | When to update | Contents |
   |------------------------------|----------------|----------|
   | **`version.txt`** | Every **stable** release | Four-part version only, e.g. `10.1.1.1` (plain text, trimmed; no `v` prefix) |
   | **`version_dev.txt`** | Every **development / current** build you want clients to treat as newest dev | Same format |

3. Deploy those files so they are reachable as:
   - `http://opendcl.com/version/version.txt`
   - `http://opendcl.com/version/version_dev.txt`
4. Spot-check: open the URLs in a browser, or POST to  
   `http://opendcl.com/version/vercheck.php` with an older `userVersion` and
   productName `OpenDCL Runtime` / `OpenDCL Runtime Dev` and confirm a notification
   is returned.

**Local copy of the site tree** (for reference / deploy source on Owen’s machine):  
`P:\Work\OpenDCL\OpenDCL.com\version\` — not part of the `opendcl/OpenDCL` GitHub
repo; update wherever production `opendcl.com` is actually published from.

**Note:** `vercheck.php` still references legacy download URLs in the notification
`Action` (historically SourceForge). Updating the **version numbers** is the
required per-release step; fixing the Action URL to GitHub Releases can be a
separate website improvement later.

### Release checklist (append)

- [ ] Installers built, signed, GitHub Release `vA.B.C.D` published  
- [ ] Localization packs refreshed if needed  
- [ ] **`version.txt` and/or `version_dev.txt` updated on opendcl.com**  
- [ ] Optional: `opendcl.github.io` `assets/versions.js` for the download page  

## Security checklist

- [ ] PIN never in git or scripts  
- [ ] Thumbprint env/secret only (public identifier; still prefer not to scatter)  
- [ ] Broken old Sectigo store entry not used for release  
- [ ] `signtool verify /pa` succeeds on every shipped MSI/MSM  
- [ ] Timestamp present (SSL.com chain)  
- [ ] **opendcl.com `version.txt` / `version_dev.txt` updated for this release**  

## Capture lessons

If the thumbprint changes (rekey/renewal), update this skill’s example thumbprint,
`SIGN_CERT_THUMBPRINT` on the build machine, and any operator notes—not the PIN.
