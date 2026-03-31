// ModelessDlg.h : header file
//

#pragma once

#include "BaseDlg.h"


/////////////////////////////////////////////////////////////////////////////
// CModelessDlg dialog

class CModelessDlg : public CBaseDlg
{
	CWnd* mpParent;
	bool mbKeepFocus;
	bool mbTrackingMouse;
	bool mbMouseLeft;
	bool mbInMenuLoop;
	HWND mhwndKeyboardFocus;

// Construction	
public:
	CModelessDlg( TDclFormPtr pSourceForm, CWnd* pParent = NULL, DialogParams* pParams = NULL );
	~CModelessDlg();

// Atributes
public:
	bool IsKeepFocus() const { return mbKeepFocus; }

// CDialogObject overrides
public:
	FormType GetType() const override { return FrmModelessDlg; }
	bool IsModeless() const override { return true; }
	bool IsDockable() const override { return false; }
	bool CreateModeless( UINT nID ) override;
	void CloseDialog(int nStatus) override;
protected:
	bool Create( CWnd* pParentWnd, UINT nID ) override { return false; }
	bool ApplyProperty( TPropertyPtr pProp ) override;

// CDialog Overrides
protected:
	void OnOK() override;
	void OnCancel() override;

protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg LRESULT onAcadKeepFocus(WPARAM, LPARAM);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMouseEnter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterMenuLoop(BOOL bPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bPopupMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};
