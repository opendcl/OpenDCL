// Methods_Grid.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_Grid.h"
#include "ArgumentsRetrieval.h"
#include "ArxGridCtrl.h"
#include "Resource.h"
#include "ArxWorkspace.h"


static void ReturnRowCol(int nRow, int nCol)
{
	resbuf rbColumn = { NULL, RTSHORT };
	rbColumn.resval.rint = nCol;
	resbuf rbRow = { &rbColumn, RTSHORT };
	rbRow.resval.rint = nRow;
	acedRetList( &rbRow );
}


ADSRESULT Grid::AddColumns()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	bool bNestedLists = GetListBeginArgument( pArgs, true );

	CString sCaption;
	while( GetStringArgument( pArgs, sCaption, true ) )
	{
		int nFormat = LVCFMT_CENTER;
		int nColWidth = -1;
		int nImage = -1;
		int nJustification;
		if( GetIntArgument( pArgs, nJustification, true ) )
		{
			switch( nJustification )
			{
			case 1:
				nFormat = LVCFMT_CENTER;
				break;
			case 2:
				nFormat = LVCFMT_RIGHT;
				break;
			case 0:
				nFormat = LVCFMT_LEFT;
				break;
			default:
				HandleArgValueError( pArgs );
				return RSERR;
			}

			if( GetIntArgument( pArgs, nColWidth, true ) )
				GetIntArgument( pArgs, nImage, true );
		}

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input

		if( -1 == pCtrl->InsertColumn( pCtrl->GetColumnCount(), sCaption, nFormat, nColWidth, nImage ) )
			return RSRSLT;

		if( !GetListBeginArgument( pArgs, true ) )
			break;
	}

	if( bNestedLists && !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::InsertColumn()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nInsertIdx;
	if( !GetIntArgument( pArgs, nInsertIdx ) )
		return RSERR; //invalid input

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	CString sCaption;
	if( !GetStringArgument( pArgs, sCaption ) )
		return RSERR; //invalid input

	int nFormat = LVCFMT_CENTER;
	int nColWidth = -1;
	int nImage = -1;
	int nJustification;
	if( GetIntArgument( pArgs, nJustification, true ) )
	{
		switch( nJustification )
		{
		case 1:
			nFormat = LVCFMT_CENTER;
			break;
		case 2:
			nFormat = LVCFMT_RIGHT;
			break;
		case 0:
			nFormat = LVCFMT_LEFT;
			break;
		default:
			HandleArgValueError( pArgs );
			return RSERR;
		}

		if( GetIntArgument( pArgs, nColWidth, true ) )
			GetIntArgument( pArgs, nImage, true );
	}

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( -1 == pCtrl->InsertColumn( nInsertIdx, sCaption, nFormat, nColWidth, nImage ) )
		return RSRSLT;

	acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::AddRow()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	int nImage = -1;
	GetIntArgument( pArgs, nImage, true );

	CString sItemText;
	if( !GetStringArgument( pArgs, sItemText ) )
		return RSERR; //invalid input

	CStringArray rsText;
	GetStringArrayArgument( pArgs, rsText, true );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	int nRow = pCtrl->InsertItem( pCtrl->GetItemCount(), sItemText, nImage );
	if( nRow == -1 )
		return RSRSLT;
	for( int idx = rsText.GetCount() - 1; idx >= 0; --idx )
		pCtrl->SetCellText( nRow, idx + 1, rsText.GetAt( idx ) );

	acedRetInt( nRow );
	return RSRSLT;
}

ADSRESULT Grid::InsertRow()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nInsertIdx;
	if( !GetIntArgument( pArgs, nInsertIdx ) )
		return RSERR; //invalid input

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	int nImage = -1;
	GetIntArgument( pArgs, nImage, true );

	CString sItemText;
	if( !GetStringArgument( pArgs, sItemText ) )
		return RSERR; //invalid input

	CStringArray rsText;
	GetStringArrayArgument( pArgs, rsText, true );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	int nRow = pCtrl->InsertItem( nInsertIdx, sItemText, nImage );
	if( nRow == -1 )
		return RSRSLT;
	for( int idx = rsText.GetCount() - 1; idx >= 0; --idx )
		pCtrl->SetCellText( nRow, idx + 1, rsText.GetAt( idx ) );

	acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->DeleteAllItems() )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::GetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	theArxWorkspace.RetHandle( pCtrl->GetItemData( nItem ) );
	return RSRSLT;
}

ADSRESULT Grid::SetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	DWORD_PTR dwData = -1;
	if( !GetHandleArgument( pArgs, dwData ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SetItemData( nItem, dwData ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::GetItemImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = 0;
	GetIntArgument( pArgs, nCol, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetCellImage( nRow, nCol ) );
	return RSRSLT;
}

ADSRESULT Grid::GetItemText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = 0;
	GetIntArgument( pArgs, nCol, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetCellText( nRow, nCol ) );
	return RSRSLT;
}

ADSRESULT Grid::GetRowItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	resbuf* prbResult = NULL;
	resbuf* prbTail = NULL;
	size_t ctCol = pCtrl->GetColumnCount();
	for( int idxCol = 0; idxCol < ctCol; ++idxCol )
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
		acutNewString( pCtrl->GetCellText( nItem, idxCol ), prbTail->resval.rstring );
	}
	acedRetList( prbResult );
	acutRelRb( prbResult );
	return RSRSLT;
}

ADSRESULT Grid::GetColumnItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	resbuf* prbResult = NULL;
	resbuf* prbTail = NULL;
	int ctRow = pCtrl->GetItemCount();
	for( int idxRow = 0; idxRow < ctRow; ++idxRow )
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
		acutNewString( pCtrl->GetCellText( idxRow, nCol ), prbTail->resval.rstring );
	}
	acedRetList( prbResult );
	acutRelRb( prbResult );
	return RSRSLT;
}

ADSRESULT Grid::SetItemText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	CString sText;
	if( !GetStringArgument( pArgs, sText ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SetItemText( nRow, nCol, sText ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::SetItemStyle()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	int nStyle = -1;
	if( !GetIntArgument( pArgs, nStyle ) )
		return RSERR; //invalid input

	int nData1 = -1;
	int nData2 = -1;
	if( GetIntArgument( pArgs, nData1, true ) )
		GetIntArgument( pArgs, nData2, true );

	CString sOptionalText;
	GetStringArgument( pArgs, sOptionalText, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	pCtrl->SetCellStyle( nRow, nCol, (CellStyle)nStyle, nData1, nData2, sOptionalText );
	acedRetT();
	return RSRSLT;
}			

ADSRESULT Grid::HitPointTest()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nX = -1;
	if( !GetIntArgument( pArgs, nX ) )
		return RSERR; //invalid input

	int nY = -1;
	if( !GetIntArgument( pArgs, nY ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	int nRow = -1, nCol = -1;
	pCtrl->CellHitTest( CPoint( nX, nY ), nRow, nCol );
	ReturnRowCol( nRow, nCol );
	return RSRSLT;
}

ADSRESULT Grid::GetItemDropList()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	CStringArray rsText;
	CArray< int, int > rnImage;
	if( pCtrl->GetCellListData( nRow, nCol, rnImage, rsText ) )
	{
		resbuf* prbResult = acutNewRb( RTLB );
		resbuf* prbTail = prbResult;
		for( int idx = rsText.GetCount() - 1; idx >= 0; --idx )
		{
			prbTail->rbnext = acutNewRb( RTSTR );
			prbTail = prbTail->rbnext;
			acutNewString( rsText.GetAt( idx ), prbTail->resval.rstring );
		}
		prbTail->rbnext = acutNewRb( RTLE );
		prbTail = prbTail->rbnext;
		prbTail->rbnext = acutNewRb( RTLB );
		prbTail = prbTail->rbnext;
		for( int idx = rnImage.GetCount() - 1; idx >= 0; --idx )
		{
			prbTail->rbnext = acutNewRb( RTSHORT );
			prbTail = prbTail->rbnext;
			prbTail->resval.rint = rnImage.GetAt( idx );
		}
		prbTail->rbnext = acutNewRb( RTLE );
		acedRetList( prbResult );
	}
	return RSRSLT;
}			

ADSRESULT Grid::SetItemDropList()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	CStringArray rsText;
	if( !GetStringArrayArgument( pArgs, rsText ) )
		return RSERR; //invalid input

	CArray< int, int > rnImage;
	GetIntArrayArgument( pArgs, rnImage, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SetCellListData( nRow, nCol, rnImage, rsText ) )
		acedRetT();
	return RSRSLT;
}			

ADSRESULT Grid::SetItemImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	int nImage;
	if( !GetIntArgument( pArgs, nImage ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	pCtrl->SetCellImage( nRow, nCol, nImage );
	acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::GetColWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetColumnWidth( nCol ) );
	return RSRSLT;
}

ADSRESULT Grid::SetColWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	int nColWidth = -1;
	if( !GetIntArgument( pArgs, nColWidth ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SetColumnWidth( nCol, nColWidth ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::CalcColWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	CString sText;
	if( !GetStringArgument( pArgs, sText ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetStringWidth( sText ) );
	return RSRSLT;
}

ADSRESULT Grid::DeleteRow()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->DeleteItem( nRow ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::DeleteColumn()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->DeleteColumn( nCol ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::FillList()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	pCtrl->DeleteAllItems();

	if( GetNilArgument( pArgs, true ) )
	{
		acedRetT();
		return RSRSLT;
	}

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	bool bVisible = (pCtrl->IsWindowVisible() != FALSE);
	if( bVisible )
		pCtrl->SetRedraw( FALSE );
	int nRow = 0;
	do
	{
		int nCol = 0;
		while( !GetListEndArgument( pArgs, true ) )
		{
			CString sCaption;
			if( !GetStringArgument( pArgs, sCaption ) )
			{
				if( bVisible )
					pCtrl->SetRedraw( TRUE );
				pCtrl->OnNeedRepaint();
				return RSERR; //invalid input
			}

			int nImage = -1;
			GetIntArgument( pArgs, nImage, true );

			if( nCol == 0 )
				pCtrl->InsertItem( nRow, sCaption, nImage );
			else
				pCtrl->SetCellTextImage( nRow, nCol, sCaption, nImage );
			++nCol;
		}
		++nRow;
	} while( GetListBeginArgument( pArgs, true ) );

	if( !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( bVisible )
		pCtrl->SetRedraw( TRUE );
	pCtrl->OnNeedRepaint();
	pCtrl->UpdateWindow(); 

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::GetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	ReturnRowCol( pCtrl->GetCurRow(), pCtrl->GetCurColumn() );
	return RSRSLT;
}

ADSRESULT Grid::CancelItemEdit()
{	
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	pCtrl->HideEditControls();
	acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::SortTextItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	bool bAscending = true;
	GetBoolArgument( pArgs, bAscending, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SortTextItems( nCol, bAscending ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::SortNumericItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	bool bAscending = true;
	GetBoolArgument( pArgs, bAscending, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SortNumericItems( nCol, bAscending ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::SetColumnImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	int nImage = -1;
	if( !GetIntArgument( pArgs, nImage ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	CHeaderCtrl* pHeader = pCtrl->GetHeaderCtrl();
	if( !pHeader )
		return RSRSLT;

	CImageList* pImageList = pHeader->GetImageList();
	if( !pImageList || nImage < 0 || nImage >= pImageList->GetImageCount() )
		return HandleArgValueError( pArgs, IDS_ARG_IMAGENOTFOUND );

	HDITEM hdi = { HDI_IMAGE };
	hdi.iImage = nImage;
	if( pHeader->SetItem( nCol, &hdi ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::GetColumnImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	CHeaderCtrl* pHeader = pCtrl->GetHeaderCtrl();
	if( !pHeader )
		return RSRSLT;

	HDITEM hdi = { HDI_IMAGE };
	if( pHeader->GetItem( nCol, &hdi ) )
		acedRetInt( hdi.iImage );
	return RSRSLT;
}

ADSRESULT Grid::GetRowCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	acedRetInt( pCtrl->GetItemCount() );
	return RSRSLT;
}

ADSRESULT Grid::GetColumnCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	acedRetInt( pCtrl->GetColumnCount() );
	return RSRSLT;
}

ADSRESULT Grid::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = -1;
	GetIntArgument( pArgs, nCol, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	pCtrl->SetCurCell( nRow, nCol );
	acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::StartItemEdit()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = -1;
	GetIntArgument( pArgs, nCol, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	pCtrl->SetCurCell( nRow, nCol );
	pCtrl->EditCurCell();
	acedRetT();
	return RSRSLT;
}


ADSRESULT Grid::GetItemCheck()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->IsCellChecked( nRow, nCol ) )
		acedRetInt( 1 );
	else
		acedRetInt( 0 );
	return RSRSLT;
}

ADSRESULT Grid::SetItemCheck()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	int nChecked = 1;
	if( !GetIntArgument( pArgs, nChecked ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->SetCellChecked( nRow, nCol, (nChecked > 0) ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT Grid::AddString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	CString sRowText;
	if( !GetStringArgument( pArgs, sRowText ) )
		return RSERR; //invalid input

	CString sDelimiters = _T("\t");
	GetStringArgument( pArgs, sDelimiters, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	int nTokenPos = 0;
	int nRow = pCtrl->InsertItem( pCtrl->GetItemCount(), sRowText.Tokenize( sDelimiters, nTokenPos ) );
	if( nRow == -1 )
		return RSRSLT;
	int idxCol = 0;
	int cchText = sRowText.GetLength();
	while( nTokenPos >= 0 && nTokenPos < cchText )
		pCtrl->SetCellText( nRow, ++idxCol, sRowText.Tokenize( sDelimiters, nTokenPos ) );

	acedRetInt( nRow );
	return RSRSLT;
}

ADSRESULT Grid::InsertString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlGrid))
		return RSERR; //invalid input

	int nInsertIdx;
	if( !GetIntArgument( pArgs, nInsertIdx ) )
		return RSERR; //invalid input

	CString sRowText;
	if( !GetStringArgument( pArgs, sRowText ) )
		return RSERR; //invalid input

	CString sDelimiters = _T("\t");
	GetStringArgument( pArgs, sDelimiters, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxGridCtrl* pCtrl = (CArxGridCtrl*)pDlgControl->GetControlWnd();

	int nTokenPos = 0;
	int nRow = pCtrl->InsertItem( nInsertIdx, sRowText.Tokenize( sDelimiters, nTokenPos ) );
	if( nRow == -1 )
		return RSRSLT;
	int idxCol = 0;
	int cchText = sRowText.GetLength();
	while( nTokenPos >= 0 && nTokenPos < cchText )
		pCtrl->SetCellText( nRow, ++idxCol, sRowText.Tokenize( sDelimiters, nTokenPos ) );

	acedRetT();
	return RSRSLT;
}