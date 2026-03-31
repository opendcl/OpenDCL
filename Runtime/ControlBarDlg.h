// ControlBarDlg.h : header file
//

#pragma once

#include "AcadDockBarHost.h"
#include "ArxDialogObject.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CControlBarDlg dialog

class CControlBarDlg : public CDialog, public CArxDialogObject
{
	CWnd* mpParent;
	CAcadDockBarHost& mHostControlBar;
	CPoint mptInitPos;
	bool mbKeepFocus;
	bool mbResizable;
	bool mbHiding;
	CAcadColorService mColorService;

// Construction
public:
	CControlBarDlg( TDclFormPtr pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CControlBarDlg();

// Atributes
public:
	bool IsKeepFocus() const { return mbKeepFocus; }

// CDialogObject overrides
public:
	CAcadColorService* GetColorService() override { return &mColorService; }
	FormType GetType() const override { return FrmControlBar; }
	CWnd* GetTopLevelWnd() override;
	bool IsModeless() const override { return true; }
	bool IsDockable() const override { return !IsFloating(); }
	bool IsResizable() const override { return mbResizable; }
	bool IsFloating() const override;
	bool CreateModeless( UINT nID ) override;
	void CloseDialog(int nStatus) override;
	bool CenterDialog() override;
	bool MoveDialog( long nNewLeft, long nNewTop ) override;
	bool ResizeDialog( long nNewWidth, long nNewHeight ) override;
	bool CenterAndResizeDialog( long nNewWidth, long nNewHeight ) override;
	CRect GetEffectiveWindowRect() const override;
	CRect GetEffectiveClientRect() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
protected:
	bool OnApplyCaption( TPropertyPtr pProp ) override; //Prop::Caption, Prop::TitleBarText
	bool OnApplyResizable( TPropertyPtr pProp ) override; //Prop::AllowResizing
	bool Create( CWnd* pParentWnd, UINT nID ) override { return false; }
	void OnFrameChanged() override; //called by member functions that change the non-client size
	void ApplyPosition() override; //move control window to new position

protected:
friend class CAcadDockBarHost;
	virtual void GetClientArea(CRect &rect);
	virtual bool OnClosing();
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();

protected:
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT_PTR nID );
	void PostNcDestroy() override;
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
