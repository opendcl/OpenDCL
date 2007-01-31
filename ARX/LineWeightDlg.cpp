// LineWeightDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LineWeightDlg.h"


static const char gpszaAcedLineWeightDialog_FN[] = "?acedLineWeightDialog@@YA_NW4LineWeight@AcDb@@_NAAW412@@Z";

typedef bool (*FT_acedLineWeightDialog)( AcDb::LineWeight, bool, AcDb::LineWeight& );


static FT_acedLineWeightDialog GetProc(void)
{
	static FT_acedLineWeightDialog pfProc = NULL;
	if( pfProc )
		return pfProc;
	pfProc = (FT_acedLineWeightDialog)GetProcAddress(GetModuleHandle(NULL), gpszaAcedLineWeightDialog_FN);
	assert(pfProc != NULL);
	return pfProc;
}


bool SelectLineWeightUI( /*in-out*/ AcDb::LineWeight& lw,
												 /*in*/ bool bIncludeMetaTypes /*= true*/,
												 /*in*/ CWnd *pParent /*= NULL*/ )
{
	FT_acedLineWeightDialog pfProc = GetProc();
	if( !pfProc )
		return false;

	BOOL bParentEnabled = TRUE;
	if( pParent )
	{
		bParentEnabled = pParent->IsWindowEnabled();
		pParent->EnableWindow( FALSE );
	}
	bool bResult = pfProc( lw, bIncludeMetaTypes, lw ); //call the exported function
	if( pParent )
		pParent->EnableWindow( bParentEnabled );
	return bResult;
}
