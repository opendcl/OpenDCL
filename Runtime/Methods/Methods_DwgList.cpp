// Methods_DwgList.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_DwgList.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"
#include "ArxDwgListCtrl.h"
#include "Workspace.h"


ADSRESULT DwgList::GetType()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	DWORD_PTR dwData = pCtrl->GetItemData( nItem );
	if( dwData != LB_ERR )
		acedRetInt( (dwData & 1) );
	return RSRSLT;
}

ADSRESULT DwgList::Dir()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	CString sDirectory;
	if( !GetStringArgument( pArgs, sDirectory ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxDwgListCtrl* pCtrl = (CArxDwgListCtrl*)pDlgControl->GetControlWnd();
	pCtrl->Dir( sDirectory );
	acedRetT();
	return RSRSLT;
}

ADSRESULT DwgList::GetDir()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxDwgListCtrl* pCtrl = (CArxDwgListCtrl*)pDlgControl->GetControlWnd();
	CString sPath = pCtrl->GetDragTextPrefix();
	sPath.TrimRight( _T("\\/") );
	sPath += _T('\\');
	acedRetStr( sPath );
	return RSRSLT;
}

ADSRESULT DwgList::GetFileName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxDwgListCtrl* pCtrl = (CArxDwgListCtrl*)pDlgControl->GetControlWnd();
	int nCurSel = pCtrl->GetCurSel();
	if( nCurSel >= 0 )
	{
		CString sFilename;
		pCtrl->GetText( nCurSel, sFilename );
		acedRetStr( pCtrl->GetDragTextPrefix() + sFilename );
	}
	return RSRSLT;
}

ADSRESULT DwgList::GetText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	CString sText;
	pCtrl->GetText( nItem, sText );
	acedRetStr( sText );
	return RSRSLT;
}

ADSRESULT DwgList::GetSelectedItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int nCount = pCtrl->GetSelCount();
	if( nCount == 0 )
		return RSRSLT;

	if( nCount == -1 )
	{ //only a single selection
		CString sSel;
		pCtrl->GetText( pCtrl->GetCurSel(), sSel );
		resbuf rbSel = { NULL, RTSTR };
		rbSel.resval.rstring = sSel.LockBuffer();
		acedRetList( &rbSel );
		return RSRSLT;
	}

	CArray< int, int > rnSel;
	rnSel.SetSize( nCount );
	if( LB_ERR == pCtrl->GetSelItems( nCount, rnSel.GetData() ) )
		return RSRSLT;

	resbuf* prbResult = NULL;
	resbuf* prbTail = NULL;
	for( int idx = 0; idx < nCount; ++idx )
	{
		CString sSel;
		pCtrl->GetText( rnSel.GetAt( idx ), sSel );
		if( prbTail )
		{
			prbTail->rbnext = acutNewRb( RTSTR );
			prbTail = prbTail->rbnext;
		}
		else
		{
			prbTail = acutNewRb( RTSTR );
			prbResult = prbTail;
		}
		acutNewString( sSel, prbTail->resval.rstring );
	}
	acedRetList( prbResult );
	acutRelRb( prbResult );
	return RSRSLT;
}

ADSRESULT DwgList::GetSelectedNths()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int nCount = pCtrl->GetSelCount();
	if( nCount <= 0 )
		return RSRSLT;

	CArray< int, int > rnSel;
	rnSel.SetSize( nCount );
	if( LB_ERR == pCtrl->GetSelItems( nCount, rnSel.GetData() ) )
		return RSRSLT;

	resbuf* prbResult = NULL;
	resbuf* prbTail = NULL;
	for( int idx = 0; idx < nCount; ++idx )
	{
		if( prbTail )
		{
			prbTail->rbnext = acutNewRb( RTSHORT );
			prbTail = prbTail->rbnext;
		}
		else
		{
			prbTail = acutNewRb( RTSHORT );
			prbResult = prbTail;
		}
		prbTail->resval.rint = rnSel.GetAt( idx );
	}
	acedRetList( prbResult );
	acutRelRb( prbResult );
	return RSRSLT;
}

ADSRESULT DwgList::GetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt(pCtrl->GetCurSel());
	return RSRSLT;
}

ADSRESULT DwgList::GetCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetCount() );
	return RSRSLT;
}

ADSRESULT DwgList::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nCurSel = -1;
	if( !GetIntArgument( pArgs, nCurSel ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( LB_ERR != pCtrl->SetCurSel( nCurSel ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT DwgList::GetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetTopIndex() );
	return RSRSLT;
}

ADSRESULT DwgList::SetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( LB_ERR != pCtrl->SetTopIndex( nItem ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT DwgList::DeleteItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	pCtrl->DeleteString( nIndex );
	acedRetT();
	return RSRSLT;
}

ADSRESULT DwgList::SelectItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	bool bSelected = true;
	GetBoolArgument( pArgs, bSelected, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( LB_ERR != pCtrl->SetSel( nItem, bSelected ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT DwgList::IsItemSelected()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( pCtrl->GetSel( nItem ) > 0 )
		acedRetT();
	return RSRSLT;
}

ADSRESULT DwgList::SetFocusIndex()// SetCaretIndex
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( LB_ERR == pCtrl->SetCaretIndex( nItem, TRUE ) )
		return RSRSLT;
	pCtrl->SetFocus();
	acedRetT();
	return RSRSLT;
}

ADSRESULT DwgList::GetFocusIndex()// GetCaretIndex
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int nCaretIndex = pCtrl->GetCaretIndex();
	if( LB_ERR == nCaretIndex )
		return RSRSLT;
	acedRetInt( nCaretIndex );
	return RSRSLT;
}

ADSRESULT DwgList::GetSelCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetSelCount() );
	return RSRSLT;
}

ADSRESULT DwgList::SelItemRange()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nStart = -1;
	if( !GetIntArgument( pArgs, nStart ) )
		return RSERR; //invalid input

	int nEnd = -1;
	if( !GetIntArgument( pArgs, nEnd ) )
		return RSERR; //invalid input

	bool bSelect = true;
	if( !GetBoolArgument( pArgs, bSelect ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( LB_ERR != pCtrl->SelItemRange( bSelect, nStart, nEnd ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT DwgList::SetAnchorIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	pCtrl->SetAnchorIndex( nItem );
	acedRetT();
	return RSRSLT;
}

ADSRESULT DwgList::GetAnchorIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlDwgList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetAnchorIndex() );
	return RSRSLT;
}
