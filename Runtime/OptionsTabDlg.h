// OptionsTabDlg.h : header file
//

#pragma once

#include "ArxDialogObject.h"
#include "AcadColorService.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// COptionsTabDlg dialog

class COptionsTabDlg : public CAcUiTabChildDialog, public CArxDialogObject
{
	CAcadColorService mColorService;

	enum { IDD = IDD_CFGTAB };

	// Construction
public:
	COptionsTabDlg(TDclFormPtr pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL) ;
	virtual ~COptionsTabDlg();

// CDialogObject overrides
public:
	FormType GetType() const override { return FrmOptionsTab; }
	bool IsModeless() const override { return true; }
	bool IsDockable() const override { return false; }
	bool IsResizable() const override { return true; }
	bool IsDirty() const override { return (__super::IsDirty() != FALSE); }
	bool SetDirty( bool bDirty = true ) override { __super::SetDirty( bDirty? TRUE : FALSE ); return true; }
	void CloseDialog(int nStatus) override;
protected:
	bool Create( CWnd* pParentWnd, UINT nID ) override { return false; }
	bool OnApplyResizable( TPropertyPtr pProp ) override { return true; }
	bool IsAsyncEvents() const override { return false; }

// CAcUiTabChildDialog overrides
	void OnMainDialogApply() override;
	void OnMainDialogCancel() override;
	void OnMainDialogOK() override;
	BOOL OnMainDialogHelp() override;
	void OnTabActivation (BOOL bActivate) override;
	BOOL OnTabChanging () override;

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	void PostNcDestroy() override;
	BOOL OnInitDialog() override;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT_PTR nID );
	afx_msg void OnSize(UINT nType, int cx, int cy);
	LRESULT OnRecalcLayout(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
