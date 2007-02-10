// PrinterComboBox.h : header file
//

#pragma once

#include "PPToolTip.h"
#include "ArxDialogControl.h"

class CDclControlObject;
class CControlPane;


class CPrinterComboControlX : public CArxDialogControl
{
public:
	CPrinterComboControlX( CDclControlObject* pTemplate, CControlPane* pPane, CWnd* pWnd )
		: CArxDialogControl( pTemplate, pPane, pWnd ) {}
	virtual ~CPrinterComboControlX() {}

	// attributes
	virtual CRect GetWndRect() const; //get window position from properties
	virtual DWORD GetWndStyle() const; //get window style from properties
};


/////////////////////////////////////////////////////////////////////////////
// CPrinterComboBox window

class CPrinterComboBox : public CComboBox
{
protected:
	CPrinterComboControlX mControlX;
	CDclControlObject* mpSourceControl;	
	CControlPane* mpControlPane;

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

// Interface
public:
	CArxDialogControl& GetDialogControl() { return mControlX; }
	const CArxDialogControl& GetDialogControl() const { return mControlX; }

// Operations
public:
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool Create( CWnd* pParentWnd, UINT nID, CRect rc );

// Implementation
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
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
