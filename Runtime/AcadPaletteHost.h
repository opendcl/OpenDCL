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
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags);
	virtual bool OnClosing();

private:
	virtual bool CanFrameworkTakeFocus();
	virtual void DrawBorders(CDC* pDC, CRect& rect);
	virtual void OnUserSizing(UINT fwSide, LPRECT pRect);
	virtual DWORD RecalcDelayShow(AFX_SIZEPARENTPARAMS* lpLayout);
	virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );	
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
	virtual void GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight);	
	virtual void GetMinimumSize(CSize& size);
	virtual void GetMaximumSize(CSize& size);
	virtual void TitleBarLocationUpdated( AdUiTitleBarLocation newLoc );
	virtual BOOL Load(IUnknown*);
	virtual BOOL Save(IUnknown*);

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
	afx_msg LRESULT OnFrameChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void PostNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
};
