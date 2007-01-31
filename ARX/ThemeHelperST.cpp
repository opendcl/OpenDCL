#include "stdafx.h"
#include "ThemeHelperST.h"


const TCHAR sDll[] = _T("UxTheme.dll");
CThemeHelperST::CThemeHelperST()
{
	// Load DLL. This call will fails (and return NULL) if
	// we are not running under Windows XP (or greater)
	m_hDll = ::LoadLibrary(sDll);
}

CThemeHelperST::~CThemeHelperST()
{
	// Unload DLL (if any)
	if (m_hDll)	::FreeLibrary(m_hDll);
	m_hDll = NULL;
}

LPVOID CThemeHelperST::GetProc(LPCSTR szProc, LPVOID pfnFail)
{
	LPVOID lpRet = pfnFail;

	if (m_hDll)
		lpRet = GetProcAddress(m_hDll, szProc);

	return lpRet;
} // End of GetProc

const char sOpen[] = "OpenThemeData";
HTHEME CThemeHelperST::OpenThemeData(HWND hwnd, LPCWSTR pszClassList)
{
	PFNOPENTHEMEDATA pfnOpenThemeData = (PFNOPENTHEMEDATA)GetProc(sOpen, (LPVOID)OpenThemeDataFail);
	return (*pfnOpenThemeData)(hwnd, pszClassList);
} // End of OpenThemeData

const char sClose[] = "CloseThemeData";
HRESULT CThemeHelperST::CloseThemeData(HTHEME hTheme)
{
	PFNCLOSETHEMEDATA pfnCloseThemeData = (PFNCLOSETHEMEDATA)GetProc(sClose, (LPVOID)CloseThemeDataFail);
	return (*pfnCloseThemeData)(hTheme);
} // End of CloseThemeData

const char sDrawBack[] = "DrawThemeBackground";
HRESULT CThemeHelperST::DrawThemeBackground(HTHEME hTheme, HWND hWnd, HDC hdc, int iPartId, int iStateId, const RECT* pRect, const RECT* pClipRect)
{
  PFNDRAWTHEMEPARENTBACKGROUND pfnDrawThemeParentBackground = (PFNDRAWTHEMEPARENTBACKGROUND)GetProc("DrawThemeParentBackground", NULL);
  if (pfnDrawThemeParentBackground && hWnd)
  {    
	  (*pfnDrawThemeParentBackground)(hWnd, hdc, (PRECT)pRect);  
  } // if
  
  PFNDRAWTHEMEBACKGROUND pfnDrawThemeBackground = (PFNDRAWTHEMEBACKGROUND)GetProc(sDrawBack, (LPVOID)DrawThemeBackgroundFail);
	return (*pfnDrawThemeBackground)(hTheme, hdc, iPartId, iStateId, pRect, pClipRect);
} // End of DrawThemeBackground

HRESULT CThemeHelperST::DrawThemeText(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect)
{
	PFNDRAWTHEMETEXT pfn = (PFNDRAWTHEMETEXT)GetProc("DrawThemeText", (LPVOID)DrawThemeTextFail);
	return (*pfn)(hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect);
} // End of DrawThemeText

const char sIsApp[] = "IsAppThemed";
BOOL CThemeHelperST::IsAppThemed()
{
	PFNISAPPTHEMED pfnIsAppThemed = (PFNISAPPTHEMED)GetProc(sIsApp, (LPVOID)IsAppThemedFail);
	return (*pfnIsAppThemed)();
} // End of IsAppThemed

const char sIsThemeActive[] = "IsThemeActive";
BOOL CThemeHelperST::IsThemeActive()
{
	PFNISTHEMEACTIVE pfn = (PFNISTHEMEACTIVE)GetProc(sIsThemeActive, (LPVOID)IsThemeActiveFail);
	return (*pfn)();
} // End of IsThemeActive
