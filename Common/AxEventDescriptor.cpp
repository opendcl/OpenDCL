// AxEventDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxEventDescriptor.h"
#include "VarUtils.h"
#include "Filing.h"
#include "Project.h"


AxEventDescriptor::AxEventDescriptor(void)
: mDispId( 0 )
{
}

AxEventDescriptor::~AxEventDescriptor(void)
{
}

AxEventDescriptor::AxEventDescriptor( FUNCDESC* pFuncDesc, ITypeInfo* pTypeInfo, bool bUseAsType /*= true*/ )
: mDispId( 0 )
{
	SHORT ctParams = pFuncDesc->cParams;
	assert( pFuncDesc->invkind == INVOKE_FUNC );
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
					VARTYPE vt = e.tdesc.vt;
					bool bPtr = (vt == VT_PTR);
					if( bPtr )
						vt = (VT_BYREF | e.tdesc.lptdesc->vt);
					if (vt == VT_USERDEFINED) 
						SetRefType( vt, pTypeInfo,
												(bPtr? e.tdesc.lptdesc->hreftype : e.tdesc.hreftype), 
												mrArgs[n].clsid );
					mrArgs[n].vt = vt;
					if( !!rbstrNames[n] && n < ctNames - 1 )	
						mrArgs[n].name = (LPCTSTR)bstr_t( rbstrNames[n + 1] );
				}
			}
			for( SHORT idx = ctParams; idx >= 0; --idx )
				SysFreeString( rbstrNames[idx] );
			msName = bstrName; //set name last and only if no errors, then it can be used to check for constructor errors
		}
		delete [] rbstrNames;
	}
}


void AxEventDescriptor::Serialize( CArchive& ar, BYTE nPropertyVersion )
{
	BYTE nThisVersion = GetCurrentSaveVersion();
	
	if (ar.IsStoring())
	{
		ar << nThisVersion;
		ar << mDispId;
		ar << msName;
		ar << msDesc;
		ar << unsigned short(mrArgs.size());
		for (size_t i = 0; i < mrArgs.size(); ++i)
		{
			ar << mrArgs[i].vt;
			ar << mrArgs[i].name;
			SerializeCLSID(ar, mrArgs[i].clsid);
		}
	}
	else
	{
		if( nPropertyVersion <= 7 )
			nThisVersion = 1;
		else
			ar >> nThisVersion;
		ar >> mDispId;
		ar >> msName;
		ar >> msDesc;
		unsigned short ctParams;
		if (nPropertyVersion <= 7)
		{
			unsigned long lParams;
			ar >> lParams;
			ctParams = (unsigned short)lParams;
		}
		else
			ar >> ctParams;
		mrArgs.resize( ctParams );
		for( size_t i = 0; i < ctParams; ++i )
		{
			ar >> mrArgs[i].vt;
			ar >> mrArgs[i].name;
			if (nPropertyVersion >= 4)
				SerializeCLSID(ar, mrArgs[i].clsid);
		}
		if( nPropertyVersion <= 5 && ctParams < 16 )
		{ //original code was hardcoded to always write 16! [ORW]
			size_t ctDiscard = 16 - ctParams;
			for( size_t i = 0; i < ctDiscard; ++i )
			{
				AxArg discard;
				ar >> discard.vt;
				ar >> discard.name;
				if (nPropertyVersion >= 4)
					SerializeCLSID(ar, discard.clsid);
			}
		}
		if( nThisVersion <= 1 )
		{
			CString sUnusedParams;
			ar >> sUnusedParams;
		}
	}
}

IOStatus AxEventDescriptor::ReadFromTextFile( std::ifstream &sFile, BYTE nPropertyVersion )
{
  if (!readDISPID(sFile, mDispId)) return statInvalidFormat;
	CStringA sName;
  if (!readString(sFile, sName)) return statInvalidFormat;
	msName = sName;
	CStringA sDesc;
  if (!readString(sFile, sDesc)) return statInvalidFormat;
	msDesc = sDesc;

	int ctParams;
  if (!readInt(sFile, ctParams)) return statInvalidFormat;
	mrArgs.resize( ctParams );
	//16 arguments were written out, even if none were used.
	//Read them all in, putting the ones that exist in the argument array,
	//and ignore the others
  for (int i = 0; i < 16; ++i)
  {
		if (i < ctParams) {
			//Argument should be read into array
			if (!readVARTYPE(sFile, mrArgs[i].vt)) return statInvalidFormat;
			CStringA sName;
			if (!readString(sFile, sName)) return statInvalidFormat;
			mrArgs[i].name = sName;
			if (!readCLSID(sFile, mrArgs[i].clsid)) return statInvalidFormat;
		} else {
			//Argument should be ignored
			AxArg tempArg;
			if (!readVARTYPE(sFile, tempArg.vt)) return statInvalidFormat;
			CStringA sName;
			if (!readString(sFile, sName)) return statInvalidFormat;
			tempArg.name = sName;
			if (!readCLSID(sFile, tempArg.clsid)) return statInvalidFormat;
		}
	}
	CStringA sUnusedParams;
  if (!readString(sFile, sUnusedParams)) return statInvalidFormat;

	return statOK;
}

//IOStatus AxEventDescriptor::WriteToTextFile( FILE* pFile ) const
//{
//	writeDISPID(pFile, mDispId);
//  writeString(pFile, msName);
//  writeInt(pFile, mrArgs.size());
//  for (size_t i = 0; i < mrArgs.size(); ++i)
//  {
//    writeVARTYPE(pFile, mrArgs[i].vt);
//		writeString(pFile, mrArgs[i].name);
//		writeCLSID(pFile, mrArgs[i].clsid);
//  }
//  writeString(pFile, msParams);
//	return statOK;
//}


#ifdef _DIAGNOSTIC
LPCTSTR AxEventDescriptor::toString() const
{
	static TCHAR buf[1024];
	_sntprintf( buf, _elements(buf), _T("{DISPID: %s, \"%s\", \"%s\"}"),
							asString( mDispId ), (LPCTSTR)msName, (LPCTSTR)msDesc );
	return buf;
}
#endif
