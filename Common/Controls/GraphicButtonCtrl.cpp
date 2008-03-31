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
	}
	return !bFailed;
}

void CGraphicButtonCtrl::SetPicture( CPictureObject* pPict )
{
	mpPicture = pPict;
	if( pPict && !mpMouseOverPicture )
		mpMouseOverPicture = pPict;
	UpdateButtonGraphic();
}

void CGraphicButtonCtrl::SetMouseOverPicture( CPictureObject* pPict )
{
	if( !pPict && mpPicture )
		mpMouseOverPicture = mpPicture;
	else
		mpMouseOverPicture = pPict;
	UpdateButtonGraphic();
}

void CGraphicButtonCtrl::UpdateButtonGraphic()
{
	if( (!mpPicture || mpPicture->GetPicType() == PICTYPE_BITMAP) &&
			(!mpMouseOverPicture || mpMouseOverPicture->GetPicType() == PICTYPE_BITMAP) )
		SetBitmaps( mpMouseOverPicture? mpMouseOverPicture->CloneBitmap() : NULL, RGB(192,192,192),
								mpPicture? mpPicture->CloneBitmap() : NULL, RGB(192,192,192) );
	else
		SetIcon( mpMouseOverPicture? mpMouseOverPicture->CloneIcon() : NULL,
						 mpPicture? mpPicture->CloneIcon() : NULL );
}


BEGIN_MESSAGE_MAP(CGraphicButtonCtrl, CButtonCtrl)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGraphicButtonCtrl message handlers

BOOL CGraphicButtonCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent( pMsg );
	return __super::PreTranslateMessage( pMsg );
}
