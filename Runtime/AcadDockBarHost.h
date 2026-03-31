// AcadDockBarHost.h : header file
//

#pragma once

#include "AcUiDock.h"

class CControlBarDlg;


/////////////////////////////////////////////////////////////////////////////
// CAcadDockBarHost dialog

class CAcadDockBarHost : public CAdUiDockControlBar
{
	CControlBarDlg* mpDlgObject;
	CWnd* mpParent;
	bool mbTrackingMouse;
	bool mbMouseLeft;
	bool mbInMenuLoop;
	HWND mhwndKeyboardFocus;

// Construction
public:
	CAcadDockBarHost( CControlBarDlg* pDlgObject, CWnd *pParent = NULL );
	virtual ~CAcadDockBarHost();

public:
	virtual bool Create( LPCTSTR lpszTitle, CRect rect, UINT nID );
	virtual void GetClientArea(CRect &rect);
	void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) override;
	bool OnClosing() override;

private:
	bool CanFrameworkTakeFocus() override;
	void OnUserSizing(UINT fwSide, LPRECT pRect) override;
	CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz ) override;
#if !defined(_GRXTARGET)
	void GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight) override;
#endif
	BOOL AddCustomMenuItems(LPARAM hMenu) override;

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMouseEnter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterMenuLoop(BOOL bPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bPopupMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void PostNcDestroy() override;
	afx_msg LRESULT OnFrameChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
};
