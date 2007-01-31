// CResizableDockingDialog.h : header file
//

#pragma once

#include "Resource.h"
#include "ControlPane.h"

class CDialogControl;
class CDclFormObject;
class CDclControlObject;
class CDialogControl;
class CFontCollection;
class CAcadDocReactor;


/////////////////////////////////////////////////////////////////////////////
// CResizableDockingDialog dialog

class CResizableDockingDialog : public CAdUiDockControlBar
{
	DECLARE_DYNAMIC(CResizableDockingDialog);

protected:
	CDclFormObject* mpSourceForm;
	CControlPane mControlPane;
	CDclControlObject* mpControl;

public:
	CString				m_sProjectName;
	CString				m_sDialogName;
	CList<CArxDialogControl*>			m_ControlCol;
	CFontCollection		*m_pFontCollection;
	CAcadDocReactor		*m_pDocToModReactor;
	bool				m_bClosing;

// Construction
public:
	CResizableDockingDialog( CDclFormObject* pSourceForm );
	virtual ~CResizableDockingDialog();

	//Attributes
	const CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	CDclFormObject* GetSourceForm() { return mpSourceForm; }
	const CControlPane& GetControlPane() const { return mControlPane; }
	CControlPane& GetControlPane() { return mControlPane; }
	const CDclControlObject* GetControl() const { return mpControl; }
	CDclControlObject* GetControl() { return mpControl; }


public:
	void SetDclForm(CDclFormObject *pDclFormObject);
	void GetClientArea(CRect &rect);
	
	void CleanupDockable();
// Dialog Data
	//{{AFX_DATA(CResizableDockingDialog)
	enum { IDD = 13101 };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CResizableDockingDialog)
	public:
	virtual BOOL Create(CWnd*pParent, LPCTSTR lpszTitle, CRect rect);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL
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
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CResizableDockingDialog)	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void PostNcDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual bool OnClosing();
	virtual BOOL AddCustomMenuItems(LPARAM hMenu);
};
