// ComboEditCtrlBase.h : header file
//

#pragma once

#include "GridCellEditCtrl.h"
#include "FilteredComboCtrl.h"
#include "GridCtrl.h"
#include "ComboHandler.h"


/////////////////////////////////////////////////////////////////////////////
// CComboEditCtrlBase class

class CComboEditCtrlBase : public CFilteredComboCtrl, public CGridCellEditCtrl
{
	CComboHandler* mpHandler;
	CStatic mClippingWnd;
	std::vector< tstring > mrsComboList;
	static CRect CalcRect( const CRect& rcCell )
		{
			return CRect( rcCell.left, rcCell.top, rcCell.right, rcCell.top + 120 );
		}
public:
	CComboEditCtrlBase( CGridCtrl* pGridCtrl, int nRow, int nCol, DWORD dwComboStyle, CComboHandler* pHandler = NULL, UINT nID = 100 )
		: CFilteredComboCtrl( NULL )
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
			dwComboStyle |= (WS_VSCROLL | CBS_AUTOHSCROLL);
			Create( &mClippingWnd,
							rcCtrl,
							dwComboStyle,
							100 );
			SetFont( pGridCtrl->GetFont() );
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
					LPCTSTR pszText = mrsComboList.at( idx ).c_str();
					if( idxMatch < 0 && sText == pszText )
						idxMatch = (int)idx;
					AddString( pszText );
				}
			}
			if( GetCount() == 0 )
			{
				CRect rcWnd;
				GetWindowRect( &rcWnd );
				SetWindowPos( NULL, 0, 0, rcWnd.Width(), rcWnd.Height(), (SWP_NOZORDER | SWP_NOMOVE) );
			}
			SetWindowText( sText );
			SetEditSel( 0, -1 );
			if( idxMatch >= 0 )
				SetCurSel( idxMatch );
			SetFocus();
			if( (GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST )
				ShowDropDown();
		}
	virtual ~CComboEditCtrlBase()
		{
			CString sText;
			GetWindowText( sText );
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
			mClippingWnd.DestroyWindow();
			delete mpHandler;
		}
	virtual CInputFilter* GetInputFilter() { return mpHandler? mpHandler->GetInputFilter() : NULL; }

protected:
	static const AFX_MSGMAP* PASCAL GetThisMessageMap()
		{
			typedef CComboEditCtrlBase ThisClass;
			typedef CFilteredComboCtrl TheBaseClass;
			static const AFX_MSGMAP_ENTRY _messageEntries[] =
			{
				ON_WM_MEASUREITEM_REFLECT()
				ON_CONTROL_REFLECT(CBN_KILLFOCUS, &CComboEditCtrlBase::OnKillFocus)
				ON_CONTROL_REFLECT(CBN_CLOSEUP, &CComboEditCtrlBase::OnCbnCloseup)
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
				lpMeasureItemStruct->itemHeight = host()->FromDIP(nItemHeight);
		}
	afx_msg void OnKillFocus()
		{
			mpGridCtrl->PostMessage( WM_CANCELMODE, 0, 0 );
			//mpGridCtrl->SendMessage( WM_COMMAND, MAKEWPARAM(0, CBN_KILLFOCUS), (LPARAM)m_hWnd );
		}
	afx_msg void OnCbnCloseup()
		{
			if( mpHandler )
				mpHandler->OnDropdownClose( this );
			if( ::GetFocus() == m_hWnd && (GetStyle() & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST )
				mpGridCtrl->PostMessage( WM_CANCELMODE, 0, 0 );
		}
};

class CComboDropdownListEditCtrl : public CComboEditCtrlBase
{
public:
	CComboDropdownListEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CComboHandler* pHandler = NULL, UINT nID = 100 )
		: CComboEditCtrlBase( pGridCtrl, nRow, nCol, CBS_DROPDOWNLIST, pHandler, nID )
		{
		}
};

class CComboDropdownEditCtrl : public CComboEditCtrlBase
{
public:
	CComboDropdownEditCtrl( CGridCtrl* pGridCtrl, int nRow, int nCol, CComboHandler* pHandler = NULL, UINT nID = 100 )
		: CComboEditCtrlBase( pGridCtrl, nRow, nCol, CBS_DROPDOWN, pHandler, nID )
		{
		}
};
