// Methods_Hatch.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Hatch.h"
#include "ArgumentsRetrieval.h"
#include "ArxGsViewCtrl.h"
#include "ControlTypes.h"


ADSRESULT Hatch::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlHatch))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->clearAll();	
	pCtrl->m_Line[0] = NULL;
	pCtrl->m_Line[1] = NULL;
	pCtrl->m_Line[2] = NULL;
	pCtrl->m_Line[3] = NULL;			
	pCtrl->m_BlockName.Empty();
	pCtrl->m_FileName.Empty();
	pCtrl->m_bSelectedRect = false;
	if( pCtrl->m_pLoadedDwg )
	{
		delete pCtrl->m_pLoadedDwg;
		pCtrl->m_pLoadedDwg = NULL;
	}
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

	CArxGsViewCtrl* pCtrl = (CArxGsViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->clearAll();	
	pCtrl->m_Line[0] = NULL;
	pCtrl->m_Line[1] = NULL;
	pCtrl->m_Line[2] = NULL;
	pCtrl->m_Line[3] = NULL;			
	pCtrl->m_BlockName.Empty();
	pCtrl->m_FileName.Empty();
	pCtrl->m_bSelectedRect = false;
	if( pCtrl->m_pLoadedDwg )
	{
		delete pCtrl->m_pLoadedDwg;
		pCtrl->m_pLoadedDwg = NULL;
	}
	if( pCtrl->DisplayHatchPattern( sPattern ) )
		acedRetT();
	return RSRSLT;
}
