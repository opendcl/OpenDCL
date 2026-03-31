// Methods_ListView.cpp : implementation file
//

#include "stdafx.h"
#include "Methods_ListView.h"
#include "ArgumentsRetrieval.h"
#include "ArxListViewCtrl.h"
#include "ControlTypes.h"
#include "Resource.h"
#include "ArxWorkspace.h"


static void ReturnRowCol( int nRow, int nCol )
{
	resbuf rbColumn = { NULL, RTSHORT };
	rbColumn.resval.rint = nCol;
	resbuf rbRow = { &rbColumn, RTSHORT };
	rbRow.resval.rint = nRow;
	acedRetList( &rbRow );
}

//This function performs the same task as CString::Tokenize, except this one accepts empty tokens
static CString TokenizeAllowNull( const CString& sSubject, LPCTSTR pszTokens, int& idxStart )
{
	if( sSubject.IsEmpty() )
	{
		idxStart = -1;
		return CString();
	}
	int cchToken = sSubject.Mid( idxStart ).FindOneOf( pszTokens );
	if( cchToken == 0 )
		cchToken = sSubject.Mid( ++idxStart ).FindOneOf( pszTokens );
	if( cchToken < 0 )
		cchToken = sSubject.GetLength() - idxStart;
	int idxFirst = idxStart;
	idxStart += cchToken;
	return sSubject.Mid( idxFirst, cchToken );
}


ADSRESULT ListView::AddColumns()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	if( !GetListBeginArgument( pArgs ) )
		return RSERR; //invalid input

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	bool bNestedLists = GetListBeginArgument( pArgs, true );

	CString sCaption;
	while( GetStringArgument( pArgs, sCaption, true ) )
	{
		int nFormat = LVCFMT_CENTER;
		int nColWidth = -1;
		int nImage = -1;
		int nJustification = -1;
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

		CHeaderCtrl* pHeaderCtrl = pCtrl->GetHeaderCtrl();
		if( !pHeaderCtrl )
			return RSRSLT;

		LVCOLUMN column;
		column.mask = (LVCF_TEXT | LVCF_FMT);
		column.pszText = sCaption.LockBuffer();
		column.fmt = nFormat;
		if (nColWidth != -1)
		{
			column.mask |= LVCF_WIDTH;
			column.cx = pDlgControl->FromDIP( nColWidth );
		}
		if (nImage != -1)
		{
			column.mask |= LVCF_IMAGE;
			column.iImage = nImage;
		}
		if( -1 == pCtrl->InsertColumn( pHeaderCtrl->GetItemCount(), &column ) )
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

ADSRESULT ListView::AddItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nImage = -1;
	CString sItemText;
	PropVal::TCStringArray rsText;
	if( GetListBeginArgument( pArgs, true ) )
	{
		GetIntArgument( pArgs, nImage, true );

		if( !GetStringArgument( pArgs, sItemText ) )
			return RSERR; //invalid input

		GetStringArrayArgument( pArgs, rsText, true );

		if( !GetListEndArgument( pArgs ) )
			return RSERR; //invalid input
	}
	else
	{
		GetIntArgument( pArgs, nImage, true );

		if( !GetStringArgument( pArgs, sItemText ) )
			return RSERR; //invalid input

		GetStringArrayArgument( pArgs, rsText, true );
	}

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	int nRow = pCtrl->InsertItem( pCtrl->GetItemCount(), sItemText, nImage );
	if( nRow == -1 )
		return RSRSLT;
	for( size_t idx = rsText.size(); idx > 0; --idx )
		pCtrl->SetItemText( nRow, idx, rsText[idx - 1] );

	acedRetInt( nRow );
	return RSRSLT;
}

ADSRESULT ListView::InsertItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nInsertIdx;
	if( !GetIntArgument( pArgs, nInsertIdx ) )
		return RSERR; //invalid input

	bool bListBegin = GetListBeginArgument( pArgs, true );

	int nImage = -1;
	GetIntArgument( pArgs, nImage, true );

	CString sItemText;
	if( !GetStringArgument( pArgs, sItemText ) )
		return RSERR; //invalid input

	PropVal::TCStringArray rsText;
	GetStringArrayArgument( pArgs, rsText, true );

	if( bListBegin && !GetListEndArgument( pArgs ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	int nRow = pCtrl->InsertItem( nInsertIdx, sItemText, nImage );
	if( nRow == -1 )
		return RSRSLT;
	for( size_t idx = rsText.size(); idx > 0; --idx )
		pCtrl->SetItemText( nRow, idx, rsText[idx - 1] );

	acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::Clear()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->DeleteAllItems() )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::GetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	theArxWorkspace.RetHandle( pCtrl->GetItemData( nItem ) );
	return RSRSLT;
}

ADSRESULT ListView::SetItemData()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	long lData = -1;
	if( !GetLongArgument( pArgs, lData ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SetItemData( nItem, lData ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::GetItemImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = 0;
	GetIntArgument( pArgs, nCol, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetItemImage( nRow, nCol ) );
	return RSRSLT;
}

ADSRESULT ListView::GetItemText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = 0;
	GetIntArgument( pArgs, nCol, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	acedRetStr( pCtrl->GetItemText( nRow, nCol ) );
	return RSRSLT;
}

ADSRESULT ListView::GetSelCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListView ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pCtrl->GetSelectedCount() );
	return RSRSLT;
}

ADSRESULT ListView::GetSelectedItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListView ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

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

ADSRESULT ListView::GetSelectedNths()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if( !GetDlgControlArgument( pArgs, pDlgControl, CtlListView ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

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

ADSRESULT ListView::GetRowItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nItem = -1;
	if( !GetIntArgument( pArgs, nItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	CHeaderCtrl* pHeaderCtrl = pCtrl->GetHeaderCtrl();
	size_t ctCol = 1;
	if( pHeaderCtrl )
		ctCol = pHeaderCtrl->GetItemCount();

	resbuf* prbResult = NULL;
	resbuf* prbTail = NULL;
	for( size_t idxCol = 0; idxCol < ctCol; ++idxCol )
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
		acutNewString( pCtrl->GetItemText( nItem, idxCol ), prbTail->resval.rstring );
	}
	acedRetList( prbResult );
	acutRelRb( prbResult );
	return RSRSLT;
}

ADSRESULT ListView::GetColumnItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

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
		acutNewString( pCtrl->GetItemText( idxRow, nCol ), prbTail->resval.rstring );
	}
	acedRetList( prbResult );
	acutRelRb( prbResult );
	return RSRSLT;
}

ADSRESULT ListView::SetItemText()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = 0;
	GetIntArgument( pArgs, nCol, true );

	CString sText;
	if( !GetStringArgument( pArgs, sText ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SetItemText( nRow, nCol, sText ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::HitPointTest()
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

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	LVHITTESTINFO lvhti = { { pDlgControl->FromDIP( nX ), pDlgControl->FromDIP( nY ) } };
	if( -1 != pCtrl->SubItemHitTest( &lvhti ) )
		ReturnRowCol( lvhti.iItem, lvhti.iSubItem );
	return RSRSLT;
}

ADSRESULT ListView::SetItemImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	int nCol = 0;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	int nImage = nCol;
	if( !GetIntArgument( pArgs, nImage, true ) )
		nCol = 0;

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	pCtrl->SetItemImage( nRow, nCol, nImage );
	acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::GetColumnWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pDlgControl->ToDIP( pCtrl->GetColumnWidth( nCol ) ) );
	return RSRSLT;
}

ADSRESULT ListView::SetColumnWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	int nColWidth = -1;
	if( !GetIntArgument( pArgs, nColWidth ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SetColumnWidth( nCol, pDlgControl->FromDIP( nColWidth ) ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::CalcColumnWidth()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	CString sText;
	if( !GetStringArgument( pArgs, sText ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	acedRetInt( pDlgControl->ToDIP( pCtrl->GetStringWidth( sText ) ) );
	return RSRSLT;
}

ADSRESULT ListView::Arrange()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
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

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	if( pCtrl->Arrange( nStyle ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::DeleteItem()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->DeleteItem( nRow ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::DeleteItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	PropVal::TIntArray rnItem;
	if( !GetIntArrayArgument( pArgs, rnItem ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	//sort the indices in ascending order first
	bool bStillSorting = (rnItem.size() > 1);
	while( bStillSorting )
	{
		bStillSorting = false;
		for( size_t idx = 1; idx < rnItem.size(); ++idx )
		{
			int nLow = rnItem[idx - 1];
			int nHigh = rnItem[idx];
			if( nLow > nHigh )
			{
				bStillSorting = true;
				rnItem[idx - 1] = nHigh;
				rnItem[idx] = nLow;
			}
		}
	}
	for( size_t idx = rnItem.size(); idx > 0; --idx )
		pCtrl->DeleteItem( rnItem[idx - 1] );
	acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::DeleteColumn()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->DeleteColumn( nCol ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::DeleteColumns()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	PropVal::TIntArray rnCol;
	if( !GetIntArrayArgument( pArgs, rnCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	//sort the indices in ascending order first
	bool bStillSorting = (rnCol.size() > 1);
	while( bStillSorting )
	{
		bStillSorting = false;
		for( size_t idx = 1; idx < rnCol.size(); ++idx )
		{
			int nLow = rnCol[idx - 1];
			int nHigh = rnCol[idx];
			if( nLow > nHigh )
			{
				bStillSorting = true;
				rnCol[idx - 1] = nHigh;
				rnCol[idx] = nLow;
			}
		}
	}
	for( size_t idx = rnCol.size(); idx > 0; --idx )
		pCtrl->DeleteColumn( rnCol[idx - 1] );
	acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::FillList()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->DeleteAllItems();

	if( GetNilArgument( pArgs, true ) )
	{
		if( !AssertOutOfArgs( pArgs ) )
			return RSERR;
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
			{
				pCtrl->SetItemText( nRow, nCol, sCaption );
				pCtrl->SetItemImage( nRow, nCol, nImage );
			}
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

ADSRESULT ListView::GetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	acedRetInt( pCtrl->GetSelectionMark() );
	return RSRSLT;
}

ADSRESULT ListView::SortTextItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	bool bAscending = true;
	GetBoolArgument( pArgs, bAscending, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SortTextItems( nCol, bAscending ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::SortNumericItems()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	bool bAscending = true;
	GetBoolArgument( pArgs, bAscending, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	if( pCtrl->SortNumericItems( nCol, bAscending ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::SetColumnImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	int nImage = -1;
	if( !GetIntArgument( pArgs, nImage ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	CHeaderCtrl* pHeader = pCtrl->GetHeaderCtrl();
	if( !pHeader )
		return RSRSLT;

	CImageList* pImageList = pHeader->GetImageList();
	if( nImage != -1 && (!pImageList || nImage < 0 || nImage >= pImageList->GetImageCount()) )
		return HandleArgValueError( pArgs, IDS_ARG_IMAGENOTFOUND );

	HDITEM hdi = { HDI_IMAGE };
	hdi.iImage = nImage;
	if( nImage == -1)
		nImage = I_IMAGENONE;
	if( pHeader->SetItem( nCol, &hdi ) )
		acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::GetColumnImage()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nCol = -1;
	if( !GetIntArgument( pArgs, nCol ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	CHeaderCtrl* pHeader = pCtrl->GetHeaderCtrl();
	if( !pHeader )
		return RSRSLT;

	HDITEM hdi = { HDI_IMAGE };
	if( pHeader->GetItem( nCol, &hdi ) )
		acedRetInt( hdi.iImage );
	return RSRSLT;
}

ADSRESULT ListView::GetCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	acedRetInt( pCtrl->GetItemCount() );
	return RSRSLT;
}

ADSRESULT ListView::GetColumnCount()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	CHeaderCtrl* pHeader = pCtrl->GetHeaderCtrl();
	if( pHeader )
		acedRetInt( pHeader->GetItemCount() );
	else
		acedRetInt( 1 );
	return RSRSLT;
}

ADSRESULT ListView::SetCurSel()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	if( nRow > -1 )
	{
		pCtrl->SetItem( nRow, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED, 0 );
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
	pCtrl->SetSelectionMark( nRow );
	acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::StartLabelEdit()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	int nRow = -1;
	if( !GetIntArgument( pArgs, nRow ) )
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	pCtrl->SetFocus();
	pCtrl->EditLabel( nRow );
	acedRetT();
	return RSRSLT;
}

ADSRESULT ListView::CancelLabelEdit()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();
	CEdit* pEditCtrl = pCtrl->GetEditControl();
	if( pEditCtrl )
	{
		pEditCtrl->ShowWindow( SW_HIDE );
		acedRetT();
	}
	return RSRSLT;
}

ADSRESULT ListView::AddString()
{
	struct resbuf *pArgs =acedGetArgs () ;

	CDialogControl* pDlgControl = NULL;
	if (!GetDlgControlArgument (pArgs, pDlgControl, CtlListView))
		return RSERR; //invalid input

	CString sRowText;
	if( !GetStringArgument( pArgs, sRowText ) )
		return RSERR; //invalid input

	CString sDelimiters = _T("\t");
	GetStringArgument( pArgs, sDelimiters, true );

	if( !AssertOutOfArgs( pArgs ) )
		return RSERR;

	CArxListViewCtrl* pCtrl = (CArxListViewCtrl*)pDlgControl->GetControlWnd();

	int nTokenPos = 0;
	int nRow = pCtrl->InsertItem( pCtrl->GetItemCount(), TokenizeAllowNull( sRowText, sDelimiters, nTokenPos ) );
	if( nRow == -1 )
		return RSRSLT;
	int idxCol = 0;
	int cchText = sRowText.GetLength();
	while( nTokenPos >= 0 && nTokenPos < cchText )
		pCtrl->SetItemText( nRow, ++idxCol, TokenizeAllowNull( sRowText, sDelimiters, nTokenPos ) );

	acedRetInt( nRow );
	return RSRSLT;
}
