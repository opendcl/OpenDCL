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
	~CTabPageDlg();

// CDialogObject overrides
public:
	virtual FormType GetType() const { return FrmTabPage; }
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const { return false; }
	virtual void CloseDialog(int nStatus) {}
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
	virtual void ApplyPosition(); //move control window to new position

protected:
	DECLARE_MESSAGE_MAP()

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
