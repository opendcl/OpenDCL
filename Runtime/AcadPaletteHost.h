// AcadPaletteHost.h : header file
//

#pragma once

#include "AdUiPaletteSet.h"

class CPaletteDlg;


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

#ifdef _BRXTARGET
	CSize mszMRUSize;
#endif

// Construction
public:
	CAcadPaletteHost( CPaletteDlg* pDlgObject, CWnd *pParent = NULL );
	virtual ~CAcadPaletteHost();

public:
	virtual bool Create( LPCTSTR lpszTitle, CRect rect );
	virtual void GetClientArea( CRect& rect );
	void SizeChanged (CRect *lpRect, BOOL bFloating, int flags) override;
	bool OnClosing() override;
#if defined(_BRXTARGET) && (_BRXTARGET == 17)
	HICON SetIcon(HICON, BOOL) { return NULL; }
	HICON GetIcon(BOOL) const { return NULL; }
#endif

private:
	bool CanFrameworkTakeFocus() override;
	void OnUserSizing(UINT fwSide, LPRECT pRect) override;
	CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz ) override;
#if defined(_BRXTARGET) && (_BRXTARGET <= 15)
	CSize CalcDynamicLayout(int nLength, DWORD nMode) override;
#endif
#if !defined(_GRXTARGET)
	void GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight) override;
#endif
	void GetMinimumSize(CSize& size) override;
	void GetMaximumSize(CSize& size) override;
	void TitleBarLocationUpdated( AdUiTitleBarLocation newLoc ) override;
	BOOL Load(IUnknown*) override;
	BOOL Save(IUnknown*) override;

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnCommand (WPARAM wParam, LPARAM lParam) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnDestroy();
	afx_msg LRESULT OnMouseEnter(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterMenuLoop(BOOL bPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bPopupMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnFrameChanged(WPARAM wParam, LPARAM lParam);
	void PostNcDestroy() override;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
};
