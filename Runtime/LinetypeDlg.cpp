// LineWeightDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LineTypeDlg.h"


#ifdef _ZRXTARGET

bool zcedLinetypeDialog(AcDbObjectId,bool,ZTCHAR * &,AcDbObjectId &);

bool SelectLinetypeUI( /*in-out*/ AcDbObjectId& idLinetype,
											 /*out*/ CString& sLinetype,
											 /*in*/ bool bIncludeMetaTypes /*= true*/,
											 /*in*/ CWnd* pParent /*= NULL*/ )
{
	BOOL bParentEnabled = TRUE;
	if( pParent )
	{
		bParentEnabled = pParent->IsWindowEnabled();
		pParent->EnableWindow( FALSE );
	}
	ACHAR* pszLinetype = NULL;
	bool bResult = zcedLinetypeDialog( idLinetype, bIncludeMetaTypes, pszLinetype, idLinetype ); //call the exported function
	if( bResult )
		sLinetype = pszLinetype;
	acutDelString( pszLinetype );
	if( pParent )
		pParent->EnableWindow( bParentEnabled );
	return bResult;
}

#else //_ZRXTARGET

#if (_ACADTARGET <= 16)
static const char gpszaAcedLinetypeDialog_FN[] = "?acedLinetypeDialog@@YA_NVAcDbObjectId@@_NAAPADAAV1@@Z";
#else //(_ACADTARGET <= 16)
#ifdef _WIN64
static const char gpszaAcedLinetypeDialog_FN[] = "?acedLinetypeDialog@@YA_NVAcDbObjectId@@_NAEAPEA_WAEAV1@@Z";
#else
static const char gpszaAcedLinetypeDialog_FN[] = "?acedLinetypeDialog@@YA_NVAcDbObjectId@@_NAAPA_WAAV1@@Z";
#endif
#endif //(_ACADTARGET <= 16)

typedef bool (*FT_acedLinetypeDialog)( AcDbObjectId, bool, ACHAR*&, AcDbObjectId& );

static FT_acedLinetypeDialog GetProc(void)
{
	static FT_acedLinetypeDialog pfProc = NULL;
	if( pfProc )
		return pfProc;
#if (_BRXTARGET == 9)
	HMODULE hmodApi = GetModuleHandle( _T("brx.dll") );
#elif (_BRXTARGET == 10)
	HMODULE hmodApi = GetModuleHandle( _T("brx10.dll") );
#elif (_BRXTARGET == 11)
	HMODULE hmodApi = GetModuleHandle( _T("brx11.dll") );
#elif (_BRXTARGET == 12)
	HMODULE hmodApi = GetModuleHandle( _T("brx12.dll") );
#elif (_BRXTARGET == 13)
	HMODULE hmodApi = GetModuleHandle( _T("brx13.dll") );
#elif (_BRXTARGET == 14)
	HMODULE hmodApi = GetModuleHandle( _T("brx14.dll") );
#elif (_BRXTARGET == 15)
	HMODULE hmodApi = GetModuleHandle( _T("brx15.dll") );
#elif (_BRXTARGET == 16)
	HMODULE hmodApi = GetModuleHandle( _T("brx16.dll") );
#elif (_BRXTARGET == 17)
	HMODULE hmodApi = GetModuleHandle( _T("brx17.dll") );
#elif (_BRXTARGET)
	#error Unknown BRX target!
#else
	HMODULE hmodApi = GetModuleHandle( NULL );
#endif
	pfProc = (FT_acedLinetypeDialog)GetProcAddress( hmodApi, gpszaAcedLinetypeDialog_FN );
	assert(pfProc != NULL);
	return pfProc;
}


bool SelectLinetypeUI( /*in-out*/ AcDbObjectId& idLinetype,
											 /*out*/ CString& sLinetype,
											 /*in*/ bool bIncludeMetaTypes /*= true*/,
											 /*in*/ CWnd* pParent /*= NULL*/ )
{
	FT_acedLinetypeDialog pfProc = GetProc();
	if( !pfProc )
		return false;

	BOOL bParentEnabled = TRUE;
	if( pParent )
	{
		bParentEnabled = pParent->IsWindowEnabled();
		pParent->EnableWindow( FALSE );
	}
	ACHAR* pszLinetype = NULL;
	bool bResult = pfProc( idLinetype, bIncludeMetaTypes, pszLinetype, idLinetype ); //call the exported function
	if( bResult )
		sLinetype = pszLinetype;
	acutDelString( pszLinetype );
	if( pParent )
		pParent->EnableWindow( bParentEnabled );
	return bResult;
}

#endif //_ZRXTARGET
