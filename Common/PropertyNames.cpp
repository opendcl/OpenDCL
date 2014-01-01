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

	// hardcode API names for events and all properties that expose lisp callable get/set functions
	mmapIdToApiName[Prop::AllowResizing] = _T("AllowResizing");
	mmapIdToApiName[Prop::AllowScrolling] = _T("AllowScrolling");
	mmapIdToApiName[Prop::AlternatingColor] = _T("AlternatingColor");
	mmapIdToApiName[Prop::AlternateOrient] = _T("AltOrientation");
	mmapIdToApiName[Prop::AsReadOnly] = _T("AsReadOnly");
	mmapIdToApiName[Prop::AutoArrange] = _T("IconAutoArrange");
	mmapIdToApiName[Prop::AutoHScroll] = _T("AutoHScroll");
	mmapIdToApiName[Prop::AutoSize] = _T("AutoSize");
	mmapIdToApiName[Prop::AutoVScroll] = _T("AutoVScroll");
	mmapIdToApiName[Prop::AutoWrap] = _T("AutoWrap");
	mmapIdToApiName[Prop::BackgroundColor] = _T("BackColor");
	mmapIdToApiName[Prop::BeginGroup] = _T("BeginGroup");
	mmapIdToApiName[Prop::BlockListStyle] = _T("BlockListStyle");
	mmapIdToApiName[Prop::BlockName] = _T("BlockName");
	mmapIdToApiName[Prop::BorderStyle] = _T("BorderStyle");
	mmapIdToApiName[Prop::BottomFromBottom] = _T("BottomFromBottom");
	mmapIdToApiName[Prop::BtnCaption] = _T("ButtonCaptionList");
	mmapIdToApiName[Prop::BtnToolTips] = _T("ButtonToolTipList");
	mmapIdToApiName[Prop::GraphicButtonStyle] = _T("GraphicButtonStyle");
	mmapIdToApiName[Prop::Caption] = _T("Caption");
	mmapIdToApiName[Prop::CaptionMinute] = _T("CaptionMinute");
	mmapIdToApiName[Prop::CaptionMinutes] = _T("CaptionMinutes");
	mmapIdToApiName[Prop::CaptionRemaining] = _T("CaptionRemaining");
	mmapIdToApiName[Prop::CaptionSecond] = _T("CaptionSecond");
	mmapIdToApiName[Prop::CaptionSeconds] = _T("CaptionSeconds");
	mmapIdToApiName[Prop::CheckBoxes] = _T("HasCheckBoxes");
	mmapIdToApiName[Prop::ColHeader] = _T("ColumnHeader");
	mmapIdToApiName[Prop::Color] = _T("Color");
	mmapIdToApiName[Prop::ColumnAlignments] = _T("ColumnAlignmentList");
	mmapIdToApiName[Prop::ColumnAlternateImages] = _T("ColumnAlternateImageList");
	mmapIdToApiName[Prop::ColumnCaptions] = _T("ColumnCaptionList");
	mmapIdToApiName[Prop::ColumnDefaultImages] = _T("ColumnDefaultImageList");
	mmapIdToApiName[Prop::ColumnImages] = _T("ColumnImageList");
	mmapIdToApiName[Prop::ColumnListImages] = _T("ColumnListImages");
	mmapIdToApiName[Prop::ColumnListItems] = _T("ColumnListItems");
	mmapIdToApiName[Prop::ColumnStyles] = _T("ColumnStyleList");
	mmapIdToApiName[Prop::ColumnWidth] = _T("ColumnWidth");
	mmapIdToApiName[Prop::ColumnWidths] = _T("ColumnWidthList");
	mmapIdToApiName[Prop::ComboBoxStyle] = _T("ComboBoxStyle");
	mmapIdToApiName[Prop::ControlBrowser] = _T("(ControlBrowser)");
	mmapIdToApiName[Prop::CreationPrompt] = _T("CreationPrompt");
	mmapIdToApiName[Prop::CurSelIndex] = _T("CurrentSelection");
	mmapIdToApiName[Prop::Custom] = _T("(Custom)");
	mmapIdToApiName[Prop::DefaultDockedSide] = _T("DockingDefault");
	mmapIdToApiName[Prop::DefaultExtension] = _T("DefaultExtension");
	mmapIdToApiName[Prop::DisableNoScroll] = _T("DisableNoScroll");
	mmapIdToApiName[Prop::DisplayPercentage] = _T("DisplayPercentage");
	mmapIdToApiName[Prop::DisplaySeconds] = _T("DisplaySeconds");
	mmapIdToApiName[Prop::DocEventActivated] = _T("DocActivated");
	mmapIdToApiName[Prop::DocEventEnteringNoDocState] = _T("EnteringNoDocState");
	mmapIdToApiName[Prop::DockableSides] = _T("DockableSides");
	mmapIdToApiName[Prop::DragnDropAllowBegin] = _T("DragnDropAllowBegin");
	mmapIdToApiName[Prop::DragnDropAllowDrop] = _T("DragnDropAllowDrop");
	mmapIdToApiName[Prop::DragnDropBegin] = _T("DragnDropBegin");
	mmapIdToApiName[Prop::DragnDropFromControl] = _T("DragnDropFromControl");
	mmapIdToApiName[Prop::DragnDropFromOther] = _T("DragnDropFromOther");
	mmapIdToApiName[Prop::DragnDropToAutoCAD] = _T("DragnDropToAutoCAD");
	mmapIdToApiName[Prop::DragOverFromControl] = _T("DragOverFromControl");
	mmapIdToApiName[Prop::DropDownHeight] = _T("DropDownHeight");
	mmapIdToApiName[Prop::EditLabels] = _T("EditLabels");
	mmapIdToApiName[Prop::Enabled] = _T("Enabled");
	mmapIdToApiName[Prop::EventBeginLabelEdit] = _T("BeginLabelEdit");
	mmapIdToApiName[Prop::EventBtnClicked] = _T("ButtonClicked");
	mmapIdToApiName[Prop::EventChanged] = _T("Changed");
	mmapIdToApiName[Prop::EventClicked] = _T("Clicked");
	mmapIdToApiName[Prop::EventColumnClick] = _T("ColumnClick");
	mmapIdToApiName[Prop::EventDblClicked] = _T("DblClicked");
	mmapIdToApiName[Prop::EventDeleteItem] = _T("DeleteItem");
	mmapIdToApiName[Prop::EventDocumentComplete] = _T("DocumentComplete");
	mmapIdToApiName[Prop::EventDropDown] = _T("DropDown");
	mmapIdToApiName[Prop::EventEditChanged] = _T("EditChanged");
	mmapIdToApiName[Prop::EventEndLabelEdit] = _T("EndLabelEdit");
	mmapIdToApiName[Prop::EventFolderChanged] = _T("FolderChanged");
	mmapIdToApiName[Prop::EventGetDayState] = _T("GetDayState");
	mmapIdToApiName[Prop::EventHelp] = _T("Help");
	mmapIdToApiName[Prop::EventInvoke] = _T("EventInvoke");
	mmapIdToApiName[Prop::EventItemExpanded] = _T("ItemExpanded");
	mmapIdToApiName[Prop::EventItemExpanding] = _T("ItemExpanding");
	mmapIdToApiName[Prop::EventKeyDown] = _T("KeyDown");
	mmapIdToApiName[Prop::EventKeyUp] = _T("KeyUp");
	mmapIdToApiName[Prop::EventKillFocus] = _T("KillFocus");
	mmapIdToApiName[Prop::EventLMouse] = _T("LMouse");
	mmapIdToApiName[Prop::EventMaxText] = _T("MaxText");
	mmapIdToApiName[Prop::EventMMouse] = _T("MMouse");
	mmapIdToApiName[Prop::EventMouseDblClick] = _T("MouseDblClick");
	mmapIdToApiName[Prop::EventMouseDown] = _T("MouseDown");
	mmapIdToApiName[Prop::EventMouseEntered] = _T("MouseEntered");
	mmapIdToApiName[Prop::EventMouseMove] = _T("MouseMove");
	mmapIdToApiName[Prop::EventMouseMovedOff] = _T("MouseMovedOff");
	mmapIdToApiName[Prop::EventMouseUp] = _T("MouseUp");
	mmapIdToApiName[Prop::EventMouseWheel] = _T("MouseWheel");
	mmapIdToApiName[Prop::EventNavigateComplete] = _T("NavigationComplete");
	mmapIdToApiName[Prop::EventNodeClicked] = _T("NodeClicked");
	mmapIdToApiName[Prop::EventOptionsApply] = _T("OptionsApply");
	mmapIdToApiName[Prop::EventOptionsCancel] = _T("OptionsCancel");
	mmapIdToApiName[Prop::EventOptionsHelp] = _T("OptionsHelp");
	mmapIdToApiName[Prop::EventOptionsOK] = _T("OptionsOk");
	mmapIdToApiName[Prop::EventOutOfMemory] = _T("OutOfMemory");
	mmapIdToApiName[Prop::EventPaint] = _T("Paint");
	mmapIdToApiName[Prop::EventRightClick] = _T("RightClick");
	mmapIdToApiName[Prop::EventRightDblClick] = _T("RightDblClick");
	mmapIdToApiName[Prop::EventReleasedCapture] = _T("ReleasedCapture");
	mmapIdToApiName[Prop::EventReturn] = _T("Return");
	mmapIdToApiName[Prop::EventReturnPressed] = _T("ReturnPressed");
	mmapIdToApiName[Prop::EventRMouse] = _T("RMouse");
	mmapIdToApiName[Prop::EventScroll] = _T("Scroll");
	mmapIdToApiName[Prop::EventScrolled] = _T("Scrolled");
	mmapIdToApiName[Prop::EventSelChanged] = _T("SelChanged");
	mmapIdToApiName[Prop::EventSelChanging] = _T("SelChanging");
	mmapIdToApiName[Prop::EventSelect] = _T("Select");
	mmapIdToApiName[Prop::EventSetFocus] = _T("SetFocus");
	mmapIdToApiName[Prop::EventSplitterMoved] = _T("SplitterMoved");
	mmapIdToApiName[Prop::EventTypeChanged] = _T("TypeChanged");
	mmapIdToApiName[Prop::EventUpdate] = _T("Update");
	mmapIdToApiName[Prop::ExtCanBeDiff] = _T("ExtCanBeDiff");
	mmapIdToApiName[Prop::FileDlgStyle] = _T("FileExplorerStyle");
	mmapIdToApiName[Prop::FileMustExist] = _T("FileMustExist");
	mmapIdToApiName[Prop::FileName] = _T("FileName");
	mmapIdToApiName[Prop::Filter] = _T("Filter");
	mmapIdToApiName[Prop::FilterStyle] = _T("FilterStyle");
	mmapIdToApiName[Prop::FontBold] = _T("FontBold");
	mmapIdToApiName[Prop::FontColor] = _T("FontColor");
	mmapIdToApiName[Prop::FontItalic] = _T("FontItalic");
	mmapIdToApiName[Prop::FontName] = _T("Font");
	mmapIdToApiName[Prop::FontSize] = _T("FontSize");
	mmapIdToApiName[Prop::FontSizeStyle] = _T("FontSizeStyle");
	mmapIdToApiName[Prop::FontStrikeout] = _T("FontStrikeout");
	mmapIdToApiName[Prop::FontUnderline] = _T("FontUnderline");
	mmapIdToApiName[Prop::ForegroundColor] = _T("ForeColor");
	mmapIdToApiName[Prop::FormEventCancelClose] = _T("CancelClose");
	mmapIdToApiName[Prop::FormEventClose] = _T("Close");
	mmapIdToApiName[Prop::FormEventInitialize] = _T("Initialize");
	mmapIdToApiName[Prop::FormEventMove] = _T("Move");
	mmapIdToApiName[Prop::FormEventOnCancel] = _T("Cancel");
	mmapIdToApiName[Prop::FormEventOnOk] = _T("OK");
	mmapIdToApiName[Prop::FormEventShow] = _T("Show");
	mmapIdToApiName[Prop::FormEventSize] = _T("Size");
	mmapIdToApiName[Prop::FormEventTimer] = _T("Timer");
	mmapIdToApiName[Prop::FullRowSelect] = _T("FullRowSelect");
	mmapIdToApiName[Prop::VarName] = _T("VarName");
	mmapIdToApiName[Prop::GridLines] = _T("GridLines");
	mmapIdToApiName[Prop::HasButtons] = _T("HasButtons");
	mmapIdToApiName[Prop::HasLines] = _T("HasLines");
	mmapIdToApiName[Prop::HatchScale] = _T("HatchScale");
	mmapIdToApiName[Prop::Height] = _T("Height");
	mmapIdToApiName[Prop::HScrollBar] = _T("HScrollBar");
	mmapIdToApiName[Prop::Hyperlink] = _T("Hyperlink");
	mmapIdToApiName[Prop::TitleBarIcon] = _T("TitleBarIcon");
	mmapIdToApiName[Prop::IconXSpacing] = _T("IconXSpacing");
	mmapIdToApiName[Prop::IconYSpacing] = _T("IconYSpacing");
	mmapIdToApiName[Prop::ImageList] = _T("ImageList");
	mmapIdToApiName[Prop::Indent] = _T("Indent");
	mmapIdToApiName[Prop::InputFilter] = _T("InputFilter");
	mmapIdToApiName[Prop::InsertOrXref] = _T("InsertOrXref");
	mmapIdToApiName[Prop::InterfaceMode] = _T("InterfaceMode");
	mmapIdToApiName[Prop::IsTabStop] = _T("IsTabStop");
	mmapIdToApiName[Prop::ItemData] = _T("ItemData");
	mmapIdToApiName[Prop::Justification] = _T("Justification");
	mmapIdToApiName[Prop::KeepFocus] = _T("KeepFocus");
	mmapIdToApiName[Prop::LabelAlignment] = _T("LabelAlignment");
	mmapIdToApiName[Prop::LabelWrap] = _T("LabelWrap");
	mmapIdToApiName[Prop::LargeChange] = _T("LargeChange");
	mmapIdToApiName[Prop::Left] = _T("Left");
	mmapIdToApiName[Prop::LeftFromRight] = _T("LeftFromRight");
	mmapIdToApiName[Prop::LinesAtRoot] = _T("LinesAtRoot");
	mmapIdToApiName[Prop::List] = _T("List");
	mmapIdToApiName[Prop::ListImages] = _T("ListImages");
	mmapIdToApiName[Prop::ListViewIconAlign] = _T("IconAlignment");
	mmapIdToApiName[Prop::ListViewSort] = _T("Sorting");
	mmapIdToApiName[Prop::ListViewStyle] = _T("ListViewStyle");
	mmapIdToApiName[Prop::MarginLeft] = _T("MarginLeft");
	mmapIdToApiName[Prop::MarginRight] = _T("MarginRight");
	mmapIdToApiName[Prop::MaxDialogHeight] = _T("MaxHeight");
	mmapIdToApiName[Prop::MaxDialogWidth] = _T("MaxWidth");
	mmapIdToApiName[Prop::MaxValue] = _T("MaxValue");
	mmapIdToApiName[Prop::MinDialogHeight] = _T("MinHeight");
	mmapIdToApiName[Prop::MinDialogWidth] = _T("MinWidth");
	mmapIdToApiName[Prop::MinTabWidth] = _T("MinTabWidth");
	mmapIdToApiName[Prop::MinValue] = _T("MinValue");
	mmapIdToApiName[Prop::MouseOverPicture] = _T("MouseOverPicture");
	mmapIdToApiName[Prop::MultiColumn] = _T("MultiColumn");
	mmapIdToApiName[Prop::MultiRow] = _T("MultiRow");
	mmapIdToApiName[Prop::MultiSelect] = _T("MultipleSelection");
	mmapIdToApiName[Prop::MultiSelection] = _T("MaxSelectCount");
	mmapIdToApiName[Prop::Name] = _T("(Name)");
	mmapIdToApiName[Prop::NoIntegralHeight] = _T("NoIntegralHeight");
	mmapIdToApiName[Prop::OptionsTabCaption] = _T("OptionsTabCaption");
	mmapIdToApiName[Prop::Orientation] = _T("Orientation");
	mmapIdToApiName[Prop::OverWritePrompt] = _T("OverWritePrompt");
	mmapIdToApiName[Prop::PathMustExist] = _T("PathMustExist");
	mmapIdToApiName[Prop::Picture] = _T("Picture");
	mmapIdToApiName[Prop::ProgressLegend] = _T("LegendType");
	mmapIdToApiName[Prop::ReadOnly] = _T("ReadOnly");
	mmapIdToApiName[Prop::RectangleStyle] = _T("RectangleStyle");
	mmapIdToApiName[Prop::RenderMode] = _T("RenderMode");
	mmapIdToApiName[Prop::ReturnAsTab] = _T("ReturnAsTab");
	mmapIdToApiName[Prop::RightFromRight] = _T("RightFromRight");
	mmapIdToApiName[Prop::RowHeader] = _T("RowHeader");
	mmapIdToApiName[Prop::RowHeight] = _T("RowHeight");
	mmapIdToApiName[Prop::SelectionStyle] = _T("SelectionStyle");
	mmapIdToApiName[Prop::ShowCancel] = _T("ShowCancel");
	mmapIdToApiName[Prop::ShowEditBox] = _T("ShowEditBox");
	mmapIdToApiName[Prop::ShowHelp] = _T("ShowHelp");
	mmapIdToApiName[Prop::ShowNameLabel] = _T("ShowNameLabel");
	mmapIdToApiName[Prop::ShowNameTextBox] = _T("ShowNameTextBox");
	mmapIdToApiName[Prop::ShowOK] = _T("ShowOK");
	mmapIdToApiName[Prop::ShowOrbitCircles] = _T("ShowOrbitCircles");
	mmapIdToApiName[Prop::ShowReadOnlyCheckBox] = _T("ShowReadOnlyCheckBox");
	mmapIdToApiName[Prop::ShowSelectAlways] = _T("ShowSelectAlways");
	mmapIdToApiName[Prop::ShowTicks] = _T("ShowTicks");
	mmapIdToApiName[Prop::ShowTypeComboBox] = _T("ShowTypeComboBox");
	mmapIdToApiName[Prop::ShowTypeLabel] = _T("ShowTypeLabel");
	mmapIdToApiName[Prop::SingleClickExpand] = _T("SingleClickExpand");
	mmapIdToApiName[Prop::SmallChange] = _T("SmallChange");
	mmapIdToApiName[Prop::SmoothProgress] = _T("SmoothProgress");
	mmapIdToApiName[Prop::Sorted] = _T("Sorted");
	mmapIdToApiName[Prop::SplitterMax] = _T("ClosestOutside");
	mmapIdToApiName[Prop::SplitterMin] = _T("ClosestInside");
	mmapIdToApiName[Prop::SplitterStyle] = _T("EdgeStyle");
	mmapIdToApiName[Prop::TabFixedWidth] = _T("TabFixedWidth");
	mmapIdToApiName[Prop::TabJustification] = _T("TabJustification");
	mmapIdToApiName[Prop::TabOrder] = _T("TabOrder");
	mmapIdToApiName[Prop::TabsCaption] = _T("TabCaptionList");
	mmapIdToApiName[Prop::TabSelected] = _T("TabSelected");
	mmapIdToApiName[Prop::TabsImageList] = _T("TabImageList");
	mmapIdToApiName[Prop::TabsTTT] = _T("TabToolTipList");
	mmapIdToApiName[Prop::TabStyle] = _T("TabStyle");
	mmapIdToApiName[Prop::Text] = _T("Text");
	mmapIdToApiName[Prop::TextLimit] = _T("TextLimit");
	mmapIdToApiName[Prop::TickFrequency] = _T("TickFrequency");
	mmapIdToApiName[Prop::TitleBar] = _T("TitleBar");
	mmapIdToApiName[Prop::TitleBarText] = _T("TitleBarText");
	mmapIdToApiName[Prop::ToolBarBtnImages] = _T("ButtonsImageList");
	mmapIdToApiName[Prop::ToolBarBtnStyle] = _T("ButtonStyleList");
	mmapIdToApiName[Prop::ToolTipAviFileName] = _T("ToolTipAviFileName");
	mmapIdToApiName[Prop::ToolTipBalloon] = _T("ToolTipBalloon");
	mmapIdToApiName[Prop::ToolTipBody] = _T("ToolTipMainText");
	mmapIdToApiName[Prop::ToolTipLine] = _T("ToolTipLine");
	mmapIdToApiName[Prop::ToolTipPicture] = _T("ToolTipPicture");
	mmapIdToApiName[Prop::ToolTipTitle] = _T("ToolTipTitle");
	mmapIdToApiName[Prop::ToolTipTitleColor] = _T("ToolTipTitleColor");
	mmapIdToApiName[Prop::Top] = _T("Top");
	mmapIdToApiName[Prop::TopFromBottom] = _T("TopFromBottom");
	mmapIdToApiName[Prop::Transparent] = _T("Transparent");
	mmapIdToApiName[Prop::URLLinkType] = _T("LinkType");
	mmapIdToApiName[Prop::UseBottomFromBottom] = _T("UseBottomFromBottom");
	mmapIdToApiName[Prop::UseLeftFromRight] = _T("UseLeftFromRight");
	mmapIdToApiName[Prop::UseMask] = _T("UseMask");
	mmapIdToApiName[Prop::UseRightFromRight] = _T("UseRightFromRight");
	mmapIdToApiName[Prop::UseTabStops] = _T("UseTabStops");
	mmapIdToApiName[Prop::UseTopFromBottom] = _T("UseTopFromBottom");
	mmapIdToApiName[Prop::UseVisualStyle] = _T("UseVisualStyle");
	mmapIdToApiName[Prop::Value] = _T("Value");
	mmapIdToApiName[Prop::Visible] = _T("Visible");
	mmapIdToApiName[Prop::VScrollBar] = _T("VScrollBar");
	mmapIdToApiName[Prop::Width] = _T("Width");
	mmapIdToApiName[Prop::ZOrder] = _T("ZOrder");

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
