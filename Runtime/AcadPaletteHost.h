// AcadPaletteHost.h : header file
//

#pragma once

#include "AdUiPaletteSet.h"

class CPaletteDlg;


#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CAcadPaletteHost dialog

class CAcadPaletteHost : public CAdUiPaletteSet
{
	CPaletteDlg* mpDlgObject;
	CWnd* mpParent;
	bool mbTrackingMouse;
	bool mbMouseLeft;
	bool mbInMenuLoop;
	HWND mhwndKeyboardFocus;

// Construction
public:
	CAcadPaletteHost( CPaletteDlg* pDlgObject, CWnd *pParent = NULL );
	virtual ~CAcadPaletteHost();

public:
	virtual bool Create( LPCTSTR lpszTitle, CRect rect );
	virtual void GetClientArea( CRect& rect );
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags);
	virtual bool OnClosing();

private:
	virtual bool CanFrameworkTakeFocus();
	virtual void OnUserSizing(UINT fwSide, LPRECT pRect);
	virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );	
	virtual void GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight);	
	virtual void GetMinimumSize(CSize& size);
	virtual void GetMaximumSize(CSize& size);
	virtual void TitleBarLocationUpdated( AdUiTitleBarLocation newLoc );

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

  virtual BOOL OnCommand (WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMouseEnter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterMenuLoop(BOOL bPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bPopupMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnFrameChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void PostNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
