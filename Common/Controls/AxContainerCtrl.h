// AxContainerCtrl.h : header file
//

#pragma once

#include "DialogControl.h"

class AxPropertyDescriptor;
class CControlPane;
class CDclFormObject;
class COleFont;
class CProject;
class CPropertyObject;

/////////////////////////////////////////////////////////////////////////////
// CAxContainerCtrl window

class CAxContainerCtrl : public CWnd, public CDialogControl
{
protected:
	CComPtr< ITypeLib > mpTypeLib;
	UINT mnTypeLibCount;
	CComPtr< IDispatch > mpDispatch; //cache the IDispatch and keep it alive for the lifetime of the container

// Construction
public:
	CAxContainerCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CAxContainerCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;

// Operations
public:
	bool Create( CWnd* pParentWnd, UINT nID, CRect ArxRect, bool bAddPropInfo );

	HRESULT SaveToStream();
	void ClearStream();
	HRESULT GetOleDispatch( IDispatch** ppDispatch );
	HRESULT GetOleObject( IOleObject** ppOleObject );

protected:
	bool Create( const CLSID& clsid, CWnd* pParentWnd, DWORD dwStyle, UINT nID, CRect ArxRect, COleStreamFile* pStreamFile, BSTR bstrLicenseKey );
	UINT ExtractPropertyInfo( TDclControlPtr pControl, ITypeInfo* pTI, LPOLEOBJECT pIObject = NULL, bool bEnumList = false );
	UINT ExtractEventInfo( TDclControlPtr pControl, ITypeInfo* pTypeInfo, bool bUseAsType );
	UINT ExtractMethodInfo( TDclControlPtr pControl, ITypeInfo* pTypeInfo );

	//ActiveX Helpers
public:
	bool SetProperty( TPropertyPtr pProp, LPCTSTR pszValue );
	HRESULT GetProperty( const AxPropertyDescriptor* axProp, CString &strReturnValue );
	HRESULT GetProperty( const AxPropertyDescriptor* axProp, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult );
	HRESULT SetProperty( const AxPropertyDescriptor* axProp, const VARIANTARG* rvarArgs, UINT ctArgs );
	HRESULT Invoke( AxMethodDescriptor* axMethod, const VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult );
	bool ExtractComponentsFromTLB( TDclControlPtr pDclControl );
	bool GetPropertyPageCLSIDs( CArray< CLSID, CLSID& >& aclsidPages );
	bool ParseTypeLibInfo();

	// Generated message map functions
protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	BOOL DestroyWindow() override;
	//OnChildNotify has not been recreated yet.
};
