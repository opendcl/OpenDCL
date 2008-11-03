#pragma once


CString VTToString( VARTYPE vt );
CString AxTypeToDisplayableLispType( VARTYPE vt, const GUID& guid = GUID_NULL );
HRESULT GetNameOfRefType( ITypeInfo* pTypeInfo, HREFTYPE hreftype, CString& sTypeName );
void SetRefType( VARTYPE& vtType, ITypeInfo* TheInfo, HREFTYPE hreftype, CLSID& Clsid );
CString GetAxTypeName( const CLSID& clsid );
