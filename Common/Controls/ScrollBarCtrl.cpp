// ScrollBarCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ScrollBarCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CScrollBarCtrl

CScrollBarCtrl::CScrollBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CScrollBarCtrl::~CScrollBarCtrl()
{
}

bool CScrollBarCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);
	if( bSuccess )
	{
	#ifdef _UNICODE
		BOOL bUnicode = TRUE;
	#else
		BOOL bUnicode = FALSE;
	#endif
		SendMessage( CCM_SETUNICODEFORMAT, (WPARAM)bUnicode, 0 );
	}

	if( bSuccess )
		ShowScrollBar();

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CScrollBarCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (SBS_BOTTOMALIGN);

	if( mpTemplate->GetLongProperty( Prop::Orientation ) == 0 )
		dwStyle |= SBS_HORZ;
	else
		dwStyle |= SBS_VERT;

	return dwStyle;
}

bool CScrollBarCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Value:
		SetScrollPos( pProp->GetLongValue() );
		break;
	case Prop::MinValue:
		SetScrollRange( pProp->GetLongValue(), mpTemplate->GetLongProperty( Prop::MaxValue ) );
		break;
	case Prop::MaxValue:
		if( !IsEnumeratingProperties() )
			SetScrollRange( mpTemplate->GetLongProperty( Prop::MinValue ), pProp->GetLongValue() );
		break;
	case Prop::Orientation:
		if( pProp->GetLongValue() == 0 )
			ModifyStyle( SBS_VERT, SBS_HORZ, SWP_FRAMECHANGED );
		else
			ModifyStyle( SBS_HORZ, SBS_VERT, SWP_FRAMECHANGED );
		break;
	case Prop::SmallChange:
		break;
	case Prop::LargeChange:
		break;
	}
	return !bFailed;
}

bool CScrollBarCtrl::OnApplyForegroundColor( TPropertyPtr pProp )
{
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
	{
		pColorService->SetForegroundColor( pProp->GetLongValue() );
		pColorService->SetBackgroundColor( pProp->GetLongValue() );
	}
	OnNeedRepaint();
	return true;
}

void CScrollBarCtrl::OnScroll(UINT nSBCode, UINT nPos) 
{
	bool bDoneScrolling = false;
	int nMinPos;
	int nMaxPos;
	GetScrollRange( &nMinPos, &nMaxPos );
	int nNewPos = mpTemplate->GetLongProperty( Prop::Value );
	
	switch( nSBCode )
	{
	case 0: // lesser or min arrow click
		nNewPos -= mpTemplate->GetLongProperty( Prop::SmallChange );
		if( nNewPos < nMinPos )
			nNewPos = nMinPos;
		break;
	case 1: // greater or max arrow click
		nNewPos += mpTemplate->GetLongProperty( Prop::SmallChange );
		if( nNewPos > nMaxPos )
			nNewPos = nMaxPos;
		break;
	case 2: // user has clicked scroll area left/above of Scroll Button indicator
		nNewPos -= mpTemplate->GetLongProperty( Prop::LargeChange );
		if( nNewPos < nMinPos )
			nNewPos = nMinPos;
		break;
	case 3: // user has click scroll area right/below of Scroll Button indicator
		nNewPos += mpTemplate->GetLongProperty( Prop::LargeChange );
		if( nNewPos > nMaxPos )
			nNewPos = nMaxPos;
		break;
	case 5: // user is scrolling the scrollbar
		nNewPos = nPos;
		break;
	case 4: // user is done scrolling the scrollbar
	case 8: // user is done scrolling the scrollbar
		bDoneScrolling = true;
		break;
	default:
		break;
	}
	
	TPropertyPtr pValueProp = mpTemplate->GetPropertyObject( Prop::Value );
	pValueProp->SetLongValue( nNewPos );
	ApplyProperty( pValueProp );
}


BEGIN_MESSAGE_MAP(CScrollBarCtrl, CScrollBar)
	ON_WM_HSCROLL_REFLECT()
	ON_WM_VSCROLL_REFLECT()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CScrollBarCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CScrollBarCtrl message handlers

LRESULT CScrollBarCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

void CScrollBarCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	__super::OnSetFocus(pOldWnd);
	if( pOldWnd )
		pOldWnd->SetFocus();
}


void CScrollBarCtrl::HScroll(UINT nSBCode, UINT nPos) 
{
	OnScroll(nSBCode, nPos);
}


void CScrollBarCtrl::VScroll(UINT nSBCode, UINT nPos) 
{
	OnScroll(nSBCode, nPos);
}

void CScrollBarCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CScrollBarCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CScrollBarCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CScrollBarCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}
