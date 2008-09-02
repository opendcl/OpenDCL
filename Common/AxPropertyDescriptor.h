#pragma once

#include "AxArg.h"

class CAxContainerCtrl;
enum IOStatus;

struct AxPropertyEnum
{
	CString Name;
	VARIANT Var;
};


// Struct for holding information about ActiveX properties so we do not
// have to keep going back to typeinfo
class AxPropertyDescriptor
{
protected:
	DISPID mDispId;
	CString msName;
	CString msDesc;
	VARTYPE mType;
	bool mbArray;
	bool mbReadOnly;
	GUID mGuid;
	CString msTypeName;
	INVOKEKIND mInvKind;
	std::vector< AxPropertyEnum > mrEnum;
	std::vector< AxArg > mrArgs;

protected:
	friend class AxInterfaceDescriptor;
	AxPropertyDescriptor(void);
public:
	AxPropertyDescriptor( DISPID dispid, LPCTSTR pszName, LPCTSTR pszDesc, VARTYPE type, INVOKEKIND kind );
	AxPropertyDescriptor( VARDESC* pVarDesc, ITypeInfo* pTypeInfo );
	AxPropertyDescriptor( FUNCDESC* pFuncDesc, ITypeInfo* pTypeInfo, LPOLEOBJECT pIObject = NULL );
	AxPropertyDescriptor( const AxPropertyDescriptor& Src );
	virtual ~AxPropertyDescriptor(void);

	// Attributes
	DISPID GetDispId() const { return mDispId; }
	const CString& GetName() const { return msName; }
	const CString& GetDesc() const { return msDesc; }
	VARTYPE GetType() const { return mType; }
	CString GetTypeDisplayName() const;
	bool IsArray() const { return mbArray; }
	bool IsReadOnly() const { return mbReadOnly; }
	GUID GetGuid() const { return mGuid; }
	INVOKEKIND GetInvKind() const { return mInvKind; }
	const std::vector< AxPropertyEnum >& GetEnum() const { return mrEnum; }
	const std::vector< AxArg >& GetArgs() const { return mrArgs; }
	CString GetEnumDisplayName( size_t idxEnum ) const;
	CString GetArgDisplayName( size_t idxArg ) const;

	// Operations
	HRESULT Get( IDispatch* pObjectDisp, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult ) const;
	HRESULT Set( IDispatch* pObjectDisp, const VARIANTARG* rvarArgs, UINT ctArgs ) const;

protected:
	HRESULT PerPropertyBrowsing( LPOLEOBJECT pIObject );
	HRESULT GetRefGuid( ITypeInfo* TheInfo, HREFTYPE hreftype );

	// File I/O
public:
	void Serialize( CArchive& ar, int nPropertyVersion );
	IOStatus ReadFromTextFile( std::ifstream &sFile, ULONG nPropertyVersion );
	//IOStatus WriteToTextFile( FILE* pFile ) const;

#ifdef _DIAGNOSTIC
	virtual LPCTSTR toString() const;
#endif
};
