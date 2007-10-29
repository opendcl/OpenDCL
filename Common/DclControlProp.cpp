// DclControlProp.cpp : implementation file
//

#include "stdafx.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "SharedRes.h"
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
		CString sFormName;
		sFormName.Format( _T("%s%d"),
											theWorkspace.LoadResourceString( IDS_DCLFORM ),
											pDclControl->GetOwnerProject()->GetDclFormList().size() );
		pDclControl->AddStringProperty( Prop::Name, PropString, sFormName );
	}
	return true;
}

static bool AddDefaultFormProperties( TDclControlPtr pDclControl, long lWidth /*= 32*/, long lHeight /*= 32*/ )
{
	assert( pDclControl->GetType() == CtlForm );
	TDclFormPtr pOwnerForm = pDclControl->GetOwnerForm();
	assert( pOwnerForm != NULL );
	if( !pOwnerForm )
		return false;
	switch( pOwnerForm->GetType() )
	{
	case VdclModal:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( Prop::ObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( Prop::GlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( Prop::Resizable, PropBool, false );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 350 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 250 );
		pDclControl->AddLongProperty( Prop::MinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MinDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MaxDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::Icon, PropPicture, -1 );
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
	case VdclModeless:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( Prop::ObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( Prop::GlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( Prop::Resizable, PropBool, true );
		pDclControl->AddLongProperty( Prop::EventInvoke, PropEnum, 0 );
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
		AddControlEvent( pDclControl, Prop::EventOnHelp );
		break;
	case VdclDockable:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( Prop::ObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( Prop::GlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( Prop::Resizable, PropBool, true );
		pDclControl->AddLongProperty( Prop::EventInvoke, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 250 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 450 );
		pDclControl->AddStringProperty( Prop::TitleBarText, PropString, pOwnerForm->GetKeyName() );
		pDclControl->AddLongProperty( Prop::DockableSides, PropEnum, 0 );
		AddControlEvent( pDclControl, Prop::FormEventInitialize );
		AddControlEvent( pDclControl, Prop::FormEventClose );
		AddControlEvent( pDclControl, Prop::FormEventSize );
		AddControlEvent( pDclControl, Prop::DocEventActivated );
		AddControlEvent( pDclControl, Prop::DocEventEnteringNoDocState );
		AddControlEvent( pDclControl, Prop::EventOnHelp );
		break;
	case VdclConfigTab:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( Prop::ObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( Prop::GlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( Prop::Resizable, PropBool, true );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 600 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 380 );
		pDclControl->AddLongProperty( Prop::MinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MinDialogHeight, PropLong, 0 );
		pDclControl->AddStringProperty( Prop::CfgTabCaption, PropString, pOwnerForm->GetKeyName() );
		AddControlEvent( pDclControl, Prop::FormEventInitialize );
		AddControlEvent( pDclControl, Prop::FormEventShow );
		AddControlEvent( pDclControl, Prop::CfgEventCancel );
		AddControlEvent( pDclControl, Prop::CfgEventHelp );
		AddControlEvent( pDclControl, Prop::CfgEventOK );
		break;
	case VdclFileDialog:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( Prop::ObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( Prop::GlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( Prop::Resizable, PropBool, true );
		pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth > 0? lWidth : 427 );
		pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight > 0? lHeight : 296 );
		pDclControl->AddStringProperty( Prop::TitleBarText, PropString, theWorkspace.LoadResourceString( IDS_OPEN ) );
		AddControlEvent( pDclControl, Prop::FormEventInitialize );
		AddControlEvent( pDclControl, Prop::FormEventCancelClose );
		AddControlEvent( pDclControl, Prop::FormEventClose );
		AddControlEvent( pDclControl, Prop::FormEventSize );
		AddControlEvent( pDclControl, Prop::EventOnHelp );
		break;
	case VdclTabForm:
		AddControlHiddenProperty( pDclControl, Prop::Width, lWidth > 0? lWidth : 600, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::Height, lHeight > 0? lHeight : 380, PropLong );
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
	assert( type != CtlInvalid );
	if( type == CtlInvalid )
		return false;
	if( type == CtlForm )
		return AddDefaultFormProperties( pDclControl, lWidth, lHeight );
	TDclFormPtr pOwnerForm = pDclControl->GetOwnerForm();
	assert( pOwnerForm != NULL );
	if( !pOwnerForm )
		return false;

	CString sList = theWorkspace.LoadResourceString( IDS_LIST );

	//Add default properties that apply to every control type
	pDclControl->AddStringProperty( Prop::Name, PropString, pDclControl->GetKeyName() );
	pDclControl->AddStringProperty( Prop::ObjectBrowser, PropActiveXMethods );
	pDclControl->AddStringProperty( Prop::Custom, PropCustom ); // add the Custom property (to display the button)
	pDclControl->AddStringProperty( Prop::GlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
	pDclControl->AddLongProperty( Prop::Left, PropLong, 0 );
	pDclControl->AddLongProperty( Prop::Top, PropLong, 0 );
	if( lWidth <= 0 )
		lWidth = 32;
	pDclControl->AddLongProperty( Prop::Width, PropLong, lWidth );
	if( lHeight <= 0 )
		lHeight = 32;
	pDclControl->AddLongProperty( Prop::Height, PropLong, lHeight );

	if( type != CtlFileDlgCtrl )
	{
		pDclControl->AddLongProperty( Prop::BottomFromBottom, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::LeftFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::RightFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::TopFromBottom, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::UseBottomFromBottom, PropLong, (type == CtlTabStrip)? 1 : 0 );
		pDclControl->AddLongProperty( Prop::UseLeftFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::UseRightFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::UseTopFromBottom, PropLong, (type == CtlTabStrip)? 1 : 0 );
		pDclControl->AddBooleanProperty( Prop::Visible, PropBool, true );
	}

	if (type != CtlActiveX && type != CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( Prop::Enabled, PropBool, true );

	TDclFormPtr pTopLevelParentForm = pOwnerForm;
	while( pTopLevelParentForm->GetParentForm() )
		pTopLevelParentForm = pTopLevelParentForm->GetParentForm();
	DclFormType eFormType = pTopLevelParentForm->GetType();
	switch( eFormType )
	{
	case VdclDockable:
	case VdclModeless:
		pDclControl->AddLongProperty( Prop::EventInvoke, PropEnum, 0 );
		break;
	}

	//Add control specific properties
	switch( type )
	{
	case Ctl3DRect:
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		break;

	case CtlActiveX:
		pDclControl->AddStringProperty( Prop::ActiveXPropPages, PropActiveXPropPages );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		break;

	case CtlAnimate:
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 0 );
		break;

	case CtlBlockList:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddStringProperty( Prop::BlockName, PropString );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, true );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddLongProperty( Prop::IconXSpacing, PropLong, 20 );
		pDclControl->AddLongProperty( Prop::IconYSpacing, PropLong, 32 );
		pDclControl->AddBooleanProperty( Prop::LabelWrap, PropBool, true );
		pDclControl->AddLongProperty( Prop::ListViewIconAlign, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::AutoArrange, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::MultiSelect, PropBool, false );
		pDclControl->AddLongProperty( Prop::ListViewSort, PropEnum, 2 );
		pDclControl->AddLongProperty( Prop::BlockListStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventReturnPressed );
		AddControlEvent( pDclControl, Prop::EventKeyDown );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlBlockView:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -22 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddLongProperty( Prop::AllowOrbiting, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::RenderMode, PropEnum, 5 );
		pDclControl->AddBooleanProperty( Prop::ShowOrbitCirlces, PropBool, false );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
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

	case CtlCheckBox:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -16 );
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::Value, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlComboBox:
		pDclControl->AddLongProperty( Prop::DropDownHeight, PropLong, 100 );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ItemData, PropIntArray, sList );
		pDclControl->AddLongProperty( Prop::LimitText, PropLong, 256 );
		pDclControl->AddStringProperty( Prop::List, PropStringArray, sList );
		pDclControl->AddBooleanProperty( Prop::ReturnAsTab, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::Sorted, PropBool, false );
		pDclControl->AddLongProperty( Prop::ComboBoxStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::Text, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventEditChanged );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		//AddControlEvent( pDclControl, Prop::EventUpdate );
		AddControlEvent( pDclControl, Prop::EventDropDown );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlDwgList:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DisableNoScroll, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, true );
		pDclControl->AddLongProperty( Prop::InsertOrXref, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::SelectStyle, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::RowHeight, PropLong, -1 );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::VScrollBar, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventFolderChanged );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlDwgPreview:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -22 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlFileDlgCtrl:
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
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlGraphicButton:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -16 );
		pDclControl->AddStringProperty( Prop::Caption, PropString );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::Picture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::MouseOverPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddLongProperty( Prop::ButtonStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlGrid:
		pDclControl->AddBooleanProperty( Prop::RowHeader, PropBool, true );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::AlternateColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::AlternateOrient, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::ColHeader, PropBool, true );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		//pDclControl->AddBooleanProperty( Prop::FullRowSelect, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::GridLines, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::LabelWrap, PropBool, true );
		pDclControl->AddStringProperty( Prop::ImageList, PropImageList, theWorkspace.LoadResourceString( IDS_IMAGELIST ) );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::ListViewSort, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::RowHeight, PropLong, 24 );
		AddControlHiddenProperty( pDclControl, Prop::ColumnCaptions, sList, PropStringArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnAlignments, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnImages, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnWidths, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnStyles, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnDefaultImages, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnAlternateImages, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::ColumnListItems, sList, PropStringArrayList );
		AddControlHiddenProperty( pDclControl, Prop::ColumnListImages, sList, PropIntArrayList );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
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
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlHatch:
		pDclControl->AddStringProperty( Prop::HatchScale, PropDouble, _T("5") );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -22 );
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, 7 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
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

	case CtlHtmlCtrl:
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventNavigateComplete );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		break;

	case CtlImageComboBox:
		pDclControl->AddLongProperty( Prop::DropDownHeight, PropLong, 100 );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddStringProperty( Prop::ImageList, PropImageList, theWorkspace.LoadResourceString(IDS_IMAGELIST) );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::LimitText, PropLong, 256 );
		pDclControl->AddBooleanProperty( Prop::ReturnAsTab, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::Sorted, PropBool, false );
		pDclControl->AddLongProperty( Prop::ComboBoxStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::Text, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventEditChanged );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		//AddControlEvent( pDclControl, Prop::EventUpdate );
		AddControlEvent( pDclControl, Prop::EventDropDown );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlLabel:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -16 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddLongProperty( Prop::Justification, PropEnum, 0 );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlListBox:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddLongProperty( Prop::ColumnWidth, PropLong, 25 );
		pDclControl->AddBooleanProperty( Prop::DisableNoScroll, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ItemData, PropIntArray, sList );
		pDclControl->AddStringProperty( Prop::List, PropStringArray, sList );
		pDclControl->AddBooleanProperty( Prop::MultiColumn, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::NoIntegralHeight, PropBool, true );
		pDclControl->AddLongProperty( Prop::SelectStyle, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::Sorted, PropBool, false );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::UseTabStops, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::VScrollBar, PropBool, false );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventRClick );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlListView:
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::ColHeader, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::EditLabels, PropBool, true );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::FullRowSelect, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::GridLines, PropBool, false );
		pDclControl->AddLongProperty( Prop::IconXSpacing, PropLong, 20 );
		pDclControl->AddLongProperty( Prop::IconYSpacing, PropLong, 32 );
		pDclControl->AddBooleanProperty( Prop::LabelWrap, PropBool, true );
		pDclControl->AddLongProperty( Prop::ListViewIconAlign, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::AutoArrange, PropBool, false );
		pDclControl->AddStringProperty( Prop::ImageList, PropImageList, theWorkspace.LoadResourceString(IDS_IMAGELIST) );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::MultiSelect, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::ShowSelectAlways, PropBool, false );
		pDclControl->AddLongProperty( Prop::ListViewSort, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::ListViewStyle, PropEnum, 3 );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::EventRClick );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventRDblClick );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventReturnPressed );
		AddControlEvent( pDclControl, Prop::EventKeyDown );
		AddControlEvent( pDclControl, Prop::EventBeginLabelEdit );
		AddControlEvent( pDclControl, Prop::EventEndLabelEdit );
		AddControlEvent( pDclControl, Prop::EventColumnClick );
		AddControlEvent( pDclControl, Prop::EventKeyDown );
		AddControlEvent( pDclControl, Prop::EventKeyUp );
		AddControlEvent( pDclControl, Prop::EventMouseDown );
		AddControlEvent( pDclControl, Prop::EventMouseUp );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlMonth:
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::MultiSelection, PropLong, 1 );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventGetDayState );
		AddControlEvent( pDclControl, Prop::EventSelect );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlOptionButton:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -16 );
		pDclControl->AddBooleanProperty( Prop::BeginGroup, PropBool, false );
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::Value, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlOptionList:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, -19 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -16 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 0 );
		{
			TPropertyPtr pProp = pDclControl->AddStringProperty( Prop::BtnCaption, PropStringArray, sList );
			if( pProp->GetStringArrayPtr()->empty() )
			{ // add three default option values
				pProp->GetStringArrayPtr()->push_back( theWorkspace.LoadResourceString( IDS_OPTION1 ) );
				pProp->GetStringArrayPtr()->push_back( theWorkspace.LoadResourceString( IDS_OPTION2 ) );
				pProp->GetStringArrayPtr()->push_back( theWorkspace.LoadResourceString( IDS_OPTION3 ) );
			}
		}
		pDclControl->AddStringProperty( Prop::BtnTTText, PropStringArray, sList );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddLongProperty( Prop::DefSelIndex, PropLong, 0 );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::RowHeight, PropLong, -1 );
		pDclControl->AddBooleanProperty( Prop::VScrollBar, PropBool, false );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventSelChanged );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlPictureBox:
		pDclControl->AddBooleanProperty( Prop::AutoSize, PropBool, false );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -6 );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::Picture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
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
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlProgress:
		pDclControl->AddStringProperty( Prop::SecondText, PropString, _T("second") );
		pDclControl->AddStringProperty( Prop::SecondsText, PropString, _T("seconds") );
		pDclControl->AddStringProperty( Prop::MinuteText, PropString, _T("minute") );
		pDclControl->AddStringProperty( Prop::MinutesText, PropString, _T("minutes") );
		pDclControl->AddBooleanProperty( Prop::DisplaySeconds, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DisplayPercentage, PropBool, true );
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 2 );
		pDclControl->AddLongProperty( Prop::MaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( Prop::MinValue, PropLong, 1 );
		pDclControl->AddLongProperty( Prop::Orientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddBooleanProperty( Prop::SmoothProgress, PropBool, false );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		pDclControl->AddLongProperty( Prop::Value, PropLong, 0 );
		break;

	case CtlRoundSlider:
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddLongProperty( Prop::Value, PropLong, 0 );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlEvent( pDclControl, Prop::EventScroll );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlScrollBar:
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
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
		pDclControl->AddLongProperty( Prop::LargeChange, PropLong, 5 );
		pDclControl->AddLongProperty( Prop::MaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( Prop::MinValue, PropLong, 1 );
		pDclControl->AddLongProperty( Prop::Orientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddBooleanProperty( Prop::ShowTicks, PropBool, true );
		pDclControl->AddLongProperty( Prop::SmallChange, PropLong, 1 );
		pDclControl->AddLongProperty( Prop::TickFrequency, PropLong, 10 );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		pDclControl->AddLongProperty( Prop::Value, PropLong, 1 );
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
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
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
		break;

	case CtlStaticURL:
		pDclControl->AddLongProperty( Prop::ForegroundColor, PropLong, 5 );
		pDclControl->AddLongProperty( Prop::BackgroundColor, PropLong, -16 );
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::URLLinkType, PropEnum, 0 );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddStringProperty( Prop::URLAddress, PropString );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlStdButton:
		pDclControl->AddStringProperty( Prop::Caption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlTabStrip:
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddLongProperty( Prop::TabLabelAlign, PropEnum, 1 );
		pDclControl->AddLongProperty( Prop::MinTabWidth, PropLong, -1 );
		pDclControl->AddBooleanProperty( Prop::TabFixedWidth, PropBool, false );
		pDclControl->AddLongProperty( Prop::TabStyle, PropEnum, 0 );
		pDclControl->AddBooleanProperty( Prop::MultiRow, PropBool, false );
		//pDclControl->AddLongProperty( Prop::TabJustified, PropEnum, 0 );
		AddControlHiddenProperty( pDclControl, Prop::TabsCaption, sList, PropStringArray );
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_TAB1));
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_TAB2));
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_TAB3));
		AddControlHiddenProperty( pDclControl, Prop::TabsImageList, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, Prop::TabsTTT, sList, PropStringArray );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::UseVisualStyle, PropBool, true );
		AddControlHiddenProperty( pDclControl, Prop::ImageList, theWorkspace.LoadResourceString( IDS_IMAGELIST ), PropImageList );
		AddControlEvent( pDclControl, Prop::EventChanged );
		AddControlEvent( pDclControl, Prop::EventSelChanging );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
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
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::HScrollBar, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddLongProperty( Prop::Justification, PropEnum, 0 );
		pDclControl->AddLongProperty( Prop::LimitText, PropLong, 32760 );
		pDclControl->AddLongProperty( Prop::MarginLeft, PropLong, 0 );
		pDclControl->AddLongProperty( Prop::MarginRight, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ReadOnly, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::ReturnAsTab, PropBool, false );
		pDclControl->AddStringProperty( Prop::Text, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::VScrollBar, PropBool, false );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::EventEditChanged );
		AddControlEvent( pDclControl, Prop::EventKillFocus );
		AddControlEvent( pDclControl, Prop::EventSetFocus );
		AddControlEvent( pDclControl, Prop::EventUpdate );
		AddControlEvent( pDclControl, Prop::EventMaxText );
		AddControlEvent( pDclControl, Prop::EventReturnPressed );
		AddControlEvent( pDclControl, Prop::EventKeyDown );
		AddControlEvent( pDclControl, Prop::EventKeyUp );
		AddControlEvent( pDclControl, Prop::EventMouseMove );
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlTree:
		pDclControl->AddLongProperty( Prop::BorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::DragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::EditLabels, PropBool, false );
		pDclControl->AddStringProperty( Prop::FontName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( Prop::HasButtons, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::HasLines, PropBool, true );
		pDclControl->AddStringProperty( Prop::ImageList, PropImageList, theWorkspace.LoadResourceString( IDS_IMAGELIST ) );
		pDclControl->AddLongProperty( Prop::Indent, PropLong, 17 );
		pDclControl->AddBooleanProperty( Prop::IsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::LinesAtRoot, PropBool, true );
		pDclControl->AddBooleanProperty( Prop::ShowSelectAlways, PropBool, false );
		pDclControl->AddBooleanProperty( Prop::SingleExpanded, PropBool, false );
		pDclControl->AddStringProperty( Prop::ToolTipTitle, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipBody, PropString );
		pDclControl->AddStringProperty( Prop::ToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( Prop::ToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( Prop::ToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( Prop::ToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( Prop::ToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, Prop::EventClicked );
		AddControlEvent( pDclControl, Prop::DragnDropToAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropFromControl );
		AddControlEvent( pDclControl, Prop::DragnDropFromAutoCAD );
		AddControlEvent( pDclControl, Prop::DragnDropBegin );
		AddControlEvent( pDclControl, Prop::EventDblClicked );
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
		AddControlHiddenProperty( pDclControl, Prop::FontBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, Prop::FontStrikeout, false, PropBool );
		AddControlHiddenProperty( pDclControl, Prop::FontUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	default:
		TraceFmt( _T("* AddDefaultProperties() called with unknown control type (%d)\r\n"), type );
		assert( false );
		return false;
		break;
	}
	return true;
};
