// DialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CDialogControl

CDialogControl::CDialogControl( CDclControlObject* pTemplate, CControlPane* pPane, CWnd* pControl )
: mpTemplate( pTemplate )
, mpControl( pControl )
, mpControlPane( pPane )
{
	pTemplate->SetControlInstance( this );
}

CDialogControl::~CDialogControl()
{
	mpTemplate->SetControlInstance( NULL );
}

ControlType CDialogControl::GetControlType() const
{
	if( mpTemplate )
		return mpTemplate->GetType();
	return CtlInvalid;
}

CString CDialogControl::GetKeyName() const
{
	if( mpTemplate )
		return mpTemplate->GetKeyName();
	return CString();
}

CString CDialogControl::GetKeyPath() const
{
	if( mpTemplate )
		return mpTemplate->GetKeyPath();
	return CString();
}

CRect CDialogControl::GetWndRect() const
{
	CPoint pntUpperLeft( mpTemplate->m_pLeft->GetLongValue(), mpTemplate->m_pTop->GetLongValue() );
	return CRect( pntUpperLeft.x,
								pntUpperLeft.y,
								pntUpperLeft.x + mpTemplate->m_pWidth->GetLongValue(),
								pntUpperLeft.y + mpTemplate->m_pHeight->GetLongValue() );
}

DWORD CDialogControl::GetWndStyle() const
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;

	if( mpTemplate->GetBoolProperty(nIsTabStop) )
		dwStyle |= WS_TABSTOP;
	else
		dwStyle |= WS_GROUP;

	return dwStyle;
}

CString CDialogControl::GetWndCaption() const
{
	return mpTemplate->GetStrProperty(nCaption);
}

bool CDialogControl::ApplyPropertiesEnum()
{
	bool bSuccess = true;
	POSITION pos = mpTemplate->GetPropertyList().GetHeadPosition();
	while( pos )
	{
		RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyList().GetNext( pos );
		assert( pProp != NULL );
		switch( pProp->GetID() )
		{
		case nBorderStyle: if( !OnApplyBorderStyle( pProp ) ) bSuccess = false; break;
		case nCaption: if( !OnApplyBorderStyle( pProp ) ) bSuccess = false; break;
		case nImageList: if( !OnApplyImageList( pProp ) ) bSuccess = false; break;
		default: if( !OnApplyProperty( pProp ) ) bSuccess = false; break;
		}
	}
	return bSuccess;
}

bool CDialogControl::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	return true;
}

bool CDialogControl::OnApplyBorderStyle( RefCountedPtr< CPropertyObject > pProp )
{
	switch( pProp->GetLongValue() )
	{
	case 0:
		mpControl->ModifyStyle( WS_BORDER, 0, SWP_FRAMECHANGED );
		break;
	case 1:
		mpControl->ModifyStyle( WS_BORDER, 0, SWP_FRAMECHANGED );
		mpControl->ModifyStyleEx( WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED );
		break;
	case 2:
		mpControl->ModifyStyle( WS_BORDER, 0, SWP_FRAMECHANGED );
		mpControl->ModifyStyleEx( WS_EX_CLIENTEDGE, WS_EX_STATICEDGE, SWP_FRAMECHANGED );
		break;
	}
	return true;
}

bool CDialogControl::OnApplyCaption( RefCountedPtr< CPropertyObject > pProp )
{
	mpControl->SetWindowText( pProp->GetStringValue() );
	return true;
}

bool CDialogControl::OnApplyImageList( RefCountedPtr< CPropertyObject > pProp )
{
	return false;
}
