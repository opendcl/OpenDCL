#pragma once

enum IOStatus;

struct AxEventArg
{
	VARTYPE vt;
	CString name;
	CLSID clsid;
	AxEventArg() : vt( VT_EMPTY ) {}
};


// Struct for holding information about ActiveX events so we do not
// have to keep going back to type info
class AxEventDescriptor
{
	DISPID mDispId;
	CString msName;
	CString msDesc;
	CString msParams;
	std::vector< AxEventArg > mrArgs;

protected:
	friend class AxInterfaceDescriptor;
  AxEventDescriptor(void);
public:
	AxEventDescriptor( FUNCDESC* pFuncDesc, ITypeInfo* pTypeInfo, bool bUseAsType = true );
	virtual ~AxEventDescriptor(void);

	// Attributes
	DISPID GetDispId() const { return mDispId; }
	const CString& GetName() const { return msName; }
	const CString& GetDesc() const { return msDesc; }
	const std::vector< AxEventArg >& GetArgs() const { return mrArgs; }

	// File I/O
public:
	void Serialize( CArchive& ar, int nPropertyVersion );
	IOStatus ReadFromTextFile( std::ifstream &sFile, ULONG nPropertyVersion );
	//IOStatus WriteToTextFile( FILE* pFile ) const;

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toString() const;
#endif
};
