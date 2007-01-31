// ControlHolder.h : header file
//

#pragma once


#include "GripRect.h"

#define nChildCtrlIdStart  100
#define nGripSizeConst  7
#define nNotSet -1
#define nDeGridIDCounter 80

class CPropertyObject;
class CAxContainer;
class AxPropertyDescriptor;


/////////////////////////////////////////////////////////////////////////////
// CControlHolder window

class CControlHolder : public CStatic
{
// Construction
public:
	CControlHolder();
protected:
	DECLARE_DYNCREATE(CControlHolder)
// ActiveX attributes
public:	
	class CDclControlObject *m_pArxObject;
	bool m_bActiveXCtrl;
// ActiveX  opertations
public:
	HRESULT SaveToStream( IStream* pStream );
	
	
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

	CPropertyObject *m_pLeftProp;
	CPropertyObject *m_pTopProp;
	CPropertyObject *m_pWidthProp;
	CPropertyObject *m_pHeightProp;

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
	CAxContainer* GetActiveXCtrl();

	void SetColor(DISPID dispid, unsigned long ulColor);
	unsigned long GetColor(DISPID dispid);
	
	HRESULT GetProperty(AxPropertyDescriptor *axProp, CString &strReturnValue);
	void SetProperty(AxPropertyDescriptor *axProp, CString sNewValue);

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

// Implementation
public:
	virtual ~CControlHolder();

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
};
