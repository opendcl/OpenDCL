// ModelessDlg.h : header file
//

#pragma once

#include "BaseDlg.h"

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


#ifndef WM_ACAD_MFC_BASE
#define WM_ACAD_MFC_BASE        (1000)
#endif

#ifndef WM_ACAD_KEEPFOCUS
#define WM_ACAD_KEEPFOCUS       (WM_ACAD_MFC_BASE + 1)
#endif


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
	virtual FormType GetType() const { return FrmModelessDlg; }
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return false; }
	virtual bool CreateModeless( UINT nID );
	virtual void CloseDialog(int nStatus);
protected:
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
	virtual bool OnApplyProperty( TPropertyPtr pProp );

// CDialog Overrides
protected:
	virtual void OnOK();
	virtual void OnCancel();

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
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
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
};
