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
	virtual ~AxInterfaceDescriptor() { clear(); }

	//Attributes
public:
	AxPropertyDescriptor*& GetProp()
		{ return mpProp; }
	AxPropertyDescriptor* const& GetProp() const
		{ return mpProp; }
	void SetProp( AxPropertyDescriptor* pProp )
		{ delete mpProp; mpProp = pProp; }
	AxPropertyDescriptor*& GetPropGet()
		{ return mpPropGet; }
	AxPropertyDescriptor* const& GetPropGet() const
		{ return mpPropGet; }
	void SetPropGet( AxPropertyDescriptor* pProp )
		{ delete mpPropGet; mpPropGet = pProp; }
	AxPropertyDescriptor*& GetPropPut()
		{ return mpPropPut; }
	AxPropertyDescriptor* const& GetPropPut() const
		{ return mpPropPut; }
	void SetPropPut( AxPropertyDescriptor* pProp )
		{ delete mpPropPut; mpPropPut = pProp; }
	AxPropertyDescriptor*& GetPropPutRef()
		{ return mpPropPutRef; }
	AxPropertyDescriptor* const& GetPropPutRef() const
		{ return mpPropPutRef; }
	void SetPropPutRef( AxPropertyDescriptor* pProp )
		{ delete mpPropPutRef; mpPropPutRef = pProp; }
	AxEventDescriptor*& GetEvent() { return mpEvent; }
	AxEventDescriptor* const& GetEvent() const { return mpEvent; }
	void SetEvent( AxEventDescriptor* pEvent )
		{ delete mpEvent; mpEvent = pEvent; }
	std::vector< RefCountedPtr< AxMethodDescriptor > >*& GetMethods()
		{ return mpMethods; }
	std::vector< RefCountedPtr< AxMethodDescriptor > >* const& GetMethods() const
		{ return mpMethods; }
	void SetMethods( std::vector< RefCountedPtr< AxMethodDescriptor > >* pMethods )
		{ delete mpMethods; mpMethods = pMethods; }

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
	CString GetAxMethodDesc(size_t nIndex) const;
	size_t CountAxMethodParams(size_t nIndex) const;
	GUID GetAxMethodParamGUID(size_t nIndex, int nParam) const;
	CString GetAxMethodParamName(size_t nIndex, int nParam) const;
	CString GetAxMethodParamVarType(size_t nIndex, int nParam) const;
	VARTYPE GetAxMethodReturnType(size_t nIndex) const;
	CString GetAxMethodReturnTypeDisplayName( size_t nIndex ) const;
	const AxMethodDescriptor* GetAxMethod(size_t nIndex) const;


	// FIle I/O
	virtual void Serialize(CArchive& ar, int nPropertyVersion);
  IOStatus ReadFromTextFile5(std::ifstream &sFile);
  //IOStatus WriteToTextFile(FILE* pFile) const;

#ifdef _DIAGNOSTIC
public:
	virtual LPCTSTR toString() const;
#endif
};
