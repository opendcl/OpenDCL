// AxMethodDescriptor.cpp : implementation file
//

#include "stdafx.h"
#include "AxMethodDescriptor.h"
#include "Project.h"
#include "Filing.h"


IMPLEMENT_SERIAL(AxMethodDescriptor, CObject, 1)

AxMethodDescriptor::AxMethodDescriptor(void) : Id( 0 ), ReturnType( 0 ), nParamQty( 0 )
{
	CallingArgs[0] = 0;
	::memset(&ReturnGuid, 0, sizeof(GUID));	
	for (int i=0; i<MAX_CALLING_ARGUMENTS; i++)
		::memset(&CallingArgClsids[i], 0, sizeof(CLSID));			
}

IOStatus AxMethodDescriptor::WriteToTextFile(FILE* pFile) const
{
  int nThisVersion = 2;

  writeInt(pFile, nThisVersion);
  LONG DispId = (LONG)Id;
  writeLong(pFile, DispId);		
  writeString(pFile, Name);
  writeString(pFile, Params);
  writeString(pFile, Desc);
  writeVARTYPE(pFile, ReturnType);
  writeInt(pFile, nParamQty);		
  writeCLSID(pFile, ReturnGuid);
  for (int i = 0; i<nParamQty; i++)
  {
    writeVARTYPE(pFile, CallingArgs[i]);
    writeString(pFile, CallingArgNames[i]);
    writeCLSID(pFile, CallingArgClsids[i]);
  }
	return statOK;
}

void AxMethodDescriptor::Serialize(CArchive& ar)
{
	CObject::Serialize( ar );
	
	int nThisVersion = 2;
	
	if (ar.IsStoring())
	{
		ar << nThisVersion;
		LONG DispId = (LONG)Id;
		ar << DispId;		
		ar << Name;
		ar << Params;
		ar << Desc;
		ar << ReturnType;
		ar << nParamQty;		
		SerializeCLSID(ar, ReturnGuid);
		for (int i = 0; i<nParamQty; i++)
		{
			ar << CallingArgs[i];
			ar << CallingArgNames[i];
			SerializeCLSID(ar, CallingArgClsids[i]);
		}
	}
	else
	{
		ar >> nThisVersion;
		LONG DispId;		
		ar >> DispId; Id = DispId;
		ar >> Name;
		ar >> Params;
		ar >> Desc;


		ar >> ReturnType;
		ar >> nParamQty;
		if (nThisVersion >= 2)
			SerializeCLSID(ar, ReturnGuid);
		for (int i = 0; i<nParamQty; i++)
		{
			ar >> CallingArgs[i];
			ar >> CallingArgNames[i];


			if (nThisVersion >= 2)
				SerializeCLSID(ar, CallingArgClsids[i]);
		}
	}
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
  LONG DispId;		
  if (!readLong(sFile, DispId)) return statInvalidFormat;
  Id = DispId;
  if (!readString(sFile, Name)) return statInvalidFormat;
  if (!readString(sFile, Params)) return statInvalidFormat;
  if (!readString(sFile, Desc)) return statInvalidFormat;
  if (!readVARTYPE(sFile, ReturnType)) return statInvalidFormat;
  if (!readInt(sFile, nParamQty)) return statInvalidFormat;
  if (!readCLSID(sFile, ReturnGuid)) return statInvalidFormat;
  for (int i = 0; i<nParamQty; i++)
  {
    if (!readVARTYPE(sFile, CallingArgs[i])) return statInvalidFormat;
    if (!readString(sFile, CallingArgNames[i])) return statInvalidFormat;
    if (!readCLSID(sFile, CallingArgClsids[i])) return statInvalidFormat;
  }
  return statOK;
}
