// Methods_Tab.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Tab.h"
#include "DclControlObject.h"
#include "ArgumentsRetrieval.h"
#include "ArxTabStripCtrl.h"
#include "ControlTypes.h"


ADSRESULT Tab::SetTabText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTabStrip))
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	CString sText;
	if( !GetStringArgument( pArgs, sText ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxTabStripCtrl* pCtrl = (CArxTabStripCtrl*)pDlgControl->GetControlWnd();

	TCITEM tcItem = { TCIF_TEXT };
	tcItem.pszText = sText.LockBuffer();
	pCtrl->GetTabCtrl().SetItem( nIndex, &tcItem );
	acedRetT();
	return RSRSLT;
}

ADSRESULT Tab::ShowTab()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTabStrip))
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxTabStripCtrl* pCtrl = (CArxTabStripCtrl*)pDlgControl->GetControlWnd();

	pCtrl->ShowTab( nIndex );
	acedRetT();
	return RSRSLT;
}

ADSRESULT Tab::HideTab()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTabStrip))
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxTabStripCtrl* pCtrl = (CArxTabStripCtrl*)pDlgControl->GetControlWnd();

	pCtrl->HideTab( nIndex );
	acedRetT();
	return RSRSLT;
}

ADSRESULT Tab::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTabStrip))
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxTabStripCtrl* pCtrl = (CArxTabStripCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SetCurrentTab( nIndex ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Tab::GetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTabStrip))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxTabStripCtrl* pCtrl = (CArxTabStripCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetCurTabPage() );
	return RSRSLT;
}

ADSRESULT Tab::GetRowCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTabStrip))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxTabStripCtrl* pCtrl = (CArxTabStripCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetTabCtrl().GetRowCount() );
	return RSRSLT;
}

ADSRESULT Tab::GetControlArea()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTabStrip))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxTabStripCtrl* pCtrl = (CArxTabStripCtrl*)pDlgControl->GetControlWnd();
	CRect rcClient = pCtrl->GetTabPageAt( pCtrl->GetCurTabPage() )->GetWndRect();
	resbuf rbHeight = { NULL, RTSHORT };
	rbHeight.resval.rint = rcClient.Height();
	resbuf rbWidth = { &rbHeight, RTSHORT };
	rbWidth.resval.rint = rcClient.Width();
	acedRetList( &rbWidth );
	return RSRSLT;
}
