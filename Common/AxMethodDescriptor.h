#pragma once

enum IOStatus;


// class for holding information about ActiveX methods so we do not
// have to keep goint back to typeinfo
class AxMethodDescriptor : public CObject
{
	static const int MAX_CALLING_ARGUMENTS = 16;
public:
	DISPID Id;
	CString Name;
	CString Params;
	CString Desc;
	VARTYPE ReturnType;
	GUID ReturnGuid;	
	VARTYPE CallingArgs[MAX_CALLING_ARGUMENTS];
	CString CallingArgNames[MAX_CALLING_ARGUMENTS+1];
	CLSID   CallingArgClsids[MAX_CALLING_ARGUMENTS];
	int nParamQty;

public:
	AxMethodDescriptor(void);

	//File I/O
	virtual void Serialize(CArchive& ar);
  IOStatus WriteToTextFile(FILE* pFile) const;
  IOStatus ReadFromTextFile(std::ifstream &sFile);
  IOStatus ReadFromTextFile2(std::ifstream &sFile);

protected:
	DECLARE_SERIAL(AxMethodDescriptor)
};
