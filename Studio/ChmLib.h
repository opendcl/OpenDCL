// ChmLib.h : header file
//

#pragma once


bool IsChmFile( LPCTSTR pszFilename );
bool IsChmFolder( LPCTSTR pszFolder );
bool ReadChmFile( LPCTSTR pszFilename, CString& sContent );
bool EnumChmFolder( /*in*/LPCTSTR pszFolder, /*out*/CStringArray& rsStreams );
