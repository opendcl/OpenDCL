// AxContainerCtrl.h : header file
//

#pragma once

#include "DialogControl.h"

class AxPropertyDescriptor;
class CControlPane;
class CDclControlObject;
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
	CAxContainerCtrl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true);
	CAxContainerCtrl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, CRect ArxRect, bool bCreate = true);
	virtual ~CAxContainerCtrl();

// Operations
public:
	CDclControlObject* GetOleControl() const { return mpTemplate; }

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

	void SetTooltipText( LPCTSTR pszText );
	CPPToolTip& GetToolTip() { return mToolTip; }

	void Initialize();
	virtual bool Create(CWnd* pParentWnd, UINT nID);
	bool Create(CWnd* pParentWnd, UINT nID, CRect ArxRect);
	unsigned long GetFlexGridColorProperty(AxPropertyDescriptor *axProp);//
	void SetFlexGridColorProperty(AxPropertyDescriptor *axProp, unsigned long newValue);//
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

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//OnChildNotify has not been recreated yet.
	afx_msg void OnPaint();//

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
