#pragma once

#include "AxArg.h"

enum IOStatus;


// class for holding information about ActiveX methods so we do not
// have to keep goint back to typeinfo
class AxMethodDescriptor
{
	DISPID mDispId;
	CString msName;
	CString msDesc;
	CString msParams;
	VARTYPE mReturnType;
	GUID mReturnGuid;	
	std::vector< AxArg > mrArgs;

protected:
	friend class AxInterfaceDescriptor;
	AxMethodDescriptor(void);
public:
	AxMethodDescriptor( FUNCDESC* pFuncDesc, ITypeInfo* pTypeInfo, bool bUseAsType = true );
	virtual ~AxMethodDescriptor(void);

	// Attributes
	DISPID GetDispId() const { return mDispId; }
	const CString& GetName() const { return msName; }
	const CString& GetDesc() const { return msDesc; }
	VARTYPE GetReturnType() const { return mReturnType; }
	GUID GetReturnGuid() const { return mReturnGuid; }
	const std::vector< AxArg >& GetArgs() const { return mrArgs; }

	//Operations
	HRESULT Invoke( IDispatch* pObjectDisp, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult ) const;

	//File I/O
	virtual void Serialize(CArchive& ar, int nPropertyVersion);
  //IOStatus WriteToTextFile(FILE* pFile) const;
  IOStatus ReadFromTextFile(std::ifstream &sFile);
  IOStatus ReadFromTextFile2(std::ifstream &sFile);

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toString() const;
#endif
};
