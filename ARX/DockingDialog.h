// DockingDialog.h : header file
//

#pragma once

#include "AcUiDock.h"
#include "ControlPane.h"
#include "Resource.h"

class CFontCollection;
class CAcadDocReactor;
class CDialogControl;
class CDclFormObject;
class CDclControlObject;

/////////////////////////////////////////////////////////////////////////////
// CDockingDialog dialog

class CDockingDialog : public CAdUiDockControlBar
{
	DECLARE_DYNAMIC(CDockingDialog);

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
	int					m_FloatingHeight;		
	int					m_FloatingWidth;
	int					m_DockedWidth;
	int					m_DockedHeight;	
	bool				m_bDockingSizeAdjusted;
	bool				m_bFloatingSizeAdjusted;

// Construction
public:
	CDockingDialog( CDclFormObject* pSourceForm );
	virtual ~CDockingDialog();

	//Attributes
public:
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
	enum { IDD = 13100 };

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
	virtual CSize CalcFloatingSize();
	virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );	
	virtual CSize CalcDockedSize();
	virtual void GetFloatingMinSize(long* pnMinWidth, long* pnMinHeight);	
	virtual void SizeChanged (CRect *lpRect, BOOL bFloating, int flags);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDockingDialog)	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void PostNcDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual bool OnClosing();
	virtual BOOL AddCustomMenuItems(LPARAM hMenu);
};
