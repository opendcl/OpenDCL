// Splitter.h : header file
//

#pragma once

#include "ControlPane.h"

class CPropertyObject;
class CDclControlObject;

#define Splitter_Raised 0
#define Splitter_DoubleRaised 1
#define Splitter_Sunken 2

#define SPN_SIZED WM_USER + 1
#define CW_LEFTALIGN 1
#define CW_RIGHTALIGN 2
#define CW_TOPALIGN 3
#define CW_BOTTOMALIGN 4
#define SPS_VERTICAL 1
#define SPS_HORIZONTAL 2

typedef struct SPC_NMHDR
{
	NMHDR hdr;
	int delta;
} SPC_NMHDR;


/////////////////////////////////////////////////////////////////////////////
// CSplitter window

class CSplitter : public CWnd
{
// Construction
public:
	CSplitter();

// Attributes
public:
	CString m_ClassName;	
	HCURSOR hCursor;
	int m_nStyle;
	bool m_bInvokeWithSendString;
	CDclControlObject *m_ArxControl;
	int			m_nX, m_nY;
	int			m_nMin, m_nMax;
	int			m_nSavePos;		// Save point on the lbutton down 
	BOOL		m_bIsPressed;
	int			m_nType;

	CControlPane		*m_pControlPane;

// Operations
public:
	CPoint ReadPlacement();
	void SavePlacement();
	void DrawLine(CDC* pDC, int x, int y);
	void MoveWindowTo(CPoint pt);
	void SetRange(int nMin, int nMax);
	void SetRange(int nSubtraction, int nAddition, int nRoot);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitter)
	public:
	virtual BOOL Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSplitter();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplitter)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_BrdStyle;
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	
};
