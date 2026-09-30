$ErrorActionPreference = 'Stop'
$version = '{{VERSION}}'
$checksum = '{{CHECKSUM}}'
$fileName = "OpenDCL.Runtime.$version.msi"
$url = "https://github.com/opendcl/OpenDCL/releases/download/v$version/$fileName"

$packageArgs = @{
  packageName    = $env:ChocolateyPackageName
  fileType       = 'msi'
  url            = $url
  checksum       = $checksum
  checksumType   = 'sha256'
  silentArgs     = '/qn /norestart ALLUSERS=1'
  validExitCodes = @(0, 3010, 1641)
  softwareName   = 'OpenDCL Runtime*'
}
Install-ChocolateyPackage @packageArgs
