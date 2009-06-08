// AxMethodDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxPropertyDescriptor.h"
#include "AxContainerCtrl.h"
#include "VarUtils.h"
#include "Filing.h"
#include "Workspace.h"


AxPropertyDescriptor::AxPropertyDescriptor(void)
: mDispId( 0 ), mType( 0 ), mbArray( false ), mbReadOnly( true ), mInvKind( (INVOKEKIND)0 )
{
	::memset(&mGuid, 0, sizeof(GUID));
}

AxPropertyDescriptor::AxPropertyDescriptor( DISPID dispid, LPCTSTR pszName, LPCTSTR pszDesc,
																						VARTYPE type, INVOKEKIND kind )
: mDispId( dispid ), msName( pszName ), msDesc( pszDesc ), mType( type ), mInvKind( kind )
, mbArray( false ), mbReadOnly( true )
{
	::memset(&mGuid, 0, sizeof(GUID));
}

AxPropertyDescriptor::AxPropertyDescriptor( VARDESC* pVarDesc, ITypeInfo* pTypeInfo )
: mDispId( 0 ), mType( 0 ), mbArray( false ), mbReadOnly( true ), mInvKind( (INVOKEKIND)0 )
{
	::memset(&mGuid, 0, sizeof(GUID));
	VARTYPE vtVar = pVarDesc->elemdescVar.tdesc.vt;
	VARTYPE vtProp = ((vtVar == VT_PTR)? pVarDesc->elemdescVar.tdesc.lptdesc->vt : vtVar);
	MEMBERID memid = pVarDesc->memid;

	CComBSTR bstrName;
	CComBSTR bstrDesc;
	CString sPropName = bstrName;
	msDesc = bstrDesc;
	HRESULT hr = pTypeInfo->GetDocumentation( memid, &bstrName, &bstrDesc, NULL, NULL );
	assert( SUCCEEDED(hr) );
	if( SUCCEEDED(hr) )
	{
		if( sPropName == _T("FontSize") )
		{
			if( msDesc.IsEmpty() )
				msDesc = _T("Indicates the size of the font to be used. ");
			msDesc += _T("\\par \\par \\b1Note: \\b0 To properly calculate the font size, multiply your new font size by 10000.");
		}
		mDispId = memid;
		mType = vtProp;
		mbReadOnly = false;
		mInvKind = INVOKE_FUNC; //just putting something in, not sure what it should be or if it matters [ORW]
		msName = sPropName; //set name last and only if no errors, then it can be used to check for constructor errors
	}
}

AxPropertyDescriptor::AxPropertyDescriptor( FUNCDESC* pFuncDesc, ITypeInfo* pTypeInfo,
																						LPOLEOBJECT pIObject /*= NULL*/ )
: mDispId( 0 ), mType( 0 ), mbArray( false ), mbReadOnly( true ), mInvKind( (INVOKEKIND)0 )
{
	::memset(&mGuid, 0, sizeof(GUID));
	VARTYPE vtVar = pFuncDesc->elemdescFunc.tdesc.vt;
	VARTYPE vtProp = ((vtVar == VT_PTR)? pFuncDesc->elemdescFunc.tdesc.lptdesc->vt : vtVar);
	SHORT ctParams = pFuncDesc->cParams;
	//[DPR] Removing check on number of parameters for property puts, because they can
	//have more than one (at least, the ComponentOne FlexGrid has puts with more than one value)
	//Also removing the check for gets, because they can have parameters
	//assert( (pFuncDesc->invkind == DISPATCH_PROPERTYGET && pFuncDesc->cParams == 0) || pFuncDesc->cParams == 1 );
	MEMBERID memid = pFuncDesc->memid;

	CComBSTR bstrName;
	CComBSTR bstrDesc;
	HRESULT hr = pTypeInfo->GetDocumentation( memid, &bstrName, &bstrDesc, NULL, NULL );
	assert( SUCCEEDED(hr) );
	if( SUCCEEDED(hr) )
	{
		msDesc = bstrDesc;
		mDispId = memid;
		mInvKind = pFuncDesc->invkind;

		mType = vtProp;
		mbReadOnly = ( (pFuncDesc->wFuncFlags & FUNCFLAG_FNONBROWSABLE) ||
									 (pFuncDesc->invkind != INVOKE_PROPERTYPUT && pFuncDesc->invkind != INVOKE_PROPERTYPUTREF) );
		mbArray = true; //not really, but this was being set in the original code [ORW]

		if( pIObject )
			PerPropertyBrowsing( pIObject );

		if( mType == VT_USERDEFINED )
		{
			HREFTYPE href = (vtVar == VT_PTR)? pFuncDesc->elemdescFunc.tdesc.lptdesc->hreftype : pFuncDesc->elemdescFunc.tdesc.hreftype;
			GetRefGuid( pTypeInfo, href );
			GetNameOfRefType( pTypeInfo, href, msTypeName );
		}

		mrArgs.resize( ctParams );
		BSTR* rbstrNames = new BSTR[ctParams + 1];
		ZeroMemory( rbstrNames, sizeof(BSTR) * (ctParams + 1) );
		UINT ctNames;
		hr = pTypeInfo->GetNames( memid, rbstrNames, ctParams + 1, &ctNames );
		assert( SUCCEEDED(hr) );
		if( SUCCEEDED(hr) )
		{
			// loop through the parameters
			for ( UINT n = 0 ; n < ctNames; n++ )
			{
				if( n < (UINT)ctParams )
				{
					AxArg& arg = mrArgs[n];
					const ELEMDESC &e = pFuncDesc->lprgelemdescParam[n];
					VARTYPE vt = e.tdesc.vt;
					bool bPtr = (vt == VT_PTR);
					if( bPtr )
						vt = (VT_BYREF | e.tdesc.lptdesc->vt);
					if (vt == VT_USERDEFINED) 
						SetRefType( vt, pTypeInfo,
												(bPtr? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype), 
												arg.clsid );
					arg.vt = vt;
					arg.optional = ((e.paramdesc.wParamFlags & PARAMFLAG_FOPT) != 0);
					if( !!rbstrNames[n] && n < ctNames - 1 )	
						arg.name = (LPCTSTR)bstr_t( rbstrNames[n + 1] );
				}
			}
			for( SHORT idx = ctParams; idx >= 0; --idx )
				SysFreeString( rbstrNames[idx] );
			msName = bstrName; //set name last and only if no errors, then it can be used to check for constructor errors
		}
		delete [] rbstrNames;
	}
}

AxPropertyDescriptor::AxPropertyDescriptor( const AxPropertyDescriptor& Src )
: mDispId( Src.mDispId ), mType( Src.mType ), mbArray( Src.mbArray ), mbReadOnly( Src.mbReadOnly )
, mInvKind( Src.mInvKind ), msDesc( Src.msDesc ), mGuid( Src.mGuid )
, mrArgs( Src.mrArgs ), mrEnum( Src.mrEnum )
{
}

AxPropertyDescriptor::~AxPropertyDescriptor(void) 
{
}

CString AxPropertyDescriptor::GetTypeDisplayName() const
{
	if( !msTypeName.IsEmpty() )
		return msTypeName;
	CString sName;
	if( mGuid != GUID_NULL )
		sName = GetAxTypeName( mGuid );
	if( sName.IsEmpty() )
		sName = AxTypeToDisplayableLispType( mType, mGuid );
	return sName;
}

CString AxPropertyDescriptor::GetEnumDisplayName( size_t idxEnum ) const
{
	if( idxEnum >= mrEnum.size() )
		return _T("");
	return mrEnum.at( idxEnum ).Name;
}

CString AxPropertyDescriptor::GetArgDisplayName( size_t idxArg ) const
{
	if( idxArg >= mrArgs.size() )
		return _T("");
	const AxArg& arg = mrArgs.at( idxArg );
	CString sName;
	if( arg.clsid != GUID_NULL )
		sName = GetAxTypeName( arg.clsid );
	if( sName.IsEmpty() )
		sName = AxTypeToDisplayableLispType( arg.vt, arg.clsid );
	return sName;
}

HRESULT AxPropertyDescriptor::Get( IDispatch* pObjectDisp, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult ) const
{
	DISPPARAMS dpParams;
	dpParams.rgvarg = rvarArgs;
	dpParams.cArgs = ctArgs;
	dpParams.rgdispidNamedArgs = NULL;
	dpParams.cNamedArgs = 0;

	EXCEPINFO excepInfo = {0};
	UINT iArgErr = 0;
  return pObjectDisp->Invoke( mDispId, IID_NULL, GetUserDefaultLCID(), WORD( DISPATCH_PROPERTYGET ),
															&dpParams, &varResult, &excepInfo, &iArgErr );
}

HRESULT AxPropertyDescriptor::Set( IDispatch* pObjectDisp, const VARIANTARG* rvarArgs, UINT ctArgs ) const
{
	if( ctArgs == 0 )
		return E_INVALIDARG;
	size_t ctArgTypes = mrArgs.size();
	COleVariant* rArgs = new COleVariant[ctArgs];
	for( UINT idxArg = 0; idxArg < ctArgs; ++idxArg )
		rArgs[idxArg] = rvarArgs[idxArg];
	if( ctArgTypes == 0 && ctArgs > 0 )
	{
		COleVariant& arg = rArgs[0];
		if( mType == VT_BOOL && rvarArgs[0].vt == VT_BSTR )
		{
			arg.vt = VT_BOOL;
			arg.boolVal = (lstrcmpi( bstr_t( rvarArgs[0].bstrVal ), _T("True") ) == 0)? VARIANT_TRUE : VARIANT_FALSE;
		}
		else
			VariantChangeType( &arg, &arg, 0, mType );
	}
	else
	{
		for( size_t idxArg = 0; idxArg < ctArgs && idxArg < ctArgTypes; ++idxArg )
		{
			size_t idxRvar = ctArgs - idxArg - 1;
			COleVariant& arg = rArgs[idxRvar];
			if( mrArgs[idxArg].vt == VT_BOOL && arg.vt == VT_BSTR )
			{
				arg.vt = VT_BOOL;
				arg.boolVal = (lstrcmpi( bstr_t( rvarArgs[idxRvar].bstrVal ), _T("True") ) == 0)? VARIANT_TRUE : VARIANT_FALSE;
			}
			else
				VariantChangeType( &arg, &arg, 0, mrArgs[idxArg].vt );
		}
	}

	WORD wFlags = mInvKind;//INVOKE_PROPERTYPUT;
	if( wFlags != INVOKE_PROPERTYPUT && wFlags != INVOKE_PROPERTYPUTREF )
		wFlags = INVOKE_PROPERTYPUT;
   
	DISPID dispidPropset = DISPID_PROPERTYPUT;
	DISPPARAMS dpParams;
	dpParams.cArgs = ctArgs;
	dpParams.rgvarg = rArgs;
	dpParams.rgdispidNamedArgs = &dispidPropset;
	dpParams.cNamedArgs = 1;

	COleVariant varResult;
	HRESULT hr = pObjectDisp->Invoke( mDispId, GUID_NULL, LOCALE_USER_DEFAULT, 
																		wFlags, &dpParams, &varResult, NULL, NULL );
	delete[] rArgs;
	return hr;
}


HRESULT AxPropertyDescriptor::PerPropertyBrowsing( LPOLEOBJECT pIObject )
{
	if (pIObject == NULL)
		return E_POINTER;

	if( (mType != VT_I2) &&
			(mType != VT_UI2) &&
			(mType != VT_I4) &&
			(mType != VT_UI4) )
		return E_UNEXPECTED; // wrong type

	CComPtr< IPerPropertyBrowsing > pPpb;
	HRESULT hr = pIObject->QueryInterface( IID_IPerPropertyBrowsing, (void **)&pPpb );
	if (FAILED(hr))
		return hr;

	CALPOLESTR pStr;
	CADWORD cadword;
	hr = pPpb->GetPredefinedStrings( GetDispId(), &pStr, &cadword );
	if (FAILED(hr) || hr == S_FALSE)
		return hr;
	if ((pStr.cElems == 1) || (pStr.cElems != cadword.cElems))
	{
		hr = E_UNEXPECTED; // unexpected state
		goto Cleanup;
	}

	mrEnum.resize( pStr.cElems );
	for (ULONG i = 0; i < pStr.cElems; i++)
	{
		VARIANT vt;
		hr = pPpb->GetPredefinedValue( GetDispId(), cadword.pElems[i], &vt ); 
		ASSERT(SUCCEEDED(hr));
		
		mrEnum[i].Name = pStr.pElems[i]; //buf;
		mrEnum[i].Var = vt;
	}

Cleanup:
	CoTaskMemFree((void *)cadword.pElems);
	for (ULONG i=0; i < (int) pStr.cElems; i++) 
		CoTaskMemFree((void *)pStr.pElems[i]);  
	CoTaskMemFree((void *)pStr.pElems); 

	return hr;
}


HRESULT AxPropertyDescriptor::GetRefGuid( ITypeInfo* TheInfo, HREFTYPE hreftype )
{
	ITypeInfo *TheRefType = NULL;
	if ((hreftype != -1) &&
		SUCCEEDED(TheInfo->GetRefTypeInfo(hreftype, &TheRefType)))
	{
		LPTYPEATTR pTA;
		if (SUCCEEDED(TheRefType->GetTypeAttr(&pTA)))
		{
			mGuid = pTA->guid;
			switch (pTA->typekind)
			{
			case TKIND_ALIAS: // color, etc.
				if ((pTA->tdescAlias.vt == VT_USERDEFINED) &&
					(hreftype < VT_USERDEFINED))
					mType = (VARTYPE)hreftype;
				else
					mType = pTA->tdescAlias.vt;

				if (mType == VT_USERDEFINED)
					GetRefGuid( TheRefType, ((pTA->tdescAlias.vt == VT_PTR) ? pTA->tdescAlias.lptdesc->hreftype : pTA->tdescAlias.hreftype) );
				break;
			case TKIND_DISPATCH: // font, etc.
				mType = VT_DISPATCH;
				mGuid = pTA->guid;
				break;
			case TKIND_ENUM:
				{
					VARDESC *pVarDesc;
					mrEnum.resize( pTA->cVars );
					for (int i = 0; i < pTA->cVars; i++)
					{ 
						if(SUCCEEDED(TheRefType->GetVarDesc( i, &pVarDesc )))
						{
	   					CComBSTR  bstrName;
							if (SUCCEEDED(TheRefType->GetDocumentation(pVarDesc->memid, &bstrName, NULL,NULL,NULL ))) 
							{   
								if (i == 0)
									mType = pVarDesc->lpvarValue->vt;
								mrEnum[i].Name = bstrName;
								mrEnum[i].Var = *pVarDesc->lpvarValue;
							}
							TheRefType->ReleaseVarDesc(pVarDesc);
						}
					}
				}
			}
		}
		TheRefType->Release();
	}
	return S_OK;
}


void AxPropertyDescriptor::Serialize( CArchive& ar, BYTE nPropertyVersion )
{
	BYTE nThisVersion = GetCurrentSaveVersion();
	
	if (ar.IsStoring())
	{
		ar << nThisVersion;
		ar << mDispId;
		ar << msName;
		ar << msDesc;
		ar << mType;
		ar << bool(mbArray);
		ar << bool(mbReadOnly);
		SerializeCLSID(ar, mGuid);
		ar << unsigned short(mrEnum.size());
		ar << unsigned short(mrArgs.size());
		ar << int(mInvKind);
		for (size_t i = 0; i < mrEnum.size(); ++i)
		{
			ar << mrEnum[i].Name;
			ar << COleVariant( mrEnum[i].Var );
		}
		for (size_t i = 0; i < mrArgs.size(); ++i)
		{
			//embed the new "optional" member in the VARTYPE in order to preserve file format
			VARTYPE vt = mrArgs[i].vt;
			if( mrArgs[i].optional )
				vt |= VT_RESERVED;
			ar << vt;
			ar << mrArgs[i].name;
			SerializeCLSID(ar, mrArgs[i].clsid);
		}
	}
	else
	{
		if (nPropertyVersion <= 7)
			nThisVersion = 1;
		else
			ar >> nThisVersion;
		ar >> mDispId;
		ar >> msName;
		ar >> msDesc;
		ar >> mType;
		if( nPropertyVersion <= 7)
		{
			BOOL bArray;
			ar >> bArray;
			mbArray = (bArray != FALSE);
		}
		else
			ar >> mbArray;
		if( nPropertyVersion <= 7)
		{
			BOOL bNotReadOnly;
			ar >> bNotReadOnly;
			mbReadOnly = (bNotReadOnly == FALSE);
		}
		else
			ar >> mbReadOnly;
		SerializeCLSID(ar, mGuid);
		unsigned short ctEnum;
		if( nPropertyVersion <= 7)
		{
			unsigned long lEnum;
			ar >> lEnum;
			ctEnum = unsigned short(lEnum);
		}
		else
			ar >> ctEnum;
		assert( ctEnum < 0x7FFFFFFF ); //found one .odc file where this value was -2! --ORW [2007-11-13]
		mrEnum.resize( ctEnum );
		unsigned short ctParams;
		if( nPropertyVersion <= 7)
		{
			unsigned long lParams;
			ar >> lParams;
			ctParams = unsigned short(lParams);
		}
		else
			ar >> ctParams;
		mrArgs.resize( ctParams );
		int iKind;
		ar >> iKind;
		mInvKind = (INVOKEKIND)iKind;
		for (size_t i = 0; i < ctEnum; ++i)
		{
			ar >> mrEnum[i].Name;
			COleVariant var;
			ar >> var; 
      mrEnum[i].Var = var;
		}
		for (size_t i = 0; i < ctParams; ++i)
		{
			VARTYPE vt;
			ar >> vt;
			mrArgs[i].optional = ((vt & VT_RESERVED) != 0);
			mrArgs[i].vt = (vt & VT_ILLEGALMASKED);
			ar >> mrArgs[i].name;
			if (nPropertyVersion >= 4)
				SerializeCLSID(ar, mrArgs[i].clsid);
		}
	}

}

IOStatus AxPropertyDescriptor::ReadFromTextFile( std::ifstream &sFile, BYTE nPropertyVersion )
{
  if (!readDISPIDAsLong(sFile, mDispId)) return statInvalidFormat;
	CStringA sName;
  if (!readString(sFile, sName)) return statInvalidFormat;
	msName = sName;
	CStringA sDesc;
  if (!readString(sFile, sDesc)) return statInvalidFormat;
	msDesc = sDesc;

  if (!readVARTYPE(sFile, mType)) return statInvalidFormat;
	BOOL bArray;
  if (!readBOOL(sFile, bArray)) return statInvalidFormat;
	mbArray = (bArray != FALSE);
	BOOL bNotReadOnly;
  if (!readBOOL(sFile, bNotReadOnly)) return statInvalidFormat;
	mbReadOnly  = (bNotReadOnly == FALSE);

  if (!readCLSID(sFile, mGuid)) return statInvalidFormat;
	int ctEnum;
  if (!readInt(sFile, ctEnum)) return statInvalidFormat;
	mrEnum.resize( ctEnum );
	int ctParams;
  if (!readInt(sFile, ctParams)) return statInvalidFormat;
	mrArgs.resize( ctParams );
  int iKind;
  if (!readInt(sFile, iKind)) return statInvalidFormat;
	mInvKind = (INVOKEKIND)iKind;
  for (size_t i = 0; i < (size_t)ctEnum; ++i)
  {
		CStringA sName;
    if (!readString(sFile, sName)) return statInvalidFormat;
		mrEnum[i].Name = sName;
    COleVariant var;
    if (!readOleVariant(sFile, var)) return statInvalidFormat;
    mrEnum[i].Var = var;
  }
  for (int i = 0; i < ctParams; ++i)
  {
    if (!readVARTYPE(sFile, mrArgs[i].vt)) return statInvalidFormat;
		CStringA sName;
    if (!readString(sFile, sName)) return statInvalidFormat;
		mrArgs[i].name = sName;
		if (!readCLSID(sFile, mrArgs[i].clsid)) return statInvalidFormat;
  }

  return statOK;
}

//IOStatus AxPropertyDescriptor::WriteToTextFile( FILE* pFile ) const
//{
//  writeDISPID(pFile, mDispId);
//  writeString(pFile, msName);
//  writeString(pFile, msDesc);
//  writeVARTYPE(pFile, mType);
//  writeBOOL(pFile, BOOL(mbArray));
//  writeBOOL(pFile, BOOL(!mbReadOnly));
//  writeCLSID(pFile, mGuid);
//  writeInt(pFile, mrEnum.size());
//  writeInt(pFile, mrArgs.size());
//  writeInt(pFile, int(mInvKind));
//  for (size_t i = 0; i < mrEnum.size(); ++i)
//  {
//    writeString(pFile, mrEnum[i].Name);
//    writeOleVariant(pFile, COleVariant(mrEnum[i].Var));
//  }
//  for (size_t i = 0; i < mrArgs.size(); ++i)
//  {
//    writeVARTYPE(pFile, mrArgs[i].vt);
//		writeString(pFile, mrArgs[i].name);
//		writeCLSID(pFile, mrArgs[i].clsid);
//  }
//	return statOK;
//}


#ifdef _DIAGNOSTIC
LPCTSTR AxPropertyDescriptor::toString() const
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("{DISPID: %s, \"%s\", \"%s\"}"),
							asString( mDispId ), (LPCTSTR)msName, (LPCTSTR)msDesc );
	return buf;
}
#endif
