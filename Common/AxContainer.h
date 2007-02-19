// AxContainer.h : header file
//

#pragma once

#include "PPTooltip.h"

class COleFont;
class CDclControlObject;
class CDclFormObject;
class CControlPane;
class AxPropertyDescriptor;
class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CAxContainer window

class CAxContainer : public CWnd
{
protected:
	CDclFormObject* mpParent;
	CDclControlObject* mpOleControl;
	CPPToolTip mToolTip;
	CComPtr< ITypeLib > mpTypeLib;
	UINT mnTypeLibCount;
	CComPtr< IDispatch > mpDispatch; //cache the IDispatch and keep it alive for the lifetime of the container

// Attributes
	BOOL m_bInvokeWithSendString;

// Construction
public:
	CAxContainer(CDclFormObject* pParent);
	virtual ~CAxContainer();

// Operations
public:
	CDclFormObject* GetParent() const { return mpParent; }
	CDclControlObject* GetOleControl() const { return mpOleControl; }

	void SetTooltipText( LPCTSTR pszText );
	CPPToolTip& GetToolTip() { return mToolTip; }

	void TryToFireAxEvent(UINT idCtrl, AFX_EVENT* pEvent);
	void FireAxEvent(UINT idCtrl, RefCountedPtr< CPropertyObject > pProp, AFX_EVENT* pEvent);

	BOOL Create(CLSID clsid, LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, UINT nID,
		CFile* pPersist = NULL, BOOL bStorage = FALSE,
		BSTR bstrLicKey = NULL)
	{ return CreateControl(clsid, lpszWindowName, dwStyle, rect, pParentWnd, nID,
		pPersist, bStorage, bstrLicKey); }
	void InitToolTip();

	HRESULT SaveToStream( IStream* pStream );
	
	HRESULT GetOleDispatch( IDispatch** ppDispatch );
	HRESULT GetOleObject( IOleObject** ppOleObject );
	UINT ExtractPropertyInfo( CDclControlObject *pControl, LPOLEOBJECT pIObject, bool bEnumList = false );
	UINT ExtractPropertyInfo( CDclControlObject *pControl, ITypeInfo* pTI, LPOLEOBJECT pIObject = NULL, bool bEnumList = false );
	UINT ExtractEventInfo( CDclControlObject *pControl, LPOLEOBJECT pIObject, bool bUseAsType );
	UINT ExtractEventInfo( CDclControlObject *pControl, ITypeInfo* pTypeInfo, bool bUseAsType );
	UINT ExtractMethodInfo( CDclControlObject *pControl, LPOLEOBJECT pIObject );
	UINT ExtractMethodInfo( CDclControlObject *pControl, ITypeInfo* pTypeInfo );

	IDispatch *GetChildIDispatch(DISPID dispid);

	void Initialize();
	virtual BOOL CreateCtrl(CLSID Clsid, CDclControlObject *pControl, int nID, CWnd *pParent);
	virtual BOOL CreateCtrl(CLSID Clsid, CDclControlObject *pControl, const RECT& rect, int nID, CWnd *pParent, bool bAddPropInfo);
	COleFont GetFont(DISPID dispid);
	void SetFont(DISPID dispid, LPDISPATCH newValue);
	unsigned long GetColor(DISPID dispid);
	void SetColor(DISPID dispid, unsigned long propVal);
	void LoadPictureFile(DISPID dispid, CString sFile, WORD flag);
	void LoadPicture(DISPID dispid, int nId);
	void SetPicture(DISPID dispid, LPDISPATCH newValue, WORD flag);
	BOOL GetPropertyPageCLSIDs( CArray< CLSID, CLSID& >& aclsidPages );
	void ShowPropertyPages();

	void SetRefImageList(DISPID dispid, LPDISPATCH newValue);
	void SetImageList(DISPID dispid, LPDISPATCH newValue);

	//ActiveX Helpers
public:
	HRESULT GetProperty(AxPropertyDescriptor* axProp, CString &strReturnValue);
	HRESULT GetProperty( AxPropertyDescriptor* axProp, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult );
	HRESULT SetProperty( AxPropertyDescriptor* axProp, VARIANTARG* rvarArgs, UINT ctArgs );
	HRESULT SetProperty( AxPropertyDescriptor* axProp, COleVariant varArg );
	HRESULT Invoke( AxMethodDescriptor* axMethod, VARIANTARG* rvarArgs, UINT ctArgs, VARIANT& varResult );
	BOOL ExtractComponentsFromTLB(CDclControlObject *pControl, CLSID clsid);

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
