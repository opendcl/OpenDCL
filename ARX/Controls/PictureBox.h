// PictureBox.h : header file
//

#pragma once

#include "OleOdcDropTarget.h"
#include "PPToolTip.h"
#include "ControlPane.h"

class CDclControlObject;
class CPictureObject;

const int nUseBackColor = -100;


/////////////////////////////////////////////////////////////////////////////
// CPictureBox window

class CPictureBox : public CButton
{
// Construction
public:
	CBrush m_brBackground;
	//CPen StaticPen;

	CPictureBox();
	void AutoSize();
	void Refresh(CDC *pdc);
	void SetAcadColor(long nColor);
	void SetPictureID(int nPictureID);
	void SetPictureBlank();
	void Clear();
	void Refresh();
	void DrawLine(int sX, int sY, int eX, int eY, COLORREF rgb);
	void DrawPoint(int nX, int nY, COLORREF rgb);
	void DrawArc(int sX, int sY, int eX, int eY, int saX, int saY, int eaX, int eaY, COLORREF rgb);
	void DrawCircle(int sX, int sY, int eX, int eY, COLORREF rgb);
	void DrawFillRect(int sX, int sY, int eX, int eY, COLORREF rgb);
	void DrawHatchRect(int sX, int sY, int eX, int eY, COLORREF rgb, int nHatchPattern);
	int  DrawWrappedText(int sX, int sY, int eX, int nForeColor, int nBackColor, CString sText, CString sJustification);
	void DrawText(int sX, int sY, int nForeColor, int nBackColor, CString sText, CString sJustification, int nEnabled);
	void DrawEdge(int sX, int sY, int eX, int eY, int nEdge);
	void DrawFocusRect(int sX, int sY, int eX, int eY);
	void DrawRect(int sX, int sY, int eX, int eY, COLORREF rgb);
	void GetTextExtent(CString sText);
	void PaintPicture(int sX, int sY, int nPictureID, int nEnabled, int nUseMask);
	void LoadPictureFile(LPCTSTR szFile, bool bStretch = false);
	void CopyDC();
	void SetTooltipText(CString* spText, BOOL bActivate = TRUE);
	void InitToolTip();
	CPPToolTip m_ToolTip;
	void SetDragnDrop(BOOL bRegister);

// Implementation
public:
	//*****
    // COleOdcDropTarget, derived from COleDropTarget gives us
    // the functionality to be an OLE drop target.
    //*****
    COleOdcDropTarget m_DropTarget;
	
// Attributes
public:
	CControlPane	*m_pParentPane;
	bool			m_bHasFocus;
	bool			m_bInvokeWithSendString;
	int				m_cxIcon;
	int				m_cyIcon;
	BOOL			m_AutoSize;
	int				m_BorderStyle;
	CProject*	m_pProject;
	COLORREF		m_BkColor;
	BOOL			m_bMouseTracking;
	LPPICTURE		m_pPictureHolder;
	HBITMAP			m_hbmMem;
	bool			m_bStretchLoadedPicture;
	
protected:
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureBox)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CProject *pProject, CWnd* pParentWnd, UINT nID );
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetProperty(int nID);
	void SetAllProperties();
	virtual ~CPictureBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPictureBox)
	afx_msg void OnSysColorChange();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);	
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClicked();
	afx_msg void OnPaint();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int					m_ButtonStyle;
	//CPictureHolder		*m_pPropPictureHolder;	
	int					m_PictureID;
	CDclControlObject	*m_ArxControl;
	CPictureObject		*m_pPicture;
	bool				m_bLoadPicture;
	UINT				m_hPos;
};
