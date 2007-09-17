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
	CDclControlObject* mpTemplate;
	TDialogControlPtr mpDlgControl;
	int mnControlId;

// Construction
protected:
	CControlHolder();
public:
	CControlHolder( CDclControlObject* pTemplate );
	virtual ~CControlHolder();

	// Attributes
protected:
	CControlGripper mGripper;

public:
	const CDclControlObject* GetTemplate() const { return mpTemplate; }
	CDclControlObject* GetTemplate() { return mpTemplate; }
	TDialogControlPtr GetControl() const { return mpDlgControl; }
	void SetControl( TDialogControlPtr pControl ) { mpDlgControl = pControl; }

	void UpdateProperty(Prop::Id nID);
	void UpdateChildControl();
	bool CreateNewDialogControl();
	void AutoSize();

protected:
	void SetupTreeControl(CTreeCtrl *pControl);
	void ResetImageList(CWnd *pControl, int nID);
	CSize GetControlSize( CWnd* pControl, ControlType nControlType );
	TDialogControlPtr CreateComboBox( CDclControlObject* pTemplate );
	TDialogControlPtr CreateTextBox( CDclControlObject* pTemplate );

// CControlPane
public:
	virtual bool FindControl(HWND hwndControl, /*out*/ CString& sControlName) const { return false; }
	virtual TDialogControlPtr FindControl(HWND hwndControl) const { return NULL; }
	virtual TDialogControlPtr FindControl( LPCTSTR pszControlName, ControlType type = CtlInvalid ) const { return NULL; }
protected:
	virtual TDialogControlPtr CreateNewDialogControl( CDclControlObject* pTemplate, UINT nID ) { return NULL; }

public:
	HRESULT SaveToStream( IStream* pStream );

// Operations
public:
	CWnd* GetControl() { return mpDlgControl? mpDlgControl->GetControl() : NULL; }
	int GetId() const { return mnControlId; }
	void SetSelected();

	// Grip interface
public:
	void HideGrips() { mGripper.Hide(); }
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
