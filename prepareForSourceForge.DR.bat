@set PATH=C:\WINDOWS\;C:\WINDOWS\SYSTEM32\;C:\Program Files\Microsoft Visual Studio 8\Common7\IDE;C:\Program Files\WinZip
copy /Y C:\dm\source\ObjectDCL\ARX\interfaceArx2004.vcprojTrue C:\dm\source\ObjectDCL\ARX\interfaceArx2004.vcproj
devenv c:\dm\source\objectdcl\ARX\ObjectDCLARX.sln /build "Release 2004"
copy /Y C:\dm\source\ObjectDCL\ARX\interfaceArx2004.vcprojFalse C:\dm\source\ObjectDCL\ARX\interfaceArx2004.vcproj
devenv c:\dm\source\objectdcl\ARX\ObjectDCLARX.sln /build "Release 2007"
devenv c:\dm\source\objectdcl\Editor\ObjectDCLEditor.sln /build "Release"
robocopy C:\dm\source\ObjectDCL C:\ObjectDCLForSourceForge /mir /xd debug* release* .svn /xf *.lsp *._ls *.ncb *.user *.suo *.aps
del c:\ObjectDCLSource.zip
wzzip c:\ObjectDCLSource.zip C:\ObjectDCLForSourceForge\*.* -r -p -m
del c:\ObjectDCLBin.zip
wzzip c:\ObjectDCLBin.zip C:\dm\base\ObjectDCL*.arx
wzzip c:\ObjectDCLBin.zip "C:\Program Files\3rd day software\ObjectDCL\objectdcl.exe"
