// DialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControl.h"
#include "DclControlObject.h"
#include "AcadColorService.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "ToolTips.h"


/////////////////////////////////////////////////////////////////////////////
// CDialogControl

CDialogControl::CDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControl )
: mpTemplate( pTemplate )
, mpControl( pControl )
, mpControlPane( pPane )
, mbEnumProps( false )
{
	pTemplate->SetControlInstance( this );
	TPropertyPtr pToolTipBalloon = pTemplate->GetPropertyObject(Prop::ToolTipBalloon);
	mToolTip.SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );
}

CDialogControl::~CDialogControl()
{
	mpTemplate->SetControlInstance( NULL );
}

CThemeHelperST* CDialogControl::GetThemeHelper()
{
	return (mpControlPane? mpControlPane->GetThemeHelper() : NULL);
}

ControlType CDialogControl::GetControlType() const
{
	if( mpTemplate )
		return mpTemplate->GetType();
	return CtlInvalid;
}

bool CDialogControl::IsAsyncEvents() const
{
	if( mpTemplate )
		return (mpTemplate->GetLongProperty(Prop::EventInvoke) == 1);
	return false;
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
	CPoint pntUpperLeft( mpTemplate->GetLongProperty(Prop::Left), mpTemplate->GetLongProperty(Prop::Top) );
#endif
	return CRect( pntUpperLeft.x,
								pntUpperLeft.y,
								pntUpperLeft.x + mpTemplate->GetLongProperty(Prop::Width),
								pntUpperLeft.y + mpTemplate->GetLongProperty(Prop::Height) );
}

DWORD CDialogControl::GetWndStyle() const
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;

	if( mpTemplate->GetBooleanProperty(Prop::IsTabStop) )
		dwStyle |= WS_TABSTOP;
	if( mpTemplate->GetBooleanProperty(Prop::BeginGroup) )
		dwStyle |= WS_GROUP;
	if( mpTemplate->GetBooleanProperty(Prop::VScrollBar) )
		dwStyle |= WS_VSCROLL;

	return dwStyle;
}

CString CDialogControl::GetWndCaption() const
{
	return mpTemplate->GetStringProperty(Prop::Caption);
}

bool CDialogControl::ApplyPropertiesEnum()
{
	bool bSuccess = true;
	mbEnumProps = true;
	TPropertyPtr pAutoSizeProp = mpTemplate->GetPropertyObject( Prop::AutoSize );
	const TPropertyList& Props = mpTemplate->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
	{
		if( (*iter) != pAutoSizeProp ) //save autosize for last
			OnApplyProperty( (*iter) );
	}
	if( pAutoSizeProp )
		OnApplyProperty( pAutoSizeProp );
	mbEnumProps = false;
	mpControl->Invalidate();
	return bSuccess;
}

bool CDialogControl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !pProp )
		return false;
	bool bSuccess = true;
	switch( pProp->GetID() )
	{
	case Prop::ForegroundColor: if( !OnApplyForegroundColor( pProp ) ) bSuccess = false; break;
	case Prop::BackgroundColor: if( !OnApplyBackgroundColor( pProp ) ) bSuccess = false; break;
	case Prop::BorderStyle: if( !OnApplyBorderStyle( pProp ) ) bSuccess = false; break;
	case Prop::Enabled: if( !OnApplyEnabled( pProp ) ) bSuccess = false; break;
	case Prop::Visible: if( !OnApplyVisible( pProp ) ) bSuccess = false; break;
	case Prop::Caption: if( !OnApplyCaption( pProp ) ) bSuccess = false; break;
	case Prop::TitleBarText: if( !OnApplyCaption( pProp ) ) bSuccess = false; break;
	case Prop::VScrollBar: if( !OnApplyVScrollBar( pProp ) ) bSuccess = false; break;
	case Prop::HScrollBar: if( !OnApplyHScrollBar( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipTitle: if( !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipBalloon: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipLine: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipBody: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipPicture: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipAviFileName: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipTitleColor: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::FontName: if( !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontSize: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontBold: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontItalic: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontUnderline: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontStrikeout: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	}
	return bSuccess;
}

bool CDialogControl::OnApplyForegroundColor( TPropertyPtr pProp )
{
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
		pColorService->SetForegroundColor( pProp->GetLongValue() );
	return true;
}

bool CDialogControl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
		pColorService->SetBackgroundColor( pProp->GetLongValue() );
	return true;
}

bool CDialogControl::OnApplyBorderStyle( TPropertyPtr pProp )
{
	switch( pProp->GetLongValue() )
	{
	case 0:
		mpControl->ModifyStyle( WS_BORDER, 0, SWP_FRAMECHANGED );
		mpControl->ModifyStyleEx( (WS_EX_STATICEDGE | WS_EX_CLIENTEDGE), 0, SWP_FRAMECHANGED );
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

bool CDialogControl::OnApplyEnabled( TPropertyPtr pProp )
{
	mpControl->EnableWindow( pProp->GetBooleanValue() );
	return true;
}

bool CDialogControl::OnApplyVisible( TPropertyPtr pProp )
{
	mpControl->ShowWindow( pProp->GetBooleanValue()? SW_SHOW : SW_HIDE );
	return true;
}

bool CDialogControl::OnApplyCaption( TPropertyPtr pProp )
{
	mpControl->SetWindowText( pProp->GetStringValue() );
	return true;
}

bool CDialogControl::OnApplyVScrollBar( TPropertyPtr pProp )
{
	if( pProp->GetBooleanValue() )
		mpControl->ModifyStyle( 0, WS_VSCROLL, SWP_FRAMECHANGED );
	else
		mpControl->ModifyStyle( WS_VSCROLL, 0, SWP_FRAMECHANGED );
	return true;
}

bool CDialogControl::OnApplyHScrollBar( TPropertyPtr pProp )
{
	if( pProp->GetBooleanValue() )
		mpControl->ModifyStyle( 0, WS_HSCROLL, SWP_FRAMECHANGED );
	else
		mpControl->ModifyStyle( WS_HSCROLL, 0, SWP_FRAMECHANGED );
	return true;
}

bool CDialogControl::OnApplyToolTip( TPropertyPtr pProp )
{
	GetToolTipCtrl().RemoveAllTools();
	SetToolTipEx( mpControl, GetToolTipCtrl(), mpTemplate );
	return true;
}

bool CDialogControl::OnApplyFont( TPropertyPtr pProp )
{
	mpControl->SetFont( theWorkspace.GetFontCollection().GetFont( mpTemplate, mpControl ) );
	return true;
}
