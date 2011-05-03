// stdafx.cpp : source file that includes just the standard includes
// OpenDCL.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


#if (_MSC_VER == 1500)
// Prevent MFC from pulling in 2 MB worth of junk via themehelper.obj in VS 2008 SP1

HMODULE AfxLoadSystemLibraryUsingFullPath(const WCHAR *pszLibrary)
{
	WCHAR wszLoadPath[MAX_PATH+1];
	if (::GetSystemDirectoryW(wszLoadPath, _countof(wszLoadPath)) == 0)
		return NULL;

	if (wszLoadPath[wcslen(wszLoadPath)-1] != L'\\')
	{
		if (wcscat_s(wszLoadPath, _countof(wszLoadPath), L"\\") != 0)
			return NULL;
	}

	if (wcscat_s(wszLoadPath, _countof(wszLoadPath), pszLibrary) != 0)
		return NULL;

	return(::AfxCtxLoadLibraryW(wszLoadPath));
}

#endif //(_MSC_VER >= 1500)


#if (_MSC_VER >= 1600)
// Prevent MFC from pulling in 2 MB worth of junk in VS 2010

// afxctrlcontainer.obj
void __cdecl AfxRegisterMFCCtrlClasses(void) {}
void CMFCControlContainer::SetControlData(WORD nIDC, DWORD dwSize, BYTE* pbData) {}
BOOL CMFCControlContainer::SubclassDlgControls() { return FALSE; }
CMFCControlContainer::CMFCControlContainer(CWnd* pWnd) {}
CMFCControlContainer::~CMFCControlContainer() {}
BOOL CMFCControlContainer::ReSubclassControl(HWND hWndCtrl, WORD nIDC, CWnd& control) { return FALSE; }
BOOL CMFCControlContainer::IsSubclassedFeaturePackControl(HWND hWndCtrl) { return FALSE; }
BOOL __stdcall CMFCControlContainer::ReadBoolProp(CTagManager& tagManager, LPCTSTR lpszTag, BOOL& bMember) { return FALSE; }
int __stdcall CMFCControlContainer::UTF8ToString(LPCSTR lpSrc, CString& strDst, int nLength) { return -1; }

// afxglobals.obj
struct AFX_GLOBAL_DATA
{
	BOOL InitD2D(enum D2D1_FACTORY_TYPE, enum DWRITE_FACTORY_TYPE) { return FALSE; }
	HRESULT ShellCreateItemFromParsingName(PCWSTR, IBindCtx*, REFIID, void**) { return E_NOTIMPL; }
	ITaskbarList3* GetITaskbarList3() { return NULL; }
	ID2D1Factory* GetDirect2dFactory()	{ return NULL; }
	struct IWICImagingFactory* GetWICFactory()	{ return NULL; }
	AFX_GLOBAL_DATA()
	{ //reference all the functions so they don't get optimized away
		AfxRegisterMFCCtrlClasses();
		InitD2D(D2D1_FACTORY_TYPE_SINGLE_THREADED, DWRITE_FACTORY_TYPE_SHARED);
		ShellCreateItemFromParsingName( NULL, NULL, IID_NULL, NULL );
		GetITaskbarList3();
		GetDirect2dFactory();
		GetWICFactory();
	}
	protected: static struct ID2D1Factory * m_pDirect2dFactory;
	protected: static struct IDWriteFactory * m_pWriteFactory;
	protected: static struct IWICImagingFactory * m_pWicFactory;
	protected: static BOOL m_bD2DInitialized;
} afxGlobalData;
struct ID2D1Factory * AFX_GLOBAL_DATA::m_pDirect2dFactory = NULL;
struct IDWriteFactory * AFX_GLOBAL_DATA::m_pWriteFactory = NULL;
struct IWICImagingFactory * AFX_GLOBAL_DATA::m_pWicFactory = NULL;
BOOL AFX_GLOBAL_DATA::m_bD2DInitialized = NULL;
void __cdecl ControlBarCleanUp(void) {}

#endif //(_MSC_VER >= 1600)
