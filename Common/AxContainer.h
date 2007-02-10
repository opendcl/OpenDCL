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

// Attributes
	ITypeLib *m_pTypeLib;
	UINT m_nTypeLibCount;
	BOOL m_bInvokeWithSendString;

	// this container is used to store COleDispatchDriver's
	// this is done because if we dont any IDispatch's sent back will become NULL by the time the 
	// next ActiveX method or property is called on it.
	CList< COleDispatchDriver* > mOleDispatchDrivers; // this needs to be eliminated [ORW]


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
	
	IDispatch *GetOleIDispatch();
	IOleObject *GetIOleObject();
	UINT ExtractPropertyInfo(CDclControlObject *pControl, LPOLEOBJECT pIObject, BOOL bEnumList = FALSE);
	UINT ExtractPropertyInfo(CDclControlObject *pControl, ITypeInfo *TheInfo, BOOL bEnumList, LPOLEOBJECT pIObject = NULL);
	UINT ExtractEventInfo(CDclControlObject *pControl, LPOLEOBJECT pIObject, BOOL bUseAsType);
	UINT ExtractEventInfo(CDclControlObject *pControl, ITypeInfo *TheInfo, BOOL bUseAsType);
	UINT ExtractMethodInfo(CDclControlObject *pControl, LPOLEOBJECT pIObject);
	UINT ExtractMethodInfo(CDclControlObject *pControl, ITypeInfo *TheInfo);

	IDispatch *GetChildIDispatch(DISPID dispid);
	void GetRefGuid(AxPropertyDescriptor* pAProp, ITypeInfo* TheInfo, HREFTYPE hreftype, AxPropertyDescriptor *pProperty = NULL);
	BOOL ExtractComponentsFromTLB(CDclControlObject *pControl, CLSID clsid);

	void Initialize();
	virtual BOOL CreateCtrl(CLSID Clsid, CDclControlObject *pControl, int nID, CWnd *pParent);
	virtual BOOL CreateCtrl(CLSID Clsid, CDclControlObject *pControl, const RECT& rect, int nID, CWnd *pParent, bool bAddPropInfo);
	HRESULT GetProperty(AxPropertyDescriptor *axProp, CString &strReturnValue);
	void SetProperty(AxPropertyDescriptor *axProp, CString sNewValue);
	COleFont GetFont(DISPID dispid);
	void SetFont(DISPID dispid, LPDISPATCH newValue);
	unsigned long GetColor(DISPID dispid);
	void SetColor(DISPID dispid, unsigned long propVal);
	void LoadPictureFile(DISPID dispid, CString sFile, WORD flag);
	void LoadPicture(DISPID dispid, int nId);
	void SetPicture(DISPID dispid, LPDISPATCH newValue, WORD flag);
	BOOL GetPropertyPageCLSIDs( CArray< CLSID, CLSID& >& aclsidPages );
	void ShowPropertyPages();
	void SetBoolProperty(AxPropertyDescriptor *axProp, CString sNewValue);

	void SetRefImageList(DISPID dispid, LPDISPATCH newValue);
	void SetImageList(DISPID dispid, LPDISPATCH newValue);

// Overrides
	public:

// Implementation
public:
	void AddDispatchDriver( COleDispatchDriver* pDriver ) { mOleDispatchDrivers.AddTail(pDriver); }

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
