#pragma once

#include "RefCountedPtr.h"


class CDclAxCtrlInitInfo
{
	CLSID mClsid;
	CString msProgId;
	CString msLicenseKey;
	CComPtr< IStream > mpStream; //for persisted state
	CString msTypeDisplayName;

public:
	CDclAxCtrlInitInfo();
	CDclAxCtrlInitInfo( const CDclAxCtrlInitInfo& Src );
	CDclAxCtrlInitInfo( const CLSID& clsid, LPCTSTR pszLicenseKey = NULL, LPCTSTR pszProgId = NULL );
	virtual ~CDclAxCtrlInitInfo();

protected:
	//2014-02-04 [ORW]: initial save version set to 1
	ULONG GetCurrentSaveVersion() const { return 1; }

// Operations
public:
	CDclAxCtrlInitInfo& operator =( const CDclAxCtrlInitInfo& Src );

// Attributes
public:
	bool IsNull() const;
	const CLSID& GetClsid() const { return mClsid; }
	LPCTSTR GetProgId() const { return msProgId; }
	LPCTSTR GetLicenseKey() { return msLicenseKey; }
	const CComPtr< IStream >& GetIStream() const { return mpStream; }
	CComPtr< IStream >& GetIStream() { return mpStream; }
	CString GetDisplayName() const;
	void SetDisplayName( LPCTSTR pszDisplayName ) { msTypeDisplayName = pszDisplayName; }

// Implementation

	//File I/O
public:
	virtual void Serialize(CArchive& ar);
};

typedef RefCountedPtr< CDclAxCtrlInitInfo > TAxCtrlInitInfoPtr;
typedef RefCountedPtrAsIUnknown< TAxCtrlInitInfoPtr > TAxCtrlInitInfoPtrIUnknown;
