// FontBox.cpp : implementation file
//

#include "Stdafx.h"
#include "ArxFontComboBoxCtrl.h"
#include "FontComboHandler.h"
#include "ComboStyles.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"
#include "Workspace.h"
#include "ControlPane.h"

const int SHX_FONTTYPE = 6;


/////////////////////////////////////////////////////////////////////////////
// CArxFontComboBoxCtrl

CArxFontComboBoxCtrl::CArxFontComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
																						bool bCreate /*= true*/ )
: CArxComboBoxCtrl( pTemplate, pPane, nID, new CFontComboHandler, false )
, mhwndList( NULL )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CArxFontComboBoxCtrl::~CArxFontComboBoxCtrl()
{
}

bool CArxFontComboBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess =
		__super::Create( pParentWnd, nID );

	if( bSuccess )
		mwndTip.Create( this );

	// We set the timer because its the only way we know when a selection
	// has changed - use for tip window
	SetTimer( 1, 200, NULL );

	return bSuccess;
}

CRect CArxFontComboBoxCtrl::GetWndRect() const
{
	CRect rcWnd = __super::GetWndRect();
	//switch( mpTemplate->GetLongProperty( Prop::ComboBoxStyle ) )
	//{
	//case 8: //font dropdown
	//	{
	//		long lListHeight = mpTemplate->GetLongProperty( Prop::DropDownHeight );
	//		long lNewHeight = 16 * ((lListHeight + 13) / 16) + 3; //make it an integral height, + 2 pixels for the border
	//		mpTemplate->SetLongProperty( Prop::DropDownHeight, lNewHeight );
	//		rcWnd.bottom += (lNewHeight - lListHeight); //to prevent autoscrolling
	//		break;
	//	}
	//}
	return rcWnd;
}

DWORD CArxFontComboBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	dwStyle |= (CBS_NOINTEGRALHEIGHT | CBS_OWNERDRAWFIXED);
	switch( mpTemplate->GetLongProperty( Prop::ComboBoxStyle ) )
	{
	case ComboStyle::FontDropList:
		dwStyle |= CBS_DROPDOWNLIST;
		break;
	case ComboStyle::FontSimpleList:
		dwStyle |= CBS_SIMPLE;
		dwStyle &= ~(DWORD)CBS_DROPDOWN;
		break;
	}

	return dwStyle;
}

bool CArxFontComboBoxCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	//switch( pProp->GetID() )
	//{
	//case Prop::DragnDropAllowDrop:
	//	{
	//		SetDragnDrop( pProp->GetBooleanValue() );
	//		break;
	//	}
	//}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CArxFontComboBoxCtrl, CArxComboBoxCtrl)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_WM_TIMER()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CArxFontComboBoxCtrl message handlers

void CArxFontComboBoxCtrl::OnCloseUp()
{
	mwndTip.ShowWindow( SW_HIDE );
	__super::OnCloseUp();
}

void CArxFontComboBoxCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	// Is combo open
	if (GetDroppedState( ))
	{
		int nSel = GetCurSel();
		int FirstVis = GetTopIndex();
		
		// Selected
		if (nSel != -1 && FirstVis <= nSel)
		{
			CRect rc;
			::GetWindowRect( mhwndList, &rc );
			//GetDroppedControlRect(rc);
			int itemHeight = GetItemHeight(0);
			int lastVis = FirstVis + (rc.Height() + 2) / itemHeight - 1;
			if (nSel <= lastVis)
			{
				int nHeight = itemHeight * ((nSel - FirstVis) + 1);
				
				CPoint pt(rc.right + 5,rc.top + nHeight);

				// Show tip in correct position
				CString str;
				GetLBText(nSel,str);
				mwndTip.ShowTips( pt, str );
			}
		}
	}
	__super::OnTimer(nIDEvent);
}

void CArxFontComboBoxCtrl::OnKillfocus() 
{
	mwndTip.ShowWindow( SW_HIDE );
}

LRESULT CArxFontComboBoxCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_CTLCOLORLISTBOX )
		mhwndList = (HWND)lParam;
	return __super::WindowProc(message, wParam, lParam);
}
