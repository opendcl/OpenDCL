#include "stdafx.h"
#include "GraphicButtonCtrl.h"
#include "ControlPane.h"
#include "PictureObject.h"
#include "Project.h"


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

CGraphicButtonCtrl::CGraphicButtonCtrl( CDclControlObject* pTemplate,
																				CControlPane* pPane,
																				UINT nID,
																				bool bCreate /*= true*/ )
: CButtonCtrl( pTemplate, pPane, nID, false )
, mpPicture( NULL )
, mpPressedPicture( NULL )
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

bool CGraphicButtonCtrl::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case nPicture:
		{
			SetPicture( mpTemplate->GetOwnerProject()->FindPicture( pProp->GetLongValue() ) );
			if( !IsEnumeratingProperties() )
				OnApplyProperty( mpTemplate->GetPropertyObject( nAutoSize ) );
			break;
		}
	case nPressedPicture:
		{
			SetPressedPicture( mpTemplate->GetOwnerProject()->FindPicture( pProp->GetLongValue() ) );
			break;
		}
	}
	return !bFailed;
}

void CGraphicButtonCtrl::SetPicture( CPictureObject* pPict )
{
	mpPicture = pPict;
	UpdateButtonGraphic();
}

void CGraphicButtonCtrl::SetPressedPicture( CPictureObject* pPict )
{
	mpPressedPicture = pPict;
	UpdateButtonGraphic();
}

void CGraphicButtonCtrl::UpdateButtonGraphic()
{
	if( (!mpPicture || mpPicture->GetPicType() == PICTYPE_BITMAP) &&
			(!mpPressedPicture || mpPressedPicture->GetPicType() == PICTYPE_BITMAP) )
		SetBitmaps( mpPicture? mpPicture->CloneBitmap() : NULL, RGB(192,192,192),
								mpPressedPicture? mpPressedPicture->CloneBitmap() : NULL, RGB(192,192,192) );
	else
		SetIcon( mpPicture? mpPicture->CloneIcon() : NULL,
						 mpPressedPicture? mpPressedPicture->CloneIcon() : NULL );
}


BEGIN_MESSAGE_MAP(CGraphicButtonCtrl, CButtonCtrl)
END_MESSAGE_MAP()
