$ErrorActionPreference = 'Stop'
$softwareName = 'OpenDCL Studio*'
[array]$keys = Get-UninstallRegistryKey -SoftwareName $softwareName
if ($keys.Count -eq 0) {
  Write-Warning "$env:ChocolateyPackageName not found in Apps & Features (already removed?)."
  return
}
foreach ($k in $keys) {
  $code = $k.PSChildName
  Write-Host "Uninstalling $($k.DisplayName) $code"
  $p = Start-Process -FilePath 'msiexec.exe' -ArgumentList @('/x', $code, '/qn', '/norestart') -Wait -PassThru -NoNewWindow
  if ($p.ExitCode -notin @(0, 1605, 1614, 1641, 3010)) {
    throw "msiexec /x $code exited $($p.ExitCode)"
  }
}
