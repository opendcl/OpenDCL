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
		Map[CtlStdButton] = IDI_STDBUTTON;
		Map[CtlGraphicButton] = IDI_GRAPHICBUTTON;
		Map[CtlFrame] = IDI_FRAME;
		Map[CtlTextBox] = IDI_TEXTBOX;
		Map[CtlCheckBox] = IDI_CHECKBOX;
		Map[CtlOptionButton] = IDI_OPTION;
		Map[CtlComboBox] = IDI_COMBOBOX;
		Map[CtlListBox] = IDI_LISTBOX;
		Map[CtlScrollBar] = IDI_SCROLLBAR;
		Map[CtlSlider] = IDI_SLIDERBAR;
		Map[CtlPictureBox] = IDI_PICTUREBOX;
		Map[CtlTabStrip] = IDI_TABS;
		Map[CtlMonth] = IDI_MONTH;
		Map[CtlTree] = IDI_TREE;
		Map[CtlRectangle] = IDI_3DRECT;
		Map[CtlProgress] = IDI_PROGRESSBAR;
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
		Map[CtlAnimate] = IDI_ANIMATE;
		Map[CtlImageComboBox] = IDI_IMAGECOMBOBOX;
		Map[CtlGrid] = IDI_GRID;
		Map[CtlSplitter] = IDI_SPLITTER;
		Map[CtlHatch] = IDI_HATCH;
		Map[CtlFileDlgCtrl] = IDI_OPENFOLDER;
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
		Map[CtlLabel] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlLabel )) );
		Map[CtlStdButton] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlStdButton )) );
		Map[CtlGraphicButton] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlGraphicButton )) );
		Map[CtlFrame] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlFrame )) );
		Map[CtlTextBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlTextBox )) );
		Map[CtlCheckBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlCheckBox )) );
		Map[CtlOptionButton] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlOptionButton )) );
		Map[CtlComboBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlComboBox )) );
		Map[CtlListBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlListBox )) );
		Map[CtlScrollBar] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlScrollBar )) );
		Map[CtlSlider] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlSlider )) );
		Map[CtlPictureBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlPictureBox )) );
		Map[CtlTabStrip] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlTabStrip )) );
		Map[CtlMonth] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlMonth )) );
		Map[CtlTree] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlTree )) );
		Map[CtlRectangle] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlRectangle )) );
		Map[CtlProgress] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlProgress )) );
		Map[CtlSpinButton] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlSpinButton )) );
		Map[CtlUrlLink] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlUrlLink )) );
		Map[CtlAngleSlider] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlAngleSlider )) );
		Map[CtlBlockView] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlBlockView )) );
		Map[CtlSlideView] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlSlideView )) );
		Map[CtlHtmlCtrl] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlHtmlCtrl )) );
		Map[CtlDwgPreview] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlDwgPreview )) );
		Map[CtlListView] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlListView )) );
		Map[CtlBlockList] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlBlockList )) );
		Map[CtlOptionList] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlOptionList )) );
		Map[CtlActiveX] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlActiveX )) );
		Map[CtlDwgList] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlDwgList )) );
		Map[CtlAnimate] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlAnimate )) );
		Map[CtlImageComboBox] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlImageComboBox )) );
		Map[CtlGrid] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlGrid )) );
		Map[CtlSplitter] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlSplitter )) );
		Map[CtlHatch] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlHatch )) );
		Map[CtlFileDlgCtrl] = LoadIcon( hmodRes, MAKEINTRESOURCE(GetControlTypeIconResId( CtlFileDlgCtrl )) );
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
