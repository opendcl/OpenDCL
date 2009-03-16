// ChmLib.h : header file
//

#pragma once


bool IsChmFile( LPCTSTR pszFilename );
bool ReadChmFile( LPCTSTR pszFilename, CString& sContent );
