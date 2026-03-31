// HtmlCtrl.h : header file
//

#pragma once

#include "HtmlBrowser.h"
#include "DialogControl.h"
#include "AcadColorService.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl window

class CHtmlCtrl : public CHtmlBrowser, public CDialogControl
{
	CAcadColorService mColorService;

// Construction
public:
	CHtmlCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CHtmlCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	void HandleDpiChanged() override; //handle relayed WM_DPICHANGED_AFTERPARENT message
	bool ApplyProperty( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	
	HRESULT OnGetHostInfo(DOCHOSTUIINFO *pInfo) override;
	void OnDocumentComplete(LPCTSTR lpszURL) override;
	BOOL PreTranslateMessage( MSG* pMsg ) override;
	void PostNcDestroy() override;
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
