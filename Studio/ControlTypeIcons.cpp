#include "StdAfx.h"
#include "ControlTypeIcons.h"
#include "StudioWorkspace.h"
#include "Resource.h"
#include <map>


static std::map< ControlType, UINT >& GetIconResIdMap()
{
	static std::map< ControlType, UINT >* pMap = NULL;
	if( !pMap )
	{
		static std::map< ControlType, UINT > Map;
		Map[CtlLabel] = IDI_LABEL;
		Map[CtlTextButton] = IDI_STDBUTTON;
		Map[CtlGraphicButton] = IDI_GRAPHICBUTTON;
		Map[CtlFrame] = IDI_FRAME;
		Map[CtlTextBox] = IDI_TEXTBOX;
		Map[CtlCheckBox] = IDI_CHECKBOX;
		Map[CtlOptionButton] = IDI_OPTION;
		Map[CtlComboBox] = IDI_COMBOBOX;
		Map[CtlListBox] = IDI_LISTBOX;
		Map[CtlScrollBar] = IDI_SCROLLBAR;
		Map[CtlStraightSlider] = IDI_SLIDERBAR;
		Map[CtlPictureBox] = IDI_PICTUREBOX;
		Map[CtlTabStrip] = IDI_TABS;
		Map[CtlCalendar] = IDI_MONTH;
		Map[CtlImageTree] = IDI_TREE;
		Map[CtlRectangle] = IDI_3DRECT;
		Map[CtlProgressBar] = IDI_PROGRESSBAR;
		Map[CtlSpinButton] = IDI_SPINBUTTON;
		Map[CtlUrlLink] = IDI_URL;
		Map[CtlAngleSlider] = IDI_ANGLESLIDER;
		Map[CtlBlockView] = IDI_BLOCKVIEW;
		Map[CtlSlideView] = IDI_SLIDEVW;
		Map[CtlHtmlCtrl] = IDI_HTML;
		Map[CtlDwgPreview] = IDI_DWGPREVIEW;
		Map[CtlListView] = IDI_LISTVIEW;
		Map[CtlBlockList] = IDI_BLOCKLIST;
		Map[CtlOptionList] = IDI_OPTIONLIST;
		Map[CtlActiveX] = IDI_ACTIVEX;
		Map[CtlDwgList] = IDI_DWGLIST;
		Map[CtlAnimation] = IDI_ANIMATE;
		Map[CtlImageComboBox] = IDI_IMAGECOMBOBOX;
		Map[CtlGrid] = IDI_GRID;
		Map[CtlSplitter] = IDI_SPLITTER;
		Map[CtlHatch] = IDI_HATCH;
		Map[CtlFileExplorer] = IDI_OPENFOLDER;
		pMap = &Map;
	}
	return *pMap;
}


static std::map< ControlType, HICON >& GetIconMap()
{
	static std::map< ControlType, HICON >* pMap = NULL;
	if( !pMap )
	{
		static std::map< ControlType, HICON > Map;
		HMODULE hmodRes = theStudioWorkspace.GetLocalResourceModule();
		Map[CtlActiveX] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlActiveX )) );
		Map[CtlAngleSlider] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlAngleSlider )) );
		Map[CtlAnimation] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlAnimation )) );
		Map[CtlBlockView] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlBlockView )) );
		Map[CtlBlockList] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlBlockList )) );
		Map[CtlCalendar] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlCalendar )) );
		Map[CtlCheckBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlCheckBox )) );
		Map[CtlComboBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlComboBox )) );
		Map[CtlDwgList] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlDwgList )) );
		Map[CtlDwgPreview] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlDwgPreview )) );
		Map[CtlFileExplorer] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlFileExplorer )) );
		Map[CtlFrame] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlFrame )) );
		Map[CtlGraphicButton] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlGraphicButton )) );
		Map[CtlGrid] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlGrid )) );
		Map[CtlHatch] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlHatch )) );
		Map[CtlHtmlCtrl] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlHtmlCtrl )) );
		Map[CtlImageComboBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlImageComboBox )) );
		Map[CtlLabel] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlLabel )) );
		Map[CtlListBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlListBox )) );
		Map[CtlListView] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlListView )) );
		Map[CtlOptionButton] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlOptionButton )) );
		Map[CtlOptionList] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlOptionList )) );
		Map[CtlPictureBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlPictureBox )) );
		Map[CtlProgressBar] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlProgressBar )) );
		Map[CtlRectangle] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlRectangle )) );
		Map[CtlScrollBar] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlScrollBar )) );
		Map[CtlSlideView] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlSlideView )) );
		Map[CtlSpinButton] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlSpinButton )) );
		Map[CtlSplitter] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlSplitter )) );
		Map[CtlStraightSlider] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlStraightSlider )) );
		Map[CtlTabStrip] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlTabStrip )) );
		Map[CtlTextBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlTextBox )) );
		Map[CtlTextButton] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlTextButton )) );
		Map[CtlImageTree] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlImageTree )) );
		Map[CtlUrlLink] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlUrlLink )) );
		pMap = &Map;
	}
	return *pMap;
}


UINT GetControlTypeIconResId( ControlType type )
{
	std::map< ControlType, UINT >& Map = GetIconResIdMap();
	std::map< ControlType, UINT >::const_iterator iter = Map.find( type );
	if( iter == Map.end() )
		return (UINT)-1;
	return iter->second;
}


HICON GetControlTypeIcon( ControlType type )
{
	std::map< ControlType, HICON >& Map = GetIconMap();
	std::map< ControlType, HICON >::const_iterator iter = Map.find( type );
	if( iter == Map.end() )
		return NULL;
	return iter->second;
}
