// ControlHolder.h : header file
//

#pragma once


#include "ControlPane.h"
#include "GripRect.h"

#define nGripSizeConst  7
#define nNotSet -1
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

// Construction
protected:
	CControlHolder();
public:
	CControlHolder( CDclControlObject* pTemplate );
	virtual ~CControlHolder();

public:	
	bool m_bActiveXCtrl;

	// Attributes
public:
	CString m_ClassName;
	CFont m_Font;
	int m_ControlId;
	bool m_bSelected;
	int m_ControlType;
	BOOL m_bGripsCreate;
	CGripRect m_GripRect1;
	CGripRect m_GripRect2;
	CGripRect m_GripRect3;
	CGripRect m_GripRect4;
	CGripRect m_GripRect5;
	CGripRect m_GripRect6;
	CGripRect m_GripRect7;
	CGripRect m_GripRect8;

	RefCountedPtr< CPropertyObject > m_pLeftProp;
	RefCountedPtr< CPropertyObject > m_pTopProp;
	RefCountedPtr< CPropertyObject > m_pWidthProp;
	RefCountedPtr< CPropertyObject > m_pHeightProp;

public:
	const CDclControlObject* GetTemplate() const { return mpTemplate; }
	CDclControlObject* GetTemplate() { return mpTemplate; }
	TDialogControlPtr GetControl() const { return mpDlgControl; }
	void SetControl( TDialogControlPtr pControl ) { mpDlgControl = pControl; }

	void UpdateClientHeight();
	void CheckAutoSizeProp();
	void UpdateProperty(PropertyId nID);
	void ResetImageList(CWnd *pControl, int nID);
	CSize GetControlSize(CWnd *pControl, int nControlType);
	void UpdateChildControl();
	void SetupTreeControl(CTreeCtrl *pControl);
	CWnd* CreateComboBox(CDclControlObject *pArxObject);
	CWnd* CreateTextBox(CDclControlObject *pArxObject);
	bool CreateNewDialogControl();


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
	CWnd *GetChildControl();
	int GetId();
	void SetSelected(BOOL bSelected);
	void ReleaseSelection();
	void ForceGripsForward();
	void HideGrips();
	void CreateGrips();
	
	// ActiveX helping methods
	CAxContainerCtrl* GetActiveXCtrl();

	void SetColor(DISPID dispid, unsigned long ulColor);
	unsigned long GetColor(DISPID dispid);
	
	HRESULT GetProperty(AxPropertyDescriptor *axProp, CString &strReturnValue);
	void SetProperty( AxPropertyDescriptor *axProp, LPCTSTR pszNewValue );

	void ShowPropertyPages();

	void LoadPictureFile(DISPID dispid, CString sFile, WORD flag);
	void SetPicture(DISPID dispid, LPDISPATCH newValue, WORD flag);

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CControlHolder)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar);
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CControlHolder)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void ParentNotify(UINT /*message*/, LPARAM /*lParam*/);
};
