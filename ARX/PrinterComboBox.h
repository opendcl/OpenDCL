// PrinterComboBox.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;
class CControlPane;


/////////////////////////////////////////////////////////////////////////////
// CPrinterComboBox window

class CPrinterComboBox : public CComboBox
{
// Construction
public:
	CPrinterComboBox();
	CPrinterComboBox( CComboBox*& pPaperSizesCombo );

// Attributes
public:
	bool		m_bInvokeWithSendString;
	CImageList	m_img;
	CComboBox*& mpPaperSizesCombo; //hokey kludge so the printer combo can update the paper size combo [ORW]
// ListCtrl edit data members
	bool m_bESC;
	virtual BOOL Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID);

// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	int CreatePrinterList();
	CString GetPlottersPath();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrinterComboBox)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPrinterComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPrinterComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelchange();
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
