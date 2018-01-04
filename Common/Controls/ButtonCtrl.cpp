#include "stdafx.h"
#include "ButtonCtrl.h"
#include "AcadColorTable.h"
#include "DclControlTemplate.h"
#include "ControlPane.h"
#include "Workspace.h"
#include "Resource.h"


void CButtonAcadColorService::ResetButtonForegroundColor() const
{
	COLORREF crFgnd = GetForegroundColor();
	mButton.SetColor( CButtonST::BTNST_COLOR_FG_IN, crFgnd, FALSE );
	mButton.SetColor( CButtonST::BTNST_COLOR_FG_OUT, crFgnd, FALSE );
	mButton.SetColor( CButtonST::BTNST_COLOR_FG_FOCUS, crFgnd, FALSE );
}

void CButtonAcadColorService::ResetButtonBackgroundColor() const
{
	COLORREF crBkgnd = GetBackgroundColor();
	mButton.SetColor( CButtonST::BTNST_COLOR_BK_IN, crBkgnd, FALSE );
	mButton.SetColor( CButtonST::BTNST_COLOR_BK_OUT, crBkgnd, FALSE );
	mButton.SetColor( CButtonST::BTNST_COLOR_BK_FOCUS, crBkgnd, FALSE );
}


/////////////////////////////////////////////////////////////////////////////
// CButtonCtrl

CButtonCtrl::CButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, mColorService( *this )
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
	bool bSuccess = (__super::Create( NULL, GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	//if( mpTemplate->GetLongProperty( Prop::AutoSize ) > 0 )
	//	SizeToContent();

	return bSuccess;
}

bool CButtonCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::AutoSize:
		{
			if( pProp->GetBooleanValue() )
				SizeToContent();
			break;
		}
	case Prop::GraphicButtonStyle:
		{
			switch( pProp->GetLongValue() )
			{
			case ButtonStyle_Raised:
				mbUsingPresetGraphic = false;
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_Flat:
				mbUsingPresetGraphic = false;
				SetFlat( TRUE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( TRUE, FALSE );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_Pick:
				mbUsingPresetGraphic = true;
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetResourceIcon( IDI_PICK );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_Select:
				mbUsingPresetGraphic = true;
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetResourceIcon( IDI_SELECT );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_Filter:
				mbUsingPresetGraphic = true;
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetResourceIcon( IDI_FILTER );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_NoBorder:
				mbUsingPresetGraphic = false;
				SetFlat( TRUE );
				m_bDrawBorder = FALSE;
				DrawAsToolbar( FALSE, FALSE );
				SetThemeHelper( NULL );
				break;
			case ButtonStyle_XPTheme:
				mbUsingPresetGraphic = false;
				SetFlat( FALSE );
				m_bDrawBorder = TRUE;
				DrawAsToolbar( FALSE, FALSE );
				SetThemeHelper( &mThemeHelper );
				break;
			default:
				mbUsingPresetGraphic = false;
				bFailed = true;
				break;
			}
			UpdateButtonGraphic();
			break;
		}
	}
	return !bFailed;
}

DWORD CButtonCtrl::OnDrawBackground(CDC* pDC, CRect* pRect)
{
	//if( mpTemplate->GetLongProperty( Prop::GraphicButtonStyle ) == ButtonStyle_XPTheme )
	//	return BTNST_OK;
	return __super::OnDrawBackground( pDC, pRect );
}

void CButtonCtrl::SetResourceIcon(UINT idIcon)
{
	HICON hIcon = LoadIcon( theWorkspace.GetLocalResourceModule(), MAKEINTRESOURCE(idIcon) );
	ICONINFO ii;
	GetIconInfo( hIcon, &ii );
	::DeleteObject(ii.hbmMask);
	::DeleteObject(ii.hbmColor);
	//the icon has to be added to, then extracted from, an image list in order to center the cursor hotspot
	//dimensions (the default cursor hotspot for resource loaded icons is the lower right corner, which 
	//causes CButtonST::SetIcon() to calculate incorrect icon dimensions)  2007-03-01 [ORW]
	CImageList imglistIcon;
	imglistIcon.Create( ii.xHotspot, ii.yHotspot, ILC_COLOR32 | ILC_MASK, 0, 1 );
	imglistIcon.Add( hIcon );
	SetIcon( imglistIcon.ExtractIcon( 0 ) );
	imglistIcon.DeleteImageList();
}

BEGIN_MESSAGE_MAP(CButtonCtrl, CXPStyleButtonST)
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CButtonCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl message handlers

LRESULT CButtonCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

void CButtonCtrl::PreSubclassWindow() 
{
	__super::PreSubclassWindow();
	SetButtonStyle( GetButtonStyle() & SS_TYPEMASK );
}

void CButtonCtrl::OnKillFocus(CWnd * pNewWnd)
{
	m_bIsDefault = FALSE;
	OnNeedRepaint( true );
	__super::OnKillFocus( pNewWnd );
}

BOOL CButtonCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return __super::OnEraseBkgnd(pDC);
}

void CButtonCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}

BOOL CButtonCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent( pMsg );
	return __super::PreTranslateMessage( pMsg );
}
