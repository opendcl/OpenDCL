// HostThemeHelper.h : shared host light/dark UxTheme walk
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CHostThemeHelper

class CHostThemeHelper
{
public:
	static bool HostMaps();
	static LPCWSTR ThemeClass( bool bVisual );
	static void Apply( HWND hwnd, LPCWSTR pszTheme );
	static void ApplyTree( HWND hwndRoot, LPCWSTR pszTheme );
};
