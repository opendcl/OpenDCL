// Methods_Hatch.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Hatch.h"
#include "ArgumentsRetrieval.h"
#include "ArxHatchCtrl.h"
#include "ControlTypes.h"


ADSRESULT Hatch::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHatch))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxHatchCtrl* pCtrl = (CArxHatchCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Clear();	
	pCtrl->RedrawWindow();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Hatch::SetPattern()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHatch))
		return RSERR; //invalid input

	CString sPattern;
	if (!GetStringArgument (pArgs, sPattern))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxHatchCtrl* pCtrl = (CArxHatchCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->DisplayHatchPattern( sPattern ) )
		acedRetT();
	return RSRSLT;
}
