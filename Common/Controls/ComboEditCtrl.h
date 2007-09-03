// ComboEditCtrlBase.h : header file
//

#pragma once

#include "GridCellEditCtrl.h"
#include "FilteredEditEx.h"
#include "GridCtrl.h"

class CInputFilter;

/////////////////////////////////////////////////////////////////////////////
// CComboEditCtrlBase class

class CComboEditCtrlBase : public CComboBox, public CGridCellEditCtrl
{
	CStatic mClippingWnd;
	std::vector< tstring > mrsComboList;
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left, rcCell.top, rcCell.right, rcCell.top + 120 );
		}
public:
	CComboEditCtrlBase( CGridCtrl* pGridCtrl, int nRow, int nCol, DWORD dwComboStyle, CInputFilter* pFilter = NULL, UINT nID = 100 )
		: CComboBox()
		, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		{
			CRect rcCell = pGridCtrl->GetCellRect( nRow, nCol );
			rcCell.DeflateRect( 2, 2 );
			CRect rcCtrl = CalcRect( rcCell );
			mClippingWnd.Create( _T(""), WS_CHILD, rcCell, pGridCtrl );
			rcCtrl.MoveToXY( 0, 0 );
			Create( (WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_AUTOHSCROLL | dwComboStyle),
							rcCtrl,
							&mClippingWnd,
							100 );
			SetFont( pGridCtrl->GetFont() );
			GetWindowRect( &rcCtrl );
			mClippingWnd.ScreenToClient( &rcCtrl );
			CRect rcClip;
			rcClip.IntersectRect( &rcCtrl, &CRect( 0, 0, rcCell.Width(), rcCell.Height() ) );
			mClippingWnd.SetWindowPos( NULL, 0, 0, rcClip.Width(), rcClip.Height(),
																 (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING) );
			rcCtrl.MoveToY( rcCtrl.top + rcClip.Height() - rcCtrl.Height() );
			SetWindowPos( NULL, 0, rcCtrl.top, 0, 0,
										(SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING) );
			mClippingWnd.ShowWindow( SW_SHOW );
			CString sText = pGridCtrl->GetCellText( nRow, nCol );
			std::vector< int > rImage;
			pGridCtrl->GetCellComboListItems( nRow, nCol, mrsComboList, rImage );
			int idxMatch = -1;
			for( size_t idx = 0; idx < mrsComboList.size(); ++idx )
			{
				LPCTSTR pszText = mrsComboList.at( idx ).c_str();
				if( idxMatch < 0 && sText == pszText )
					idxMatch = (int)idx;
				AddString( pszText );
			}
			SetWindowText( sText );
			SetEditSel( 0, -1 );
			if( idxMatch >= 0 )
				SetCurSel( idxMatch );
			SetFocus();
		}
	virtual ~CComboEditCtrlBase()
		{
			CString sText;
			GetWindowText( sText );
			mpGridCtrl->SetCellText( mnRow, mnCol, sText );
			DestroyWindow();
			mClippingWnd.DestroyWindow();
		}
};

class CComboDropdownListEditCtrl : public CComboEditCtrlBase
{
public:
	CComboDropdownListEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CInputFilter* pFilter = NULL, UINT nID = 100 )
		: CComboEditCtrlBase( pGridCtrl, nRow, nCol, CBS_DROPDOWNLIST, pFilter, nID )
		{
		}
};

class CComboDropdownEditCtrl : public CComboEditCtrlBase
{
public:
	CComboDropdownEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CInputFilter* pFilter = NULL, UINT nID = 100 )
		: CComboEditCtrlBase( pGridCtrl, nRow, nCol, CBS_DROPDOWN, pFilter, nID )
		{
		}
};
