// DockingDialog.h : header file
//

#pragma once

#include "AcUiDock.h"
#include "Resource.h"
#include "ArxDialogObject.h"

class CFontCollection;
class CDialogControl;
class CDclFormObject;
class CDclControlObject;


class CDockingDialogX : public CArxDialogObject
{
	friend class CDockingDialog;
	CDockingDialog* mpOwner;
protected:
	CDockingDialogX( CDockingDialog& Owner, CDclFormObject* pDclForm );
	~CDockingDialogX();

	virtual DclFormType GetType() const;
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return true; }
	virtual bool IsResizable() const { return false; }
	virtual HWND GetHWnd() const;
	virtual bool IsFloating() const;
	virtual bool CreateModeless( UINT nID ) const;
	virtual void CloseDialog(int nStatus);
	virtual bool GetWindowRect( CRect& rcDlg ) const;
	virtual bool GetClientRect( CRect& rcDlg ) const;
};


/////////////////////////////////////////////////////////////////////////////
// CDockingDialog dialog

class CDockingDialog : public CAdUiDockControlBar
{
	CWnd* mpParent;
	CDockingDialogX mDialogX;
	bool mbHiding;
	bool mbTrackingMouse;
	bool mbInMenuLoop;
	HWND mhwndKeyboardFocus;

public:
	int					m_FloatingHeight;		
	int					m_FloatingWidth;
	int					m_DockedWidth;
	int					m_DockedHeight;	
	bool				m_bDockingSizeAdjusted;
	bool				m_bFloatingSizeAdjusted;

// Construction
public:
	CDockingDialog( CDclFormObject* pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CDockingDialog();

public:
	CDialogObject& GetDialogObject() { return mDialogX; }
	const CDialogObject& GetDialogObject() const { return mDialogX; }

public:
	virtual bool Create( LPCTSTR lpszTitle, CRect rect, UINT nID );
	virtual void GetClientArea(CRect &rect);

// Overrides
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

private:
	// this override is to make the any control
	// that extends beyond the dockbar to be
	// selectable. Otherwise, the default
	// is to restore Acad cursor so the portion
	// is repainted immediately.
	bool CanFrameworkTakeFocus();
	virtual void OnUserSizing(UINT nSide, LPRECT pRect);
	virtual CSize CalcFloatingSize();
	virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );	
	virtual CSize CalcDockedSize();
	virtual void GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight);	
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags);
	virtual bool OnClosing();
	virtual BOOL AddCustomMenuItems(LPARAM hMenu);

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void PostNcDestroy();
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterMenuLoop(BOOL bPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bPopupMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
