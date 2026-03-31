// Methods_TabStrip.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_TabStrip.h"
#include "DclControlTemplate.h"
#include "ArgumentsRetrieval.h"
#include "ArxTabStripCtrl.h"
#include "ControlTypes.h"


ADSRESULT TabStrip::SetTabCaption()
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

	TPropertyPtr pItemList = pDlgControl->GetTemplate()->GetPropertyObject( Prop::TabsCaption );
	if( nIndex < 0 || static_cast<size_t>( nIndex ) >= pItemList->size() )
		return RSRSLT;
	PropVal::TCStringArray* pItems = pItemList->GetStringArrayPtr();
	PropVal::TCStringArray::iterator iterAt = pItems->begin();
	while( nIndex-- > 0 ) iterAt++;
	(*iterAt) = sText;

	if( pDlgControl->ApplyProperty( pItemList ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT TabStrip::ShowTab()
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

ADSRESULT TabStrip::HideTab()
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

ADSRESULT TabStrip::SetCurSel()
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

ADSRESULT TabStrip::GetCurSel()
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

ADSRESULT TabStrip::GetRowCount()
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

ADSRESULT TabStrip::GetControlArea()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlTabStrip))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxTabStripCtrl* pCtrl = (CArxTabStripCtrl*)pDlgControl->GetControlWnd();
	CRect rcClient = pCtrl->GetTabPageAt( pCtrl->GetCurTabPage() )->GetWndRect();
	pDlgControl->ToDIP( rcClient );
	resbuf rbHeight = { NULL, RTSHORT };
	rbHeight.resval.rint = rcClient.Height();
	resbuf rbWidth = { &rbHeight, RTSHORT };
	rbWidth.resval.rint = rcClient.Width();
	acedRetList( &rbWidth );
	return RSRSLT;
}
