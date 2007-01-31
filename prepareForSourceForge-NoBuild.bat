@set PATH=%PATH%;C:\Program Files\Microsoft Visual Studio 8\Common7\IDE;C:\Program Files\WinZip
robocopy . ..\SourceForge /mir /xd debug* release* .svn /xf *.lsp *._ls *.ncb *.user *.suo *.aps *.ilk *.pch *.htm *.err *.dmp *.pdb
del ..\ObjectDCLSource.zip
wzzip ..\ObjectDCLSource.zip ..\SourceForge\*.* -r -p -m
del ..\ObjectDCLBin.zip
wzzip ..\ObjectDCLBin.zip "ARX\ARX.17\Release\ObjectDCL2007.arx"
wzzip ..\ObjectDCLBin.zip "ARX\ARX.16\Release\ObjectDCL2004.arx"
wzzip ..\ObjectDCLBin.zip "Editor\Release\ObjectDCL.exe"
