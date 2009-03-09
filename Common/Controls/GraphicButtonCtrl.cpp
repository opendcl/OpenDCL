#include "stdafx.h"
#include "GraphicButtonCtrl.h"
#include "ControlPane.h"
#include "PictureObject.h"
#include "Project.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// CGraphicButtonCtrl

// Mask for control's type
#define ODCL_TYPEMASK SS_TYPEMASK

#ifndef	TTM_SETTITLE
#define TTM_SETTITLEA           (WM_USER + 32)  // wParam = TTI_*, lParam = char* szTitle
#define TTM_SETTITLEW           (WM_USER + 33)  // wParam = TTI_*, lParam = wchar* szTitle
#ifdef	UNICODE
#define TTM_SETTITLE            TTM_SETTITLEW
#else
#define TTM_SETTITLE            TTM_SETTITLEA
#endif
#endif

CGraphicButtonCtrl::CGraphicButtonCtrl( TDclControlPtr pTemplate,
																				CControlPane* pPane,
																				UINT nID,
																				bool bCreate /*= true*/ )
: CButtonCtrl( pTemplate, pPane, nID, false )
, mpPicture( NULL )
, mpMouseOverPicture( NULL )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CGraphicButtonCtrl::~CGraphicButtonCtrl()
{
}

DWORD CGraphicButtonCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();
	dwStyle |= BS_NOTIFY;
	return dwStyle;
}

bool CGraphicButtonCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Picture:
		{
			SetPicture( mpTemplate->GetOwnerProject()->FindPicture( pProp->GetLongValue() ) );
			if( !IsEnumeratingProperties() )
				OnApplyProperty( mpTemplate->GetPropertyObject( Prop::AutoSize ) );
			break;
		}
	case Prop::MouseOverPicture:
		{
			SetMouseOverPicture( mpTemplate->GetOwnerProject()->FindPicture( pProp->GetLongValue() ) );
			break;
		}
	case Prop::BackgroundColor:
		{
			if( GetColorService()->IsBackgroundTransparent() )
				DrawTransparent();
			else
				m_bDrawTransparent = FALSE;
			break;
		}
	}
	return !bFailed;
}

void CGraphicButtonCtrl::SetPicture( TPicturePtr pPict )
{
	mpPicture = pPict;
	UpdateButtonGraphic();
}

void CGraphicButtonCtrl::SetMouseOverPicture( TPicturePtr pPict )
{
	mpMouseOverPicture = pPict;
	UpdateButtonGraphic();
}

void CGraphicButtonCtrl::UpdateButtonGraphic()
{
	if( IsUsingPresetGraphic() )
		return;
	if( (!mpPicture || mpPicture->GetPicType() == PICTYPE_BITMAP) &&
			(!mpMouseOverPicture || mpMouseOverPicture->GetPicType() == PICTYPE_BITMAP) )
	{
		HBITMAP hbmpIn = NULL;
		if( mpMouseOverPicture )
			hbmpIn = mpMouseOverPicture->CloneBitmap();
		else if( mpPicture )
			hbmpIn = mpPicture->CloneBitmap();
		HBITMAP hbmpOut = NULL;
		if( mpPicture )
			hbmpOut = mpPicture->CloneBitmap();
		SetBitmaps( hbmpIn, RGB(192,192,192), hbmpOut, RGB(192,192,192) );
	}
	else
	{
		HICON hIn = NULL;
		if( mpMouseOverPicture )
			hIn = mpMouseOverPicture->CloneIcon();
		else if( mpPicture )
			hIn = mpPicture->CloneIcon();
		HICON hOut = NULL;
		if( mpPicture )
			hOut = mpPicture->CloneIcon();
		SetIcon( hIn, hOut );
	}
}


BEGIN_MESSAGE_MAP(CGraphicButtonCtrl, CButtonCtrl)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphicButtonCtrl message handlers
