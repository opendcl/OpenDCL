// OdclLayerCombo.h : header file
//

#pragma once

#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// OdclLayerCombo window

class OdclLayerCombo : public CComboBox
{
// Construction
public:
	OdclLayerCombo();

// Attributes
public:
	CImageList m_Images;
	bool  m_bESC;	
	CPPToolTip m_ToolTip;
	bool m_bInvokeWithSendString;
	
// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	
	void GetVisibility(int nItem, bool &bFrozen, bool &bOn);
	void DrawColor(CDC* pDC, CRect rc, int nColor);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(OdclLayerCombo)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	BOOL Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID);
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );	
	//}}AFX_VIRTUAL
// Implementation
public:
	virtual ~OdclLayerCombo();

	// Generated message map functions
protected:
	//{{AFX_MSG(OdclLayerCombo)
	afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);	
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
	afx_msg void OnDestroy();	
	afx_msg void OnKillfocus();
	afx_msg void OnSetfocus();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CDclControlObject *m_ArxControl;
};
