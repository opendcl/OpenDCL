robocopy "<ODCL5>\Studio\DEU\Content" "<TEMPFOLDER>" /mir /xd debug* release* .svn _* /xf *.zip thumbs.db *.apf *.reg *.tif
@set PATH=%PATH%;C:\Program Files\WinZip
del OpenDCLDEU.zip
wzzip OpenDCLDEU.zip "<TEMPFOLDER>\*.*" -r -p -m
