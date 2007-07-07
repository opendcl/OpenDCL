// PrinterComboBox.h : header file
//

#pragma once

#include "PPToolTip.h"
#include "ArxDialogControl.h"

class CDclControlObject;
class CControlPane;


/////////////////////////////////////////////////////////////////////////////
// CPrinterComboBox window

class CPrinterComboBox : public CComboBox, public CArxDialogControl
{
public:
	bool		m_bInvokeWithSendString;
	CImageList	m_img;

	// ListCtrl edit data members
	bool m_bESC;

// Construction
public:
	CPrinterComboBox( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID );
	CPrinterComboBox( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID, CRect rc );
	virtual ~CPrinterComboBox();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool Create( CWnd* pParentWnd, UINT nID, CRect rc );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;

// Operations
public:

// Implementation
public:
	int CreatePrinterList();
	CString GetPlottersPath();
protected:
	CComboBox* FindPaperSizesCombo() const;

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelchange();
	afx_msg void OnDestroy();
	afx_msg void PostNcDestroy();

protected:
	DECLARE_MESSAGE_MAP()
};
