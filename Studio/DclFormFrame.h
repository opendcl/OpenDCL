// DclFormFrame.h : interface of the CDclFormFrame class
//


#pragma once

class CStudioDialogObject;


class CDclFormFrame : public CMDIChildWnd
{
	CStudioDialogObject* mpDlgObject;

protected: // create from serialization only
	CDclFormFrame();
	DECLARE_DYNCREATE(CDclFormFrame)

// Attributes
public:
	CStudioDialogObject* GetDialogObject() const { return mpDlgObject; }
protected:
	friend class CStudioFrame;
	void SetDialogObject( CStudioDialogObject* pDlgObject ) { mpDlgObject = pDlgObject; }

// Operations
public:

// Overrides
public:
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;

// Implementation
public:
	virtual ~CDclFormFrame();
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnWindowMinimize();
	afx_msg void OnUpdateWindowMinimize(CCmdUI *pCmdUI);
	afx_msg void OnWindowMaximize();
	afx_msg void OnUpdateWindowMaximize(CCmdUI *pCmdUI);
	afx_msg void OnWindowRestore();
	afx_msg void OnUpdateWindowRestore(CCmdUI *pCmdUI);
};
