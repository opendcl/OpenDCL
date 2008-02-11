// AcadDockBarHost.h : header file
//

#pragma once

#include "AcUiDock.h"

class CDockingDialog;


#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CAcadDockBarHost dialog

class CAcadDockBarHost : public CAdUiDockControlBar
{
	CDockingDialog* mpDlgObject;
	CWnd* mpParent;
	bool mbTrackingMouse;
	bool mbInMenuLoop;
	HWND mhwndKeyboardFocus;

// Construction
public:
	CAcadDockBarHost( CDockingDialog* pDlgObject, CWnd *pParent = NULL );
	virtual ~CAcadDockBarHost();

public:
	virtual bool Create( LPCTSTR lpszTitle, CRect rect, UINT nID );
	virtual void GetClientArea(CRect &rect);
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags);
	virtual bool OnClosing();

private:
	virtual bool CanFrameworkTakeFocus();
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
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void PostNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
