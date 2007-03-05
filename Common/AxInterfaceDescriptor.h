#pragma once

#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxMethodDescriptor.h"
#include <vector>

class CAxContainerCtrl;
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
	CString GetDesc() const;
	GUID GetGuid() const;
	VARTYPE GetType() const;
	size_t GetParamQty() const;
	DISPID GetGetDispId() const;
	DISPID GetPutDispId() const;
	AxPropertyDescriptor* GetGetDescriptor() const;
	AxPropertyDescriptor* GetPutDescriptor() const;
	AxPropertyDescriptor* GetEnumDescriptor() const;
	AxPropertyDescriptor* GetArgDescriptor() const;
	CString GetEnumDesc(CString sValue) const;
	CString GetEnumValue(int nEnumIndex) const;
	void DoActiveXFontPropDlg(CAxContainerCtrl *axContainer);
	CString GetAxMethodDesc(size_t nIndex);
	size_t CountAxMethodParams(size_t nIndex);
	GUID GetAxMethodParamGUID(size_t nIndex, int nParam);
	CString GetAxMethodParamName(size_t nIndex, int nParam);
	CString GetAxMethodParamVarType(size_t nIndex, int nParam);
	VARTYPE GetAxMethodReturnType(size_t nIndex);
	AxMethodDescriptor *GetAxMethod(size_t nIndex);


	// FIle I/O
	virtual void Serialize(CArchive& ar, int nPropertyVersion);
  IOStatus ReadFromTextFile5(std::ifstream &sFile);
  //IOStatus WriteToTextFile(FILE* pFile) const;

#ifdef _DIAGNOSTIC
public:
	virtual LPCTSTR toString() const;
#endif
};
