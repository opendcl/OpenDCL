#include "stdafx.h"
#include "OdclJson.h"

#include "ControlApiName.h"
#include "ControlTypes.h"
#include "DclControlTemplate.h"
#include "DclFormTemplate.h"
#include "FormTypes.h"
#include "Project.h"
#include "PropertyIds.h"
#include "PropertyNames.h"
#include "PropertyObject.h"
#include "UndoManager.h"

#include <nlohmann/json.hpp>

#include <cctype>
#include <fstream>
#include <iterator>
#include <set>
#include <string>

namespace {

const int kJsonFormat = 1;

CString Utf8ToCString( const std::string& s )
{
	if( s.empty() )
		return CString();
	const int n = ::MultiByteToWideChar( CP_UTF8, 0, s.c_str(), static_cast<int>( s.size() ), NULL, 0 );
	CString w;
	wchar_t* buf = w.GetBuffer( n );
	::MultiByteToWideChar( CP_UTF8, 0, s.c_str(), static_cast<int>( s.size() ), buf, n );
	w.ReleaseBuffer( n );
	return w;
}

std::string CStringToUtf8( const CString& w )
{
	if( w.IsEmpty() )
		return std::string();
	const int n = ::WideCharToMultiByte( CP_UTF8, 0, w, w.GetLength(), NULL, 0, NULL, NULL );
	std::string s( static_cast<size_t>( n ), 0 );
	::WideCharToMultiByte( CP_UTF8, 0, w, w.GetLength(), &s[0], n, NULL, NULL );
	return s;
}

std::string JsonStr( const nlohmann::json& o, const char* key, const char* fallback = "" )
{
	if( !o.contains( key ) || !o[key].is_string() )
		return fallback;
	return o[key].get<std::string>();
}

long JsonLong( const nlohmann::json& o, const char* key, long fallback )
{
	if( !o.contains( key ) || o[key].is_null() )
		return fallback;
	if( o[key].is_number_integer() )
		return static_cast<long>( o[key].get<long long>() );
	if( o[key].is_number_float() )
		return static_cast<long>( o[key].get<double>() );
	if( o[key].is_string() )
	{
		try
		{
			return static_cast<long>( std::stol( o[key].get<std::string>() ) );
		}
		catch( ... )
		{
			return fallback;
		}
	}
	return fallback;
}

CString PathExt( const CString& path )
{
	CString p = path;
	p.MakeLower();
	const int dot = p.ReverseFind( _T('.') );
	if( dot < 0 )
		return CString();
	return p.Mid( dot );
}

bool PathEndsWith( const CString& path, const TCHAR* suffix )
{
	CString p = path;
	CString s = suffix;
	p.MakeLower();
	s.MakeLower();
	if( p.GetLength() < s.GetLength() )
		return false;
	return p.Right( s.GetLength() ) == s;
}

FormType ParseFormType( const std::string& s )
{
	std::string t = s;
	for( size_t i = 0; i < t.size(); ++i )
		t[i] = static_cast<char>( ::tolower( static_cast<unsigned char>( t[i] ) ) );
	if( t.empty() || t == "modal" || t == "modaldlg" || t == "dialog" )
		return FrmModalDlg;
	if( t == "modeless" || t == "modelessdlg" )
		return FrmModelessDlg;
	if( t == "palette" || t == "palettedlg" )
		return FrmPaletteDlg;
	if( t == "controlbar" || t == "control_bar" )
		return FrmControlBar;
	if( t == "filedialog" || t == "filedlg" || t == "file_dialog" )
		return FrmFileDlg;
	if( t == "optionstab" || t == "options_tab" )
		return FrmOptionsTab;
	if( t == "tabpage" || t == "tab_page" )
		return FrmTabPage;
	return _FrmInvalid;
}

const char* FormTypeName( FormType t )
{
	switch( t )
	{
	case FrmModalDlg: return "modal";
	case FrmModelessDlg: return "modeless";
	case FrmPaletteDlg: return "palette";
	case FrmControlBar: return "controlBar";
	case FrmFileDlg: return "fileDialog";
	case FrmOptionsTab: return "optionsTab";
	case FrmTabPage: return "tabPage";
	default: return "modal";
	}
}

ControlType ParseControlType( const std::string& s )
{
	if( s == "AxControl" || s == "ActiveX" ) return CtlActiveX;
	const CString want = Utf8ToCString( s );
	for( int i = ( int )_CtlFirst; i <= ( int )_CtlMax; ++i )
	{
		const ControlType ct = static_cast<ControlType>( i );
		if( GetControlApiName( ct ).CompareNoCase( want ) == 0 )
			return ct;
	}
	return _CtlInvalid;
}

Prop::Id ParseEventId( const std::string& name )
{
	if( name == "Initialize" ) return Prop::FormEventInitialize;
	if( name == "Close" ) return Prop::FormEventClose;
	if( name == "OnOk" || name == "OnOK" ) return Prop::FormEventOnOk;
	if( name == "OnCancel" ) return Prop::FormEventOnCancel;
	if( name == "CancelClose" ) return Prop::FormEventCancelClose;
	if( name == "Timer" ) return Prop::FormEventTimer;
	if( name == "Move" ) return Prop::FormEventMove;
	if( name == "Size" ) return Prop::FormEventSize;
	if( name == "Clicked" ) return Prop::EventClicked;
	if( name == "DblClicked" ) return Prop::EventDblClicked;
	if( name == "BtnClicked" ) return Prop::EventBtnClicked;
	if( name == "SelChanged" ) return Prop::EventSelChanged;
	if( name == "Help" ) return Prop::EventHelp;
	return GetPropertyId( Utf8ToCString( name ) );
}

bool IsReservedKey( const std::string& k )
{
	return k == "format" || k == "name" || k == "type" || k == "left" || k == "top" ||
	       k == "width" || k == "height" || k == "caption" || k == "text" || k == "events" ||
	       k == "controls" || k == "forms" || k == "output" || k == "tabs" || k == "parent" ||
	       k == "tabIndex" || k == "lispFile";
}

bool IsGeometryOrName( Prop::Id id )
{
	return id == Prop::Name || id == Prop::Left || id == Prop::Top || id == Prop::Width ||
	       id == Prop::Height;
}

CRect ControlRect( const nlohmann::json& c, ControlType type )
{
	const long left = JsonLong( c, "left", 0 );
	const long top = JsonLong( c, "top", 0 );
	long w = JsonLong( c, "width", 0 );
	long h = JsonLong( c, "height", 0 );
	if( w <= 0 )
	{
		switch( type )
		{
		case CtlLabel: w = 100; break;
		case CtlTextButton: w = 80; break;
		case CtlTextBox:
		case CtlComboBox: w = 120; break;
		default: w = 32; break;
		}
	}
	if( h <= 0 )
	{
		switch( type )
		{
		case CtlLabel: h = 20; break;
		case CtlTextButton: h = 24; break;
		case CtlTextBox:
		case CtlComboBox: h = 24; break;
		default: h = 32; break;
		}
	}
	return CRect( left, top, left + w, top + h );
}

void ApplyEvents( TDclControlPtr ctrl, const nlohmann::json& node )
{
	if( !ctrl || !node.contains( "events" ) || !node["events"].is_object() )
		return;
	for( nlohmann::json::const_iterator it = node["events"].begin(); it != node["events"].end(); ++it )
	{
		if( !it.value().is_string() )
			continue;
		const Prop::Id id = ParseEventId( it.key() );
		if( id == Prop::_All || id == Prop::_Private )
			continue;
		ctrl->SetStringProperty( id, Utf8ToCString( it.value().get<std::string>() ) );
	}
}

void ApplyOneProperty( TDclControlPtr ctrl, Prop::Id id, const nlohmann::json& v )
{
	if( !ctrl || id == Prop::_All || id == Prop::_Private )
		return;
	TPropertyPtr existing = ctrl->GetPropertyObject( id );
	const PropertyType pt = existing ? existing->GetType() : PropInvalid;
	if( pt == PropEvent )
	{
		if( v.is_string() )
			ctrl->SetStringProperty( id, Utf8ToCString( v.get<std::string>() ) );
		return;
	}
	if( v.is_boolean() )
	{
		ctrl->SetBooleanProperty( id, v.get<bool>() );
		return;
	}
	if( v.is_number() && pt == PropDouble && existing )
	{
		existing->SetDoubleValue( v.get<double>() );
		return;
	}
	if( v.is_number() )
	{
		if( pt == PropOLEColor && existing )
			existing->SetOLEColorValue( static_cast<OLE_COLOR>( v.get<unsigned long long>() ) );
		else
			ctrl->SetLongProperty( id, static_cast<long>( v.get<long long>() ) );
		return;
	}
	if( v.is_string() )
	{
		ctrl->SetStringProperty( id, Utf8ToCString( v.get<std::string>() ) );
		return;
	}
	if( v.is_array() && existing )
	{
		if( pt == PropStringArray )
		{
			PropVal::TCStringArray* arr = existing->GetStringArrayPtr();
			if( !arr )
				return;
			arr->clear();
			for( const auto& item : v )
			{
				if( item.is_string() )
					arr->push_back( Utf8ToCString( item.get<std::string>() ) );
			}
		}
		else if( pt == PropIntArray )
		{
			PropVal::TIntArray* arr = existing->GetIntArrayPtr();
			if( !arr )
				return;
			arr->clear();
			for( const auto& item : v )
			{
				if( item.is_number() )
					arr->push_back( static_cast<int>( item.get<long long>() ) );
			}
		}
		else if( pt == PropStringArrayList )
		{
			PropVal::TCStringArrayList* list = existing->GetStringArrayListPtr();
			if( !list )
				return;
			list->clear();
			for( const auto& row : v )
			{
				PropVal::TCStringArray one;
				if( row.is_array() )
				{
					for( const auto& item : row )
					{
						if( item.is_string() )
							one.push_back( Utf8ToCString( item.get<std::string>() ) );
					}
				}
				list->push_back( one );
			}
		}
		else if( pt == PropIntArrayList )
		{
			PropVal::TIntArrayList* list = existing->GetIntArrayListPtr();
			if( !list )
				return;
			list->clear();
			for( const auto& row : v )
			{
				PropVal::TIntArray one;
				if( row.is_array() )
				{
					for( const auto& item : row )
					{
						if( item.is_number() )
							one.push_back( static_cast<int>( item.get<long long>() ) );
					}
				}
				list->push_back( one );
			}
		}
	}
}

void ApplyGeometry( TDclControlPtr ctrl, const nlohmann::json& node )
{
	if( !ctrl || !node.is_object() )
		return;
	// AddControl only passes width/height into AddDefaultProperties; Left/Top
	// default to 0. Overlay all four when the JSON names them.
	if( node.contains( "left" ) )
		ctrl->SetLongProperty( Prop::Left, JsonLong( node, "left", 0 ) );
	if( node.contains( "top" ) )
		ctrl->SetLongProperty( Prop::Top, JsonLong( node, "top", 0 ) );
	if( node.contains( "width" ) )
	{
		const long w = JsonLong( node, "width", -1 );
		if( w > 0 )
			ctrl->SetLongProperty( Prop::Width, w );
	}
	if( node.contains( "height" ) )
	{
		const long h = JsonLong( node, "height", -1 );
		if( h > 0 )
			ctrl->SetLongProperty( Prop::Height, h );
	}
}

void ApplyPropertyBag( TDclControlPtr ctrl, const nlohmann::json& node )
{
	if( !ctrl || !node.is_object() )
		return;
	ApplyGeometry( ctrl, node );
	const std::string caption = JsonStr( node, "caption" );
	if( !caption.empty() )
	{
		if( ctrl->GetType() == _CtlForm )
			ctrl->SetStringProperty( Prop::TitleBarText, Utf8ToCString( caption ) );
		else
			ctrl->SetStringProperty( Prop::Caption, Utf8ToCString( caption ) );
	}
	const std::string text = JsonStr( node, "text" );
	if( !text.empty() )
		ctrl->SetStringProperty( Prop::Text, Utf8ToCString( text ) );
	ApplyEvents( ctrl, node );
	for( nlohmann::json::const_iterator it = node.begin(); it != node.end(); ++it )
	{
		if( IsReservedKey( it.key() ) )
			continue;
		const Prop::Id id = GetPropertyId( Utf8ToCString( it.key() ) );
		if( id == Prop::_All || id == Prop::_Private )
			continue;
		ApplyOneProperty( ctrl, id, it.value() );
	}
}

nlohmann::json PropToJson( TPropertyPtr p )
{
	if( !p )
		return nullptr;
	switch( p->GetType() )
	{
	case PropBool:
		return p->GetBooleanValue();
	case PropLong:
	case PropEnum:
	case PropPicture:
		return p->GetLongValue();
	case PropOLEColor:
		return static_cast<unsigned long>( p->GetOLEColorValue() );
	case PropDouble:
		return p->GetDoubleValue();
	case PropString:
	case PropEvent:
		return CStringToUtf8( p->GetStringValue() );
	case PropStringArray:
	{
		nlohmann::json a = nlohmann::json::array();
		const PropVal::TCStringArray* arr = p->GetConstStringArrayPtr();
		if( arr )
		{
			for( size_t i = 0; i < arr->size(); ++i )
				a.push_back( CStringToUtf8( ( *arr )[i] ) );
		}
		return a;
	}
	case PropIntArray:
	{
		nlohmann::json a = nlohmann::json::array();
		const PropVal::TIntArray* arr = p->GetConstIntArrayPtr();
		if( arr )
		{
			for( size_t i = 0; i < arr->size(); ++i )
				a.push_back( ( *arr )[i] );
		}
		return a;
	}
	case PropStringArrayList:
	{
		nlohmann::json a = nlohmann::json::array();
		const PropVal::TCStringArrayList* list = p->GetConstStringArrayListPtr();
		if( list )
		{
			for( size_t r = 0; r < list->size(); ++r )
			{
				nlohmann::json row = nlohmann::json::array();
				for( size_t c = 0; c < ( *list )[r].size(); ++c )
					row.push_back( CStringToUtf8( ( *list )[r][c] ) );
				a.push_back( row );
			}
		}
		return a;
	}
	case PropIntArrayList:
	{
		nlohmann::json a = nlohmann::json::array();
		const PropVal::TIntArrayList* list = p->GetConstIntArrayListPtr();
		if( list )
		{
			for( size_t r = 0; r < list->size(); ++r )
			{
				nlohmann::json row = nlohmann::json::array();
				for( size_t c = 0; c < ( *list )[r].size(); ++c )
					row.push_back( ( *list )[r][c] );
				a.push_back( row );
			}
		}
		return a;
	}
	default:
		return nullptr;
	}
}

bool JsonEqual( const nlohmann::json& a, const nlohmann::json& b )
{
	if( a.is_number() && b.is_number() )
		return a.get<double>() == b.get<double>();
	return a == b;
}

void EmitSparseProperties( nlohmann::json& o, TDclControlPtr ctrl, TDclControlPtr defaults )
{
	if( !ctrl )
		return;
	nlohmann::json events = nlohmann::json::object();
	const TPropertyList& props = ctrl->GetPropertyList();
	for( TPropertyList::const_iterator it = props.begin(); it != props.end(); ++it )
	{
		TPropertyPtr p = *it;
		if( !p )
			continue;
		const Prop::Id id = p->GetID();
		if( IsGeometryOrName( id ) )
			continue;
		if( p->GetType() == PropImageList || p->GetType() == PropCustom ||
		    p->GetType() == PropActiveXPropPages || p->GetType() == PropActiveXProp ||
		    p->GetType() == PropActiveXEnum || p->GetType() == PropActiveXEvent ||
		    p->GetType() == PropActiveXRunTime || p->GetType() == PropActiveXMethods )
			continue;
		const nlohmann::json cur = PropToJson( p );
		if( cur.is_null() )
			continue;
		TPropertyPtr d = defaults ? defaults->GetPropertyObject( id ) : TPropertyPtr();
		if( d )
		{
			const nlohmann::json def = PropToJson( d );
			if( JsonEqual( cur, def ) )
				continue;
		}
		if( p->GetType() == PropEvent )
		{
			if( cur.is_string() && !cur.get<std::string>().empty() )
			{
				LPCTSTR api = GetPropertyApiName( id );
				if( api && *api )
					events[CStringToUtf8( api )] = cur;
			}
			continue;
		}
		if( id == Prop::TitleBarText || id == Prop::Caption )
		{
			if( cur.is_string() )
				o["caption"] = cur;
			continue;
		}
		LPCTSTR api = GetPropertyApiName( id );
		if( !api || !*api )
			continue;
		o[CStringToUtf8( api )] = cur;
	}
	if( !events.empty() )
		o["events"] = events;
}

TDclControlPtr DefaultFormProps( FormType ft )
{
	static CProject s_proj( _T("_json_defaults") );
	CString name;
	name.Format( _T("F%d"), static_cast<int>( ft ) );
	TDclFormPtr existing = s_proj.FindDclForm( name );
	if( !existing )
		existing = s_proj.AddForm( ft, name );
	return existing ? existing->GetControlProperties() : TDclControlPtr();
}

TDclControlPtr DefaultControlProps( ControlType ct, const CRect& rc )
{
	static CProject s_proj( _T("_json_ctl_defaults") );
	TDclFormPtr form = s_proj.FindDclForm( _T("Host") );
	if( !form )
		form = s_proj.AddForm( FrmModalDlg, _T("Host") );
	if( !form )
		return TDclControlPtr();
	CString name;
	name.Format( _T("C%d"), static_cast<int>( ct ) );
	TDclControlPtr existing = form->FindControl( name, ct );
	if( existing )
		return existing;
	return form->AddControl( ct, name, rc );
}

bool ReadForm( CProject& project, TDclFormPtr parent, const nlohmann::json& form, CString& error );

bool ReadControls( TDclFormPtr pForm, const nlohmann::json& form, CString& error )
{
	if( !form.contains( "controls" ) || !form["controls"].is_array() )
		return true;
	int ctlIndex = 0;
	for( nlohmann::json::const_iterator cit = form["controls"].begin();
	     cit != form["controls"].end(); ++cit )
	{
		const nlohmann::json& c = *cit;
		if( !c.is_object() )
		{
			error = _T("control entry must be an object");
			return false;
		}
		const std::string types = JsonStr( c, "type" );
		const ControlType ct = ParseControlType( types );
		if( ct == _CtlInvalid )
		{
			error = _T("unknown control type: ") + Utf8ToCString( types );
			return false;
		}
		std::string cname = JsonStr( c, "name" );
		if( cname.empty() )
		{
			++ctlIndex;
			CString tmp;
			tmp.Format( _T("%s%d"), Utf8ToCString( types ).GetString(), ctlIndex );
			cname = CStringA( tmp ).GetString();
		}
		TDclControlPtr pCtl = pForm->AddControl( ct, Utf8ToCString( cname ), ControlRect( c, ct ) );
		if( !pCtl )
		{
			error = _T("AddControl failed");
			return false;
		}
		ApplyPropertyBag( pCtl, c );
	}
	return true;
}

bool ReadForm( CProject& project, TDclFormPtr parent, const nlohmann::json& form, CString& error )
{
	if( !form.is_object() )
	{
		error = _T("form entry must be an object");
		return false;
	}
	FormType ft = ParseFormType( JsonStr( form, "type", parent ? "tabPage" : "modal" ) );
	if( parent )
		ft = FrmTabPage;
	if( ft == _FrmInvalid )
	{
		error = _T("unknown form type: ") + Utf8ToCString( JsonStr( form, "type" ) );
		return false;
	}
	std::string fname = JsonStr( form, "name" );
	if( fname.empty() && !parent )
		fname = "Form";
	TDclFormPtr pForm;
	if( parent )
		pForm = parent->AddChildForm( ft );
	else
		pForm = project.AddForm( ft, Utf8ToCString( fname ) );
	if( !pForm )
	{
		error = _T("AddForm failed");
		return false;
	}
	TDclControlPtr props = pForm->GetControlProperties();
	if( parent )
	{
		// Tab pages often have an empty Name and inherit the parent via GetKeyName().
		// Do not stamp that inherited name back onto the child.
		if( !fname.empty() )
			props->SetStringProperty( Prop::Name, Utf8ToCString( fname ) );
		if( form.contains( "tabIndex" ) )
			pForm->SetTabIndex( static_cast<short>( JsonLong( form, "tabIndex", 0 ) ) );
	}
	const long width = JsonLong( form, "width", -1 );
	const long height = JsonLong( form, "height", -1 );
	if( width > 0 )
		props->SetLongProperty( Prop::Width, width );
	if( height > 0 )
		props->SetLongProperty( Prop::Height, height );
	ApplyPropertyBag( props, form );
	if( !ReadControls( pForm, form, error ) )
		return false;
	if( form.contains( "tabs" ) && form["tabs"].is_array() )
	{
		for( const auto& tab : form["tabs"] )
		{
			if( !ReadForm( project, pForm, tab, error ) )
				return false;
		}
	}
	return true;
}

nlohmann::json WriteControl( TDclControlPtr pCtl )
{
	nlohmann::json c = nlohmann::json::object();
	c["type"] = CStringToUtf8( GetControlApiName( pCtl->GetType() ) );
	c["name"] = CStringToUtf8( pCtl->GetKeyName() );
	const CRect rc = pCtl->GetWndRect();
	c["left"] = rc.left;
	c["top"] = rc.top;
	c["width"] = rc.Width();
	c["height"] = rc.Height();
	EmitSparseProperties( c, pCtl, DefaultControlProps( pCtl->GetType(), rc ) );
	return c;
}

nlohmann::json WriteForm( const CProject& project, TDclFormPtr pForm )
{
	nlohmann::json f = nlohmann::json::object();
	f["type"] = FormTypeName( pForm->GetType() );
	TDclControlPtr props = pForm->GetControlProperties();
	if( pForm->GetParentForm() )
	{
		f["tabIndex"] = pForm->GetTabIndex();
		CString tabName = props ? props->GetStringProperty( Prop::Name ) : CString();
		if( !tabName.IsEmpty() )
			f["name"] = CStringToUtf8( tabName );
	}
	else
		f["name"] = CStringToUtf8( pForm->GetKeyName() );
	if( props && props->GetType() == _CtlForm )
	{
		const long w = props->GetLongProperty( Prop::Width );
		const long h = props->GetLongProperty( Prop::Height );
		if( w > 0 )
			f["width"] = w;
		if( h > 0 )
			f["height"] = h;
		EmitSparseProperties( f, props, DefaultFormProps( pForm->GetType() ) );
	}
	nlohmann::json controls = nlohmann::json::array();
	const TDclControlList& list = pForm->GetControlList();
	for( TDclControlList::const_iterator it = list.begin(); it != list.end(); ++it )
	{
		TDclControlPtr pCtl = *it;
		if( !pCtl || pCtl->GetType() == _CtlForm )
			continue;
		controls.push_back( WriteControl( pCtl ) );
	}
	if( !controls.empty() )
		f["controls"] = controls;
	TDclFormList children;
	if( project.FindChildForms( pForm, children ) )
	{
		nlohmann::json tabs = nlohmann::json::array();
		for( TDclFormList::const_iterator it = children.begin(); it != children.end(); ++it )
			tabs.push_back( WriteForm( project, *it ) );
		if( !tabs.empty() )
			f["tabs"] = tabs;
	}
	return f;
}

} // namespace

bool StudioPathIsJson( const CString& path )
{
	return PathEndsWith( path, _T(".odcl.json") ) || PathExt( path ) == _T(".json");
}

bool StudioProjectFromJsonFile( const CString& jsonPath, CProject& project, CString& error )
{
	error.Empty();
	std::ifstream in( jsonPath.GetString(), std::ios::binary );
	if( !in )
	{
		error.Format( _T("cannot open %s"), static_cast<LPCTSTR>( jsonPath ) );
		return false;
	}
	const std::string text( ( std::istreambuf_iterator<char>( in ) ), std::istreambuf_iterator<char>() );
	nlohmann::json ir;
	try
	{
		ir = nlohmann::json::parse( text );
	}
	catch( const std::exception& e )
	{
		error = _T("JSON parse: ") + Utf8ToCString( e.what() );
		return false;
	}
	if( !ir.is_object() )
	{
		error = _T("project JSON must be an object");
		return false;
	}
	int format = kJsonFormat;
	if( ir.contains( "format" ) && ir["format"].is_number_integer() )
		format = static_cast<int>( ir["format"].get<long long>() );
	if( format > kJsonFormat )
	{
		error.Format( _T("unsupported .odcl.json format %d"), format );
		return false;
	}
	std::string key = JsonStr( ir, "name" );
	if( key.empty() )
	{
		CString stem = jsonPath;
		const int slash1 = stem.ReverseFind( _T('\\') );
		const int slash2 = stem.ReverseFind( _T('/') );
		const int slash = ( slash1 > slash2 ) ? slash1 : slash2;
		if( slash >= 0 )
			stem = stem.Mid( slash + 1 );
		if( PathEndsWith( stem, _T(".odcl.json") ) )
			stem = stem.Left( stem.GetLength() - 9 );
		else
		{
			const int dot = stem.ReverseFind( _T('.') );
			if( dot > 0 )
				stem = stem.Left( dot );
		}
		key = CStringA( stem ).GetString();
	}
	if( key.empty() )
		key = "sandbox";
	DisableUndoManager disableUndo( project.GetUndoManager() );
	project.SetKeyName( Utf8ToCString( key ) );
	if( ir.contains( "lispFile" ) && ir["lispFile"].is_string() )
		project.SetLispFileName( Utf8ToCString( ir["lispFile"].get<std::string>() ) );
	if( !ir.contains( "forms" ) || !ir["forms"].is_array() || ir["forms"].empty() )
	{
		error = _T("JSON needs a non-empty forms array");
		return false;
	}
	try
	{
		for( const auto& form : ir["forms"] )
		{
			if( !ReadForm( project, TDclFormPtr(), form, error ) )
				return false;
		}
		return true;
	}
	catch( CException* e )
	{
		TCHAR msg[256] = {};
		e->GetErrorMessage( msg, 256 );
		e->Delete();
		error = _T("MFC exception: ");
		error += msg;
		return false;
	}
	catch( const std::exception& e )
	{
		error = Utf8ToCString( e.what() );
		return false;
	}
}

bool StudioProjectToJsonFile( const CProject& project, const CString& jsonPath, CString& error )
{
	error.Empty();
	nlohmann::json root = nlohmann::json::object();
	root["format"] = kJsonFormat;
	root["name"] = CStringToUtf8( project.GetKeyName() );
	if( !project.GetLispFileName().IsEmpty() )
		root["lispFile"] = CStringToUtf8( project.GetLispFileName() );
	nlohmann::json forms = nlohmann::json::array();
	const TDclFormList& list = project.GetDclFormList();
	for( TDclFormList::const_iterator it = list.begin(); it != list.end(); ++it )
	{
		TDclFormPtr pForm = *it;
		if( !pForm || pForm->GetParentForm() )
			continue;
		forms.push_back( WriteForm( project, pForm ) );
	}
	root["forms"] = forms;
	std::ofstream out( jsonPath.GetString(), std::ios::binary | std::ios::trunc );
	if( !out )
	{
		error.Format( _T("cannot write %s"), static_cast<LPCTSTR>( jsonPath ) );
		return false;
	}
	const std::string text = root.dump( 2 );
	out.write( text.data(), static_cast<std::streamsize>( text.size() ) );
	out.put( '\n' );
	return true;
}

void StudioWriteConsoleMessage( const CString& msg, bool err )
{
	::AttachConsole( ATTACH_PARENT_PROCESS );
	HANDLE h = ::GetStdHandle( err ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE );
	if( !h || h == INVALID_HANDLE_VALUE )
		return;
	CString line = msg;
	if( line.IsEmpty() || line[line.GetLength() - 1] != _T('\n') )
		line += _T("\r\n");
	CStringA a( line );
	DWORD written = 0;
	::WriteFile( h, static_cast<LPCSTR>( a ), static_cast<DWORD>( a.GetLength() ), &written, NULL );
}

bool StudioTryGetConvertJob( CString& inPath, CString& outPath )
{
	inPath.Empty();
	outPath.Empty();
	int argc = 0;
	LPWSTR* argv = ::CommandLineToArgvW( ::GetCommandLineW(), &argc );
	if( !argv )
		return false;
	CString firstPos;
	for( int i = 0; i < argc; ++i )
	{
		CString a( argv[i] );
		CString lower = a;
		lower.MakeLower();
		if( lower == _T("/out") || lower == _T("-out") )
		{
			if( i + 1 < argc )
				outPath = argv[++i];
			continue;
		}
		if( lower.Left( 5 ) == _T("/out:") || lower.Left( 5 ) == _T("-out:") )
		{
			outPath = a.Mid( 5 );
			continue;
		}
		if( !a.IsEmpty() && ( a[0] == _T('/') || a[0] == _T('-') ) )
			continue;
		if( lower.Right( 4 ) == _T(".exe") )
			continue;
		if( firstPos.IsEmpty() )
			firstPos = a;
	}
	::LocalFree( argv );
	inPath = firstPos;
	return !outPath.IsEmpty();
}

static CString CombinePath( const CString& dir, const CString& rel )
{
	if( rel.IsEmpty() )
		return dir;
	if( rel.GetLength() >= 2 && rel[1] == _T(':') )
		return rel;
	if( rel.GetLength() >= 1 && ( rel[0] == _T('\\') || rel[0] == _T('/') ) )
		return rel;
	CString d = dir;
	if( !d.IsEmpty() && d[d.GetLength() - 1] != _T('\\') && d[d.GetLength() - 1] != _T('/') )
		d += _T('\\');
	return d + rel;
}

static CString PathDir( const CString& path )
{
	const int slash1 = path.ReverseFind( _T('\\') );
	const int slash2 = path.ReverseFind( _T('/') );
	const int cut = ( slash1 > slash2 ) ? slash1 : slash2;
	if( cut < 0 )
	{
		TCHAR cwd[MAX_PATH] = {};
		::GetCurrentDirectory( MAX_PATH, cwd );
		return cwd;
	}
	return path.Left( cut );
}

bool StudioConvertProject( const CString& inPath, CString& outPath, CString& error )
{
	error.Empty();
	if( inPath.IsEmpty() )
	{
		error = _T("/out needs an input file");
		return false;
	}
	if( outPath.IsEmpty() )
	{
		error = _T("/out path is empty");
		return false;
	}
	outPath = CombinePath( PathDir( inPath ), outPath );
	const CString absOut = outPath;
	CProject project;
	if( StudioPathIsJson( inPath ) )
	{
		if( !StudioProjectFromJsonFile( inPath, project, error ) )
			return false;
	}
	else
	{
		const IOStatus st = project.ReadFromFile( inPath );
		if( st != statOK )
		{
			error.Format( _T("cannot read %s"), static_cast<LPCTSTR>( inPath ) );
			return false;
		}
	}
	if( StudioPathIsJson( absOut ) )
		return StudioProjectToJsonFile( project, absOut, error );
	const IOStatus st = project.WriteToFile( absOut );
	if( st != statOK )
	{
		error.Format( _T("cannot write %s"), static_cast<LPCTSTR>( absOut ) );
		return false;
	}
	return true;
}
