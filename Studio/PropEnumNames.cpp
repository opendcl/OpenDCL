#include "StdAfx.h"
#include "PropEnumNames.h"
#include "Workspace.h"
#include "Resource.h"
#include <map>


static void FillEnumNamesList( TEnumNames& Names, UINT nResIdStart, size_t ctNames )
{
	while( ctNames-- > 0 )
	{
		CString sName = theWorkspace.LoadResourceString( nResIdStart++ );
		assert( !sName.IsEmpty() );
		if( !sName.IsEmpty() )
			Names.push_back( sName );
	}
}

static std::map< Prop::Id, TEnumNames >& GetEnumNamesMap()
{
	static std::map< Prop::Id, TEnumNames >* pMap = NULL;
	if( !pMap )
	{
		static std::map< Prop::Id, TEnumNames > Map;
		FillEnumNamesList( Map[Prop::InterfaceMode], IDS_BV_UM_0, 4 );
		FillEnumNamesList( Map[Prop::AlternateOrient], IDS_ORIENTATION_0, 2 );
		FillEnumNamesList( Map[Prop::BlockListStyle], IDS_BL_STYLE_0, 2 );
		FillEnumNamesList( Map[Prop::BorderStyle], IDS_BORDERSTYLE_0, 3 );
		FillEnumNamesList( Map[Prop::GraphicButtonStyle], IDS_BUTTONSTYLE_0, 7 );
		FillEnumNamesList( Map[Prop::ComboBoxStyle], IDS_COMBOBOXSTYLE_0, 15 );
		FillEnumNamesList( Map[Prop::DefaultDockedSide], IDS_DOCK_DEFAULT_0, 4 );
		FillEnumNamesList( Map[Prop::DockableSides], IDS_DOCK_SIDES_0, 6 );
		FillEnumNamesList( Map[Prop::EventInvoke], IDS_EVENTINVOKE_0, 2 );
		FillEnumNamesList( Map[Prop::FileDlgStyle], IDS_FILEDLGSTYLE_0, 2 );
		FillEnumNamesList( Map[Prop::FilterStyle], IDS_FILTERSTYLE_0, 9 );
		FillEnumNamesList( Map[Prop::InsertOrXref], IDS_DRAGSTYLE_0, 2 );
		FillEnumNamesList( Map[Prop::Justification], IDS_JUSTIFICATION_0, 3 );
		FillEnumNamesList( Map[Prop::ListViewStyle], IDS_LV_STYLE_0, 4 );
		FillEnumNamesList( Map[Prop::ListViewSort], IDS_LV_SORT_0, 3 );
		FillEnumNamesList( Map[Prop::ListViewIconAlign], IDS_LV_IA_0, 2 );
		FillEnumNamesList( Map[Prop::Orientation], IDS_ORIENTATION_0, 2 );
		FillEnumNamesList( Map[Prop::ProgressLegend], IDS_PROGRESSLEGEND_0, 3 );
		FillEnumNamesList( Map[Prop::RectangleStyle], IDS_RECTSTYLE_0, 5 );
		FillEnumNamesList( Map[Prop::RenderMode], IDS_BV_RM_0, 8 );
		FillEnumNamesList( Map[Prop::SelectionStyle], IDS_SELECTSTYLE_0, 3 );
		FillEnumNamesList( Map[Prop::SplitterStyle], IDS_SPLITTERSTYLE_0, 6 );
		FillEnumNamesList( Map[Prop::TabJustification], IDS_TAB_JUSTIFY_0, 2 );
		FillEnumNamesList( Map[Prop::LabelAlignment], IDS_JUSTIFICATION_0, 2 );
		FillEnumNamesList( Map[Prop::TabStyle], IDS_TABSTYLE_0, 2 );
		FillEnumNamesList( Map[Prop::URLLinkType], IDS_URL_TYPE_0, 2 );
		FillEnumNamesList( Map[Prop::Value], IDS_CHECKSTATE_0, 3 );
		pMap = &Map;
	}
	return *pMap;
}


const TEnumNames& GetPropEnumNames( Prop::Id id, ControlType type /*= _CtlInvalid*/ )
{
	static const TEnumNames Null;
	std::map< Prop::Id, TEnumNames >& Map = GetEnumNamesMap();
	std::map< Prop::Id, TEnumNames >::const_iterator iter = Map.find( id );
	if( iter == Map.end() )
		return Null;
	//special handling
	if( id == Prop::ComboBoxStyle && type != CtlComboBox )
	{
		static TEnumNames ShortComboStylesList = iter->second;
		ShortComboStylesList.resize( 3 ); //anything except CtlComboBox only uses the first three styles
		return ShortComboStylesList;
	}
	if( id == Prop::Value && type == CtlOptionButton )
	{
		static TEnumNames ShortCheckStateList = iter->second;
		ShortCheckStateList.resize( 2 ); //option button doesn't support indeterminate state
		return ShortCheckStateList;
	}
	return iter->second;
}
