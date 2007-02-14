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
	AxPropertyDescriptor* mpProp;
	AxPropertyDescriptor* mpPropGet;
	AxPropertyDescriptor* mpPropPut;
	AxPropertyDescriptor* mpPropPutRef;
	AxEventDescriptor* mpEvent;
	std::vector< RefCountedPtr< AxMethodDescriptor > >* mpMethods;

public:
	AxInterfaceDescriptor()
		: mpProp( NULL )
		, mpPropGet( NULL )
		, mpPropPut( NULL )
		, mpPropPutRef( NULL )
		, mpEvent( NULL )
		, mpMethods( NULL )
		{}
	virtual ~AxInterfaceDescriptor() {}

	//Attributes
public:
	AxPropertyDescriptor*& GetProp()
		{ return mpProp; }
	void SetProp( AxPropertyDescriptor* pProp )
		{ mpProp = pProp; }
	AxPropertyDescriptor*& GetPropGet()
		{ return mpPropGet; }
	void SetPropGet( AxPropertyDescriptor* pProp )
		{ mpPropGet = pProp; }
	AxPropertyDescriptor*& GetPropPut()
		{ return mpPropPut; }
	void SetPropPut( AxPropertyDescriptor* pProp )
		{ mpPropPut = pProp; }
	AxPropertyDescriptor*& GetPropPutRef()
		{ return mpPropPutRef; }
	void SetPropPutRef( AxPropertyDescriptor* pProp )
		{ mpPropPutRef = pProp; }
	AxEventDescriptor*& GetEvent() { return mpEvent; }
	void SetEvent( AxEventDescriptor* pEvent )
		{ mpEvent = pEvent; }
	std::vector< RefCountedPtr< AxMethodDescriptor > >*& GetMethods()
		{ return mpMethods; }
	void SetMethods( std::vector< RefCountedPtr< AxMethodDescriptor > >* pMethods )
		{ mpMethods = pMethods; }

	//Operations
public:
	void clear();

	//Old functions moved here from CPropertyObject
public:
	CString GetName() const;
	CString GetDocumentationDesc();
	GUID GetActiveXProperyGuid();
	DISPID GetActiveXGetProperyId();
	DISPID GetActiveXSetProperyId();
	int GetActiveXParamQty();
	void SetActiveXPropery(CAxContainer *axContainer, CString sNewValue);
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

#ifdef _DIAGNOSTIC
public:
	virtual LPCTSTR toString() const;
#endif
};
