#include "stdafx.h"
#include "ButtonCtrl.h"
#include "AcadColorTable.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "ToolTips.h"
#include "SharedRes.h"


/////////////////////////////////////////////////////////////////////////////
// CButtonCtrl

CButtonCtrl::CButtonCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	m_pStaticBrush = new CBrush();
	m_nDirectory		= 0;
	m_bHasBorder		= TRUE;
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CButtonCtrl::~CButtonCtrl()
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
}

bool CButtonCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (CXPStyleButtonST::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);
	SetThemeHelper( mpControlPane->GetThemeHelper() );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	//if( mpTemplate->GetLngProperty( nAutoSize ) > 0 )
	//	SizeToContent();

	if (mToolTip.m_hWnd == NULL)
	{
		mToolTip.Create(this);
		mToolTip.Activate(FALSE);
	}
	SetToolTipEx(this, mToolTip, GetTemplate());

	return bSuccess;
}

bool CButtonCtrl::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case nAcadColor:
		{
			SetAcadColor( pProp->GetLongValue() );
			break;
		}
	case nForeColor:
		{
			SetForeColor( pProp->GetLongValue() );
			break;
		}
	case nAutoSize:
		{
			if( pProp->GetBooleanValue() )
				SizeToContent();
			break;
		}
	case nButtonStyle:
		{
			switch( pProp->GetLongValue() )
			{
			case ButtonStyle_Filter:
				SetFlat( FALSE );
				SetIcon( IDI_FILTER );
				break;
			case ButtonStyle_Flat:
				SetFlat( TRUE );
				DrawAsToolbar( TRUE );
				break;
			case ButtonStyle_Pick:
				SetFlat( FALSE );
				SetIcon( IDI_PICK );
				break;
			case ButtonStyle_Select:
				SetFlat( FALSE );
				SetIcon( IDI_SELECT );
				break;
			case ButtonStyle_Raised:
				break;
			case ButtonStyle_NoBorder:
				SetFlat( TRUE );
				DrawBorder( FALSE );
				m_bHasBorder = FALSE;
				break;
			default:
				bFailed = true;
				break;
			}
			break;
		}
	}
	return !bFailed;
}

BEGIN_MESSAGE_MAP(CButtonCtrl, CXPStyleButtonST)
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

void CButtonCtrl::SetAcadColor(long nColor)
{
	m_BackColor = GetRGBColor(nColor);
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(m_BackColor);
}

void CButtonCtrl::SetForeColor(long nColor)
{
	m_ForeColor = GetRGBColor(nColor);
}

void CButtonCtrl::PreSubclassWindow() 
{
	__super::PreSubclassWindow();
	SetButtonStyle( GetButtonStyle() & SS_TYPEMASK );
}

HBRUSH CButtonCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled())
		return __super::CtlColor(pDC, nCtlColor);
	
	pDC->SetBkMode(TRANSPARENT);	
	pDC->SetBkColor(m_BackColor);	
	pDC->SetTextColor(m_ForeColor);
	pDC->SelectObject(m_pStaticBrush);
	return (HBRUSH)(m_pStaticBrush->GetSafeHandle());
}

void CButtonCtrl::OnKillFocus(CWnd * pNewWnd)
{
	m_bIsDefault = FALSE;
	Invalidate();
	__super::OnKillFocus(pNewWnd);
}

void CButtonCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
