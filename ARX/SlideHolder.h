// CSlideHolder.h : header file
//

#pragma once

#include "CxAcadSlide.h"
#include "OleOdcDropTarget.h"
#include "PPToolTip.h"

class CControlPane;
class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CSlideHolder window

class CSlideHolder : public CButton
{
// Construction
public:
	CSlideHolder();

// Attributes
public:
	
	CPPToolTip  		m_ToolTip;	
	bool				m_bInvokeWithSendString;	
	CRect				m_rcFocus;
	bool				m_bSelectedRect;
	COLORREF			m_HighlightColor;
	HBITMAP				m_hbmMem;
	CControlPane		*m_pParentPane;
	COLORREF			m_BkColor;
	BOOL				m_bMouseTracking;
	bool				m_bHasFocus;
// Operations
public:
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	void DrawLine(int sX, int sY, int eX, int eY, int nLineColor);
	void DrawFillRect(int sX, int sY, int eX, int eY, int nLineColor);
	void SetHighLight(int nColorIndex);
	void DrawASlide(int nX, int nY, int nSlideWidth, int nSlideHeight, CString sFileName);
	void DrawASlide(int nX, int nY, int nSlideWidth, int nSlideHeight, CString sFileName, CString sLibSldName);
	void RemoveHighLight();
	void SetAcadColor(long nColor);
	bool SetFileName(CString sFileName, bool slb, CString slbSldName);
	void Clear();
	void CopyDC();
	void SetDragnDrop(BOOL bRegister);

// Implementation
public:
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget m_DropTarget;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSlideHolder)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSlideHolder();

	// Generated message map functions
protected:
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
	//{{AFX_MSG(CSlideHolder)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void PaintCrontrol(CDC *pdc);
	//CString m_ClassName;
	CxAcadSlide m_SlideControl;
	CDclControlObject *m_ArxControl;	
};
