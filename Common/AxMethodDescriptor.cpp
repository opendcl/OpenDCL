// AxMethodDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxMethodDescriptor.h"
#include "VarUtils.h"
#include "Project.h"
#include "Filing.h"


AxMethodDescriptor::AxMethodDescriptor(void)
: mDispId( 0 ), mReturnType( 0 )
{
	::memset(&mReturnGuid, 0, sizeof(GUID));	
}

AxMethodDescriptor::AxMethodDescriptor( FUNCDESC* pFuncDesc, ITypeInfo* pTypeInfo, bool bUseAsType /*= true*/ )
: mDispId( 0 ), mReturnType( 0 )
{
	::memset(&mReturnGuid, 0, sizeof(GUID));	
	SHORT ctParams = pFuncDesc->cParams;
	assert( pFuncDesc->invkind == DISPATCH_METHOD && ((LONG)pFuncDesc->memid < (LONG)(1<<16)) );
	MEMBERID memid = pFuncDesc->memid;

	CComBSTR bstrName;
	CComBSTR bstrDesc;
	HRESULT hr = pTypeInfo->GetDocumentation( memid, &bstrName, &bstrDesc, NULL, NULL );
	assert( SUCCEEDED(hr) );
	if( SUCCEEDED(hr) )
	{
		msDesc = bstrDesc;
		mDispId = memid;

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
					const ELEMDESC &e = pFuncDesc->lprgelemdescParam[n];
					bool bNotByVal = (e.tdesc.vt == VT_PTR);
					VARTYPE vt = (bNotByVal? e.tdesc.lptdesc->vt : e.tdesc.vt);
					if (vt == VT_USERDEFINED) 
						SetRefType( vt, pTypeInfo,
												(bNotByVal? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype), 
												&mrArgs[n].clsid );
					mrArgs[n].vt = vt;
					if( !!rbstrNames[n] && n < ctNames - 1 )	
						mrArgs[n].name = (LPCTSTR)bstr_t( rbstrNames[n + 1] );
				}
			}
			for( SHORT idx = ctParams; idx >= 0; --idx )
				SysFreeString( rbstrNames[idx] );
			msParams = FuncDescToString( pTypeInfo, pFuncDesc, bUseAsType );

			// pick up return type
			const ELEMDESC& e = pFuncDesc->elemdescFunc; 
			bool bNotByVal = (e.tdesc.vt == VT_PTR);
			VARTYPE vt = ((bNotByVal) ? e.tdesc.lptdesc->vt : e.tdesc.vt);

			bool bReadOnly = false;
			if( pFuncDesc->wFuncFlags & (FUNCFLAG_FNONBROWSABLE|FUNCFLAG_FHIDDEN) )
				bReadOnly = true; 

			if( !bReadOnly || mDispId == -552 ) 
			{
				if( vt == VT_USERDEFINED ) 
					SetRefType( vt, pTypeInfo, (bNotByVal) ? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype, &mReturnGuid );						
			}
			mReturnType = vt;

			msName = bstrName; //set name last and only if no errors, then it can be used to check for constructor errors
			if( msName.Left( 1 ) == _T("_") )
				msName.Empty(); //signal failure
		}
		delete [] rbstrNames;
	}
}

AxMethodDescriptor::~AxMethodDescriptor(void)
{
}

HRESULT AxMethodDescriptor::Invoke( IDispatch* pObjectDisp, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult ) const
{
	DISPPARAMS dpParams;
	dpParams.rgvarg = rvarArgs;
	dpParams.cArgs = ctArgs;
	dpParams.rgdispidNamedArgs = NULL;
	dpParams.cNamedArgs = 0;

	EXCEPINFO excepInfo = {0};
	UINT iArgErr = 0;
  return pObjectDisp->Invoke( mDispId, IID_NULL, GetUserDefaultLCID(), WORD( INVOKE_PROPERTYGET ),
															&dpParams, &varResult, &excepInfo, &iArgErr );
}

void AxMethodDescriptor::Serialize(CArchive& ar, int nPropertyVersion)
{
	int nThisVersion = 2;
	
	if (ar.IsStoring())
	{
		ar << nThisVersion;
		ar << mDispId;		
		ar << msName;
		ar << msParams;
		ar << msDesc;
		ar << mReturnType;
		ar << mrArgs.size();		
		SerializeCLSID(ar, mReturnGuid);
		for (size_t i = 0; i < mrArgs.size(); ++i)
		{
			ar << mrArgs[i].vt;
			ar << mrArgs[i].name;
			SerializeCLSID(ar, mrArgs[i].clsid);
		}
	}
	else
	{
		ar >> nThisVersion;
		ar >> mDispId;
		ar >> msName;
		ar >> msParams;
		ar >> msDesc;
		ar >> mReturnType;
		size_t ctArgs;
		ar >> ctArgs;
		if (nThisVersion >= 2)
			SerializeCLSID(ar, mReturnGuid);
		mrArgs.resize( ctArgs );
		for (size_t i = 0; i < ctArgs; ++i)
		{
			ar >> mrArgs[i].vt;
			ar >> mrArgs[i].name;
			if (nThisVersion >= 2)
				SerializeCLSID(ar, mrArgs[i].clsid);
		}
	}
}

IOStatus AxMethodDescriptor::WriteToTextFile(FILE* pFile) const
{
  int nThisVersion = 2;

  writeInt(pFile, nThisVersion);
  writeDISPID(pFile, mDispId);		
  writeString(pFile, msName);
  writeString(pFile, msParams);
  writeString(pFile, msDesc);
  writeVARTYPE(pFile, mReturnType);
	int ctArgs = (int)mrArgs.size();
  writeInt(pFile, ctArgs);		
  writeCLSID(pFile, mReturnGuid);
  for (int i = 0; i < ctArgs; i++)
  {
		writeVARTYPE(pFile, mrArgs[i].vt);
		writeString(pFile, mrArgs[i].name);
    writeCLSID(pFile, mrArgs[i].clsid);
  }
	return statOK;
}

IOStatus AxMethodDescriptor::ReadFromTextFile(std::ifstream &sFile)
{
  int iVersion;
  if (!readInt(sFile, iVersion)) return statInvalidFormat;

  switch (iVersion) {
    case 2 : 
      return ReadFromTextFile2(sFile);
      break;
  }
  return statInvalidFormat;
}

IOStatus AxMethodDescriptor::ReadFromTextFile2(std::ifstream &sFile)
{
  if (!readDISPIDAsLong(sFile, mDispId)) return statInvalidFormat;
  if (!readString(sFile, msName)) return statInvalidFormat;
  if (!readString(sFile, msParams)) return statInvalidFormat;
  if (!readString(sFile, msDesc)) return statInvalidFormat;
  if (!readVARTYPE(sFile, mReturnType)) return statInvalidFormat;
	int ctArgs;
  if (!readInt(sFile, ctArgs)) return statInvalidFormat;
  if (!readCLSID(sFile, mReturnGuid)) return statInvalidFormat;
	mrArgs.resize(ctArgs);
  for (int i = 0; i < ctArgs; ++i)
  {
    if (!readVARTYPE(sFile, mrArgs[i].vt)) return statInvalidFormat;
    if (!readString(sFile, mrArgs[i].name)) return statInvalidFormat;
    if (!readCLSID(sFile, mrArgs[i].clsid)) return statInvalidFormat;
  }
  return statOK;
}


#ifdef _DIAGNOSTIC
LPCTSTR AxMethodDescriptor::toString() const
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("{DISPID: %s, \"%s\", \"%s\", \"%s\"}"),
							asString( mDispId ), (LPCTSTR)msName, (LPCTSTR)msDesc, (LPCTSTR)msParams );
	return buf;
}
#endif
