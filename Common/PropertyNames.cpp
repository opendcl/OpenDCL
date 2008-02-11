#include "stdAfx.h"
#include "PropertyNames.h"
#include "Workspace.h"
#include "PropertyIds.h"
#include <map>
#include <string>
#include <locale>

#if defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif //_UNICODE



//For now, property resources are ordered sequentially, so just add a base offset.
//It would be more flexible and less error prone to create a constant array that maps
//property id to resource id. This may need to be done if resource id collisions occur. [ORW]
static UINT GetPropertyNameResourceId( Prop::Id eProp )
{
	static const int PROPRESIDBASE = 3200;
	return eProp + PROPRESIDBASE;
}


//wrap the map in a class in order to manage delayed initialization
//(this avoids the time consuming resource loading when names are not needed)
class PropNameMap
{
	typedef std::map< tstring, Prop::Id > T_MapNameToId;
	typedef std::map< Prop::Id, tstring > T_MapIdToName;
	T_MapNameToId mmapApiNameToId;
	T_MapIdToName mmapIdToName;
	T_MapIdToName mmapIdToApiName;
	bool mbInitialized;
public:
	PropNameMap() : mbInitialized( false ) {}
	PropNameMap( bool bInitializeNow ) : mbInitialized( false ) { if( bInitializeNow ) Initialize(); }
	~PropNameMap() {}

public:
	LPCTSTR GetName( Prop::Id eProp );
	LPCTSTR GetApiName( Prop::Id eProp );
	Prop::Id GetId( LPCTSTR pszName );
	void GetApiSet( T_PropertyIdSet& IdSet );

protected:
	void Initialize();

	static tstring& toLString( tstring& sInput )
  {
		for( tstring::iterator iter = sInput.begin(); iter != sInput.end(); ++iter )
			*iter = std::tolower( *iter, std::locale() );
		return sInput;
  }
};


//delayed construction of the class
static PropNameMap& theMap()
{
	static PropNameMap _theMap;
	return _theMap;
}


//Exported interface
LPCTSTR GetPropertyName( Prop::Id eProp )
{
	return theMap().GetName( eProp );
}

LPCTSTR GetPropertyApiName( Prop::Id eProp )
{
	return theMap().GetApiName( eProp );
}

Prop::Id GetPropertyId( LPCTSTR pszName )
{
	return theMap().GetId( pszName );
}

void GetApiPropertyIdSet( T_PropertyIdSet& IdSet )
{
	return theMap().GetApiSet( IdSet );
}

//Internal
LPCTSTR PropNameMap::GetName( Prop::Id eProp )
{
	Initialize();
	T_MapIdToName::const_iterator iter = mmapIdToName.find( eProp );
	if( iter != mmapIdToName.end() )
		return iter->second.c_str();
	return GetApiName( eProp );
}

LPCTSTR PropNameMap::GetApiName( Prop::Id eProp )
{
	Initialize();
	T_MapIdToName::const_iterator iter = mmapIdToApiName.find( eProp );
	if( iter != mmapIdToApiName.end() )
		return iter->second.c_str();
	return NULL;
}

Prop::Id PropNameMap::GetId( LPCTSTR pszName )
{
	Initialize();
	T_MapNameToId::const_iterator iter = mmapApiNameToId.find( toLString( tstring( pszName ) ) );
	if( iter != mmapApiNameToId.end() )
		return iter->second;
	return Prop::_Private;
}

void PropNameMap::GetApiSet( T_PropertyIdSet& IdSet )
{
	Initialize();
	IdSet.clear();
	for( T_MapIdToName::const_iterator iter = mmapIdToApiName.begin(); iter != mmapIdToApiName.end(); ++iter )
		IdSet.insert( iter->first );
}

void PropNameMap::Initialize()
{
	if( mbInitialized )
		return; //no-op
	//iterate over all defined property ids to load and save the associated name from resources
	for( int idxProp = Prop::_MinId; idxProp <= Prop::_MaxId; ++idxProp )
	{
		Prop::Id id = static_cast<Prop::Id>(idxProp);
		tstring sName( theWorkspace.LoadResourceString( GetPropertyNameResourceId( id ) ) );
		if( !sName.empty() )
			mmapIdToName[id] = sName;
	}

	// hardcode API names for all properties that expose lisp callable get/set functions
	mmapIdToApiName[Prop::AutoSize] = _T("AutoSize");
	mmapIdToApiName[Prop::BorderStyle] = _T("BorderStyle");
	mmapIdToApiName[Prop::ButtonStyle] = _T("Style");
	mmapIdToApiName[Prop::Caption] = _T("Caption");
	mmapIdToApiName[Prop::ComboBoxStyle] = _T("Style");
	mmapIdToApiName[Prop::Enabled] = _T("Enabled");
	mmapIdToApiName[Prop::EventBtnClicked] = _T("ButtonClicked");
	mmapIdToApiName[Prop::EventClicked] = _T("Clicked");
	mmapIdToApiName[Prop::EventDblClicked] = _T("DblClicked");
	mmapIdToApiName[Prop::EventEditChanged] = _T("EditChanged");
	mmapIdToApiName[Prop::EventNodeClicked] = _T("NodeClicked");
	mmapIdToApiName[Prop::FilterStyle] = _T("FilterStyle");
	mmapIdToApiName[Prop::Height] = _T("Height");
	mmapIdToApiName[Prop::ImageList] = _T("ImageList");
	mmapIdToApiName[Prop::LargeChange] = _T("LargeChange");
	mmapIdToApiName[Prop::Left] = _T("Left");
	mmapIdToApiName[Prop::List] = _T("List");
	mmapIdToApiName[Prop::MaxValue] = _T("MaxValue");
	mmapIdToApiName[Prop::MinValue] = _T("MinValue");
	mmapIdToApiName[Prop::MultiRow] = _T("MultiRow");
	mmapIdToApiName[Prop::Orientation] = _T("Orientation");
	mmapIdToApiName[Prop::Picture] = _T("Picture");
	mmapIdToApiName[Prop::RectStyle] = _T("Style");
	mmapIdToApiName[Prop::SmallChange] = _T("SmallChange");
	mmapIdToApiName[Prop::TabsCaption] = _T("TabsCaptionList");
	mmapIdToApiName[Prop::TabsImageList] = _T("TabsImageList");
	mmapIdToApiName[Prop::TabsTTT] = _T("TabsToolTipTextList");
	mmapIdToApiName[Prop::Text] = _T("Text");
	mmapIdToApiName[Prop::TickFrequency] = _T("TickFrequency");
	mmapIdToApiName[Prop::TitleBarText] = _T("TitleBarText");
	mmapIdToApiName[Prop::ToolTipTitle] = _T("ToolTipTitle");
	mmapIdToApiName[Prop::Top] = _T("Top");
	mmapIdToApiName[Prop::Value] = _T("Value");
	mmapIdToApiName[Prop::Visible] = _T("Visible");
	mmapIdToApiName[Prop::Width] = _T("Width");
	mmapIdToApiName[Prop::ShowTicks] = _T("ShowTicks");
	mmapIdToApiName[Prop::ReadOnly] = _T("ReadOnly");
	mmapIdToApiName[Prop::Justification] = _T("Justification");
	mmapIdToApiName[Prop::MarginLeft] = _T("MarginLeft");
	mmapIdToApiName[Prop::MarginRight] = _T("MarginRight");
	mmapIdToApiName[Prop::LimitText] = _T("LimitText");
	mmapIdToApiName[Prop::AutoHScroll] = _T("AutoHScroll");
	mmapIdToApiName[Prop::AutoVScroll] = _T("AutoVScroll");
	mmapIdToApiName[Prop::MultiColumn] = _T("MultiColumn");
	mmapIdToApiName[Prop::SelectionStyle] = _T("SelectionStyle");
	mmapIdToApiName[Prop::NoIntegralHeight] = _T("NoIntegralHeight");
	mmapIdToApiName[Prop::Sorted] = _T("Sorted");
	mmapIdToApiName[Prop::UseTabStops] = _T("UseTabStops");
	mmapIdToApiName[Prop::DisableNoScroll] = _T("DisableNoScroll");
	mmapIdToApiName[Prop::HScrollBar] = _T("HScrollBar");
	mmapIdToApiName[Prop::VScrollBar] = _T("VScrollBar");
	mmapIdToApiName[Prop::ColumnWidth] = _T("ColumnWidth");
	mmapIdToApiName[Prop::SmoothProgress] = _T("SmoothProgress");
	mmapIdToApiName[Prop::AutoWrap] = _T("AutoWrap");
	mmapIdToApiName[Prop::URLAddress] = _T("URLAddress");
	mmapIdToApiName[Prop::TabJustification] = _T("TabJustification");
	mmapIdToApiName[Prop::TabStyle] = _T("TabStyle");
	mmapIdToApiName[Prop::LabelAlignment] = _T("LabelAlignment");
	mmapIdToApiName[Prop::TabFixedWidth] = _T("TabFixedWidth");
	mmapIdToApiName[Prop::URLLinkType] = _T("LinkType");
	mmapIdToApiName[Prop::MinTabWidth] = _T("MinTabWidth");
	mmapIdToApiName[Prop::TabSelected] = _T("TabSelected");
	mmapIdToApiName[Prop::ItemData] = _T("ItemData");
	mmapIdToApiName[Prop::BtnCaption] = _T("ButtonList");
	mmapIdToApiName[Prop::ToolBarBtnImages] = _T("ImageList");
	mmapIdToApiName[Prop::BtnTTText] = _T("ButtonToolTipTitleList");
	mmapIdToApiName[Prop::ToolBarBtnStyle] = _T("StyleList");
	mmapIdToApiName[Prop::UseTopFromBottom] = _T("UseTopFromBottom");
	mmapIdToApiName[Prop::UseBottomFromBottom] = _T("UseBottomFromBottom");
	mmapIdToApiName[Prop::UseLeftFromRight] = _T("UseLeftFromRight");
	mmapIdToApiName[Prop::UseRightFromRight] = _T("UseRightFromRight");
	mmapIdToApiName[Prop::TopFromBottom] = _T("TopFromBottom");
	mmapIdToApiName[Prop::BottomFromBottom] = _T("BottomFromBottom");
	mmapIdToApiName[Prop::LeftFromRight] = _T("LeftFromRight");
	mmapIdToApiName[Prop::RightFromRight] = _T("RightFromRight");
	mmapIdToApiName[Prop::Resizable] = _T("AllowResizing");
	mmapIdToApiName[Prop::OnEvent] = _T("OnEvent");
	mmapIdToApiName[Prop::MultiSelection] = _T("MaxSelectCount");
	mmapIdToApiName[Prop::EventGetDayState] = _T("GetDayState");
	mmapIdToApiName[Prop::EventSelect] = _T("Select");
	mmapIdToApiName[Prop::EventKillFocus] = _T("KillFocus");
	mmapIdToApiName[Prop::EventOutOfMemory] = _T("OutOfMemory");
	mmapIdToApiName[Prop::EventRClick] = _T("RightClick");
	mmapIdToApiName[Prop::EventRDblClick] = _T("RightDblClick");
	mmapIdToApiName[Prop::EventReturn] = _T("Return");
	mmapIdToApiName[Prop::EventSetFocus] = _T("SetFocus");
	mmapIdToApiName[Prop::EventBeginLabelEdit] = _T("BeginLabelEdit");
	mmapIdToApiName[Prop::EventDeleteItem] = _T("DeleteItem");
	mmapIdToApiName[Prop::EventEndLabelEdit] = _T("EndLabelEdit");
	mmapIdToApiName[Prop::EventItemExpanded] = _T("ItemExpanded");
	mmapIdToApiName[Prop::EventItemExpanding] = _T("ItemExpanding");
	mmapIdToApiName[Prop::EventKeyDown] = _T("KeyDown");
	mmapIdToApiName[Prop::EventSelChanging] = _T("SelChanging");
	mmapIdToApiName[Prop::EventChanged] = _T("Changed");
	mmapIdToApiName[Prop::EventMaxText] = _T("MaxText");
	mmapIdToApiName[Prop::EventUpdate] = _T("Update");
	mmapIdToApiName[Prop::EventDropDown] = _T("DropDown");
	mmapIdToApiName[Prop::EventScroll] = _T("Scroll");
	mmapIdToApiName[Prop::EventReleasedCapture] = _T("ReleasedCapture");
	mmapIdToApiName[Prop::FontUnderline] = _T("FontUnderline");
	mmapIdToApiName[Prop::FontItalic] = _T("FontItalic");
	mmapIdToApiName[Prop::FontBold] = _T("FontBold");
	mmapIdToApiName[Prop::FontSize] = _T("FontSize");
	mmapIdToApiName[Prop::FontName] = _T("Font");
	mmapIdToApiName[Prop::FontStrikeout] = _T("FontStrikeout");
	mmapIdToApiName[Prop::FontColor] = _T("FontColor");
	mmapIdToApiName[Prop::BlockName] = _T("BlockName");
	mmapIdToApiName[Prop::FileName] = _T("FileName");
	mmapIdToApiName[Prop::Color] = _T("Color");
	mmapIdToApiName[Prop::CfgTabCaption] = _T("CfgTabText");
	mmapIdToApiName[Prop::AllowOrbiting] = _T("InterfaceMode");
	mmapIdToApiName[Prop::FormEventClose] = _T("Close");
	mmapIdToApiName[Prop::FormEventInitialize] = _T("Initialize");
	mmapIdToApiName[Prop::FormEventSize] = _T("Size");
	mmapIdToApiName[Prop::FormEventShow] = _T("Show");
	mmapIdToApiName[Prop::DocEventActivated] = _T("DocActivated");
	mmapIdToApiName[Prop::CfgEventCancel] = _T("Cancel");
	mmapIdToApiName[Prop::CfgEventOK] = _T("OK");
	mmapIdToApiName[Prop::CfgEventHelp] = _T("Help");
	mmapIdToApiName[Prop::CfgEventApply] = _T("Apply");
	mmapIdToApiName[Prop::BackgroundColor] = _T("BackColor");
	mmapIdToApiName[Prop::ShowSelectAlways] = _T("ShowSelectAlways");
	mmapIdToApiName[Prop::Indent] = _T("Indent");
	mmapIdToApiName[Prop::HasLines] = _T("HasLines");
	mmapIdToApiName[Prop::LinesAtRoot] = _T("LinesAtRoot");
	mmapIdToApiName[Prop::HasButtons] = _T("HasButtons");
	mmapIdToApiName[Prop::EditLabels] = _T("EditLabel");
	mmapIdToApiName[Prop::CheckBoxes] = _T("HasCheckBoxes");
	mmapIdToApiName[Prop::Icon] = _T("TitleBarIcon");
	mmapIdToApiName[Prop::EventKeyUp] = _T("KeyUp");
	mmapIdToApiName[Prop::EventMouseDown] = _T("MouseDown");
	mmapIdToApiName[Prop::EventMouseUp] = _T("MouseUp");
	mmapIdToApiName[Prop::EventMouseMove] = _T("MouseMove");
	mmapIdToApiName[Prop::EventMouseMovedOff] = _T("MouseMovedOff");
	mmapIdToApiName[Prop::EventMouseDblClick] = _T("MouseDblClick");
	mmapIdToApiName[Prop::EventMouseWheel] = _T("MouseWheel");
	mmapIdToApiName[Prop::EventPaint] = _T("Paint");
	mmapIdToApiName[Prop::UseMask] = _T("UseMask");
	mmapIdToApiName[Prop::ShowEditBox] = _T("ShowEditBox");
	mmapIdToApiName[Prop::EventNavigateComplete] = _T("NavigationComplete");
	mmapIdToApiName[Prop::InputFilter] = _T("InputFilter");
	mmapIdToApiName[Prop::TabOrder] = _T("TabOrder");
	mmapIdToApiName[Prop::EventMouseEntered] = _T("MouseEntered");
	mmapIdToApiName[Prop::BeginGroup] = _T("BeginGroup");
	mmapIdToApiName[Prop::MouseOverPicture] = _T("MouseOverPicture");
	mmapIdToApiName[Prop::RenderMode] = _T("RenderMode");
	mmapIdToApiName[Prop::EventReturnPressed] = _T("ReturnPressed");
	mmapIdToApiName[Prop::FormEventCancelClose] = _T("CancelClose");
	mmapIdToApiName[Prop::OnLMouseEvent] = _T("LMouse");
	mmapIdToApiName[Prop::OnMMouseEvent] = _T("MMouse");
	mmapIdToApiName[Prop::OnRMouseEvent] = _T("RMouse");
	mmapIdToApiName[Prop::ZOrder] = _T("ZOrder");
	mmapIdToApiName[Prop::ListViewStyle] = _T("Style");
	mmapIdToApiName[Prop::ListViewSort] = _T("Sorting");
	mmapIdToApiName[Prop::ListViewIconAlign] = _T("IconAlignment");
	mmapIdToApiName[Prop::BlockListStyle] = _T("Style");
	mmapIdToApiName[Prop::AutoArrange] = _T("IconAutoArrange");
	mmapIdToApiName[Prop::ColHeader] = _T("ColumnHeader");
	mmapIdToApiName[Prop::LabelWrap] = _T("LabelWrap");
	mmapIdToApiName[Prop::AllowScrolling] = _T("AllowScrolling");
	mmapIdToApiName[Prop::MultiSelect] = _T("MultipleSelection");
	mmapIdToApiName[Prop::IconXSpacing] = _T("IconXSpacing");
	mmapIdToApiName[Prop::IconYSpacing] = _T("IconYSpacing");
	mmapIdToApiName[Prop::DragnDropToAutoCAD] = _T("DragnDropToAutoCAD");
	mmapIdToApiName[Prop::DragnDropFromControl] = _T("DragnDropFromControl");
	mmapIdToApiName[Prop::DragnDropFromAutoCAD] = _T("DragnDropFromOther");
	mmapIdToApiName[Prop::DragnDropBegin] = _T("DragnDropBegin");
	mmapIdToApiName[Prop::DragnDropAllowBegin] = _T("DragnDropAllowBegin");
	mmapIdToApiName[Prop::DragnDropAllowDrop] = _T("DragnDropAllowDrop");
	mmapIdToApiName[Prop::GridLines] = _T("GridLines");
	mmapIdToApiName[Prop::FullRowSelect] = _T("FullRowSelect");
	mmapIdToApiName[Prop::MinDialogWidth] = _T("MinDialogWidth");
	mmapIdToApiName[Prop::MaxDialogWidth] = _T("MaxDialogWidth");
	mmapIdToApiName[Prop::MinDialogHeight] = _T("MinDialogHeight");
	mmapIdToApiName[Prop::MaxDialogHeight] = _T("MaxDialogHeight");
	mmapIdToApiName[Prop::EventColumnClick] = _T("ColumnClick");
	mmapIdToApiName[Prop::FontSizeStyle] = _T("FontSizeStyle");
	mmapIdToApiName[Prop::ForegroundColor] = _T("ForeColor");
	mmapIdToApiName[Prop::Transparent] = _T("Transparent");
	mmapIdToApiName[Prop::EventScrolled] = _T("Scrolled");
	mmapIdToApiName[Prop::DockableSides] = _T("DockableSides");
	mmapIdToApiName[Prop::DefaultDockedSide] = _T("DockingDefault");
	mmapIdToApiName[Prop::EventInvoke] = _T("EventInvoke");
	mmapIdToApiName[Prop::ReturnAsTab] = _T("ReturnAsTab");
	mmapIdToApiName[Prop::SingleExpanded] = _T("SingleExpanded");
	mmapIdToApiName[Prop::RowHeight] = _T("RowHeight");
	mmapIdToApiName[Prop::DefSelIndex] = _T("DefaultSelection");
	mmapIdToApiName[Prop::FileDlgStyle] = _T("Style");
	mmapIdToApiName[Prop::AsReadOnly] = _T("AsReadOnly");
	mmapIdToApiName[Prop::CreationPrompt] = _T("CreationPrompt");
	mmapIdToApiName[Prop::DefaultExtension] = _T("DefaultExtension");
	mmapIdToApiName[Prop::ExtCanBeDiff] = _T("ExtCanBeDiff");
	mmapIdToApiName[Prop::FileMustExist] = _T("FileMustExist");
	mmapIdToApiName[Prop::Filter] = _T("Filter");
	mmapIdToApiName[Prop::OverWritePrompt] = _T("OverWritePrompt");
	mmapIdToApiName[Prop::PathMustExist] = _T("PathMustExist");
	mmapIdToApiName[Prop::ShowCancel] = _T("ShowCancel");
	mmapIdToApiName[Prop::ShowHelp] = _T("ShowHelp");
	mmapIdToApiName[Prop::ShowNameLabel] = _T("ShowNameLabel");
	mmapIdToApiName[Prop::ShowNameTextBox] = _T("ShowNameTextBox");
	mmapIdToApiName[Prop::ShowOK] = _T("ShowOK");
	mmapIdToApiName[Prop::ShowReadOnlyCheckBox] = _T("ShowReadOnlyCheckBox");
	mmapIdToApiName[Prop::ShowTypeComboBox] = _T("ShowTypeComboBox");
	mmapIdToApiName[Prop::ShowTypeLabel] = _T("ShowTypeLabel");
	mmapIdToApiName[Prop::EventFolderChanged] = _T("FolderChange");
	mmapIdToApiName[Prop::EventOnHelp] = _T("Help");
	mmapIdToApiName[Prop::EventOnTypeChange] = _T("TypeChange");
	mmapIdToApiName[Prop::InsertOrXref] = _T("InsertOrXref");
	mmapIdToApiName[Prop::RowHeader] = _T("RowHeader");
	mmapIdToApiName[Prop::ColumnCaptions] = _T("ColumnCaptionList");
	mmapIdToApiName[Prop::ColumnAlignments] = _T("ColumnAlignmentList");
	mmapIdToApiName[Prop::ColumnImages] = _T("ColumnImageList");
	mmapIdToApiName[Prop::ColumnWidths] = _T("ColumnWidthList");
	mmapIdToApiName[Prop::ColumnStyles] = _T("ColumnStyleList");
	mmapIdToApiName[Prop::ColumnDefaultImages] = _T("ColumnDefaultImageList");
	mmapIdToApiName[Prop::ColumnAlternateImages] = _T("ColumnAlternateImageList");
	mmapIdToApiName[Prop::ColumnListItems] = _T("ColumnListItems");
	mmapIdToApiName[Prop::ColumnListImages] = _T("ColumnListImages");
	mmapIdToApiName[Prop::AlternateColor] = _T("AlternateColor");
	mmapIdToApiName[Prop::AlternateOrient] = _T("AltOrientation");
	mmapIdToApiName[Prop::ToolTipBody] = _T("ToolTipMainText");
	mmapIdToApiName[Prop::ToolTipPicture] = _T("ToolTipPicture");
	mmapIdToApiName[Prop::ToolTipAviFileName] = _T("ToolTipAviFileName");
	mmapIdToApiName[Prop::ToolTipLine] = _T("ToolTipLine");
	mmapIdToApiName[Prop::ToolTipTitleColor] = _T("ToolTipTitleColor");
	mmapIdToApiName[Prop::SplitterStyle] = _T("EdgeStyle");
	mmapIdToApiName[Prop::SplitterMin] = _T("ClosestInside");
	mmapIdToApiName[Prop::SplitterMax] = _T("ClosestOutside");
	mmapIdToApiName[Prop::HatchScale] = _T("HatchScale");
	mmapIdToApiName[Prop::EventSplitterMoved] = _T("SplitterMoved");
	mmapIdToApiName[Prop::SecondText] = _T("CaptionSecond");
	mmapIdToApiName[Prop::SecondsText] = _T("CaptionSeconds");
	mmapIdToApiName[Prop::MinuteText] = _T("CaptionMinute");
	mmapIdToApiName[Prop::MinutesText] = _T("CaptionMinutes");
	mmapIdToApiName[Prop::DisplaySeconds] = _T("DisplaySeconds");
	mmapIdToApiName[Prop::DisplayPercentage] = _T("DisplayPercentage");
	mmapIdToApiName[Prop::FormEventOnOk] = _T("OK");
	mmapIdToApiName[Prop::TitleBar] = _T("TitleBar");
	mmapIdToApiName[Prop::FormEventOnCancel] = _T("Cancel");
	mmapIdToApiName[Prop::ToolTipBalloon] = _T("ToolTipBalloon");
	mmapIdToApiName[Prop::DocEventEnteringNoDocState] = _T("EnteringNoDocState");
	mmapIdToApiName[Prop::UseVisualStyle] = _T("UseVisualStyle");
	mmapIdToApiName[Prop::RemainingText] = _T("CaptionRemaining");
	mmapIdToApiName[Prop::ProgressLegend] = _T("LegendType");
	mmapIdToApiName[Prop::KeepFocus] = _T("KeepFocus");

	// fill the reverse lookup map
	for( T_MapIdToName::const_iterator iter = mmapIdToApiName.begin();
			 iter != mmapIdToApiName.end();
			 ++iter )
	{
		tstring sApiName = iter->second;
		mmapApiNameToId[toLString( sApiName )] = iter->first;
	}

	mbInitialized = true;
}
