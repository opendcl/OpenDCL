#pragma once

#include "AxArg.h"

enum IOStatus;


// Struct for holding information about ActiveX events so we do not
// have to keep going back to type info
class AxEventDescriptor
{
	DISPID mDispId;
	CString msName;
	CString msDesc;
	std::vector< AxArg > mrArgs;

protected:
	friend class AxInterfaceDescriptor;
  AxEventDescriptor(void);
public:
	AxEventDescriptor( FUNCDESC* pFuncDesc, ITypeInfo* pTypeInfo, bool bUseAsType = true );
	virtual ~AxEventDescriptor(void);

	//2008-11-02 [ORW]: save version set to 2 (removed msParams and changed arg count to unsigned short)
	BYTE GetCurrentSaveVersion() const { return 2; }

	// Attributes
	DISPID GetDispId() const { return mDispId; }
	const CString& GetName() const { return msName; }
	const CString& GetDesc() const { return msDesc; }
	const std::vector< AxArg >& GetArgs() const { return mrArgs; }

	// File I/O
public:
	void Serialize( CArchive& ar, BYTE nPropertyVersion );
	IOStatus ReadFromTextFile( std::ifstream &sFile, BYTE nPropertyVersion );
	//IOStatus WriteToTextFile( FILE* pFile ) const;

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toString() const;
#endif
};
