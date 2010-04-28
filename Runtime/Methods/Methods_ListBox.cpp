// Methods_ListBox.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ListBox.h"
#include "ArgumentsRetrieval.h"
#include "ControlTypes.h"
#include "ArxListBoxCtrl.h"
#include "ArxWorkspace.h"


ADSRESULT ListBox::AddString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	PropVal::TCStringArray rsToAdd;
	if( !GetStringArrayArgument( pArgs, rsToAdd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idxNewItem = -1;
	TPropertyPtr pListProp = pDlgControl->GetTemplate()->GetPropertyObject( Prop::List );
	if( pListProp )
	{
		PropVal::TCStringArray* prsList = pListProp->GetStringArrayPtr();
		prsList->insert( prsList->end(), rsToAdd.begin(), rsToAdd.end() );
		if( pDlgControl->ApplyProperty( pListProp ) )
			idxNewItem = prsList->size();
	}

	if( idxNewItem >= 0 )
		acedRetInt( idxNewItem );
	return RSRSLT;
}

ADSRESULT ListBox::AddList()
{
	return ListBox::AddString();
}

ADSRESULT ListBox::GetItemText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CString sText = pDlgControl->GetTemplate()->GetPropertyListItem( Prop::List, nItem );
	acedRetStr( sText );
	return RSRSLT;
}

ADSRESULT ListBox::GetSelectedItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int nCount = pCtrl->GetSelCount();
	if( nCount == 0 )
		return RSRSLT;

	if( nCount == -1 )
	{ //only a single selection
		int nCurSel = pCtrl->GetCurSel();
		if( nCurSel >= 0 )
		{
			CString sSel;
			pCtrl->GetText( nCurSel, sSel );
			acedRetStr( sSel );
		}
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

ADSRESULT ListBox::GetSelectedNths()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
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

ADSRESULT ListBox::GetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int nSel = pCtrl->GetCurSel();
	if( nSel >= 0 )
		acedRetInt( nSel );
	return RSRSLT;
}

ADSRESULT ListBox::GetCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetCount() );
	return RSRSLT;
}

ADSRESULT ListBox::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	int nCurSel = -1;
	if( !GetIntArgument( pArgs, nCurSel ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( nCurSel == pCtrl->SetCurSel( nCurSel ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListBox::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();
	pCtrl->ResetContent();
	acedRetT();
	return RSRSLT;
}

ADSRESULT ListBox::GetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetTopIndex() );
	return RSRSLT;
}

ADSRESULT ListBox::SetTopIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
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

ADSRESULT ListBox::SetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	DWORD_PTR dwData = -1;
	if( !GetHandleArgument( pArgs, dwData ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	if( LB_ERR != pCtrl->SetItemData( nItem, dwData ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListBox::GetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	DWORD_PTR dwData = pCtrl->GetItemData( nItem );
	theArxWorkspace.RetHandle( dwData );
	return RSRSLT;
}

ADSRESULT ListBox::DeleteItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();
	int nResult = pCtrl->DeleteString( nIndex );
	if( nResult >= 0 )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListBox::InsertString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	int nIndex;
	if( !GetIntArgument( pArgs, nIndex ) )
		return RSERR; //invalid input

	CString sToAdd;
	if( !GetStringArgument( pArgs, sToAdd ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();
	int idxNewItem = pCtrl->InsertString( nIndex, sToAdd );
	if( idxNewItem >= 0 )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListBox::SelectString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	CString sSearch;
	if( !GetStringArgument( pArgs, sSearch ) )
		return RSERR; //invalid input

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int nStartIndex = pCtrl->GetCurSel();
	GetIntArgument( pArgs, nStartIndex, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idx = pCtrl->SelectString( nStartIndex, sSearch );
	acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ListBox::FindString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	CString sSearch;
	if( !GetStringArgument( pArgs, sSearch ) )
		return RSERR; //invalid input

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int nStartIndex = -1;
	GetIntArgument( pArgs, nStartIndex, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idx = pCtrl->FindString( nStartIndex, sSearch );
	acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ListBox::FindStringExact()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	CString sSearch;
	if( !GetStringArgument( pArgs, sSearch ) )
		return RSERR; //invalid input

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	int nStartIndex = -1;
	GetIntArgument( pArgs, nStartIndex, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	int idx = pCtrl->FindStringExact( nStartIndex, sSearch );
	acedRetInt( idx );
	return RSRSLT;
}

ADSRESULT ListBox::SelectItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
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

ADSRESULT ListBox::HitPointTest()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nX = -1;
	if( !GetIntArgument( pArgs, nX ) )
		return RSERR; //invalid input

	int nY = -1;
	if( !GetIntArgument( pArgs, nY ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	BOOL bOutside;
	UINT nItem = pCtrl->ItemFromPoint( CPoint( nX, nY ), bOutside );
	acedRetInt( (int)nItem );
	return RSRSLT;
}

ADSRESULT ListBox::IsItemSelected()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
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

ADSRESULT ListBox::SetFocusIndex()// SetCaretIndex
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
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

ADSRESULT ListBox::GetFocusIndex()// GetCaretIndex
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
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

ADSRESULT ListBox::GetSelCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetSelCount() );
	return RSRSLT;
}

ADSRESULT ListBox::SelItemRange()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
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

ADSRESULT ListBox::Dir()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	CString sDirectory;
	if( !GetStringArgument( pArgs, sDirectory ) )
		return RSERR; //invalid input

	CString sFilter = _T("*.*");
	GetStringArgument( pArgs, sFilter, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	TCHAR szOldPath[MAX_PATH];
	::GetCurrentDirectory( MAX_PATH, szOldPath );
	::SetCurrentDirectory( sDirectory );
	pCtrl->ResetContent();
	pCtrl->Dir( DDL_READWRITE | DDL_DIRECTORY, sFilter );
	::SetCurrentDirectory( szOldPath );
	acedRetT();
	return RSRSLT;
}

ADSRESULT ListBox::SetAnchorIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
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

ADSRESULT ListBox::GetAnchorIndex()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListBox ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CListBox* pCtrl = (CListBox*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetAnchorIndex() );
	return RSRSLT;
}
