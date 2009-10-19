// FrameCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "FrameCtrl.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl

CFrameCtrl::CFrameCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mColorService( GetSysColor( COLOR_CAPTIONTEXT ), GetSysColor( COLOR_BTNFACE ) )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CFrameCtrl::~CFrameCtrl()
{
}

bool CFrameCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess )
		ModifyStyleEx( 0, WS_EX_TRANSPARENT );

	if( mpControlPane->GetThemeHelper() )
	{
		TDclFormPtr pParentForm = mpTemplate->GetOwnerForm()->GetParentForm();
		if( pParentForm )
		{
			TDclControlPtr pTabStrip = pParentForm->FindFirstControlOfType( CtlTabStrip );
			if( pTabStrip && pTabStrip->GetBooleanProperty( Prop::UseVisualStyle ) )
				mColorService.SetBackgroundColor( GetSysColor( COLOR_WINDOW ) );
		}
	}

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CFrameCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (WS_CLIPSIBLINGS | BS_GROUPBOX);
	return dwStyle;
}


BEGIN_MESSAGE_MAP(CFrameCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl message handlers

BOOL CFrameCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CFrameCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	HBRUSH hbrBackground = mColorService.CtlColor( pDC, nCtlColor );
	//return NULL;
	if( (LOBYTE(LOWORD(GetVersion())) < 6) &&
			!(GetThemeHelper() && mpTemplate->GetBooleanProperty( Prop::UseVisualStyle )) )
		return NULL; //must use class brush in XP when themes are inactive (else XP paints text same color as background)
	return hbrBackground;
	//return mColorService.GetTransparentBrush();
}

void CFrameCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CFrameCtrl::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	//lpwndpos->flags |= SWP_NOZORDER;
	__super::OnWindowPosChanging(lpwndpos);
}
