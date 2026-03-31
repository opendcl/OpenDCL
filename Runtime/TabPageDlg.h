// TabPageDlg.h : header file
//

#pragma once

#include "ArxDialogObject.h"
#include "AcadColorService.h"

class CDclFormObject;

typedef RefCountedPtr< class CTabPageDlg > TTabPagePtr;


/////////////////////////////////////////////////////////////////////////////
// CTabPageDlg dialog

class CTabPageDlg : public CDialog, public CArxDialogObject
{
	CAcadColorService mColorService;
	bool mbRecalcQueued;

// Construction
public:
	CTabPageDlg( TDclFormPtr pSourceForm, CTabCtrl* pTabCtrl, CRect rectPane, UINT& nId );
	virtual ~CTabPageDlg();

// CDialogObject overrides
public:
	FormType GetType() const override { return FrmTabPage; }
	bool IsModeless() const override { return true; }
	bool IsDockable() const override { return false; }
	bool IsResizable() const override { return false; }
	void CloseDialog(int nStatus) override {}
	CAcadColorService* GetColorService() override { return &mColorService; }
	bool Create( CWnd* pParentWnd, UINT nID ) override { return false; }
	void ApplyPosition() override; //move control window to new position

protected:
	DECLARE_MESSAGE_MAP()

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
