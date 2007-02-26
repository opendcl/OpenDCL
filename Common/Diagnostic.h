// Diagnostic.h
//

#pragma once


#ifdef _DIAGNOSTIC

#include <StrSafe.h>
#include "DclFormTypes.h"
#include "ControlTypes.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "PropertyNames.h"

#pragma warning(push)
#pragma warning(disable:4995)

#ifdef _UNICODE
inline const TCHAR* tString( LPCWSTR pszWide ) { return pszWide? pszWide : _T("<null>"); }
inline const TCHAR* tString( LPCSTR pszAnsi )
{
	if( !pszAnsi )
		return _T("<null>");
	static TCHAR buf[1024];
	MultiByteToWideChar( CP_ACP, 0, pszAnsi, -1, buf, _elements(buf) );
	return buf;
}
#else //_UNICODE
inline const TCHAR* tString( LPCWSTR pszWide )
{
	if( !pszWide )
		return _T("<null>");
	static TCHAR buf[1024];
	WideCharToMultiByte( CP_ACP, 0, pszWide, -1, buf, _elements(buf), NULL, NULL );
	return buf;
}
inline const TCHAR* tString( LPCSTR pszAnsi ) { return pszAnsi? pszAnsi : _T("<null>"); }
#endif //_UNICODE


template< typename T >
const TCHAR* asString( const T& value, const size_t Size )
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("%s [%s bytes]"), asString( value ), asString( (unsigned long)Size ) );
	return buf;
}

inline
const TCHAR* asString( const CString& s )
{
	return (LPCTSTR)s;
}

inline
const TCHAR* asString( const long n )
{
	static TCHAR buf[64];
	return _ltot( n, buf, 10 );
}

inline
const TCHAR* asString( const unsigned long n )
{
	static TCHAR buf[64];
	return _ultot( n, buf, 10 );
}

inline
const TCHAR* asString( const int n )
{
	static TCHAR buf[64];
	return _itot( n, buf, 10 );
}

inline
const TCHAR* asString( const size_t n )
{
	static TCHAR buf[64];
	return _ultot( n, buf, 10 );
}

inline
const TCHAR* asString( const bool b )
{
	return b? _T("True") : _T("False");
}

inline
const TCHAR* asString( const TCHAR Ch )
{
	static TCHAR buf[64];
	return _itot( Ch, buf, 10 );
}

inline
const TCHAR* asString( const TCHAR* psz )
{
	if( !psz )
		return _T("<null>");
	size_t cchStr = lstrlen( psz );
	const TCHAR* pszUnits = (cchStr == 1? _T("character") : _T("characters"));
	static TCHAR buf[1024];
	if( cchStr < 255 )
		_sntprintf( buf, _elements(buf), _T("%s [%s %s]"), psz, asString( cchStr ), pszUnits );
	else
	{
		lstrcpyn( buf, psz, 30 );
		_sntprintf( buf + 30, _elements(buf) - 30, _T("... [%s %s]"), asString( cchStr ), pszUnits );
	}
	return buf;
}

inline
const TCHAR* asString( const void* pv )
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("[%X]"), INT_PTR(pv) );
	return buf;
}

inline
const TCHAR* asString( const double& dbl )
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("%G"), dbl );
	return buf;
}

#ifdef _ACRXAPP

inline
const TCHAR* asString( const Acad::ErrorStatus& es )
{
	return acadErrorStatusText( es );
}

inline
const TCHAR* asString( const AcGePoint2d& pnt )
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("(%G, %G)"), pnt.x, pnt.y );
	return buf;
}

inline
const TCHAR* asString( const AcGePoint3d& pnt )
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("(%G, %G, %G)"), pnt.x, pnt.y, pnt.z );
	return buf;
}

inline
const TCHAR* asString( const AcGeVector2d& vec )
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("(%G, %G)"), vec.x, vec.y );
	return buf;
}

inline
const TCHAR* asString( const AcGeVector3d& vec )
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("(%G, %G, %G)"), vec.x, vec.y, vec.z );
	return buf;
}

inline
const TCHAR* asString( const AcGeMatrix2d& mat )
{
	static TCHAR buf[512];
	_sntprintf( buf, _elements(buf),
							_T("    { %16G | %16G | %16G }\r\n")
							_T("    { %16G | %16G | %16G }\r\n")
							_T("    { %16G | %16G | %16G }"),
							mat.entry[0][0],
							mat.entry[1][0],
							mat.entry[2][0],
							mat.entry[0][1],
							mat.entry[1][1],
							mat.entry[2][1],
							mat.entry[0][2],
							mat.entry[1][2],
							mat.entry[2][2] );
	return buf;
}

inline
const TCHAR* asString( const AcGeMatrix3d& mat )
{
	static TCHAR buf[512];
	_sntprintf( buf, _elements(buf),
							_T("    { %16G | %16G | %16G | %16G }\r\n")
							_T("    { %16G | %16G | %16G | %16G }\r\n")
							_T("    { %16G | %16G | %16G | %16G }\r\n")
							_T("    { %16G | %16G | %16G | %16G }"),
							mat.entry[0][0],
							mat.entry[1][0],
							mat.entry[2][0],
							mat.entry[3][0],
							mat.entry[0][1],
							mat.entry[1][1],
							mat.entry[2][1],
							mat.entry[3][1],
							mat.entry[0][2],
							mat.entry[1][2],
							mat.entry[2][2],
							mat.entry[3][2],
							mat.entry[0][3],
							mat.entry[1][3],
							mat.entry[2][3],
							mat.entry[3][3] );
	return buf;
}

inline
const TCHAR* asString( const AcGeScale3d& sc )
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("(%G, %G, %G)"), sc.sx, sc.sy, sc.sz );
	return buf;
}

#endif //_ACRXAPP


template< typename T >
const TCHAR* asString( const std::vector< T >& rT )
{
	size_t ctElem = rT.size();
	if( ctElem == 0 )
		return _T("<empty>");
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("<%s> {%s"), asString( ctElem ), asString( rT.at( 0 ) ) );
	for( size_t idx = 1; idx < ctElem; ++idx )
	{
		TCHAR elem[1024];
		_sntprintf( elem, _elements(elem), _T(", %s"), asString( rT.at( idx ) ) );
		StringCbCat( buf, _elements(buf), elem );
	}
	StringCbCat( buf, _elements(buf), _T("}") );
	return buf;
}

template<>
inline
const TCHAR* asString( const std::vector< CString >& rStr )
{
	size_t ctElem = rStr.size();
	if( ctElem == 0 )
		return _T("<empty>");
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("<%s> {\"%s\""), asString( ctElem ), asString( rStr.at( 0 ) ) );
	for( size_t idx = 1; idx < ctElem; ++idx )
	{
		TCHAR elem[1024];
		_sntprintf( elem, _elements(elem), _T(", \"%s\""), asString( rStr.at( idx ) ) );
		StringCbCat( buf, _elements(buf), elem );
	}
	StringCbCat( buf, _elements(buf), _T("}") );
	return buf;
}


inline
const TCHAR* asString( DclFormType type )
{
	switch( type )
	{
	case VdclInvalid:
		return _T("VdclInvalid");
	case VdclModal:
		return _T("VdclModal");
	case VdclModeless:
		return _T("VdclModeless");
	case VdclDockable:
		return _T("VdclDockable");
	case VdclConfigTab:
		return _T("VdclConfigTab");
	case VdclTabForm:
		return _T("VdclTabForm");
	case VdclFileDialog:
		return _T("VdclFileDialog");
	};
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("<unknown: %s>"), asString( static_cast< int >(type) ) );
	return buf;
}

inline
const TCHAR* asString( ControlType type )
{
	switch( type )
	{
	case CtlInvalid:
		return _T("CtlInvalid");
	case CtlForm:
		return _T("CtlForm");
	case CtlLabel:
		return _T("CtlLabel");
	case CtlStdButton:
		return _T("CtlStdButton");
	case CtlGraphicButton:
		return _T("CtlGraphicButton");
	case CtlFrame:
		return _T("CtlFrame");
	case CtlTextBox:
		return _T("CtlTextBox");
	case CtlCheckBox:
		return _T("CtlCheckBox");
	case CtlOptionButton:
		return _T("CtlOptionButton");
	case CtlComboBox:
		return _T("CtlComboBox");
	case CtlListBox:
		return _T("CtlListBox");
	case CtlScrollBar:
		return _T("CtlScrollBar");
	case CtlSlider:
		return _T("CtlSlider");
	case CtlPictureBox:
		return _T("CtlPictureBox");
	case CtlTabStrip:
		return _T("CtlTabStrip");
	case CtlMonth:
		return _T("CtlMonth");
	case CtlTree:
		return _T("CtlTree");
	case Ctl3DRect:
		return _T("Ctl3DRect");
	case CtlProgress:
		return _T("CtlProgress");
	case CtlSpinButton:
		return _T("CtlSpinButton");
	case CtlStaticURL:
		return _T("CtlStaticURL");
	case CtlRoundSlider:
		return _T("CtlRoundSlider");
	case CtlBlockView:
		return _T("CtlBlockView");
	case CtlSlideView:
		return _T("CtlSlideView");
	case CtlHtmlCtrl:
		return _T("CtlHtmlCtrl");
	case CtlDwgPreview:
		return _T("CtlDwgPreview");
	case CtlListView:
		return _T("CtlListView");
	case CtlBlockList:
		return _T("CtlBlockList");
	case CtlOptionList:
		return _T("CtlOptionList");
	case CtlActiveX:
		return _T("CtlActiveX");
	case CtlDwgList:
		return _T("CtlDwgList");
	case CtlAnimate:
		return _T("CtlAnimate");
	case CtlImageComboBox:
		return _T("CtlImageComboBox");
	case CtlGrid:
		return _T("CtlGrid");
	case CtlSplitter:
		return _T("CtlSplitter");
	case CtlHatch:
		return _T("CtlHatch");
	case CtlFileDlgCtrl:
		return _T("CtlFileDlgCtrl");
	};
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("<unknown: %s>"), asString( static_cast< int >(type) ) );
	return buf;
}

inline
const TCHAR* asString( PropertyType type )
{
	switch( type )
	{
	case PropInvalid:
		return _T("PropInvalid");
	case PropLong:
		return _T("PropLong");
	case PropString:
		return _T("PropString");
	case PropDouble:
		return _T("PropDouble");
	case PropBool:
		return _T("PropBool");
	case PropEnum:
		return _T("PropEnum");
	case PropEvent:
		return _T("PropEvent");
	case PropPicture:
		return _T("PropPicture");
	case PropCustom:
		return _T("PropCustom");
	case PropImageList:
		return _T("PropImageList");
	case PropOLEColor:
		return _T("PropOLEColor");
	case PropStringArray:
		return _T("PropStringArray");
	case PropIntArray:
		return _T("PropIntArray");
	case PropActiveXPropPages:
		return _T("PropActiveXPropPages");
	case PropActiveXProp:
		return _T("PropActiveXProp");
	case PropActiveXEnum:
		return _T("PropActiveXEnum");
	case PropActiveXEvent:
		return _T("PropActiveXEvent");
	case PropActiveXRunTime:
		return _T("PropActiveXRunTime");
	case PropActiveXMethods:
		return _T("PropActiveXMethods");
	case PropStringArrayList:
		return _T("PropStringArrayList");
	case PropIntArrayList:
		return _T("PropIntArrayList");
	};
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("<unknown: %s>"), asString( static_cast< int >(type) ) );
	return buf;
}

inline
const TCHAR* asString( PropertyId prop )
{
	switch( prop )
	{
	case nPrivateProperty:
		return _T("Null/Private");
	};
	LPCTSTR pszName = GetPropertyName( prop );
	if( pszName && *pszName )
		return pszName;
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("<unknown: %s>"), asString( static_cast< int >(prop) ) );
	return buf;
}


#pragma warning(pop)

#else //_DIAGNOSTIC

template< typename T >
const TCHAR* asString( T ) { return _T(""); }

#endif //_DIAGNOSTIC
