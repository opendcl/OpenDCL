// DclControlProp.cpp : implementation file
//

#include "stdafx.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "SharedRes.h"
#include "Project.h"
#include "PropertyIds.h"
#include "ControlTypes.h"


static RefCountedPtr< CPropertyObject > AddControlHiddenProperty( CDclControlObject* pDclControl,
																																	PropertyId nID,
																																	LPCTSTR pszValue,
																																	PropertyType type )
{
	RefCountedPtr< CPropertyObject > pProp = pDclControl->AddStringProperty( nID, type, pszValue );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}

static RefCountedPtr< CPropertyObject > AddControlHiddenProperty( CDclControlObject* pDclControl,
																																	PropertyId nID,
																																	long lValue,
																																	PropertyType type )
{
	RefCountedPtr< CPropertyObject > pProp = pDclControl->AddLongProperty( nID, type, lValue );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}

static RefCountedPtr< CPropertyObject > AddControlHiddenProperty( CDclControlObject* pDclControl,
																																	PropertyId nID,
																																	bool bValue,
																																	PropertyType type )
{
	RefCountedPtr< CPropertyObject > pProp = pDclControl->AddBooleanProperty( nID, type, bValue );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}

static RefCountedPtr< CPropertyObject > AddControlEvent( CDclControlObject* pDclControl, PropertyId nID )
{
	RefCountedPtr< CPropertyObject > pProp = pDclControl->AddStringProperty( nID, PropEvent, NULL );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}

static bool AddDefaultFormName( CDclControlObject* pDclControl )
{
	if( !pDclControl->GetPropertyObject( nName ) )
	{
		CString sFormName;
		sFormName.Format( _T("%s%d"),
											theWorkspace.LoadResourceString( IDS_DCLFORM ),
											pDclControl->GetOwnerProject()->GetDclFormList().GetCount() );
		pDclControl->AddStringProperty( nName, PropString, sFormName );
	}
	return true;
}

static bool AddDefaultFormProperties( CDclControlObject* pDclControl, long lWidth /*= 32*/, long lHeight /*= 32*/ )
{
	assert( pDclControl->GetType() == CtlForm );
	CDclFormObject* pOwnerForm = pDclControl->GetOwnerForm();
	assert( pOwnerForm != NULL );
	if( !pOwnerForm )
		return false;
	switch( pOwnerForm->GetType() )
	{
	case VdclModal:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( nObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( nGlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( nResizable, PropBool, false );
		pDclControl->AddLongProperty( nWidth, PropLong, lWidth > 0? lWidth : 350 );
		pDclControl->AddLongProperty( nHeight, PropLong, lHeight > 0? lHeight : 250 );
		pDclControl->AddLongProperty( nMinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( nMinDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( nMaxDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( nMaxDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( nIcon, PropPicture, -1 );
		pDclControl->AddBooleanProperty( nTitleBar, PropBool, true );
		pDclControl->AddStringProperty( nTitleBarText, PropString, pOwnerForm->GetKeyName() );
		AddControlEvent( pDclControl, nFormEventInitialize );
		AddControlEvent( pDclControl, nFormEventClose );
		AddControlEvent( pDclControl, nFormEventSize );
		AddControlEvent( pDclControl, nFormEventOnOk );
		AddControlEvent( pDclControl, nFormEventOnCancel );
		AddControlEvent( pDclControl, nFormEventCancelClose );
		AddControlEvent( pDclControl, nEventOnHelp );
		break;
	case VdclModeless:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( nObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( nGlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( nResizable, PropBool, true );
		pDclControl->AddLongProperty( nEventInvoke, PropEnum, 0 );
		pDclControl->AddLongProperty( nWidth, PropLong, lWidth > 0? lWidth : 250 );
		pDclControl->AddLongProperty( nHeight, PropLong, lHeight > 0? lHeight : 150 );
		pDclControl->AddLongProperty( nMinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( nMinDialogHeight, PropLong, 0 );
		pDclControl->AddLongProperty( nMaxDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( nMaxDialogHeight, PropLong, 0 );
		pDclControl->AddBooleanProperty( nTitleBar, PropBool, true );
		pDclControl->AddStringProperty( nTitleBarText, PropString, pOwnerForm->GetKeyName() );
		AddControlEvent( pDclControl, nFormEventInitialize );
		AddControlEvent( pDclControl, nFormEventClose );
		AddControlEvent( pDclControl, nFormEventSize );
		AddControlEvent( pDclControl, nFormEventOnOk );
		AddControlEvent( pDclControl, nFormEventOnCancel );
		AddControlEvent( pDclControl, nFormEventCancelClose );
		AddControlEvent( pDclControl, nDocEventActivated );
		AddControlEvent( pDclControl, nEventOnHelp );
		break;
	case VdclDockable:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( nObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( nGlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( nResizable, PropBool, true );
		pDclControl->AddLongProperty( nEventInvoke, PropEnum, 0 );
		pDclControl->AddLongProperty( nWidth, PropLong, lWidth > 0? lWidth : 250 );
		pDclControl->AddLongProperty( nHeight, PropLong, lHeight > 0? lHeight : 450 );
		pDclControl->AddStringProperty( nTitleBarText, PropString, pOwnerForm->GetKeyName() );
		pDclControl->AddLongProperty( nDockableSides, PropEnum, 0 );
		AddControlEvent( pDclControl, nFormEventInitialize );
		AddControlEvent( pDclControl, nFormEventClose );
		AddControlEvent( pDclControl, nFormEventSize );
		AddControlEvent( pDclControl, nDocEventActivated );
		AddControlEvent( pDclControl, nEventOnHelp );
		break;
	case VdclConfigTab:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( nObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( nGlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( nResizable, PropBool, true );
		pDclControl->AddLongProperty( nWidth, PropLong, lWidth > 0? lWidth : 600 );
		pDclControl->AddLongProperty( nHeight, PropLong, lHeight > 0? lHeight : 380 );
		pDclControl->AddLongProperty( nMinDialogWidth, PropLong, 0 );
		pDclControl->AddLongProperty( nMinDialogHeight, PropLong, 0 );
		pDclControl->AddStringProperty( nCfgTabCaption, PropString, pOwnerForm->GetKeyName() );
		AddControlEvent( pDclControl, nFormEventInitialize );
		AddControlEvent( pDclControl, nFormEventShow );
		AddControlEvent( pDclControl, nCfgEventCancel );
		AddControlEvent( pDclControl, nCfgEventHelp );
		AddControlEvent( pDclControl, nCfgEventOK );
		break;
	case VdclFileDialog:
		AddDefaultFormName( pDclControl );
		pDclControl->AddStringProperty( nObjectBrowser, PropActiveXMethods );
		pDclControl->AddStringProperty( nGlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
		pDclControl->AddBooleanProperty( nResizable, PropBool, true );
		pDclControl->AddLongProperty( nWidth, PropLong, lWidth > 0? lWidth : 427 );
		pDclControl->AddLongProperty( nHeight, PropLong, lHeight > 0? lHeight : 296 );
		pDclControl->AddStringProperty( nTitleBarText, PropString, theWorkspace.LoadResourceString( IDS_OPEN ) );
		AddControlEvent( pDclControl, nFormEventInitialize );
		AddControlEvent( pDclControl, nFormEventCancelClose );
		AddControlEvent( pDclControl, nFormEventClose );
		AddControlEvent( pDclControl, nFormEventSize );
		AddControlEvent( pDclControl, nEventOnHelp );
		break;
	case VdclTabForm:
		AddControlHiddenProperty( pDclControl, nWidth, lWidth > 0? lWidth : 600, PropLong );
		AddControlHiddenProperty( pDclControl, nHeight, lHeight > 0? lHeight : 380, PropLong );
		break;
	default:
		TraceFmt( _T("* AddDefaultFormProperties() called with unknown form type (%d)\r\n"), pOwnerForm->GetType() );
		assert( false );
		return false;
		break;
	}
	return true;
}


bool AddDefaultProperties( CDclControlObject* pDclControl, long lWidth /*= -1*/, long lHeight /*= -1*/ )
{
	assert( pDclControl != NULL );
	ControlType type = pDclControl->GetType();
	assert( type != CtlInvalid );
	if( type == CtlInvalid )
		return false;
	if( type == CtlForm )
		return AddDefaultFormProperties( pDclControl, lWidth, lHeight );
	CDclFormObject* pOwnerForm = pDclControl->GetOwnerForm();
	assert( pOwnerForm != NULL );
	if( !pOwnerForm )
		return false;

	CString sList = theWorkspace.LoadResourceString( IDS_LIST );

	//Add default properties that apply to every control type
	pDclControl->AddStringProperty( nName, PropString, pDclControl->GetKeyName() );
	pDclControl->AddStringProperty( nObjectBrowser, PropActiveXMethods );
	pDclControl->AddStringProperty( nCustom, PropCustom ); // add the Custom property (to display the button)
	pDclControl->AddStringProperty( nGlobalVarName ); // now setting to empty string by default  2007-02-15 [ORW]
	pDclControl->AddLongProperty( nLeft, PropLong, 0 );
	pDclControl->AddLongProperty( nTop, PropLong, 0 );
	if( lWidth <= 0 )
		lWidth = 32;
	pDclControl->AddLongProperty( nWidth, PropLong, lWidth );
	if( lHeight <= 0 )
		lHeight = 32;
	pDclControl->AddLongProperty( nHeight, PropLong, lHeight );

	if( type != CtlFileDlgCtrl )
	{
		pDclControl->AddLongProperty( nBottomFromBottom, PropLong, 0 );
		pDclControl->AddLongProperty( nLeftFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( nRightFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( nTopFromBottom, PropLong, 0 );
		pDclControl->AddLongProperty( nUseBottomFromBottom, PropLong, (type == CtlTabStrip)? 1 : 0 );
		pDclControl->AddLongProperty( nUseLeftFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( nUseRightFromRight, PropLong, 0 );
		pDclControl->AddLongProperty( nUseTopFromBottom, PropLong, (type == CtlTabStrip)? 1 : 0 );
		pDclControl->AddBooleanProperty( nVisible, PropBool, true );
	}

	if (type != CtlActiveX && type != CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( nEnabled, PropBool, true );

	CDclFormObject* pTopLevelParentForm = pOwnerForm;
	while( pTopLevelParentForm->GetParentForm() )
		pTopLevelParentForm = pTopLevelParentForm->GetParentForm();
	DclFormType eFormType = pTopLevelParentForm->GetType();
	switch( eFormType )
	{
	case VdclDockable:
	case VdclModeless:
		pDclControl->AddLongProperty( nEventInvoke, PropEnum, 0 );
		break;
	}

	//Add control specific properties
	switch( type )
	{
	case Ctl3DRect:
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 2 );
		break;

	case CtlActiveX:
		pDclControl->AddStringProperty( nActiveXPropPages, PropActiveXPropPages );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		break;

	case CtlAnimate:
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 0 );
		break;

	case CtlBlockList:
		pDclControl->AddLongProperty( nAcadColor, PropLong, -6 );
		pDclControl->AddStringProperty( nBlockName, PropString );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, true );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddLongProperty( nIconXSpacing, PropLong, 20 );
		pDclControl->AddLongProperty( nIconYSpacing, PropLong, 32 );
		pDclControl->AddBooleanProperty( nLabelWrap, PropBool, true );
		pDclControl->AddLongProperty( nListViewIconAlign, PropEnum, 0 );
		pDclControl->AddBooleanProperty( nAutoArrange, PropBool, true );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( nMultiSelect, PropBool, false );
		pDclControl->AddLongProperty( nListViewSort, PropEnum, 2 );
		pDclControl->AddLongProperty( nBlockListStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventReturnPressed );
		AddControlEvent( pDclControl, nEventKeyDown );
		AddControlEvent( pDclControl, nEventMouseMove );
		break;

	case CtlBlockView:
		pDclControl->AddLongProperty( nAcadColor, PropLong, -22 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddLongProperty( nAllowOrbiting, PropEnum, 1 );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nRenderMode, PropEnum, 5 );
		pDclControl->AddBooleanProperty( nShowOrbitCirlces, PropBool, false );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlEvent( pDclControl, nEventRClick );
		AddControlEvent( pDclControl, nEventRDblClick );
		AddControlEvent( pDclControl, nEventMouseDown );
		AddControlEvent( pDclControl, nEventMouseUp );
		AddControlEvent( pDclControl, nEventMouseDblClick );
		break;

	case CtlCheckBox:
		pDclControl->AddLongProperty( nForeColor, PropLong, -19 );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -16 );
		pDclControl->AddStringProperty( nCaption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( nValue, PropBool, false );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlComboBox:
		pDclControl->AddLongProperty( nDropDownHeight, PropLong, 100 );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nItemData, PropIntArray, sList );
		pDclControl->AddLongProperty( nLimitText, PropLong, 256 );
		pDclControl->AddStringProperty( nList, PropStringArray, sList );
		pDclControl->AddBooleanProperty( nReturnAsTab, PropBool, false );
		pDclControl->AddBooleanProperty( nSorted, PropBool, false );
		pDclControl->AddLongProperty( nComboBoxStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( nText, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventEditChanged );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventSelChanged );
		//AddControlEvent( pDclControl, nEventUpdate );
		AddControlEvent( pDclControl, nEventDropDown );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlDwgList:
		pDclControl->AddLongProperty( nForeColor, PropLong, -19 );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -6 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( nDisableNoScroll, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, true );
		pDclControl->AddLongProperty( nInsertOrXref, PropEnum, 0 );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nSelectStyle, PropEnum, 0 );
		pDclControl->AddLongProperty( nRowHeight, PropLong, 120 );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( nVScrollBar, PropBool, true );
		AddControlEvent( pDclControl, nEventFolderChanged );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventSelChanged );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlDwgPreview:
		pDclControl->AddLongProperty( nAcadColor, PropLong, -22 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventMouseMove );
		break;

	case CtlFileDlgCtrl:
		pDclControl->AddBooleanProperty( nCreationPrompt, PropBool, true );
		pDclControl->AddBooleanProperty( nExtCanBeDiff, PropBool, false );
		pDclControl->AddBooleanProperty( nFileMustExist, PropBool, true );
		pDclControl->AddStringProperty( nFilter, PropString, theWorkspace.LoadResourceString( IDS_FILEDLGFILTERDEF ) );
		pDclControl->AddBooleanProperty( nMultiSelect, PropBool, false );
		pDclControl->AddBooleanProperty( nFileMustExist, PropBool, true );
		pDclControl->AddBooleanProperty( nOverWritePrompt, PropBool, true );
		pDclControl->AddBooleanProperty( nPathMustExist, PropBool, true );
		pDclControl->AddBooleanProperty( nShowCancel, PropBool, true );
		pDclControl->AddBooleanProperty( nShowHelp, PropBool, true );
		pDclControl->AddBooleanProperty( nShowNameLabel, PropBool, true );
		pDclControl->AddBooleanProperty( nShowNameTextBox, PropBool, true );
		pDclControl->AddBooleanProperty( nShowOK, PropBool, true );
		pDclControl->AddBooleanProperty( nShowReadOnlyCheckBox, PropBool, true );
		pDclControl->AddBooleanProperty( nShowTypeComboBox, PropBool, true );
		pDclControl->AddBooleanProperty( nShowTypeLabel, PropBool, true );
		pDclControl->AddLongProperty( nFileDlgStyle, PropEnum, 1 );
		AddControlEvent( pDclControl, nEventFolderChanged );
		AddControlEvent( pDclControl, nEventOnHelp );
		AddControlEvent( pDclControl, nEventOnTypeChange );
		AddControlEvent( pDclControl, nEventSelChanged );
		break;

	case CtlFrame:
		pDclControl->AddStringProperty( nCaption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlGraphicButton:
		pDclControl->AddLongProperty( nForeColor, PropLong, -19 );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -16 );
		pDclControl->AddStringProperty( nCaption, PropString );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nPressedPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddLongProperty( nButtonStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlGrid:
		pDclControl->AddBooleanProperty( nRowHeader, PropBool, true );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -6 );
		pDclControl->AddLongProperty( nAlternateColor, PropLong, -6 );
		pDclControl->AddLongProperty( nAlternateOrient, PropEnum, 0 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( nColHeader, PropBool, true );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		//pDclControl->AddBooleanProperty( nFullRowSelect, PropBool, false );
		pDclControl->AddBooleanProperty( nGridLines, PropBool, true );
		pDclControl->AddBooleanProperty( nLabelWrap, PropBool, true );
		pDclControl->AddStringProperty( nImageList, PropImageList, theWorkspace.LoadResourceString( IDS_IMAGELIST ) );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nListViewSort, PropEnum, 0 );
		pDclControl->AddLongProperty( nRowHeight, PropLong, 18 );
		AddControlHiddenProperty( pDclControl, nColumnCaptions, sList, PropStringArray );
		AddControlHiddenProperty( pDclControl, nColumnAlignments, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, nColumnImages, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, nColumnWidths, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, nColumnStyles, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, nColumnDefaultImages, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, nColumnAlternateImages, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, nColumnListItems, sList, PropStringArrayList );
		AddControlHiddenProperty( pDclControl, nColumnListImages, sList, PropIntArrayList );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventBtnClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventSelChanged );
		AddControlEvent( pDclControl, nEventBeginLabelEdit );
		AddControlEvent( pDclControl, nEventEndLabelEdit );
		AddControlEvent( pDclControl, nEventColumnClick );
		AddControlEvent( pDclControl, nEventMouseDown );
		AddControlEvent( pDclControl, nEventMouseUp );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlHatch:
		pDclControl->AddStringProperty( nHatchScale, PropDouble, _T("5") );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -22 );
		pDclControl->AddLongProperty( nForeColor, PropLong, 7 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlEvent( pDclControl, nEventRClick );
		AddControlEvent( pDclControl, nEventRDblClick );
		AddControlEvent( pDclControl, nEventMouseDown );
		AddControlEvent( pDclControl, nEventMouseUp );
		AddControlEvent( pDclControl, nEventMouseDblClick );
		break;

	case CtlHtmlCtrl:
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		AddControlEvent( pDclControl, nEventNavigateComplete );
		AddControlEvent( pDclControl, nEventMouseMove );
		break;

	case CtlImageComboBox:
		pDclControl->AddLongProperty( nDropDownHeight, PropLong, 100 );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddStringProperty( nImageList, PropImageList, theWorkspace.LoadResourceString(IDS_IMAGELIST) );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nLimitText, PropLong, 256 );
		pDclControl->AddBooleanProperty( nReturnAsTab, PropBool, false );
		pDclControl->AddBooleanProperty( nSorted, PropBool, false );
		pDclControl->AddLongProperty( nComboBoxStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( nText, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventEditChanged );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventSelChanged );
		//AddControlEvent( pDclControl, nEventUpdate );
		AddControlEvent( pDclControl, nEventDropDown );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlLabel:
		pDclControl->AddLongProperty( nForeColor, PropLong, -19 );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -16 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( nCaption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddLongProperty( nJustification, PropEnum, 0 );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlListBox:
		pDclControl->AddLongProperty( nForeColor, PropLong, -19 );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -6 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
		pDclControl->AddLongProperty( nColumnWidth, PropLong, 25 );
		pDclControl->AddBooleanProperty( nDisableNoScroll, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nItemData, PropIntArray, sList );
		pDclControl->AddStringProperty( nList, PropStringArray, sList );
		pDclControl->AddBooleanProperty( nMultiColumn, PropBool, false );
		pDclControl->AddBooleanProperty( nNoIntegralHeight, PropBool, true );
		pDclControl->AddLongProperty( nSelectStyle, PropEnum, 0 );
		pDclControl->AddBooleanProperty( nSorted, PropBool, false );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( nUseTabStops, PropBool, false );
		pDclControl->AddBooleanProperty( nVScrollBar, PropBool, false );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventSelChanged );
		AddControlEvent( pDclControl, nEventRClick );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlListView:
		pDclControl->AddLongProperty( nAcadColor, PropLong, -6 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( nColHeader, PropBool, true );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( nEditLabels, PropBool, true );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nFullRowSelect, PropBool, false );
		pDclControl->AddBooleanProperty( nGridLines, PropBool, false );
		pDclControl->AddLongProperty( nIconXSpacing, PropLong, 20 );
		pDclControl->AddLongProperty( nIconYSpacing, PropLong, 32 );
		pDclControl->AddBooleanProperty( nLabelWrap, PropBool, true );
		pDclControl->AddLongProperty( nListViewIconAlign, PropEnum, 0 );
		pDclControl->AddBooleanProperty( nAutoArrange, PropBool, false );
		pDclControl->AddStringProperty( nImageList, PropImageList, theWorkspace.LoadResourceString(IDS_IMAGELIST) );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( nMultiSelect, PropBool, false );
		pDclControl->AddBooleanProperty( nShowSelectAlways, PropBool, false );
		pDclControl->AddLongProperty( nListViewSort, PropEnum, 0 );
		pDclControl->AddLongProperty( nListViewStyle, PropEnum, 3 );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventRClick );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventRDblClick );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventReturnPressed );
		AddControlEvent( pDclControl, nEventKeyDown );
		AddControlEvent( pDclControl, nEventBeginLabelEdit );
		AddControlEvent( pDclControl, nEventEndLabelEdit );
		AddControlEvent( pDclControl, nEventColumnClick );
		AddControlEvent( pDclControl, nEventKeyDown );
		AddControlEvent( pDclControl, nEventKeyUp );
		AddControlEvent( pDclControl, nEventMouseDown );
		AddControlEvent( pDclControl, nEventMouseUp );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlMonth:
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nMultiSelection, PropLong, 1 );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventSelChanged );
		AddControlEvent( pDclControl, nEventGetDayState );
		AddControlEvent( pDclControl, nEventSelect );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlOptionButton:
		pDclControl->AddLongProperty( nForeColor, PropLong, -19 );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -16 );
		pDclControl->AddBooleanProperty( nBeginGroup, PropBool, false );
		pDclControl->AddStringProperty( nCaption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( nValue, PropBool, false );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlOptionList:
		pDclControl->AddLongProperty( nForeColor, PropLong, -19 );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -16 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 0 );
		{
			RefCountedPtr< CPropertyObject > pProp = pDclControl->AddStringProperty( nBtnCaption, PropStringArray, sList );
			if( pProp->GetStringArrayPtr()->empty() )
			{ // add three default option values
				pProp->GetStringArrayPtr()->push_back( theWorkspace.LoadResourceString( IDS_OPTION1 ) );
				pProp->GetStringArrayPtr()->push_back( theWorkspace.LoadResourceString( IDS_OPTION2 ) );
				pProp->GetStringArrayPtr()->push_back( theWorkspace.LoadResourceString( IDS_OPTION3 ) );
			}
		}
		pDclControl->AddStringProperty( nBtnTTText, PropStringArray, sList );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddLongProperty( nDefSelIndex, PropLong, 0 );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( nNoIntegralHeight, PropBool, true );
		pDclControl->AddLongProperty( nRowHeight, PropLong, 17 );
		pDclControl->AddBooleanProperty( nVScrollBar, PropBool, true );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventSelChanged );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlPictureBox:
		pDclControl->AddBooleanProperty( nAutoSize, PropBool, false );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -6 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventKeyDown);
		AddControlEvent( pDclControl, nEventKeyUp );
		AddControlEvent( pDclControl, nEventMouseDown );
		AddControlEvent( pDclControl, nEventMouseUp );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlEvent( pDclControl, nEventMouseMovedOff );
		AddControlEvent( pDclControl, nEventMouseDblClick );
		AddControlEvent( pDclControl, nEventMouseWheel );
		AddControlEvent( pDclControl, nEventPaint );
		AddControlEvent( pDclControl, nEventMouseEntered );
		AddControlEvent( pDclControl, nEventRClick );
		AddControlEvent( pDclControl, nEventRDblClick );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlProgress:
		pDclControl->AddStringProperty( nSecondText, PropString, _T("second remaining") );
		pDclControl->AddStringProperty( nSecondsText, PropString, _T("seconds remaining") );
		pDclControl->AddStringProperty( nMinuteText, PropString, _T("minute") );
		pDclControl->AddStringProperty( nMinutesText, PropString, _T("minutes") );
		pDclControl->AddBooleanProperty( nDisplaySeconds, PropBool, false );
		pDclControl->AddBooleanProperty( nDisplayPercentage, PropBool, true );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 2 );
		pDclControl->AddLongProperty( nMaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( nMinValue, PropLong, 1 );
		pDclControl->AddLongProperty( nOrientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddBooleanProperty( nSmoothProgress, PropBool, false );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddLongProperty( nValue, PropLong, 0 );
		break;

	case CtlRoundSlider:
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddLongProperty( nValue, PropLong, 0 );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlEvent( pDclControl, nEventScroll );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlScrollBar:
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nLargeChange, PropLong, 5 );
		pDclControl->AddLongProperty( nMaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( nMinValue, PropLong, 1 );
		pDclControl->AddLongProperty( nOrientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddLongProperty( nSmallChange, PropLong, 1 );
		pDclControl->AddLongProperty( nValue, PropLong, 1 );
		AddControlEvent( pDclControl, nEventScroll );
		AddControlEvent( pDclControl, nEventScrolled );
		break;

	case CtlSlider:
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nLargeChange, PropLong, 5 );
		pDclControl->AddLongProperty( nMaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( nMinValue, PropLong, 1 );
		pDclControl->AddLongProperty( nOrientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddBooleanProperty( nShowTicks, PropBool, true );
		pDclControl->AddLongProperty( nSmallChange, PropLong, 1 );
		pDclControl->AddLongProperty( nTickFrequency, PropLong, 10 );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddLongProperty( nValue, PropLong, 1 );
		AddControlEvent( pDclControl, nEventScroll );
		AddControlEvent( pDclControl, nEventReleasedCapture );
		AddControlEvent( pDclControl, nEventMouseMove );
		break;

	case CtlSlideView:
		pDclControl->AddLongProperty( nAcadColor, PropLong, -22 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 2 );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventKeyDown);
		AddControlEvent( pDclControl, nEventKeyUp );
		AddControlEvent( pDclControl, nEventMouseDown );
		AddControlEvent( pDclControl, nEventMouseUp );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlEvent( pDclControl, nEventMouseMovedOff );
		AddControlEvent( pDclControl, nEventMouseDblClick );
		AddControlEvent( pDclControl, nEventMouseWheel );
		AddControlEvent( pDclControl, nEventPaint );
		AddControlEvent( pDclControl, nEventMouseEntered );
		AddControlEvent( pDclControl, nEventRClick );
		AddControlEvent( pDclControl, nEventRDblClick );
		break;

	case CtlSpinButton:
		pDclControl->AddBooleanProperty( nAutoWrap, PropBool, false );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nMaxValue, PropLong, 100 );
		pDclControl->AddLongProperty( nMinValue, PropLong, 1 );
		pDclControl->AddLongProperty( nOrientation, PropEnum, (lWidth >= lHeight)? 0 : 1 );
		pDclControl->AddLongProperty( nValue, PropLong, 1 );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlEvent( pDclControl, nEventChanged );
		break;

	case CtlSplitter:
		pDclControl->AddLongProperty( nSplitterStyle, PropEnum, 0 );
		pDclControl->AddLongProperty( nSplitterMin, PropLong, 30 );
		pDclControl->AddLongProperty( nSplitterMax, PropLong, 30 );
		break;

	case CtlStaticURL:
		pDclControl->AddLongProperty( nForeColor, PropLong, 5 );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -16 );
		pDclControl->AddStringProperty( nCaption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nURLLinkType, PropEnum, 0 );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddStringProperty( nURLAddress, PropString );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlStdButton:
		pDclControl->AddStringProperty( nCaption, PropString, pDclControl->GetKeyName() );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlTabStrip:
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddLongProperty( nTabLabelAlign, PropEnum, 1 );
		pDclControl->AddLongProperty( nMinTabWidth, PropLong, -1 );
		pDclControl->AddBooleanProperty( nTabFixedWidth, PropBool, false );
		pDclControl->AddLongProperty( nTabStyle, PropEnum, 0 );
		pDclControl->AddBooleanProperty( nMultiRow, PropBool, false );
		//pDclControl->AddLongProperty( nTabJustified, PropEnum, 0 );
		AddControlHiddenProperty( pDclControl, nTabsCaption, sList, PropStringArray );
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_TAB1));
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_TAB2));
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_TAB3));
		AddControlHiddenProperty( pDclControl, nTabsImageList, sList, PropIntArray );
		AddControlHiddenProperty( pDclControl, nTabsTTT, sList, PropStringArray );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlHiddenProperty( pDclControl, nImageList, theWorkspace.LoadResourceString( IDS_IMAGELIST ), PropImageList );
		AddControlEvent( pDclControl, nEventChanged );
		AddControlEvent( pDclControl, nEventSelChanging );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlTextBox:
		pDclControl->AddBooleanProperty( nAutoHScroll, PropBool, true );
		pDclControl->AddBooleanProperty( nAutoVScroll, PropBool, true );
		pDclControl->AddLongProperty( nForeColor, PropLong, -19 );
		pDclControl->AddLongProperty( nAcadColor, PropLong, -6 );
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddLongProperty( nFilterStyle, PropEnum, 0 );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nHScrollBar, PropBool, false );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddLongProperty( nJustification, PropEnum, 0 );
		pDclControl->AddLongProperty( nLimitText, PropLong, 32760 );
		pDclControl->AddLongProperty( nMarginLeft, PropLong, 0 );
		pDclControl->AddLongProperty( nMarginRight, PropLong, 0 );
		pDclControl->AddBooleanProperty( nReadOnly, PropBool, false );
		pDclControl->AddBooleanProperty( nReturnAsTab, PropBool, false );
		pDclControl->AddStringProperty( nText, PropString, pDclControl->GetKeyName() );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		pDclControl->AddBooleanProperty( nVScrollBar, PropBool, false );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nEventEditChanged );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventUpdate );
		AddControlEvent( pDclControl, nEventMaxText );
		AddControlEvent( pDclControl, nEventReturnPressed );
		AddControlEvent( pDclControl, nEventKeyDown );
		AddControlEvent( pDclControl, nEventKeyUp );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	case CtlTree:
		pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
		pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
		pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
		pDclControl->AddBooleanProperty( nEditLabels, PropBool, false );
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		pDclControl->AddBooleanProperty( nHasButtons, PropBool, true );
		pDclControl->AddBooleanProperty( nHasLines, PropBool, true );
		pDclControl->AddStringProperty( nImageList, PropImageList, theWorkspace.LoadResourceString( IDS_IMAGELIST ) );
		pDclControl->AddLongProperty( nIndent, PropLong, 17 );
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		pDclControl->AddBooleanProperty( nLinesAtRoot, PropBool, true );
		pDclControl->AddBooleanProperty( nShowSelectAlways, PropBool, false );
		pDclControl->AddBooleanProperty( nSingleExpanded, PropBool, false );
		pDclControl->AddStringProperty( nToolTipTitle, PropString );
		pDclControl->AddStringProperty( nToolTipBody, PropString );
		pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString( IDS_NONE ) );
		pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
		pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
		pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
		pDclControl->AddBooleanProperty( nToolTipBalloon, PropBool, true );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nEventClicked );
		AddControlEvent( pDclControl, nDragnDropToAutoCAD );
		AddControlEvent( pDclControl, nDragnDropFromControl );
		AddControlEvent( pDclControl, nDragnDropFromAutoCAD );
		AddControlEvent( pDclControl, nDragnDropBegin );
		AddControlEvent( pDclControl, nEventDblClicked );
		AddControlEvent( pDclControl, nEventKillFocus );
		AddControlEvent( pDclControl, nEventSetFocus );
		AddControlEvent( pDclControl, nEventSelChanged );
		AddControlEvent( pDclControl, nEventMouseMove );
		AddControlEvent( pDclControl, nEventRClick );
		AddControlEvent( pDclControl, nEventRDblClick );
		AddControlEvent( pDclControl, nEventBeginLabelEdit );
		AddControlEvent( pDclControl, nEventDeleteItem );
		AddControlEvent( pDclControl, nEventEndLabelEdit );
		AddControlEvent( pDclControl, nEventItemExpanded );
		AddControlEvent( pDclControl, nEventItemExpanding );
		AddControlHiddenProperty( pDclControl, nLabelBold, (pDclControl->GetOwnerProject()->m_bDefaultFontBold != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelItalic, (pDclControl->GetOwnerProject()->m_bDefaultFontItalic != FALSE), PropBool );
		AddControlHiddenProperty( pDclControl, nLabelSize, pDclControl->GetOwnerProject()->m_nDefaultFontSize, PropLong );
		AddControlHiddenProperty( pDclControl, nLabelStrikeOut, false, PropBool );
		AddControlHiddenProperty( pDclControl, nLabelUnderline, (pDclControl->GetOwnerProject()->m_bDefaultFontUnderLine != FALSE), PropBool );
		break;

	default:
		TraceFmt( _T("* AddDefaultProperties() called with unknown control type (%d)\r\n"), type );
		assert( false );
		return false;
		break;
	}
	return true;
};
