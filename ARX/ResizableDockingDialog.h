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
	virtual bool CreateModeless() const;
	virtual void CloseDialog(int nStatus) const;
	virtual bool GetWindowRect( CRect& rcDlg ) const;
	virtual bool GetClientRect( CRect& rcDlg ) const;
};


/////////////////////////////////////////////////////////////////////////////
// CResizableDockingDialog dialog

class CResizableDockingDialog : public CAdUiDockControlBar
{
	CResizableDockingDialogX mDialogX;

	//DECLARE_DYNAMIC(CResizableDockingDialog);

public:
	CAcadDocReactor		*m_pDocToModReactor;
	bool				m_bClosing;

	enum { IDD = 13101 };

// Construction
public:
	CResizableDockingDialog( CDclFormObject* pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CResizableDockingDialog();

public:
	CDialogObject& GetDialogObject() { return mDialogX; }
	const CDialogObject& GetDialogObject() const { return mDialogX; }

public:
	void GetClientArea(CRect &rect);

// Overrides
	public:
	virtual BOOL Create(CWnd*pParent, LPCTSTR lpszTitle, CRect rect);
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
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void PostNcDestroy();

protected:
	DECLARE_MESSAGE_MAP()
};
