// DclFormView.h : interface of the CDclFormView class
//


#pragma once

#include "FontSettings.h"


class COpenDCLDoc;
class CStudioDialogObject;


class CDclFormView : public CScrollView
{
	CStudioDialogObject* mpDialogObject;

	//DECLARE_DYNAMIC(CDclFormView)
public:
	CDclFormView( TDclFormPtr pSourceForm );
protected: // create from serialization only
	CDclFormView();
	DECLARE_DYNCREATE(CDclFormView)

protected: //only for use by main frame
	friend class CStudioFrame;
	void SetSourceForm( TDclFormPtr pSourceForm );

// Attributes
public:
	COpenDCLDoc* GetDocument() const;
	CStudioDialogObject* GetDialogObject() const { return mpDialogObject; }
	TProjectPtr GetProject() const;
	TDclFormPtr GetSourceForm() const;

// Operations
public:

// CView Overrides
protected:
	BOOL PreCreateWindow(CREATESTRUCT& cs) override;
	void OnDraw(CDC* pDC) override;  // overridden to draw this view
	void OnInitialUpdate() override; // called first time after construct
	void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) override;

// Implementation
public:
	virtual ~CDclFormView();
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

public:
	BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) override;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/) override;
	afx_msg void OnWindowNextForm();
	afx_msg void OnUpdateWindowNextForm(CCmdUI *pCmdUI);
	afx_msg void OnWindowPrevForm();
	afx_msg void OnUpdateWindowPrevForm(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in CDclFormView.cpp
inline COpenDCLDoc* CDclFormView::GetDocument() const
   { return reinterpret_cast<COpenDCLDoc*>(m_pDocument); }
#endif

