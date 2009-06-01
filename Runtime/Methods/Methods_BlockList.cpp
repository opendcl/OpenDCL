// Methods_BlockList.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_BlockList.h"
#include "ArgumentsRetrieval.h"
#include "ArxBlockListCtrl.h"
#include "ControlTypes.h"
#include "Resource.h"
#include "Workspace.h"


static void ReturnRowCol( int nRow, int nCol )
{
	resbuf rbColumn = { NULL, RTSHORT };
	rbColumn.resval.rint = nCol;
	resbuf rbRow = { &rbColumn, RTSHORT };
	rbRow.resval.rint = nRow;
	acedRetList( &rbRow );
}


ADSRESULT BlockList::GetFileName()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();
	LPCTSTR pszFilename = pCtrl->GetLoadedDwg();
	if( pszFilename )
		acedRetStr( pszFilename );
	return RSRSLT;
}

ADSRESULT BlockList::LoadDwg()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	CString sFilename;
	if( !GetStringArgument( pArgs, sFilename ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->LoadDwg( sFilename ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT BlockList::Reset()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->LoadDwg( NULL ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT BlockList::Arrange()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	UINT nStyle = 0;
	int nArrangeStyle;
	if( !GetIntArgument( pArgs, nArrangeStyle ) )
		return RSERR; //invalid input
	switch( nArrangeStyle )
	{
	case 0:
		nStyle = LVA_ALIGNLEFT;
		break;
	case 1:
		nStyle = LVA_ALIGNTOP;
		break;
	case 2:
		nStyle = LVA_DEFAULT;
		break;
	case 3:
		nStyle = LVA_SNAPTOGRID;
		break;
	default:
		HandleArgValueError( pArgs );
		return RSERR;
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->Arrange( nStyle ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT BlockList::SortTextItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	bool bAscending = true;
	GetBoolArgument( pArgs, bAscending, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SortTextItems( 0, bAscending ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT BlockList::GetItemText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = 0;
	GetIntArgument( pArgs, nCol, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetItemText( nRow, nCol ) );
	return RSRSLT;
}

ADSRESULT BlockList::GetSelCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetSelectedCount() );
	return RSRSLT;
}

ADSRESULT BlockList::GetSelectedItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();

	resbuf* prbResult = NULL;
	resbuf* prbTail = NULL;
	POSITION pos = pCtrl->GetFirstSelectedItemPosition();
	while( pos )
	{
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
		acutNewString( pCtrl->GetItemText( pCtrl->GetNextSelectedItem( pos ), 0 ), prbTail->resval.rstring );
	}
	acedRetList( prbResult );
	acutRelRb( prbResult );
	return RSRSLT;
}

ADSRESULT BlockList::GetSelectedNths()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlBlockList ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();

	resbuf* prbResult = NULL;
	resbuf* prbTail = NULL;
	POSITION pos = pCtrl->GetFirstSelectedItemPosition();
	while( pos )
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
		prbTail->resval.rint = pCtrl->GetNextSelectedItem( pos );
	}
	acedRetList( prbResult );
	acutRelRb( prbResult );
	return RSRSLT;
}

ADSRESULT BlockList::GetCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();
	acedRetInt( pCtrl->GetItemCount() );
	return RSRSLT;
}

ADSRESULT BlockList::GetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();
	acedRetInt( pCtrl->GetSelectionMark() );
	return RSRSLT;
}

ADSRESULT BlockList::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlBlockList))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxBlockListCtrl* pCtrl = (CArxBlockListCtrl*)pDlgControl->GetControlWnd();
	if( nRow > -1 )
	{
		pCtrl->SetSelectionMark( nRow );
		pCtrl->SetItem( nRow, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0 );		
		pCtrl->EnsureVisible( nRow, TRUE );
	}
	else
	{
		for( int idx = pCtrl->GetItemCount() - 1; idx >= 0; --idx )
		{
			pCtrl->SetItemState( idx, 0, LVIS_SELECTED );
			pCtrl->SetItemState( idx, 0, LVIS_FOCUSED );
		}		
	}
	acedRetT();
	return RSRSLT;
}
