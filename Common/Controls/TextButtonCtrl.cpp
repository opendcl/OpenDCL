// TextButtonCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TextButtonCtrl.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "ControlPane.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CTextButtonCtrl

CTextButtonCtrl::CTextButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CTextButtonCtrl::~CTextButtonCtrl()
{
}

bool CTextButtonCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CTextButtonCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (/*WS_CLIPSIBLINGS | */BS_PUSHBUTTON | BS_MULTILINE | BS_NOTIFY);
	return dwStyle;
}

bool CTextButtonCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Caption:
		{
			SetWindowText( pProp->GetStringValue() );
		}
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CTextButtonCtrl, CButton)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_NCPAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextButtonCtrl message handlers

BOOL CTextButtonCtrl::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);
	return __super::PreTranslateMessage(pMsg);
}

HBRUSH CTextButtonCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	return HandleCtlColor( pDC, nCtlColor );
}

BOOL CTextButtonCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CTextButtonCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

void CTextButtonCtrl::OnSetFocus(CWnd* pOldWnd)
{
	if( (GetStyle() & BS_DEFPUSHBUTTON) == 0 )
		ModifyStyle( BS_PUSHBUTTON, BS_DEFPUSHBUTTON, SWP_FRAMECHANGED );
	__super::OnSetFocus(pOldWnd);
}

void CTextButtonCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	__super::OnKillFocus(pNewWnd);
	if( GetStyle() & BS_DEFPUSHBUTTON )
		ModifyStyle( BS_DEFPUSHBUTTON, BS_PUSHBUTTON, SWP_FRAMECHANGED );
}

void CTextButtonCtrl::OnNcPaint() 
{
	DWORD dwStyle = GetStyle();
	if( GetFocus() != this )
	{
		if( dwStyle & BS_DEFPUSHBUTTON )
			ModifyStyle( BS_DEFPUSHBUTTON, BS_PUSHBUTTON, SWP_FRAMECHANGED );
	}
	else
	{
		if( (dwStyle & BS_DEFPUSHBUTTON) == 0 )
			ModifyStyle( BS_PUSHBUTTON, BS_DEFPUSHBUTTON, SWP_FRAMECHANGED );
	}
	__super::OnNcPaint();
}
