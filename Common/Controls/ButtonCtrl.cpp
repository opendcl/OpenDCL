#include "stdafx.h"
#include "ButtonCtrl.h"
#include "AcadColorTable.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "Workspace.h"
#include "ToolTips.h"
#include "SharedRes.h"


/////////////////////////////////////////////////////////////////////////////
// CButtonCtrl

CButtonCtrl::CButtonCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	m_bDrawBorder		= TRUE;
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CButtonCtrl::~CButtonCtrl()
{
}

bool CButtonCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = (CXPStyleButtonST::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	//if( mpTemplate->GetLngProperty( nAutoSize ) > 0 )
	//	SizeToContent();

	mToolTip.Create(this);
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
			COLORREF crBkgnd = GetRGBColor( pProp->GetLongValue() );
			SetColor( BTNST_COLOR_BK_IN, crBkgnd, FALSE );
			SetColor( BTNST_COLOR_BK_OUT, crBkgnd, FALSE );
			SetColor( BTNST_COLOR_BK_FOCUS, crBkgnd, FALSE );
			break;
		}
	case nForeColor:
		{
			COLORREF crFgnd = GetRGBColor( pProp->GetLongValue() );
			SetColor( BTNST_COLOR_FG_IN, crFgnd, FALSE );
			SetColor( BTNST_COLOR_FG_OUT, crFgnd, FALSE );
			SetColor( BTNST_COLOR_FG_FOCUS, crFgnd, FALSE );
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
			case ButtonStyle_Raised:
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_Flat:
				SetFlat( TRUE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( TRUE, FALSE );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_Pick:
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetResourceIcon( IDI_PICK );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_Select:
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetResourceIcon( IDI_SELECT );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_Filter:
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetResourceIcon( IDI_FILTER );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_NoBorder:
				SetFlat( TRUE );
				m_bDrawBorder = FALSE;
				DrawAsToolbar( FALSE, FALSE );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_XPTheme:
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetThemeHelper( mpControlPane->GetThemeHelper() );
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
END_MESSAGE_MAP()

void CButtonCtrl::SetResourceIcon(UINT idIcon)
{
	HICON hIcon = LoadIcon( theWorkspace.GetResourceModule(), MAKEINTRESOURCE(idIcon) );
	ICONINFO ii;
	GetIconInfo( hIcon, &ii );
	::DeleteObject(ii.hbmMask);
	::DeleteObject(ii.hbmColor);
	//the icon has to be added to, then extracted from, an image list in order to center the cursor hotspot
	//dimensions (the default cursor hotspot for resource loaded icons is the lower right corner, which 
	//causes CButtonST::SetIcon() to calculate incorrect icon dimensions)  2007-03-01 [ORW]
	CImageList imglistIcon;
	imglistIcon.Create( ii.xHotspot, ii.yHotspot, ILC_COLOR | ILC_MASK, 0, 1 );
	imglistIcon.Add(hIcon);
	SetIcon( imglistIcon.ExtractIcon( 0 ) );
	imglistIcon.DeleteImageList();
}

void CButtonCtrl::PreSubclassWindow() 
{
	__super::PreSubclassWindow();
	SetButtonStyle( GetButtonStyle() & SS_TYPEMASK );
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
