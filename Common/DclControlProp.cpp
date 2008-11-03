// DclControlProp.cpp : implementation file
//

#include "stdafx.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "PropertyObject.h"
#include "Resource.h"
#include "Workspace.h"
#include "PropertyIds.h"
#include "ControlTypes.h"


static TPropertyPtr AddControlHiddenProperty( TDclControlPtr pDclControl,
																							Prop::Id nID,
																							LPCTSTR pszValue,
																							PropertyType type )
{
	TPropertyPtr pProp = pDclControl->AddStringProperty( nID, type, pszValue );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}

static TPropertyPtr AddControlHiddenProperty( TDclControlPtr pDclControl,
																							Prop::Id nID,
																							long lValue,
																							PropertyType type )
{
	TPropertyPtr pProp = pDclControl->AddLongProperty( nID, type, lValue );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}

static TPropertyPtr AddControlHiddenProperty( TDclControlPtr pDclControl,
																							Prop::Id nID,
																							bool bValue,
																							PropertyType type )
{
	TPropertyPtr pProp = pDclControl->AddBooleanProperty( nID, type, bValue );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}

static void AddControlTooltipProperties( TDclControlPtr pDclControl )
{
	pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
	pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
	pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture );
	AddControlHiddenProperty( pDclControl, Prop::ToolTipAviFileName, _T(""), PropString );
	pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
	pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
	pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
}

static void AddControlFontProperties( TDclControlPtr pDclControl, const FontSettings& FS )
{
	pDclControl->AddStringProperty( Prop::FontName, PropString, FS.name() );
	AddControlHiddenProperty( pDclControl, Prop::FontBold, FS.isBold(), PropBool );
	AddControlHiddenProperty( pDclControl, Prop::FontItalic, FS.isItalic(), PropBool );
	AddControlHiddenProperty( pDclControl, Prop::FontSize, FS.size(), PropLong );
	AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
	AddControlHiddenProperty( pDclControl, Prop::FontUnderline, FS.isUnderlined(), PropBool );
}

static TPropertyPtr AddControlEvent( TDclControlPtr pDclControl, Prop::Id nID )
{
	TPropertyPtr pProp = pDclControl->AddStringProperty( nID, PropEvent, NULL );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}

static bool AddDefaultFormName( TDclControlPtr pDclControl )
{
	if( !pDclControl->GetPropertyObject( Prop::Name ) )
	{
		TProjectPtr pProject = pDclControl->GetOwnerProject();
		UINT nFormId = 1;
		CString sFormName;
		bool bFoundUnusedName = false;
		while( !bFoundUnusedName )
		{
			sFormName.Format( _T("Form%d"), nFormId++ );
			bFoundUnusedName = !pProject->FindDclForm( sFormName );
		}
		pDclControl->AddStringProperty( Prop::Name, PropString, sFormName );
	}
	return true;
}

static bool AddDefaultFormProperties( TDclControlPtr pDclControl, long lWidth /*= 32*/, long lHeight /*= 32*/ )
{
	assert( pDclControl->GetType() == _CtlForm );
	TDclFormPtr pOwnerForm = pDclControl->GetOwnerForm();
	assert( pOwnerForm != NULL );
	if( !pOwnerForm )
		return false;
	FormType eFormType = pOwnerForm->GetType();
	if( eFormType != FrmTabPage )
	{
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( Prop::ControlBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( Prop::VarName ); // now setting to empty string by default  2007-02-15 [ORW]
		TPropertyPtr pProp = pDclControl->AddLongProperty( Prop::EventInvoke, PropEnum, 0 );
		pProp->SetHidden( !pOwnerForm->IsModeless() );
	}
	switch( eFormType )
	{
	case FrmModalDlg:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		pDclControl->AddBooleanProperty( Prop::AllowResizing, PropBool, false );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 350 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 250 );
		pDclControl->AddLongProperty( Prop::MinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MinDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::TitleBarIcon, PropPicture, -1 );
		pDclControl->AddBooleanProperty( Prop::TitleBar, PropBool, true );
		pDclControl->AddStringProperty( Prop::TitleBarText, PropString, pOwnerForm->GetKeyName() );
		AddControlEvent( pDclControl, Prop::FormEventInitialize );
		AddControlEvent( pDclControl, Prop::FormEventClose );
		AddControlEvent( pDclControl, Prop::FormEventSize );
		AddControlEvent( pDclControl, Prop::FormEventOnOk );
		AddControlEvent( pDclControl, Prop::FormEventOnCancel );
		AddControlEvent( pDclControl, Prop::FormEventCancelClose );
		AddControlEvent( pDclControl, Prop::EventOnHelp );
		break;
	case FrmModelessDlg:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		pDclControl->AddBooleanProperty( Prop::KeepFocus, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::AllowResizing, PropBool, true );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 250 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 150 );
		pDclControl->AddLongProperty( Prop::MinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MinDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogHeight, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::TitleBar, PropBool, true );
		pDclControl->AddStringProperty( Prop::TitleBarText, PropString, pOwnerForm->GetKeyName() );
		AddControlEvent( pDclControl, Prop::FormEventInitialize );
		AddControlEvent( pDclControl, Prop::FormEventClose );
		AddControlEvent( pDclControl, Prop::FormEventSize );
		AddControlEvent( pDclControl, Prop::FormEventOnOk );
		AddControlEvent( pDclControl, Prop::FormEventOnCancel );
		AddControlEvent( pDclControl, Prop::FormEventCancelClose );
		AddControlEvent( pDclControl, Prop::DocEventActivated );
		AddControlEvent( pDclControl, Prop::DocEventEnteringNoDocState );
		AddControlEvent( pDclControl, Prop::EventMouseEntered );
		AddControlEvent( pDclControl, Prop::EventMouseMovedOff );
		AddControlEvent( pDclControl, Prop::EventOnHelp );
		break;
		break;
	case FrmControlBar:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		pDclControl->AddBooleanProperty( Prop::KeepFocus, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::AllowResizing, PropBool, true );
		pDclControl->AddLongProperty( Prop::MinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MinDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 250 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 450 );
		pDclControl->AddStringProperty( Prop::TitleBarText, PropString, pOwnerForm->GetKeyName() );
		pDclControl->AddLongProperty( Prop::DockableSides, PropEnum, 0 );
		AddControlEvent( pDclControl, Prop::FormEventInitialize );
		AddControlEvent( pDclControl, Prop::FormEventClose );
		AddControlEvent( pDclControl, Prop::FormEventSize );
		AddControlEvent( pDclControl, Prop::DocEventActivated );
		AddControlEvent( pDclControl, Prop::DocEventEnteringNoDocState );
		AddControlEvent( pDclControl, Prop::EventMouseEntered );
		AddControlEvent( pDclControl, Prop::EventMouseMovedOff );
		AddControlEvent( pDclControl, Prop::EventOnHelp );
		break;
	case FrmOptionsTab:
		pDclControl->AddBooleanProperty( Prop::AllowResizing, PropBool, true );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 600 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 380 );
		pDclControl->AddLongProperty( Prop::MinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MinDialogHeight, PropLong, 0 );
		pDclControl->AddStringProperty( Prop::OptionsTabCaption, PropString, pOwnerForm->GetKeyName() );
		AddControlEvent( pDclControl, Prop::FormEventInitialize );
		AddControlEvent( pDclControl, Prop::FormEventShow );
		AddControlEvent( pDclControl, Prop::EventOptionsCancel );
		AddControlEvent( pDclControl, Prop::EventOptionsApply );
		AddControlEvent( pDclControl, Prop::EventOptionsHelp );
		AddControlEvent( pDclControl, Prop::EventOptionsOK );
		AddControlEvent( pDclControl, Prop::FormEventSize );
		break;
	case FrmFileDlg:
		pDclControl->AddBooleanProperty( Prop::AllowResizing, PropBool, true );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 556 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 386 );
		pDclControl->AddStringProperty( Prop::TitleBarText, PropString, theWorkspace.LoadResourceString( IDS_OPEN ) );
		AddControlEvent( pDclControl, Prop::FormEventInitialize );
		AddControlEvent( pDclControl, Prop::FormEventCancelClose );
		AddControlEvent( pDclControl, Prop::FormEventClose );
		AddControlEvent( pDclControl, Prop::FormEventSize );
		AddControlEvent( pDclControl, Prop::EventOnHelp );
		break;
	case FrmTabPage:
		AddControlHiddenProperty( pDclControl, Prop::Width, lWidth > 0? lWidth : 600, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::Height, lHeight > 0? lHeight : 380, PropLong );
		break;
	case FrmPaletteDlg:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		pDclControl->AddBooleanProperty( Prop::KeepFocus, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::AllowResizing, PropBool, true );
		pDclControl->AddLongProperty( Prop::MinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MinDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 250 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 450 );
		pDclControl->AddStringProperty( Prop::TitleBarText, PropString, pOwnerForm->GetKeyName() );
		pDclControl->AddLongProperty( Prop::DockableSides, PropEnum, 0 );
		AddControlEvent( pDclControl, Prop::FormEventInitialize );
		AddControlEvent( pDclControl, Prop::FormEventClose );
		AddControlEvent( pDclControl, Prop::FormEventSize );
		AddControlEvent( pDclControl, Prop::DocEventActivated );
		AddControlEvent( pDclControl, Prop::DocEventEnteringNoDocState );
		AddControlEvent( pDclControl, Prop::EventMouseEntered );
		AddControlEvent( pDclControl, Prop::EventMouseMovedOff );
		AddControlEvent( pDclControl, Prop::EventOnHelp );
		break;
	default:
		TraceFmt( _T("* AddDefaultFormProperties() called with unknown form type (%d)\r\n"), pOwnerForm->GetType() );
		assert( false );
		return false;
		break;
	}
	return true;
}


bool AddDefaultProperties( TDclControlPtr pDclControl, long lWidth /*= -1*/, long lHeight /*= -1*/ )
{
	assert( pDclControl != NULL );
	ControlType type = pDclControl->GetType();
	assert( type != _CtlInvalid );
	if( type == _CtlInvalid )
		return false;
	if( type == _CtlForm )
		return AddDefaultFormProperties( pDclControl, lWidth, lHeight );
	TDclFormPtr pOwnerForm = pDclControl->GetOwnerForm();
	assert( pOwnerForm != NULL );
	if( !pOwnerForm )
		return false;

	//Add default properties that apply to every control type
	pDclControl->AddStringProperty( Prop::Name, PropString, pDclControl->GetKeyName() );
	pDclControl->AddStringProperty( Prop::ControlBrowser, PropActiveXMethods );
	pDclControl->AddStringProperty( Prop::Custom, PropCustom ); // add the Custom property
	pDclControl->AddStringProperty( Prop::VarName ); // now setting to empty string by default  2007-02-15 [ORW]
	if( lWidth <= 0 )
		lWidth = 32;
	if( lHeight <= 0 )
		lHeight = 32;

	if( type != CtlFileExplorer )
	{
		pDclControl->AddLongProperty( Prop::Left, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::Top, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight );
		pDclControl->AddLongProperty( Prop::LeftFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::RightFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::TopFromBottom, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::BottomFromBottom, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::UseBottomFromBottom, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::UseLeftFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::UseRightFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::UseTopFromBottom, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::Visible, PropBool, true );
	}
	else
	{
		AddControlHiddenProperty( pDclControl, Prop::Width, lWidth, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::Height, lHeight, PropLong );
	}

	if (type != CtlActiveX && type != CtlFileExplorer)
		pDclControl->AddBooleanProperty( Prop::Enabled, PropBool, true );

	TDclFormPtr pTopLevelParentForm = pOwnerForm;
	while( pTopLevelParentForm->GetParentForm() )
		pTopLevelParentForm = pTopLevelParentForm->GetParentForm();
	FormType eFormType = pTopLevelParentForm->GetType();

	TPropertyPtr pPropEventInvoke = pDclControl->AddLongProperty( Prop::EventInvoke, PropEnum, 0 );
	pPropEventInvoke->SetHidden( !pTopLevelParentForm->IsModeless() );

	FontSettings FS = theWorkspace.GetDefaultFontSettings();

	//Add control specific properties
	switch( type )
	{
	case CtlActiveX:
		pDclControl->AddStringProperty( Prop::ActiveXPropPages, PropActiveXPropPages );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlTooltipProperties( pDclControl );
		break;

	case CtlAngleSlider:
		pDclControl->AddLongProperty( Prop::Value, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlFontProperties( pDclControl, FS );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventScroll );
		break;

	case CtlAnimation:
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 0 );
		break;

	case CtlBlockList:
		pDclControl->AddLongProperty( Prop::ListViewIconAlign, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::AutoArrange, PropBool, true );
		pDclControl->AddLongProperty( Prop::IconXSpacing, PropLong, 20 );
		pDclControl->AddLongProperty( Prop::IconYSpacing, PropLong, 32 );
		pDclControl->AddBooleanProperty( Prop::LabelWrap, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::MultiSelect, PropBool, false );
		pDclControl->AddLongProperty( Prop::ListViewSort, PropEnum, 2 );
		pDclControl->AddLongProperty( Prop::BlockListStyle, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlFontProperties( pDclControl, FS );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::EventKeyDown );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventReturnPressed );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		break;

	case CtlBlockView:
		pDclControl->AddLongProperty( Prop::InterfaceMode, PropEnum, 1 );
		pDclControl->AddStringProperty( Prop::BlockName, PropString );
		pDclControl->AddLongProperty( Prop::RenderMode, PropEnum, 5 );
		pDclControl->AddBooleanProperty( Prop::ShowOrbitCircles, PropBool, false );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -22 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventMouseDblClick );
		AddControlEvent( pDclControl, Prop::EventMouseDown );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventMouseUp );
		AddControlEvent( pDclControl, Prop::EventRClick );
		AddControlEvent( pDclControl, Prop::EventRDblClick );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		break;

	case CtlCalendar:
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::MultiSelection, PropLong, 1 );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventGetDayState );
		AddControlEvent( pDclControl, Prop::EventSelect );
		break;

	case CtlCheckBox:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddLongProperty( Prop::Value, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlComboBox:
		pDclControl->AddLongProperty( Prop::DropDownHeight, PropLong, 100 );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ItemData, PropIntArray );
		pDclControl->AddLongProperty( Prop::TextLimit, PropLong, 256 );
		pDclControl->AddStringProperty( Prop::List, PropStringArray );
		pDclControl->AddBooleanProperty( Prop::ReturnAsTab, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::Sorted, PropBool, false );
		pDclControl->AddLongProperty( Prop::ComboBoxStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::Text, PropString, pDclControl->GetKeyName() );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventEditChanged );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		//AddControlEvent( pDclControl, Prop::EventUpdate );
		AddControlEvent( pDclControl, Prop::EventDropDown );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlDwgList:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DisableNoScroll, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, true );
		pDclControl->AddLongProperty( Prop::InsertOrXref, PropEnum, 0 );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::SelectionStyle, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::RowHeight, PropLong, -1 );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddBooleanProperty( Prop::VScrollBar, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventFolderChanged );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlDwgPreview:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -22 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlFileExplorer:
		pDclControl->AddBooleanProperty( Prop::CreationPrompt, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ExtCanBeDiff, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::FileMustExist, PropBool, true );
		pDclControl->AddStringProperty( Prop::Filter, PropString, theWorkspace.LoadResourceString( IDS_FILEDLGFILTERDEF ) );
		pDclControl->AddBooleanProperty( Prop::MultiSelect, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::AsReadOnly, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::OverWritePrompt, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::PathMustExist, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowCancel, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowHelp, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowNameLabel, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowNameTextBox, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowOK, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowReadOnlyCheckBox, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowTypeComboBox, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowTypeLabel, PropBool, true );
		pDclControl->AddLongProperty( Prop::FileDlgStyle, PropEnum, 1 );
		AddControlEvent( pDclControl, Prop::EventFolderChanged );
		AddControlEvent( pDclControl, Prop::EventOnHelp );
		AddControlEvent( pDclControl, Prop::EventOnTypeChange );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		break;

	case CtlFrame:
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		break;

	case CtlGraphicButton:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -16 );
		pDclControl->AddStringProperty( Prop::Caption, PropString );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::Picture, PropPicture );
		pDclControl->AddStringProperty( Prop::MouseOverPicture, PropPicture );
		pDclControl->AddLongProperty( Prop::ButtonStyle, PropEnum, 0 );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlGrid:
		pDclControl->AddBooleanProperty( Prop::RowHeader, PropBool, true );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::AlternatingColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::AlternateOrient, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::ColHeader, PropBool, true );
		AddControlFontProperties( pDclControl, FS );
		//pDclControl->AddBooleanProperty( Prop::FullRowSelect, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::GridLines, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::LabelWrap, PropBool, true );
		pDclControl->AddStringProperty( Prop::ImageList, PropImageList );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::ListViewSort, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::RowHeight, PropLong, 24 );
		AddControlHiddenProperty( pDclControl, Prop::ColumnCaptions, _T(""), PropStringArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnAlignments, _T(""), PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnImages, _T(""), PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnWidths, _T(""), PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnStyles, _T(""), PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnDefaultImages, _T(""), PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnAlternateImages, _T(""), PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnListItems, _T(""), PropStringArrayList );
		AddControlHiddenProperty( pDclControl, Prop::ColumnListImages, _T(""), PropIntArrayList );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventBtnClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventBeginLabelEdit );
		AddControlEvent( pDclControl, Prop::EventEndLabelEdit );
		AddControlEvent( pDclControl, Prop::EventColumnClick );
		AddControlEvent( pDclControl, Prop::EventMouseDown );
		AddControlEvent( pDclControl, Prop::EventMouseUp );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlHatch:
		pDclControl->AddStringProperty( Prop::HatchScale, PropDouble, _T("5") );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -22 );
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, 7 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventRClick );
		AddControlEvent( pDclControl, Prop::EventRDblClick );
		AddControlEvent( pDclControl, Prop::EventMouseDown );
		AddControlEvent( pDclControl, Prop::EventMouseUp );
		AddControlEvent( pDclControl, Prop::EventMouseDblClick );
		break;

	case CtlHtml:
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventNavigateComplete );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlHyperlink:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, 5 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		//pDclControl->AddLongProperty( Prop::URLLinkType, PropEnum, 0 );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddStringProperty( Prop::Hyperlink, PropString, _T("http://www.OpenDCL.com") );
		break;

	case CtlImageComboBox:
		pDclControl->AddLongProperty( Prop::DropDownHeight, PropLong, 100 );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddStringProperty( Prop::ImageList, PropImageList );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ItemData, PropIntArray );
		pDclControl->AddLongProperty( Prop::TextLimit, PropLong, 256 );
		pDclControl->AddStringProperty( Prop::List, PropStringArray );
		pDclControl->AddBooleanProperty( Prop::ReturnAsTab, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::Sorted, PropBool, false );
		pDclControl->AddLongProperty( Prop::ComboBoxStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::Text, PropString, pDclControl->GetKeyName() );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventEditChanged );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		//AddControlEvent( pDclControl, Prop::EventUpdate );
		AddControlEvent( pDclControl, Prop::EventDropDown );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlLabel:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddLongProperty( Prop::Justification, PropEnum, 0 );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlListBox:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::VScrollBar, PropBool, false );
		AddControlFontProperties( pDclControl, FS );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddLongProperty( Prop::ColumnWidth, PropLong, 25 );
		pDclControl->AddBooleanProperty( Prop::DisableNoScroll, PropBool, false );
		pDclControl->AddStringProperty( Prop::ItemData, PropIntArray );
		pDclControl->AddStringProperty( Prop::List, PropStringArray );
		pDclControl->AddBooleanProperty( Prop::MultiColumn, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::NoIntegralHeight, PropBool, true );
		pDclControl->AddLongProperty( Prop::SelectionStyle, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::Sorted, PropBool, false );
		//pDclControl->AddBooleanProperty( Prop::UseTabStops, PropBool, false );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventRClick );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlListView:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlFontProperties( pDclControl, FS );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddBooleanProperty( Prop::ColHeader, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::EditLabels, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::FullRowSelect, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::GridLines, PropBool, false );
		pDclControl->AddLongProperty( Prop::IconXSpacing, PropLong, 20 );
		pDclControl->AddLongProperty( Prop::IconYSpacing, PropLong, 32 );
		pDclControl->AddBooleanProperty( Prop::LabelWrap, PropBool, true );
		pDclControl->AddLongProperty( Prop::ListViewIconAlign, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::AutoArrange, PropBool, false );
		pDclControl->AddStringProperty( Prop::ImageList, PropImageList );
		pDclControl->AddBooleanProperty( Prop::MultiSelect, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::ShowSelectAlways, PropBool, false );
		pDclControl->AddLongProperty( Prop::ListViewSort, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::ListViewStyle, PropEnum, 3 );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::EventRClick );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventRDblClick );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventReturnPressed );
		AddControlEvent( pDclControl, Prop::EventBeginLabelEdit );
		AddControlEvent( pDclControl, Prop::EventEndLabelEdit );
		AddControlEvent( pDclControl, Prop::EventColumnClick );
		AddControlEvent( pDclControl, Prop::EventKeyDown );
		AddControlEvent( pDclControl, Prop::EventKeyUp );
		AddControlEvent( pDclControl, Prop::EventMouseDown );
		AddControlEvent( pDclControl, Prop::EventMouseUp );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlOptionButton:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		pDclControl->AddBooleanProperty( Prop::BeginGroup, PropBool, false );
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddLongProperty( Prop::Value, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlOptionList:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::VScrollBar, PropBool, false );
		AddControlFontProperties( pDclControl, FS );
		{
			TPropertyPtr pProp = pDclControl->AddStringProperty( Prop::BtnCaption, PropStringArray );
			if( pProp->size() == 0 )
			{ // add three default option values
				pProp->GetStringArrayPtr()->push_back( theWorkspace.LoadResourceString( IDS_OPTION1 ) );
				pProp->GetStringArrayPtr()->push_back( theWorkspace.LoadResourceString( IDS_OPTION2 ) );
				pProp->GetStringArrayPtr()->push_back( theWorkspace.LoadResourceString( IDS_OPTION3 ) );
			}
		}
		pDclControl->AddStringProperty( Prop::BtnToolTips, PropStringArray );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddLongProperty( Prop::CurSelIndex, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::RowHeight, PropLong, -1 );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlPictureBox:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, false );
		AddControlFontProperties( pDclControl, FS );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddBooleanProperty( Prop::AutoSize, PropBool, false );
		pDclControl->AddStringProperty( Prop::Picture, PropPicture );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventKeyDown);
		AddControlEvent( pDclControl, Prop::EventKeyUp );
		AddControlEvent( pDclControl, Prop::EventMouseDown );
		AddControlEvent( pDclControl, Prop::EventMouseUp );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventMouseMovedOff );
		AddControlEvent( pDclControl, Prop::EventMouseDblClick );
		AddControlEvent( pDclControl, Prop::EventMouseWheel );
		AddControlEvent( pDclControl, Prop::EventPaint );
		AddControlEvent( pDclControl, Prop::EventMouseEntered );
		AddControlEvent( pDclControl, Prop::EventRClick );
		AddControlEvent( pDclControl, Prop::EventRDblClick );
		break;

	case CtlProgressBar:
		pDclControl->AddStringProperty( Prop::CaptionSecond, PropString );
		pDclControl->AddStringProperty( Prop::CaptionSeconds, PropString );
		pDclControl->AddStringProperty( Prop::CaptionMinute, PropString );
		pDclControl->AddStringProperty( Prop::CaptionMinutes, PropString );
		pDclControl->AddStringProperty( Prop::CaptionRemaining, PropString );
		pDclControl->AddLongProperty( Prop::ProgressLegend, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddLongProperty( Prop::MaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( Prop::MinValue, PropLong, 1 );
		pDclControl->AddLongProperty( Prop::Orientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddBooleanProperty( Prop::SmoothProgress, PropBool, false );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		pDclControl->AddLongProperty( Prop::Value, PropLong, 10 );
		break;

	case CtlRectangle:
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -24 );
		break;

	case CtlScrollBar:
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -16 );
		pDclControl->AddLongProperty( Prop::LargeChange, PropLong, 5 );
		pDclControl->AddLongProperty( Prop::MaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( Prop::MinValue, PropLong, 1 );
		pDclControl->AddLongProperty( Prop::Orientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddLongProperty( Prop::SmallChange, PropLong, 1 );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		pDclControl->AddLongProperty( Prop::Value, PropLong, 1 );
		AddControlEvent( pDclControl, Prop::EventScroll );
		AddControlEvent( pDclControl, Prop::EventScrolled );
		break;

	case CtlSlider:
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -16 );
		pDclControl->AddLongProperty( Prop::LargeChange, PropLong, 5 );
		pDclControl->AddLongProperty( Prop::MaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( Prop::MinValue, PropLong, 1 );
		pDclControl->AddLongProperty( Prop::Orientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddBooleanProperty( Prop::ShowTicks, PropBool, true );
		pDclControl->AddLongProperty( Prop::SmallChange, PropLong, 1 );
		pDclControl->AddLongProperty( Prop::TickFrequency, PropLong, 10 );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		pDclControl->AddLongProperty( Prop::Value, PropLong, 0 );
		AddControlEvent( pDclControl, Prop::EventScroll );
		AddControlEvent( pDclControl, Prop::EventReleasedCapture );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlSlideView:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -22 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventKeyDown);
		AddControlEvent( pDclControl, Prop::EventKeyUp );
		AddControlEvent( pDclControl, Prop::EventMouseDown );
		AddControlEvent( pDclControl, Prop::EventMouseUp );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventMouseMovedOff );
		AddControlEvent( pDclControl, Prop::EventMouseDblClick );
		AddControlEvent( pDclControl, Prop::EventMouseWheel );
		AddControlEvent( pDclControl, Prop::EventPaint );
		AddControlEvent( pDclControl, Prop::EventMouseEntered );
		AddControlEvent( pDclControl, Prop::EventRClick );
		AddControlEvent( pDclControl, Prop::EventRDblClick );
		break;

	case CtlSpinButton:
		pDclControl->AddBooleanProperty( Prop::AutoWrap, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::MaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( Prop::MinValue, PropLong, 1 );
		pDclControl->AddLongProperty( Prop::Orientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		pDclControl->AddLongProperty( Prop::Value, PropLong, 1 );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventChanged );
		break;

	case CtlSplitter:
		pDclControl->AddLongProperty( Prop::SplitterStyle, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::SplitterMin, PropLong, 30 );
		pDclControl->AddLongProperty( Prop::SplitterMax, PropLong, 30 );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventSplitterMoved );
		break;

	case CtlTabStrip:
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddLongProperty( Prop::LabelAlignment, PropEnum, 1 );
		pDclControl->AddLongProperty( Prop::MinTabWidth, PropLong, -1 );
		pDclControl->AddBooleanProperty( Prop::TabFixedWidth, PropBool, false );
		pDclControl->AddLongProperty( Prop::TabStyle, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::MultiRow, PropBool, false );
		//pDclControl->AddLongProperty( Prop::TabJustified, PropEnum, 0 );
		AddControlHiddenProperty( pDclControl, Prop::TabsCaption, _T(""), PropStringArray );
		AddControlHiddenProperty( pDclControl, Prop::TabsImageList, _T(""), PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::TabsTTT, _T(""), PropStringArray );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, false );
		AddControlHiddenProperty( pDclControl, Prop::ImageList, _T(""), PropImageList );
		AddControlEvent( pDclControl, Prop::EventChanged );
		AddControlEvent( pDclControl, Prop::EventSelChanging );
		break;

	case CtlTextBox:
		pDclControl->AddBooleanProperty( Prop::AutoHScroll, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::AutoVScroll, PropBool, true );
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddLongProperty( Prop::FilterStyle, PropEnum, 0 );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::HScrollBar, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::Justification, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::TextLimit, PropLong, 32760 );
		pDclControl->AddLongProperty( Prop::MarginLeft, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MarginRight, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ReadOnly, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::ReturnAsTab, PropBool, false );
		pDclControl->AddStringProperty( Prop::Text, PropString, pDclControl->GetKeyName() );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddBooleanProperty( Prop::VScrollBar, PropBool, false );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::EventEditChanged );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventUpdate );
		AddControlEvent( pDclControl, Prop::EventMaxText );
		AddControlEvent( pDclControl, Prop::EventReturnPressed );
		AddControlEvent( pDclControl, Prop::EventKeyDown );
		AddControlEvent( pDclControl, Prop::EventKeyUp );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlTextButton:
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlTooltipProperties( pDclControl );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlTree:
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::EditLabels, PropBool, false );
		AddControlFontProperties( pDclControl, FS );
		pDclControl->AddBooleanProperty( Prop::HasButtons, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::HasLines, PropBool, true );
		pDclControl->AddStringProperty( Prop::ImageList, PropImageList );
		pDclControl->AddLongProperty( Prop::Indent, PropLong, 17 );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::LinesAtRoot, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowSelectAlways, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::SingleExpanded, PropBool, false );
		AddControlTooltipProperties( pDclControl );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromOther );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKeyDown );
		AddControlEvent( pDclControl, Prop::EventKeyUp );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventRClick );
		AddControlEvent( pDclControl, Prop::EventRDblClick );
		AddControlEvent( pDclControl, Prop::EventBeginLabelEdit );
		AddControlEvent( pDclControl, Prop::EventDeleteItem );
		AddControlEvent( pDclControl, Prop::EventEndLabelEdit );
		AddControlEvent( pDclControl, Prop::EventItemExpanded );
		AddControlEvent( pDclControl, Prop::EventItemExpanding );
		break;

	default:
		TraceFmt( _T("* AddDefaultProperties() called with unknown control type (%d)\r\n"), type );
		assert( false );
		return false;
		break;
	}
	return true;
};
