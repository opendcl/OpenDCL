// CSlideHolder.h : header file
//

#pragma once

#include "CxAcadSlide.h"
#include "OleOdcDropTarget.h"
#include "PPToolTip.h"
#include "ArxDialogControl.h"

class CControlPane;
class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CSlideHolder window

class CSlideHolder : public CButton, public CArxDialogControl
{
	CxAcadSlide mSlideCtrl;

public:
	bool				m_bInvokeWithSendString;	
	CRect				m_rcFocus;
	bool				m_bSelectedRect;
	COLORREF			m_HighlightColor;
	HBITMAP				m_hbmMem;
	COLORREF			m_BkColor;
	BOOL				m_bMouseTracking;
	bool				m_bHasFocus;
	COleOdcDropTarget m_DropTarget;

// Construction
public:
	CSlideHolder( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID );
	virtual ~CSlideHolder();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyCaption( TPropertyPtr pProp ) { return true; }

// Implementation
public:
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
private:
	void PaintControl(CDC *pdc);

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
protected:
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
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
	afx_msg void PostNcDestroy();

protected:
	DECLARE_MESSAGE_MAP()
};
