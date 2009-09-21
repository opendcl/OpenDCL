// LineWeightDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LineTypeDlg.h"


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
#if (_BRXTARGET == 10)
	HMODULE hmodApi = GetModuleHandle( _T("brx10.dll") );
#elif (_BRXTARGET == 9)
	HMODULE hmodApi = GetModuleHandle( _T("brx.dll") );
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
