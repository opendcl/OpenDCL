// BaseDlg.h : header file
//
// Modified for ODCL, based on gripper-enabled dialog class (originally named CBaseDlg)
// By Mike Scanlon, 8/26/98
// email: mrs3691@megahertz.njit.edu

#pragma once

#include "ArxDialogObject.h"
#include "AcadColorService.h"


#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CBaseDlg dialog

class CBaseDlg : public CDialog, public CArxDialogObject
{
	CAcadColorService mColorService;
	CPoint mptInitial;
	CSize msizeInitial;
	int mnInitialY;
	bool mbHasTitleBar;
	bool mbResizable;
	CSize msizeGrip;
	CRect mrectGrip;

// Construction
public:
	CBaseDlg( TDclFormPtr pSourceForm, UINT idd, CWnd* pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CBaseDlg();

// CDialogObject overrides
public:
	CAcadColorService* GetColorService() override { return &mColorService; }
	bool IsResizable() const override { return mbResizable; }
	bool OnApplyResizable( TPropertyPtr pProp ) override; //Prop::AllowResizing

	virtual const CDialogObject& GetDialogObject() const { return *this; }
	virtual CDialogObject& GetDialogObject() { return *this; }

protected:
	void SavePosition();
	void ReadPosition(POINT& ptTopLeft, SIZE& size) const;
	void UpdateGripPos();

protected:
	DECLARE_MESSAGE_MAP()

	void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	BOOL OnInitDialog() override;
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT_PTR nID );
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	void PostNcDestroy() override;
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
