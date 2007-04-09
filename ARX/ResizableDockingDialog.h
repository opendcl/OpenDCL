// CResizableDockingDialog.h : header file
//

#pragma once

#include "Resource.h"
#include "ArxDialogObject.h"

class CDialogControl;
class CDclFormObject;
class CDclControlObject;
class CDialogControl;
class CFontCollection;
class CAcadDocReactor;


class CResizableDockingDialogX : public CArxDialogObject
{
	friend class CResizableDockingDialog;
	CResizableDockingDialog* mpOwner;
protected:
	CResizableDockingDialogX( CResizableDockingDialog& Owner, CDclFormObject* pDclForm );
	~CResizableDockingDialogX();

	virtual DclFormType GetType() const;
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return true; }
	virtual bool IsResizable() const { return true; }
	virtual HWND GetHWnd() const;
	virtual bool IsFloating() const;
	virtual bool CreateModeless( UINT nID ) const;
	virtual void CloseDialog(int nStatus) const;
	virtual bool GetWindowRect( CRect& rcDlg ) const;
	virtual bool GetClientRect( CRect& rcDlg ) const;
};


/////////////////////////////////////////////////////////////////////////////
// CResizableDockingDialog dialog

class CResizableDockingDialog : public CAdUiDockControlBar
{
	CWnd* mpParent;
	CResizableDockingDialogX mDialogX;
	bool mbClosing;
	bool mbHiding;
	bool mbTrackingMouse;
	bool mbInMenuLoop;

public:
	CAcadDocReactor		*m_pDocToModReactor;

// Construction
public:
	CResizableDockingDialog( CDclFormObject* pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CResizableDockingDialog();

public:
	CDialogObject& GetDialogObject() { return mDialogX; }
	const CDialogObject& GetDialogObject() const { return mDialogX; }

public:
	virtual void GetClientArea(CRect &rect);
	virtual bool Create( LPCTSTR lpszTitle, CRect rect, UINT nID );

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
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnterMenuLoop(BOOL bPopupMenu);
	afx_msg void OnExitMenuLoop(BOOL bPopupMenu);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
