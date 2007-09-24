// ControlHolder.h : header file
//

#pragma once


#include "ControlPane.h"
#include "ControlGripper.h"

#define nGripSizeConst  7
#define nDeGridIDCounter 80

class CPropertyObject;
class CAxContainerCtrl;
class AxPropertyDescriptor;


/////////////////////////////////////////////////////////////////////////////
// CControlHolder window

class CControlHolder : public CStatic, public CControlPane
{
	TDclControlPtr mpTemplate;
	TDialogControlPtr mpDlgControl;
	int mnControlId;

// Construction
protected:
	CControlHolder();
public:
	CControlHolder( TDclControlPtr pTemplate );
	virtual ~CControlHolder();

	// Attributes
protected:
	CControlGripper mGripper;

public:
	const TDclControlPtr GetTemplate() const { return mpTemplate; }
	TDclControlPtr GetTemplate() { return mpTemplate; }
	TDialogControlPtr GetControlWnd() const { return mpDlgControl; }
	void SetControl( TDialogControlPtr pControl ) { mpDlgControl = pControl; }

	void UpdateProperty(Prop::Id nID);
	void UpdateChildControl();
	bool CreateNewDialogControl();
	void AutoSize();

protected:
	void SetupTreeControl(CTreeCtrl *pControl);
	void ResetImageList(CWnd *pControl, int nID);
	CSize GetControlSize( CWnd* pControl, ControlType nControlType );
	TDialogControlPtr CreateComboBox( TDclControlPtr pTemplate );
	TDialogControlPtr CreateTextBox( TDclControlPtr pTemplate );

// CControlPane
public:
	virtual bool FindControl(HWND hwndControl, /*out*/ CString& sControlName) const { return false; }
	virtual TDialogControlPtr FindControl(HWND hwndControl) const { return NULL; }
	virtual TDialogControlPtr FindControl( LPCTSTR pszControlName, ControlType type = CtlInvalid ) const { return NULL; }
protected:
	virtual TDialogControlPtr CreateNewDialogControl( TDclControlPtr pTemplate, UINT nID ) { return NULL; }

public:
	HRESULT SaveToStream( IStream* pStream );

// Operations
public:
	CWnd* GetControlWnd() { return mpDlgControl? mpDlgControl->GetControlWnd() : NULL; }
	int GetId() const { return mnControlId; }

	// Grip interface
public:
	void SetSelected();
	void HideGrips() { mGripper.Hide(); }
	bool IsSelected() { return mGripper.IsVisible(); }

protected:
	void ForceGripsForward() { mGripper.MoveToTop(); }
	
	// ActiveX helping methods
public:
	CAxContainerCtrl* GetActiveXCtrl();
	void SetColor(DISPID dispid, unsigned long ulColor);
	unsigned long GetColor(DISPID dispid);
	HRESULT GetProperty(AxPropertyDescriptor *axProp, CString &strReturnValue);
	void SetProperty( AxPropertyDescriptor *axProp, LPCTSTR pszNewValue );
	void ShowPropertyPages();
	void LoadPictureFile(DISPID dispid, CString sFile, WORD flag);
	void SetPicture(DISPID dispid, LPDISPATCH newValue, WORD flag);

// Overrides
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar);

protected:
	DECLARE_MESSAGE_MAP()

	// Generated message map functions
protected:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
