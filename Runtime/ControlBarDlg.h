// ControlBarDlg.h : header file
//

#pragma once

#include "AcadDockBarHost.h"
#include "ArxDialogObject.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CControlBarDlg dialog

class CControlBarDlg : public CDialog, public CArxDialogObject
{
	CWnd* mpParent;
	CAcadDockBarHost& mHostControlBar;
	CPoint mptInitPos;
	bool mbKeepFocus;
	bool mbResizable;
	bool mbHiding;
	CAcadColorService mColorService;

// Construction
public:
	CControlBarDlg( TDclFormPtr pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CControlBarDlg();

// Atributes
public:
	bool IsKeepFocus() const { return mbKeepFocus; }

// CDialogObject overrides
public:
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual FormType GetType() const { return FrmControlBar; }
	virtual CWnd* GetTopLevelWnd();
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return !IsFloating(); }
	virtual bool IsResizable() const { return mbResizable; }
	virtual bool IsFloating() const;
	virtual bool CreateModeless( UINT nID );
	virtual void CloseDialog(int nStatus);
	virtual bool CenterDialog();
	virtual bool MoveDialog( long nNewLeft, long nNewTop );
	virtual bool ResizeDialog( long nNewWidth, long nNewHeight );
	virtual bool CenterAndResizeDialog( long nNewWidth, long nNewHeight );
	virtual CRect GetEffectiveWindowRect() const;
	virtual CRect GetEffectiveClientRect() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );
protected:
	virtual bool OnApplyResizable( TPropertyPtr pProp ); //Prop::AllowResizing
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
	virtual void OnFrameChanged(); //called by member functions that change the non-client size
	virtual void ApplyPosition(); //move control window to new position

protected:
friend class CAcadDockBarHost;
	virtual void GetClientArea(CRect &rect);
	virtual bool OnClosing();
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT_PTR nID );
	afx_msg void PostNcDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
