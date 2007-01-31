// ChildFrm.h : interface of the CChildFrame class
//

#pragma once

class CObjectDCLView;


class CChildFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();

// Attributes
public:
	CRect m_rcStartup;
	CSize m_StartupSize;
	CObjectDCLView *m_pChildView;
	CString m_Title;
	HICON m_hIcon;
	bool m_bResizable;
	HCURSOR m_ArrowCursor;
	HCURSOR m_CrossCursor;
	HCURSOR m_ResizeCursor;
	
// Operations
public:
	void ResetFrame();
	void SetTitleBarIcon(int nPictureID);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	//{{AFX_MSG(CChildFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditDelete();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
