#include "stdafx.h"
#include "HostThemeHelper.h"
#include "ThemeAPI.h"
#include "ColorService.h"


bool CHostThemeHelper::HostMaps()
{
	return OdclSysColor( COLOR_WINDOW ) != ::GetSysColor( COLOR_WINDOW );
}

LPCWSTR CHostThemeHelper::ThemeClass( bool bVisual )
{
	return (bVisual && !HostMaps())? NULL : L"";
}

void CHostThemeHelper::Apply( HWND hwnd, LPCWSTR pszTheme )
{
	if( hwnd )
		UxTheme::SetWindowTheme( hwnd, pszTheme, pszTheme );
}

void CHostThemeHelper::ApplyTree( HWND hwndRoot, LPCWSTR pszTheme )
{
	if( !hwndRoot )
		return;
	Apply( hwndRoot, pszTheme );
	for( HWND hwndChild = ::FindWindowEx( hwndRoot, NULL, NULL, NULL );
			 hwndChild;
			 hwndChild = ::FindWindowEx( hwndRoot, hwndChild, NULL, NULL ) )
	{
		ApplyTree( hwndChild, pszTheme );
		::InvalidateRect( hwndChild, NULL, TRUE );
	}
}
