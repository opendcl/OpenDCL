# Chocolatey and WinGet packages

Vendor packages for **OpenDCL Studio** and **OpenDCL Runtime**. They wrap the
signed GitHub Release MSIs (they are not a second installer). Language
UpgradeCodes in `catalog.json` must match `scripts/build-wix.ps1`
`$StudioLangMeta`.

| Catalog | Studio | Runtime |
| --- | --- | --- |
| WinGet | `OpenDCL.Studio` | `OpenDCL.Runtime` |
| Chocolatey | `opendcl-studio` | `opendcl-runtime` |

ARP **DisplayVersion** is the 3-part MSI `ProductVersion` (`10.1.202` for file
`10.1.2.2`). WinGet manifests set `AppsAndFeaturesEntries.DisplayVersion` to
that value and `PackageVersion` to the 4-part tag.

Studio language: WinGet `--locale de-DE` (InstallerLocale); Chocolatey
`--params "'/Language:DEU'"`. Default follows OS UI culture, else ENU.

Studio MSIs already merge Runtime. Do not make Studio depend on the Runtime
package.

## Publish

After `dist\<ver>\` exists (and the GitHub Release `v<ver>` is public, so
download URLs work):

```powershell
.\scripts\publish-package-managers.ps1 -ProductVersion 10.1.2.2
```

Dry run (write `packaging\out\<ver>\` only):

```powershell
.\scripts\publish-package-managers.ps1 -ProductVersion 10.1.2.2 -SkipPush -SkipSubmit
```

| Env | Used for |
| --- | --- |
| `CHOCOLATEY_API_KEY` | `choco push` to https://push.chocolatey.org/ |
| `WINGET_GITHUB_TOKEN` | `wingetcreate submit` (PAT that can fork `microsoft/winget-pkgs`) |

**Make release** (`.github/workflows/release.yml`) runs this after the product
`v*` GitHub Release, unless the run is a draft. Set repo secrets
`CHOCOLATEY_API_KEY` and `WINGET_GITHUB_TOKEN`. Workflow inputs
`publish_chocolatey` / `publish_winget` default on; turn either off to skip.
Missing secrets skip that catalog with a warning so the GitHub Release still
counts as published.

The WinGet PAT must be able to create a fork of `microsoft/winget-pkgs` and
open PRs (typically `public_repo`). First Studio/Runtime versions are new
packages; later versions are additional PRs (one package version per PR).

## Chocolatey community rules (do not regress)

- **`iconUrl`:** jsDelivr pinned to the **release tag**, not `raw.githubusercontent.com`.
  Nuspec uses `https://cdn.jsdelivr.net/gh/opendcl/OpenDCL@v{{VERSION}}/wix/ui/icons/badge/OpenDCLBadge-256.png`.
- **`<copyright>Copyright (c) OpenDCL Consortium</copyright>`** in both nuspecs.
- **CPMR0073:** Studio `chocolateyInstall.ps1` must call
  `Install-ChocolateyPackage -Checksum $checksum -ChecksumType $checksumType`
  as **named parameters**. A splat with `checksum = $checksums[$lang]` fails
  automated validation.
- Same-version re-push during moderation. After a human reviewer asks for
  changes, also post in the package **Review Comments** box (not email/Disqus)
  so status becomes Responded.

## Adding a Studio language

1. `$StudioLangMeta` in `scripts/build-wix.ps1` (unique UpgradeCode).
2. Matching row in `packaging/catalog.json` (`code`, BCP-47 `locale`, UpgradeCode).
