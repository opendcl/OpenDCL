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
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags);
	virtual bool OnClosing();

private:
	virtual bool CanFrameworkTakeFocus();
	virtual void DrawBorders(CDC* pDC, CRect& rect);
	virtual void OnUserSizing(UINT fwSide, LPRECT pRect);
	virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );	
	virtual void GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight);	
	virtual BOOL AddCustomMenuItems(LPARAM hMenu);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMouseEnter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterMenuLoop(BOOL bPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bPopupMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void PostNcDestroy();
	afx_msg LRESULT OnFrameChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
};
