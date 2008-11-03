#include "StdAfx.H"
#include "VarUtils.H"


static struct _VARTYPENAME_ENTRY { VARTYPE vt; LPCTSTR pszName; } g_aVarTypes[] =
{
	{ VT_EMPTY, _T("VT_EMPTY"), },
	{ VT_NULL, _T("VT_NULL"), },
	{ VT_I2, _T("VT_I2"), },
	{ VT_I4, _T("VT_I4"), },
	{ VT_R4, _T("VT_R4"), },
	{ VT_R8, _T("VT_R8"), },
	{ VT_CY, _T("VT_CY"), },
	{ VT_DATE, _T("VT_DATE"), },
	{ VT_BSTR, _T("VT_BSTR"), },
	{ VT_DISPATCH, _T("VT_DISPATCH"), },
	{ VT_ERROR, _T("VT_ERROR"), },
	{ VT_BOOL, _T("VT_BOOL"), },
	{ VT_VARIANT, _T("VT_VARIANT"), },
	{ VT_UNKNOWN, _T("VT_UNKNOWN"), },
	{ VT_DECIMAL, _T("VT_DECIMAL"), },
	{ VT_I1, _T("VT_I1"), },
	{ VT_UI1, _T("VT_UI1"), },
	{ VT_UI2, _T("VT_UI2"), },
	{ VT_UI4, _T("VT_UI4"), },
	{ VT_I8, _T("VT_I8"), },
	{ VT_UI8, _T("VT_UI8"), },
	{ VT_INT, _T("VT_INT"), },
	{ VT_UINT, _T("VT_UINT"), },
	{ VT_VOID, _T("VT_VOID"), },
	{ VT_HRESULT, _T("VT_HRESULT"), },
	{ VT_PTR, _T("VT_PTR"), },
	{ VT_SAFEARRAY, _T("VT_SAFEARRAY"), },
	{ VT_CARRAY, _T("VT_CARRAY"), },
	{ VT_USERDEFINED, _T("VT_USERDEFINED"), },
	{ VT_LPSTR, _T("VT_LPSTR"), },
	{ VT_LPWSTR, _T("VT_LPWSTR"), },
	{ VT_RECORD, _T("VT_RECORD"), },
	{ VT_INT_PTR, _T("VT_INT_PTR"), },
	{ VT_UINT_PTR, _T("VT_UINT_PTR"), },
	{ VT_FILETIME, _T("VT_FILETIME"), },
	{ VT_BLOB, _T("VT_BLOB"), },
	{ VT_STREAM, _T("VT_STREAM"), },
	{ VT_STORAGE, _T("VT_STORAGE"), },
	{ VT_STREAMED_OBJECT, _T("VT_STREAMED_OBJECT"), },
	{ VT_STORED_OBJECT, _T("VT_STORED_OBJECT"), },
	{ VT_BLOB_OBJECT, _T("VT_BLOB_OBJECT"), },
	{ VT_CF, _T("VT_CF"), },
	{ VT_CLSID, _T("VT_CLSID"), },
	{ VT_VERSIONED_STREAM, _T("VT_VERSIONED_STREAM"), },
};


CString VTToString( VARTYPE vt )
{
	CString sPrefix;
	if( (vt & VT_BYREF) != 0 )
		sPrefix += _T("'");
	VARTYPE vtSimple = (VT_TYPEMASK & vt);
	for( int idxType = 0; idxType < _elements(g_aVarTypes); ++idxType )
	{
		if( g_aVarTypes[idxType].vt == vtSimple )
			return sPrefix + g_aVarTypes[idxType].pszName;
	}
	return sPrefix;
}


CString AxTypeToDisplayableLispType( VARTYPE vt, const GUID& guid /*= GUID_NULL*/ )
{
	CString sPrefix;
	if( (vt & VT_BYREF) != 0 )
		sPrefix += _T("'");
	if( guid == GUID_COLOR )
		return sPrefix + _T("OLEColor");
	switch( vt & VT_TYPEMASK )
	{
	case VT_BOOL:
	case VT_I1:
	case VT_UI1:
		return sPrefix + _T("Boolean");
		break;                         

	case VT_I2:
	case VT_UI2:
	case VT_INT:
	case VT_UINT:
		return sPrefix + _T("Integer");
		break;

	case VT_CY:
	case VT_I4:
	case VT_UI4:
		return sPrefix + _T("Long");
		break;

	case VT_R4:
	case VT_R8:
		return sPrefix + _T("Float");
		break;

	case VT_BSTR:
	case VT_LPSTR:
		return sPrefix + _T("String");
		break;

	case VT_UNKNOWN:
	case VT_DISPATCH:
		{
			return sPrefix + _T("AxObject");
			break;
		}
	case VT_VOID:
	case VT_EMPTY:
	case VT_NULL:
		break;

	case VT_PTR:
		return sPrefix + _T("Handle");
		break;

	case VT_VARIANT:	
		return sPrefix + _T("Variant");
		break;

	default:
		break;
	}
		
	return sPrefix;
}


HRESULT GetNameOfRefType( ITypeInfo* pTypeInfo, HREFTYPE hreftype, CString& sTypeName )
{
	ITypeInfo* pRefTypeInfo = NULL;
	HRESULT hr = pTypeInfo->GetRefTypeInfo( hreftype, &pRefTypeInfo );
	if( FAILED(hr) )
		return hr;
	CComBSTR bstrName;
	hr = pRefTypeInfo->GetDocumentation( MEMBERID_NIL, &bstrName, NULL, NULL, NULL );
	if( FAILED(hr) )
		return hr;
	sTypeName = bstrName;
	if( sTypeName == _T("OLE_COLOR") )
		sTypeName = _T("OLEColor");
	return S_OK;
}


void SetRefType( VARTYPE& vtType, ITypeInfo* TheInfo, HREFTYPE hreftype, CLSID& Clsid )
{
	ITypeInfo *TheRefType = NULL;
	if ((hreftype != -1) &&
		SUCCEEDED(TheInfo->GetRefTypeInfo(hreftype, &TheRefType)))
	{
		LPTYPEATTR pTA;
		if (SUCCEEDED(TheRefType->GetTypeAttr(&pTA)))
		{
			switch (pTA->typekind)
			{
			case TKIND_ALIAS: // color, etc.
				if ((pTA->tdescAlias.vt == VT_USERDEFINED) && (hreftype < VT_USERDEFINED))
					vtType = (VARTYPE) hreftype;
				else
					vtType = pTA->tdescAlias.vt;

				if (vtType == VT_USERDEFINED)
					SetRefType(vtType, TheRefType, 
						(pTA->tdescAlias.vt == VT_PTR) ? 
							pTA->tdescAlias.lptdesc->hreftype : pTA->tdescAlias.hreftype, Clsid);
				break;
			case TKIND_DISPATCH: // font, etc.
				vtType = VT_DISPATCH;
				Clsid = pTA->guid;
				break;
			case TKIND_ENUM:
				{
					vtType = VT_I4; // better than nothing
					VARDESC *pVarDesc;
					if (pTA->cVars > 0)
					{ // try to determine real type
						if(SUCCEEDED(TheRefType->GetVarDesc(0, &pVarDesc )))
						{
						#ifdef MAINWIN
							vtType = pVarDesc->lpvarValue->n2.vt;
						#else
							vtType = pVarDesc->lpvarValue->vt;
						#endif
							TheRefType->ReleaseVarDesc(pVarDesc);
						}
					}
				}
				break;
			}
		}
		TheRefType->Release();
	}
}

CString GetAxTypeName( const CLSID& clsid )
{
	CString sName;
	WCHAR* pwszProgID = NULL;        
	HRESULT hr = ProgIDFromCLSID( clsid, &pwszProgID );
	if( SUCCEEDED(hr) )
	{
		sName = pwszProgID;
		CoTaskMemFree( pwszProgID );
	}
	sName.MakeReverse();
	sName = sName.Right( sName.GetLength() - sName.SpanIncluding( _T("0123456789.") ).GetLength() ).SpanExcluding( _T(".") );
	sName.MakeReverse();
	return sName;
}
