// ImageComboEditCtrlBase.h : header file
//

#pragma once

#include "GridCellEditCtrl.h"
#include "FilteredComboExCtrl.h"
#include "GridCtrl.h"
#include "ComboHandler.h"


/////////////////////////////////////////////////////////////////////////////
// CImageComboEditCtrlBase class

class CImageComboEditCtrlBase : public CFilteredComboExCtrl, public CGridCellEditCtrl
{
	CComboHandler* mpHandler;
	CStatic mClippingWnd;
	std::vector< tstring > mrsComboList;
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left, rcCell.top, rcCell.right, rcCell.top + 120 );
		}
public:
	CImageComboEditCtrlBase( CGridCtrl* pGridCtrl, int nRow, int nCol, DWORD dwComboStyle, CComboHandler* pHandler = NULL, UINT nID = 100 )
		: CFilteredComboExCtrl( NULL )
		, CGridCellEditCtrl( pGridCtrl, nRow, nCol )
		, mpHandler( pHandler )
		, mClippingWnd()
		{
			CRect rcCell = pGridCtrl->GetCellRect( nRow, nCol );
			rcCell.DeflateRect( 2, 2 );
			CRect rcCtrl = CalcRect( rcCell );
			mClippingWnd.Create( _T(""), WS_CHILD, rcCell, pGridCtrl );
			rcCtrl.MoveToXY( 0, 0 );
			if( pHandler )
			{
				if( pHandler->IsAutoSorted() )
					dwComboStyle |= CBS_SORT;
				if( pHandler->IsOwnerDrawn() )
					dwComboStyle |= (pHandler->GetItemHeight() > 0? CBS_OWNERDRAWFIXED : CBS_OWNERDRAWVARIABLE);
			}
			Create( &mClippingWnd,
							rcCtrl,
							dwComboStyle,
							100 );
			SetFont( pGridCtrl->GetFont() );
			SetImageList( pGridCtrl->GetImageList( LVSIL_SMALL ) );
			GetWindowRect( &rcCtrl );
			mClippingWnd.ScreenToClient( &rcCtrl );
			CRect rcClip;
			rcClip.IntersectRect( &rcCtrl, CRect( 0, 0, rcCell.Width(), rcCell.Height() ) );
			mClippingWnd.SetWindowPos( NULL, 0, 0, rcClip.Width(), rcClip.Height(),
																 (SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING) );
			rcCtrl.MoveToY( rcCtrl.top + rcClip.Height() - rcCtrl.Height() );
			SetWindowPos( NULL, 0, rcCtrl.top, 0, 0,
										(SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING) );
			mClippingWnd.ShowWindow( SW_SHOW );
			CString sText = pGridCtrl->GetCellText( nRow, nCol );
			int idxMatch = -1;
			if( pHandler && pHandler->PopulateList( this ) )
				idxMatch = FindStringExact( 0, sText );
			else
			{
				std::vector< int > rImage;
				pGridCtrl->GetCellComboListItems( nRow, nCol, mrsComboList, rImage );
				for( size_t idx = 0; idx < mrsComboList.size(); ++idx )
				{
					CString sListText = mrsComboList.at( idx ).c_str();
					if( idxMatch < 0 && sText == sListText )
						idxMatch = (int)idx;
					int nImage = (idx < rImage.size()? rImage.at( idx ) : -1);
					COMBOBOXEXITEM cbi = 
					{
						(CBEIF_TEXT | CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_INDENT),
						idx,
						sListText.LockBuffer(),
						sListText.GetLength() + 1,
						nImage,
						nImage,
						-1,
						0,
					};
					InsertItem( &cbi );
				}
			}
			if( GetCount() == 0 )
			{
				CRect rcWnd;
				GetWindowRect( &rcWnd );
				SetWindowPos( NULL, 0, 0, rcWnd.Width(), rcWnd.Height(), (SWP_NOZORDER | SWP_NOMOVE) );
			}
			SetWindowText( sText );
			CComboBox* pComboCtrl = GetComboBoxCtrl();
			ASSERT( pComboCtrl != NULL );
			if( pComboCtrl )
				pComboCtrl->SetEditSel( 0, -1 );
			if( idxMatch >= 0 )
				SetCurSel( idxMatch );
			SetFocus();
			if( (GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST )
				ShowDropDown();
		}
	virtual ~CImageComboEditCtrlBase()
		{
			CEdit* pEditCtrl = GetEditCtrl();
			if( pEditCtrl )
			{
				CString sText;
				pEditCtrl->GetWindowText( sText );
				CInputFilter* pFilter = GetInputFilter();
				if( pFilter )
				{
					if( !pFilter->OnValidateInput( sText ) )
					{
						sText = pFilter->OnBadInput();
						pFilter->ConvertForDisplay( sText );
					}
				}
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
			CComboHandler* pHandler = mpHandler;
			mClippingWnd.DestroyWindow();
			mpHandler = NULL;
			delete pHandler;
		}
	virtual CInputFilter* GetInputFilter() { return mpHandler? mpHandler->GetInputFilter() : NULL; }

protected:
	static const AFX_MSGMAP* PASCAL GetThisMessageMap()
		{
			typedef CImageComboEditCtrlBase ThisClass;
			typedef CFilteredComboExCtrl TheBaseClass;
			static const AFX_MSGMAP_ENTRY _messageEntries[] =
			{
				ON_WM_MEASUREITEM_REFLECT()
				ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CImageComboEditCtrlBase::OnKillFocus)
				ON_CONTROL_REFLECT(CBN_CLOSEUP, &CImageComboEditCtrlBase::OnCloseUp)
				{0, 0, 0, 0, AfxSig_end, (AFX_PMSG)0 },
			};
			static const AFX_MSGMAP messageMap =  { &__super::GetThisMessageMap, &_messageEntries[0] };
			return &messageMap; \
		}
	virtual const AFX_MSGMAP* GetMessageMap() const
		{
			return GetThisMessageMap();
		}

protected:
	virtual afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
		{
			if( mpHandler )
				mpHandler->DrawItem( this, lpDrawItemStruct );
		}
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
		{
			assert( mpHandler != NULL ); //should never call this function if there is no handler
			if( !mpHandler )
				return;
			UINT nItemHeight = mpHandler->GetItemHeight();
			if( nItemHeight > 0 )
				lpMeasureItemStruct->itemHeight = nItemHeight;
		}
	afx_msg void OnKillFocus()
		{
			mpGridCtrl->PostMessage( WM_CANCELMODE, 0, 0 );
			//mpGridCtrl->SendMessage( WM_COMMAND, MAKEWPARAM(0, CBN_KILLFOCUS), (LPARAM)m_hWnd );
		}
	afx_msg void OnCloseUp()
		{
			if( GetFocus() == GetComboBoxCtrl() && (GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST )
				mpGridCtrl->PostMessage( WM_CANCELMODE, 0, 0 );
		}
};

class CImageComboDropdownListEditCtrl : public CImageComboEditCtrlBase
{
public:
	CImageComboDropdownListEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CComboHandler* pHandler = NULL, UINT nID = 100 )
		: CImageComboEditCtrlBase( pGridCtrl, nRow, nCol, CBS_DROPDOWNLIST, pHandler, nID )
		{
		}
};

class CImageComboDropdownEditCtrl : public CImageComboEditCtrlBase
{
public:
	CImageComboDropdownEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CComboHandler* pHandler = NULL, UINT nID = 100 )
		: CImageComboEditCtrlBase( pGridCtrl, nRow, nCol, CBS_DROPDOWN, pHandler, nID )
		{
		}
};
