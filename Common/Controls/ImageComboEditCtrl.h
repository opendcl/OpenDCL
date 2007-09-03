// ImageComboEditCtrlBase.h : header file
//

#pragma once

#include "GridCellEditCtrl.h"
#include "FilteredEditEx.h"
#include "GridCtrl.h"

class CInputFilter;


/////////////////////////////////////////////////////////////////////////////
// CImageComboEditCtrlBase class

class CImageComboEditCtrlBase : public CComboBoxEx, public CGridCellEditCtrl
{
	CStatic mClippingWnd;
	std::vector< tstring > mrsComboList;
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left, rcCell.top, rcCell.right, rcCell.top + 120 );
		}
public:
	CImageComboEditCtrlBase( CGridCtrl* pGridCtrl, int nRow, int nCol, DWORD dwComboStyle, CInputFilter* pFilter = NULL, UINT nID = 100 )
		: CComboBoxEx()
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
			SetImageList( pGridCtrl->GetImageList( LVSIL_SMALL ) );
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
				CString sListText = mrsComboList.at( idx ).c_str();
				if( idxMatch < 0 && sText == sListText )
					idxMatch = (int)idx;
				int nImage = (idx < rImage.size()? rImage.at( idx ) : -1);
				COMBOBOXEXITEM cbItem = 
				{
					(CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_OVERLAY | CBEIF_INDENT),
					idx,
					sListText.LockBuffer(),
					sListText.GetLength() + 1,
					nImage,
					nImage,
					2,
					0,
				};
				InsertItem( &cbItem );
			}
			SetWindowText( sText );
			CComboBox* pComboCtrl = GetComboBoxCtrl();
			ASSERT( pComboCtrl != NULL );
			if( pComboCtrl )
				pComboCtrl->SetEditSel( 0, -1 );
			if( idxMatch >= 0 )
				SetCurSel( idxMatch );
			SetFocus();
		}
	virtual ~CImageComboEditCtrlBase()
		{
			CEdit* pEditCtrl = GetEditCtrl();
			if( pEditCtrl )
			{
				CString sText;
				pEditCtrl->GetWindowText( sText );
				mpGridCtrl->SetCellText( mnRow, mnCol, sText );
			}
			else
			{
				CComboBox* pComboCtrl = GetComboBoxCtrl();
				ASSERT( pComboCtrl != NULL );
				if( pComboCtrl )
				{
					int nCurSel = pComboCtrl->GetCurSel();
					if( nCurSel >= 0 )
						mpGridCtrl->SetCellText( mnRow, mnCol, mrsComboList.at( nCurSel ).c_str() );
				}
			}
			DestroyWindow();
			mClippingWnd.DestroyWindow();
		}
};

class CImageComboDropdownListEditCtrl : public CImageComboEditCtrlBase
{
public:
	CImageComboDropdownListEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CInputFilter* pFilter = NULL, UINT nID = 100 )
		: CImageComboEditCtrlBase( pGridCtrl, nRow, nCol, CBS_DROPDOWNLIST, pFilter, nID )
		{
		}
};

class CImageComboDropdownEditCtrl : public CImageComboEditCtrlBase
{
public:
	CImageComboDropdownEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CInputFilter* pFilter = NULL, UINT nID = 100 )
		: CImageComboEditCtrlBase( pGridCtrl, nRow, nCol, CBS_DROPDOWN, pFilter, nID )
		{
		}
};
