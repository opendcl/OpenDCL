#pragma once


CString VTToString( VARTYPE vt );
CString AxTypeToApiLispType( VARTYPE vt, const GUID& guid = GUID_NULL );
CString AxTypeToDisplayableLispType( VARTYPE vt, const GUID& guid = GUID_NULL );
HRESULT GetNameOfRefType( ITypeInfo* pTypeInfo, HREFTYPE hreftype, CString& sTypeName );
void SetRefType( VARTYPE& vtType, ITypeInfo* TheInfo, HREFTYPE hreftype, CLSID& Clsid );
CString GetAxProgId( const CLSID& clsid );
CString GetAxShortTypeName( LPCTSTR pszProgId );
CString GetAxShortTypeName( const CLSID& clsid );
CString GetAxLongTypeName( const CLSID& clsid );
bool IsMicrosoftCtrl( const CLSID& clsid );