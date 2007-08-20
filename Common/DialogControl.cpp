// DialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "ToolTips.h"


/////////////////////////////////////////////////////////////////////////////
// CDialogControl

CDialogControl::CDialogControl( CDclControlObject* pTemplate, CControlPane* pPane, CWnd* pControl )
: mpTemplate( pTemplate )
, mpControl( pControl )
, mpControlPane( pPane )
, mbEnumProps( false )
{
	pTemplate->SetControlInstance( this );
	RefCountedPtr< CPropertyObject > pToolTipBalloon = pTemplate->GetPropertyObject(nToolTipBalloon);
	mToolTip.SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );
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
#ifdef EDITOR
	CPoint pntUpperLeft( 0, 0 );
#else
	CPoint pntUpperLeft( mpTemplate->GetLongProperty(nLeft), mpTemplate->GetLongProperty(nTop) );
#endif
	return CRect( pntUpperLeft.x,
								pntUpperLeft.y,
								pntUpperLeft.x + mpTemplate->GetLongProperty(nWidth),
								pntUpperLeft.y + mpTemplate->GetLongProperty(nHeight) );
}

DWORD CDialogControl::GetWndStyle() const
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;

	if( mpTemplate->GetBoolProperty(nIsTabStop) )
		dwStyle |= WS_TABSTOP;
	if( mpTemplate->GetBoolProperty(nBeginGroup) )
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
	mbEnumProps = true;
	RefCountedPtr< CPropertyObject > pAutoSizeProp = mpTemplate->GetPropertyObject( nAutoSize );
	POSITION pos = mpTemplate->GetPropertyList().GetHeadPosition();
	while( pos )
	{
		RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyList().GetNext( pos );
		assert( pProp != NULL );
		if( pProp != pAutoSizeProp ) //save autosize for last
			OnApplyProperty( pProp );
	}
	if( pAutoSizeProp )
		OnApplyProperty( pAutoSizeProp );
	mbEnumProps = false;
	mpControl->Invalidate();
	return bSuccess;
}

bool CDialogControl::OnApplyProperty( RefCountedPtr< CPropertyObject > pProp )
{
	if( !pProp )
		return false;
	bool bSuccess = true;
	switch( pProp->GetID() )
	{
	case nBorderStyle: if( !OnApplyBorderStyle( pProp ) ) bSuccess = false; break;
	case nEnabled: if( !OnApplyEnabled( pProp ) ) bSuccess = false; break;
	case nVisible: if( !OnApplyVisible( pProp ) ) bSuccess = false; break;
	case nCaption: if( !OnApplyCaption( pProp ) ) bSuccess = false; break;
	case nTitleBarText: if( !OnApplyCaption( pProp ) ) bSuccess = false; break;
	case nToolTipTitle: if( !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case nToolTipBalloon: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case nToolTipLine: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case nToolTipBody: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case nToolTipPicture: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case nToolTipAviFileName: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case nToolTipTitleColor: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case nLabelName: if( !OnApplyFont( pProp ) ) bSuccess = false; break;
	case nLabelSize: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case nLabelBold: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case nLabelItalic: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case nLabelUnderline: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case nLabelStrikeOut: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	}
	return bSuccess;
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

bool CDialogControl::OnApplyEnabled( RefCountedPtr< CPropertyObject > pProp )
{
	mpControl->EnableWindow( pProp->GetBooleanValue() );
	return true;
}

bool CDialogControl::OnApplyVisible( RefCountedPtr< CPropertyObject > pProp )
{
	mpControl->ShowWindow( pProp->GetBooleanValue()? SW_SHOW : SW_HIDE );
	return true;
}

bool CDialogControl::OnApplyCaption( RefCountedPtr< CPropertyObject > pProp )
{
	mpControl->SetWindowText( pProp->GetStringValue() );
	return true;
}

bool CDialogControl::OnApplyToolTip( RefCountedPtr< CPropertyObject > pProp )
{
	GetToolTipCtrl().RemoveAllTools();
	SetToolTipEx( mpControl, GetToolTipCtrl(), mpTemplate );
	return true;
}

bool CDialogControl::OnApplyFont( RefCountedPtr< CPropertyObject > pProp )
{
	mpControl->SetFont( theWorkspace.GetFontCollection().GetFont( mpTemplate, mpControl ) );
	return true;
}
