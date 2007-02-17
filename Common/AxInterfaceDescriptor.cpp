// AxInterfaceDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxInterfaceDescriptor.h"
#include "Filing.h"
#include "Project.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include "VarUtils.h"
#include "AxContainer.h"
#include "OleFont.h"

#define MAX_CALLING_ARGUMENTS 16


void AxInterfaceDescriptor::clear()
{
	mpProp = NULL;
	mpPropGet = NULL;
	mpPropPut = NULL;
	mpPropPutRef = NULL;
	mpEvent = NULL;
	mpMethods = NULL;
}

IOStatus AxInterfaceDescriptor::ReadPropFromTextFile5(std::ifstream &sFile, AxPropertyDescriptor& axProp)
{
  LONG Id;
  if (!readLong(sFile, Id)) return statInvalidFormat;
  axProp.Id = Id;
  if (!readString(sFile, axProp.Name)) return statInvalidFormat;
  if (!readString(sFile, axProp.DocumentationDesc)) return statInvalidFormat;

  if (!readVARTYPE(sFile, axProp.Type)) return statInvalidFormat;
  if (!readBOOL(sFile, axProp.IsArray)) return statInvalidFormat;
  if (!readBOOL(sFile, axProp.CanSet)) return statInvalidFormat;

  if (!readCLSID(sFile, axProp.Guid)) return statInvalidFormat;
	int ctEnum;
  if (!readInt(sFile, ctEnum)) return statInvalidFormat;
	axProp.rEnum.resize( ctEnum );
	int ctParams;
  if (!readInt(sFile, ctParams)) return statInvalidFormat;
	axProp.rArgs.resize( ctParams );
  int iKind;
  if (!readInt(sFile, iKind)) return statInvalidFormat;
  switch(iKind)
  {
  case INVOKE_FUNC:
    axProp.invKind = INVOKE_FUNC;
    break;
  case INVOKE_PROPERTYGET:
    axProp.invKind = INVOKE_PROPERTYGET;
    break;
  case INVOKE_PROPERTYPUT:
    axProp.invKind = INVOKE_PROPERTYPUT;
    break;
  case INVOKE_PROPERTYPUTREF:
    axProp.invKind = INVOKE_PROPERTYPUTREF;
    break;
  default:
    axProp.invKind = (INVOKEKIND)iKind;
    break;
  }
  for (size_t i=0; i<axProp.rEnum.size(); i++)
  {
    if (!readString(sFile, axProp.rEnum[i].Name)) return statInvalidFormat;
    COleVariant var;
    if (!readOleVariant(sFile, var)) return statInvalidFormat;
    axProp.rEnum[i].Var = var;
  }
  for (size_t i = 0; i<axProp.rArgs.size(); i++)
  {
    if (!readVARTYPE(sFile, axProp.rArgs[i].vt)) return statInvalidFormat;
    if (!readString(sFile, axProp.rArgs[i].name)) return statInvalidFormat;
		if (!readCLSID(sFile, axProp.rArgs[i].clsid)) return statInvalidFormat;
  }

  return statOK;
}


IOStatus AxInterfaceDescriptor::ReadFromTextFile5(std::ifstream &sFile) 
{
  BOOL bProp;
  BOOL bPropGet;
  BOOL bPropPut;
  BOOL bPropPutRef;
  BOOL bPropEvent;
  BOOL bPropMethod;

  if (!readBOOL(sFile, bProp)) return statInvalidFormat;
  if (bProp)
  {
		delete mpProp;
    mpProp = new AxPropertyDescriptor;
		IOStatus stat = ReadPropFromTextFile5(sFile, *mpProp);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropGet)) return statInvalidFormat;
  if (bPropGet)
  {
		delete mpPropGet;
    mpPropGet = new AxPropertyDescriptor;
		IOStatus stat = ReadPropFromTextFile5(sFile, *mpPropGet);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropPut)) return statInvalidFormat;
  if (bPropPut)
  {
		delete mpPropPut;
    mpPropPut = new AxPropertyDescriptor;
		IOStatus stat = ReadPropFromTextFile5(sFile, *mpPropPut);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropPutRef)) return statInvalidFormat;
  if (bPropPutRef)
  {
		delete mpPropPutRef;
    mpPropPutRef = new AxPropertyDescriptor;
		IOStatus stat = ReadPropFromTextFile5(sFile, *mpPropPutRef);
    if (stat != statOK) return stat;
  }

  if (!readBOOL(sFile, bPropEvent)) return statInvalidFormat;
  if (bPropEvent)
  {
		delete mpEvent;
		mpEvent = new AxEventDescriptor;
    if (!readDISPID(sFile, mpEvent->Id)) return statInvalidFormat;
    if (!readString(sFile, mpEvent->Name)) return statInvalidFormat;
    if (!readString(sFile, mpEvent->DocumentationDesc)) return statInvalidFormat;

    if (!readInt(sFile, mpEvent->nArgs)) return statInvalidFormat;
    for (int i=0; i<MAX_CALLING_ARGUMENTS; i++) {
      if (!readVARTYPE(sFile, mpEvent->CallingArgs[i])) return statInvalidFormat;
      CString temp;
      if (!readString(sFile, temp)) return statInvalidFormat;
      mpEvent->CallingArgNames[i] = temp;
      if (!readCLSID(sFile, mpEvent->CallingArgClsids[i])) return statInvalidFormat;
    }
    if (!readString(sFile, mpEvent->Params)) return statInvalidFormat;
  }
  if (!readBOOL(sFile, bPropMethod)) return statInvalidFormat;
  if (bPropMethod)
  {
		delete mpMethods;
		mpMethods = new std::vector< RefCountedPtr< AxMethodDescriptor > >;
    int nCount;

    if (!readInt(sFile, nCount)) return statInvalidFormat;

    // do loop to navigate objects
    while (nCount-- > 0)
    {
      // get current object
			std::auto_ptr< AxMethodDescriptor > pMethod( new AxMethodDescriptor );

      // get object from archive
			IOStatus stat = pMethod->ReadFromTextFile(sFile);
			if (stat != statOK) return stat;

      // add that ArxControlObject to the list object
			mpMethods->push_back(pMethod.release());		
    }
  }

  return statOK;
}


IOStatus AxInterfaceDescriptor::WritePropToTextFile(FILE* pFile, const AxPropertyDescriptor& axProp) const
{
  LONG Id = (LONG)axProp.Id;
  writeLong(pFile, Id);
  writeString(pFile, axProp.Name);
  writeString(pFile, axProp.DocumentationDesc);
  writeVARTYPE(pFile, axProp.Type);
  writeBOOL(pFile, axProp.IsArray);
  writeBOOL(pFile, axProp.CanSet);
  writeCLSID(pFile, axProp.Guid);
  writeInt(pFile, axProp.rEnum.size());
  writeInt(pFile, axProp.rArgs.size());
  int iKind = axProp.invKind;
  writeInt(pFile, iKind);
  for (size_t i=0; i<axProp.rEnum.size(); i++)
  {
    writeString(pFile, axProp.rEnum[i].Name);
    COleVariant var = axProp.rEnum[i].Var;
    writeOleVariant(pFile, var);
  }
  for (size_t i = 0; i<axProp.rArgs.size(); i++)
  {
    writeVARTYPE(pFile, axProp.rArgs[i].vt);
		writeString(pFile, axProp.rArgs[i].name);
		writeCLSID(pFile, axProp.rArgs[i].clsid);
  }
	return statOK;
}


IOStatus AxInterfaceDescriptor::WriteToTextFile(FILE* pFile) const
{
  BOOL bProp;
  BOOL bPropGet;
  BOOL bPropPut;
  BOOL bPropPutRef;
  BOOL bPropEvent;
  BOOL bPropMethod;

  bProp = (mpProp != NULL);
  writeBOOL(pFile, bProp);
  if (bProp)
    WritePropToTextFile(pFile, *mpProp);

  bPropGet = (mpPropGet != NULL);
  writeBOOL(pFile, bPropGet);
  if (bPropGet)
    WritePropToTextFile(pFile, *mpPropGet);

  bPropPut = (mpPropPut != NULL);
  writeBOOL(pFile, bPropPut);
  if (bPropPut)
    WritePropToTextFile(pFile, *mpPropPut);

  bPropPutRef = (mpPropPutRef != NULL);
  writeBOOL(pFile, bPropPutRef);
  if (bPropPutRef)
    WritePropToTextFile(pFile, *mpPropPutRef);

  bPropEvent = (mpEvent != NULL);
  writeBOOL(pFile, bPropEvent);

  if (mpEvent)
  {
    writeDISPID(pFile, mpEvent->Id);
    writeString(pFile, mpEvent->Name);
    writeString(pFile, mpEvent->DocumentationDesc);
    writeInt(pFile, mpEvent->nArgs);
    for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
    {
      writeVARTYPE(pFile, mpEvent->CallingArgs[i]);
      writeString(pFile, mpEvent->CallingArgNames[i]);
      writeCLSID(pFile, mpEvent->CallingArgClsids[i]);
  }
    writeString(pFile, mpEvent->Params);
  }
  bPropMethod = (mpMethods != NULL);
  writeBOOL(pFile, bPropMethod);
  if (mpMethods)
  {
    int nCount = (int)mpMethods->size(); 
    writeInt(pFile, nCount);
		for(size_t idx = 0; idx < (size_t)nCount; ++idx)
      mpMethods->at(idx)->WriteToTextFile(pFile);
  }
	return statOK;
}


void AxInterfaceDescriptor::SerializeProp(CArchive& ar, AxPropertyDescriptor& axProp, int nPropertyVersion)
{
	if (ar.IsStoring())
	{
		LONG Id = (LONG)axProp.Id;
		ar << Id;
		ar << axProp.Name;
		ar << axProp.DocumentationDesc;
		ar << axProp.Type;
		ar << axProp.IsArray;
		ar << axProp.CanSet;
		SerializeCLSID(ar, axProp.Guid);
		ar << axProp.rEnum.size();
		ar << axProp.rArgs.size();
		int iKind = axProp.invKind;
		ar << iKind;
		for (size_t i=0; i<axProp.rEnum.size(); i++)
		{
			ar << axProp.rEnum[i].Name;
			COleVariant var = axProp.rEnum[i].Var;
			ar << var;
		}
		for (size_t i = 0; i<axProp.rArgs.size(); i++)
		{
			ar << axProp.rArgs[i].vt;
			ar << axProp.rArgs[i].name;
			SerializeCLSID(ar, axProp.rArgs[i].clsid);
		}
	}
	else
	{
		LONG Id;
		ar >> Id; axProp.Id = Id;
		ar >> axProp.Name;
		ar >> axProp.DocumentationDesc;


		ar >> axProp.Type; 
		ar >> axProp.IsArray;
		ar >> axProp.CanSet;
		
		SerializeCLSID(ar, axProp.Guid);
		size_t ctEnum;
		ar >> ctEnum;
		axProp.rEnum.resize( ctEnum );
		size_t ctParams;
		ar >> ctParams;
		axProp.rArgs.resize( ctParams );
		int iKind;
		ar >> iKind;
		switch(iKind)
		{
			case INVOKE_FUNC:
				axProp.invKind = INVOKE_FUNC;
				break;
			case INVOKE_PROPERTYGET:
				axProp.invKind = INVOKE_PROPERTYGET;
				break;
			case INVOKE_PROPERTYPUT:
				axProp.invKind = INVOKE_PROPERTYPUT;
				break;
			case INVOKE_PROPERTYPUTREF:
				axProp.invKind = INVOKE_PROPERTYPUTREF;
				break;
			default:
				axProp.invKind = (INVOKEKIND)iKind;
				break;
		}
		for (size_t i=0; i<ctEnum; i++)
		{
			ar >> axProp.rEnum[i].Name;
			COleVariant var;
			ar >> var; 
      axProp.rEnum[i].Var = var;
		}
		for (size_t i = 0; i<ctParams; i++)
		{
			ar >> axProp.rArgs[i].vt;
			ar >> axProp.rArgs[i].name;
			if (nPropertyVersion >= 4)
				SerializeCLSID(ar, axProp.rArgs[i].clsid);
		}
	}

}


void AxInterfaceDescriptor::Serialize(CArchive& ar, int nPropertyVersion)
{
	BOOL bProp;
	BOOL bPropGet;
	BOOL bPropPut;
	BOOL bPropPutRef;
	BOOL bPropEvent;
	BOOL bPropMethod;

	if (ar.IsStoring())
	{
		bProp = (mpProp != NULL);
		ar << bProp;
		if (bProp)
			SerializeProp(ar, *mpProp, nPropertyVersion);

		bPropGet = (mpPropGet != NULL);
		ar << bPropGet;
		if (bPropGet)
			SerializeProp(ar, *mpPropGet, nPropertyVersion);

		bPropPut = (mpPropPut != NULL);
		ar << bPropPut;
		if (bPropPut)
			SerializeProp(ar, *mpPropPut, nPropertyVersion);

		bPropPutRef = (mpPropPutRef != NULL);
		ar << bPropPutRef;
		if (bPropPutRef)
			SerializeProp(ar, *mpPropPutRef, nPropertyVersion);

		bPropEvent = (mpEvent != NULL);
		ar << bPropEvent;

		if (mpEvent != NULL)
		{
			ar << mpEvent->Id;
			ar << mpEvent->Name;
			ar << mpEvent->DocumentationDesc;
			ar << mpEvent->nArgs;
			for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
			{
				 ar << mpEvent->CallingArgs[i];
				 ar << mpEvent->CallingArgNames[i];
				 SerializeCLSID(ar, mpEvent->CallingArgClsids[i]);
			}				
			ar << mpEvent->Params;
		}
		bPropMethod = (mpMethods != NULL);
		ar << bPropMethod;
		if (mpMethods != NULL)
		{
			short nSize = mpMethods->size();
			ar << short(nSize);
			for (int idx = 0; idx < nSize; ++idx)
				mpMethods->at(idx)->Serialize(ar);
		}
	}
	else
	{
		ar >> bProp;
		if (bProp)
		{
			delete mpProp;
			mpProp = new AxPropertyDescriptor;
			SerializeProp(ar, *(AxPropertyDescriptor*)mpProp, nPropertyVersion);
		}

		ar >> bPropGet;
		if (bPropGet)
		{
			delete mpPropGet;
			mpPropGet = new AxPropertyDescriptor;
			SerializeProp(ar, *(AxPropertyDescriptor*)mpPropGet, nPropertyVersion);
		}

		ar >> bPropPut;
		if (bPropPut)
		{
			delete mpPropPut;
			mpPropPut = new AxPropertyDescriptor;
			SerializeProp(ar, *(AxPropertyDescriptor*)mpPropPut, nPropertyVersion);
		}

		ar >> bPropPutRef;
		if (bPropPutRef)
		{
			delete mpPropPutRef;
			mpPropPutRef = new AxPropertyDescriptor;
			SerializeProp(ar, *(AxPropertyDescriptor*)mpPropPutRef, nPropertyVersion);
		}

		ar >> bPropEvent;
		if (bPropEvent)
		{
			delete mpEvent;
			mpEvent = new AxEventDescriptor;
			ar >> mpEvent->Id;
			ar >> mpEvent->Name;
			ar >> mpEvent->DocumentationDesc;


			ar >> mpEvent->nArgs;
			for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
			{
				 ar >> mpEvent->CallingArgs[i];
				 ar >> mpEvent->CallingArgNames[i];


				 if (nPropertyVersion >= 4)
					SerializeCLSID(ar, mpEvent->CallingArgClsids[i]);
		}
			ar >> mpEvent->Params;
		}
		ar >> bPropMethod;
		if (bPropMethod)
		{
			delete mpMethods;
			mpMethods = new std::vector< RefCountedPtr< AxMethodDescriptor > >;
			short nCount;
			ar >> nCount;		
			while (nCount-- > 0)
			{
				AxMethodDescriptor* pMethod = new AxMethodDescriptor;
				pMethod->Serialize(ar);
				mpMethods->push_back(pMethod);		
			}
		}
	}
}



CString AxInterfaceDescriptor::GetActiveXEnumDesc(CString sValue)
{
	AxPropertyDescriptor *pPropDesc;
	bool bFoundEnum = false;
	int nThisValue;

	if (!sValue.IsEmpty())
	{
		nThisValue = _tstol(sValue);
	}
	
	if (mpPropGet != NULL)
	{
		if (!mpPropGet->rEnum.empty())
		{
			bFoundEnum = true;
			pPropDesc = mpPropGet;
		}			
	}
	else if (mpProp != NULL)
	{
		if (!mpProp->rEnum.empty())
		{
			bFoundEnum = true;
			pPropDesc = mpProp;			
		}
	}
	else if (mpPropPut != NULL)
	{
		if (!mpPropPut->rEnum.empty())
		{
			bFoundEnum = true;
			pPropDesc = mpPropPut;
		}
	}	

	if (pPropDesc == NULL)
	{
		CString sValue;
		sValue.Format(_T("%d"), nThisValue);
		return sValue;
	}

	if (bFoundEnum)
	{
		for (size_t i = 0; i<pPropDesc->rEnum.size(); i++)
		{
			if (VariantToString(pPropDesc->rEnum[i].Var) == sValue)				
				return sValue + _T('-') + pPropDesc->rEnum[i].Name;		
		}
	}
	return CString();
}

bool AxInterfaceDescriptor::GetActiveXEnum()
{
	AxPropertyDescriptor *pPropDesc;
	if (mpPropGet != NULL)
	{
		pPropDesc = mpPropGet;
		if (!pPropDesc->rEnum.empty())
			return true;
	}
	else if (mpProp != NULL)
	{
		pPropDesc = mpProp;
		if (!pPropDesc->rEnum.empty())
			return true;
	}
	else if (mpPropPut != NULL)
	{
		pPropDesc = mpPropPut;
		if (!pPropDesc->rEnum.empty())
			return true;
	}
	return false;
}

CString AxInterfaceDescriptor::GetActiveXEnumValue(int nEnumIndex)
{	
	AxPropertyDescriptor *mpProp = NULL;
	if (mpPropGet != NULL)
	{
		mpProp = mpPropGet;					
	}
	else if (mpProp != NULL)
	{
		mpProp = mpProp;
	}
	else if (mpPropPut != NULL)
	{
		mpProp = mpPropPut;
	}	
	if (mpProp == NULL)
		return CString();

	return VariantToString(mpProp->rEnum[nEnumIndex].Var);
}

CString AxInterfaceDescriptor::GetActiveXPropery(CAxContainer *axContainer)
{
	HRESULT hr = - 1;
	HRESULT hrGet = - 1;
	HRESULT hrPut = - 1;
	CString sResult;

	if (mpPropGet != NULL)
	{
		if (mpPropGet->Type == VT_DISPATCH ||
			mpPropGet->Type == VT_UNKNOWN)
			return CString();
		hrGet = axContainer->GetProperty(mpPropGet, sResult);
		if (FAILED(hrGet))
		{
			return CString();
			//mType = PropActiveXRunTime;
		}
	
	}
	else if (mpProp != NULL)
	{
		hr = axContainer->GetProperty(mpProp, sResult);
		if (FAILED(hr))
		{
			//sResult = theWorkspace.LoadResourceString(IDS_RUNTIME);
			//mType = PropActiveXRunTime;
		}
	}
	else if (mpPropPutRef != NULL)
	{
		hrGet = axContainer->GetProperty(mpPropPutRef, sResult);
		if (FAILED(hrGet))
		{
			//sResult = theWorkspace.LoadResourceString(IDS_RUNTIME);
			//mType = PropActiveXRunTime;
		}
	
	}
	else if (mpPropPut != NULL)
	{
		//sResult = theWorkspace.LoadResourceString(IDS_RUNTIME);
		//mType = PropActiveXRunTime;		
	}
	
	return sResult;
}

int AxInterfaceDescriptor::GetActiveXParamQty()
{
	int nQtyGet = 0;
	int nQty = 0;
	int nQtyPut = 0;
	if (mpPropGet != NULL)
	{
		nQtyGet = mpPropGet->rArgs.size();
		if (nQtyGet > 0)
			return nQtyGet;
	}
	if (mpProp != NULL)
	{
		nQty = mpProp->rArgs.size();
		if (mpProp->invKind == INVOKE_PROPERTYGET && nQty > 0)
		{
			//mType = PropActiveXRunTime;
			return nQty;
		}
	}
	if (mpPropPut != NULL)
	{
		nQtyPut = mpPropPut->rArgs.size();
	}

	if (nQtyGet > nQty && nQtyGet > nQtyPut)
		return nQtyGet;
	
	else if (nQty > nQtyGet && nQty > nQtyPut)
		return nQty;
	
	else if (nQtyPut > nQty && nQtyPut > nQtyGet)
		return nQtyPut;
	
	else
		return 0;
}

void AxInterfaceDescriptor::DoActiveXFontPropDlg(CAxContainer *axContainer)
{	
	try
	{
		COleFont font;
		if (mpPropGet != NULL)
			font = axContainer->GetFont(mpPropGet->Id);
		else if (mpProp != NULL)
			font = axContainer->GetFont(mpProp->Id);
		else if (mpPropPut != NULL)
			font = axContainer->GetFont(mpPropPut->Id);
		else
			return;

		CString		sFontName = font.GetName();
		CY			cyFontSize = font.GetSize();
		BOOL		bFontBold = font.GetBold();
		BOOL		bFontUnderLine = font.GetUnderline();
		BOOL		bFontItalic = font.GetItalic();
		BOOL		bFontStrikethrough = font.GetStrikethrough();
		short		nFontWeight = font.GetWeight();
		short		nFontCharset = font.GetCharset();
				
		LOGFONT lf;
		memset(&lf, 0, sizeof(LOGFONT));
		lf.lfHeight = ::MulDiv( ::MulDiv(cyFontSize.Lo, GetDeviceCaps(::GetDC(NULL), LOGPIXELSY), 72), -1, 10000 );
		lf.lfWidth = 0;
		lstrcpyn(lf.lfFaceName, sFontName, _elements(lf.lfFaceName));
		lf.lfItalic = bFontItalic;
		lf.lfStrikeOut = bFontStrikethrough;
		lf.lfUnderline = bFontUnderLine;
		lf.lfWeight = nFontWeight;
		
		CFontDialog dlg(&lf);
		INT_PTR nResult = dlg.DoModal();

		if( nResult != IDOK )
			return;

		font.SetName(dlg.m_cf.lpLogFont->lfFaceName );
		font.SetSize( COleCurrency( dlg.m_cf.iPointSize / 10, 1000 * (dlg.m_cf.iPointSize % 10) ));
		font.SetWeight(short( dlg.m_cf.lpLogFont->lfWeight ));	
		font.SetUnderline(dlg.m_cf.lpLogFont->lfUnderline);
		font.SetItalic(dlg.m_cf.lpLogFont->lfItalic);
		font.SetStrikethrough(dlg.m_cf.lpLogFont->lfStrikeOut);		
		
		if (mpPropPut != NULL)
			axContainer->SetFont(mpPropPut->Id, font);
		else if (mpProp != NULL)
			axContainer->SetFont(mpProp->Id, font);
		else if (mpPropGet != NULL)
			axContainer->SetFont(mpPropGet->Id, font);
		
	}
	catch(...)
	{
	}
}


void AxInterfaceDescriptor::SetActiveXPropery(CAxContainer *axContainer, CString sNewValue)
{
	if (mpPropPut != NULL)
		axContainer->SetProperty(mpPropPut, sNewValue);
	else if (mpPropPutRef != NULL)
		axContainer->SetProperty(mpPropPutRef, sNewValue);
	else if (mpProp != NULL)
		axContainer->SetProperty(mpProp, sNewValue);
	
	
}

CString AxInterfaceDescriptor::GetDocumentationDesc()
{
	if (mpPropGet != NULL)
		return mpPropGet->DocumentationDesc;
	else if (mpPropPut != NULL)
		return mpPropPut->DocumentationDesc;
	else if (mpPropPutRef != NULL)
		return mpPropPutRef->DocumentationDesc;
	else if (mpProp != NULL)
		return mpProp->DocumentationDesc;
	return CString();
}

CString AxInterfaceDescriptor::GetAxMethodDesc(size_t nIndex)
{
	if (nIndex < mpMethods->size())
		return mpMethods->at(nIndex)->Desc;
	return CString();
}



int AxInterfaceDescriptor::GetAxMethodParams(size_t nIndex)
{
	if (nIndex < mpMethods->size())
		return mpMethods->at(nIndex)->nParamQty;
	return 0;
}

CString AxInterfaceDescriptor::GetAxMethodParamName(size_t nIndex, int nParam)
{
	if (nIndex < mpMethods->size())
		return mpMethods->at(nIndex)->CallingArgNames[nParam];
	return CString();
}

VARTYPE AxInterfaceDescriptor::GetAxMethodReturnType(size_t nIndex)
{
	if (nIndex < mpMethods->size())
		return mpMethods->at(nIndex)->ReturnType;
	return 0;
}


AxMethodDescriptor * AxInterfaceDescriptor::GetAxMethod(size_t nIndex)
{
	if (nIndex < mpMethods->size())
		return mpMethods->at(nIndex);
	return NULL;
}

CString AxInterfaceDescriptor::GetAxMethodParamVarType(size_t nIndex, int nParam)
{
	if (nIndex < mpMethods->size())
		return VARTYPEtoString(mpMethods->at(nIndex)->CallingArgs[nParam]);
	return CString();
}

GUID AxInterfaceDescriptor::GetAxMethodParamGUID(size_t nIndex, int nParam)
{
	if (nIndex < mpMethods->size())
		return mpMethods->at(nIndex)->CallingArgClsids[nParam];
	GUID guid;
	::memset(&guid, 0, sizeof(GUID));	
	return guid;
}
	
VARTYPE AxInterfaceDescriptor::GetActiveXProperyType()
{
	VARTYPE vReturn = 0;
	if (mpPropGet != NULL)
		vReturn = mpPropGet->Type;
	else if (mpProp != NULL)
		vReturn = mpProp->Type;
	else if (mpPropPut != NULL)
		vReturn = (VARTYPE) mpPropPut->Type;
	else if (mpPropPutRef != NULL)
		vReturn = (VARTYPE) mpPropPutRef->Type;

	return vReturn;
}

GUID AxInterfaceDescriptor::GetActiveXProperyGuid()
{
	if (mpPropGet != NULL)
		return mpPropGet->Guid;
	else if (mpProp != NULL)
		return mpProp->Guid;
	else if (mpPropPut != NULL)
		return mpPropPut->Guid;
	else if (mpPropPutRef != NULL)
		return mpPropPutRef->Guid;

	GUID guid;
	::memset(&guid, 0, sizeof(GUID));	
	return guid;
}

DISPID AxInterfaceDescriptor::GetActiveXGetProperyId()
{
	if (mpPropGet != NULL)
		return mpPropGet->Id;
	else if (mpProp != NULL)
		return mpProp->Id;
	else if (mpPropPut != NULL)
		return mpPropPut->Id;
	else if (mpPropPutRef != NULL)
		return mpPropPutRef->Id;

	DISPID dispid;
	::memset(&dispid, 0, sizeof(DISPID));	
	return dispid;
}

DISPID AxInterfaceDescriptor::GetActiveXSetProperyId()
{
	if (mpPropPut != NULL)
		return mpPropPut->Id;
	else if (mpPropPutRef != NULL)
		return mpPropPutRef->Id;
	else if (mpProp != NULL)
		return mpProp->Id;
	else if (mpPropGet != NULL)
		return mpPropGet->Id;
	
	DISPID dispid;
	::memset(&dispid, 0, sizeof(DISPID));	
	return dispid;
}

CString AxInterfaceDescriptor::GetName() const
{
	if (mpPropGet != NULL)
		return mpPropGet->Name;
	if (mpProp != NULL)
		return mpProp->Name;
	if (mpPropPut != NULL)
		return mpPropPut->Name;
	if (mpPropPutRef != NULL)
		return mpPropPutRef->Name;
	if (mpEvent != NULL)
		return mpEvent->Name;
	return CString();
}

#ifdef _DIAGNOSTIC
LPCTSTR AxInterfaceDescriptor::toString() const
{
	CString sProp;
	sProp.Format( _T("Prop: %s"), mpProp? mpProp->toString() : _T("<null>") );
	CString sPropGet;
	sPropGet.Format( _T("PropGet: %s"), mpPropGet? mpPropGet->toString() : _T("<null>") );
	CString sPropPut;
	sPropPut.Format( _T("PropPut: %s"), mpPropPut? mpPropPut->toString() : _T("<null>") );
	CString sPropPutRef;
	sPropPutRef.Format( _T("PropPutRef: %s"), mpPropPutRef? mpPropPutRef->toString() : _T("<null>") );
	CString sEvent;
	sEvent.Format( _T("Event: %s"), mpEvent? mpEvent->toString() : _T("<null>") );
	CString sMethods = _T("Methods: {");
	if( mpMethods )
	{
		if( mpMethods->size() == 0 )
			sMethods += _T("<empty>");
		else
		{
			sMethods += _T("{");
			sMethods += mpMethods->at(0)->toString();
			for( size_t idx = 1; idx < mpMethods->size(); ++idx )
			{
				sMethods += _T("},{");
				sMethods += mpMethods->at(idx)->toString();
			}
			sMethods += _T("}");
		}
	}
	else
		sMethods += _T("<null>");
	sMethods += _T("}");
	static CString sVal;
	sVal.Format( _T("{%s, %s, %s, %s, %s, %s}"),
							(LPCTSTR)sProp, (LPCTSTR)sPropGet, (LPCTSTR)sPropPut, (LPCTSTR)sPropPutRef,
							(LPCTSTR)sEvent, (LPCTSTR)sMethods );
	return sVal;
}
#endif
