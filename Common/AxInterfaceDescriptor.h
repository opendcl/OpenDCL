#pragma once

#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include <vector>

class CAxContainer;
enum IOStatus;


// Struct for holding information about an OLE object's interface
class AxInterfaceDescriptor
{
public:
	RefCountedPtr< AxPropertyDescriptor > mpProp;
	RefCountedPtr< AxPropertyDescriptor > mpPropGet;
	RefCountedPtr< AxPropertyDescriptor > mpPropPut;
	RefCountedPtr< AxPropertyDescriptor > mpPropPutRef;
	RefCountedPtr< AxEventDescriptor > mpEvent;
	RefCountedPtr< std::vector< RefCountedPtr< AxMethodDescriptor > > > mpMethods;

public:
	AxInterfaceDescriptor() {}
	~AxInterfaceDescriptor() {}

	//Attributes
public:
	RefCountedAutoConstructPtr< AxPropertyDescriptor > GetProp() { return mpProp; }
	RefCountedAutoConstructPtr< AxPropertyDescriptor > GetPropGet() { return mpPropGet; }
	RefCountedAutoConstructPtr< AxPropertyDescriptor > GetPropPut() { return mpPropPut; }
	RefCountedAutoConstructPtr< AxPropertyDescriptor > GetPropPutRef() { return mpPropPutRef; }
	RefCountedAutoConstructPtr< AxEventDescriptor > GetEvent() { return mpEvent; }
	RefCountedAutoConstructPtr< std::vector< RefCountedPtr< AxMethodDescriptor > > > GetMethods() { return mpMethods; }

	//Operations
public:
	void clear();

	//Old functions moved here from CPropertyObject
public:
	CString GetName();
	CString GetDocumentationDesc();
	GUID GetActiveXProperyGuid();
	DISPID GetActiveXGetProperyId();
	DISPID GetActiveXSetProperyId();
	int GetActiveXParamQty();
	void SetActiveXPropery(CAxContainer *axContainer, CString sNewValue);
	void SetActiveXProperyName(CString sName);	
	CString GetActiveXPropery(CAxContainer *axContainer);
	VARTYPE GetActiveXProperyType();
	bool GetActiveXEnum();
	CString GetActiveXEnumDesc(CString sValue);
	CString GetActiveXEnumValue(int nEnumIndex);
	void DoActiveXFontPropDlg(CAxContainer *axContainer);
	CString GetAxMethodDesc(size_t nIndex);
	int GetAxMethodParams(size_t nIndex);
	GUID GetAxMethodParamGUID(size_t nIndex, int nParam);
	CString GetAxMethodParamName(size_t nIndex, int nParam);
	CString GetAxMethodParamVarType(size_t nIndex, int nParam);
	VARTYPE GetAxMethodReturnType(size_t nIndex);
	AxMethodDescriptor *GetAxMethod(size_t nIndex);


	// FIle I/O
	virtual void Serialize(CArchive& ar, int nPropertyVersion);
  IOStatus WriteToTextFile(FILE* pFile) const;
  IOStatus ReadFromTextFile5(std::ifstream &sFile);

protected:
	virtual void SerializeProp(CArchive& ar, AxPropertyDescriptor& axProp, int nPropertyVersion);	
  IOStatus WritePropToTextFile(FILE* pFile, const AxPropertyDescriptor& axProp) const;
  IOStatus ReadPropFromTextFile5(std::ifstream &sFile, AxPropertyDescriptor& axProp);
};
