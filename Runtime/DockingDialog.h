// DockingDialog.h : header file
//

#pragma once

#include "AcadDockBarHost.h"
#include "ArxDialogObject.h"
#include "Resource.h"


#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CDockingDialog dialog

class CDockingDialog : public CDialog, public CArxDialogObject
{
	CWnd* mpParent;
	CAcadDockBarHost& mHostControlBar;
	bool mbKeepFocus;
	bool mbResizable;
	bool mbHiding;

// Construction
public:
	CDockingDialog( TDclFormPtr pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CDockingDialog();

// Atributes
public:
	bool IsKeepFocus() const { return mbKeepFocus; }

// CDialogObject overrides
public:
	virtual FormType GetType() const { return FrmDockableDlg; }
	virtual CWnd* GetTopLevelWnd();
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return !IsFloating(); }
	virtual bool IsResizable() const { return mbResizable; }
	virtual bool IsFloating() const;
	virtual bool CreateModeless( UINT nID );
	virtual void CloseDialog(int nStatus);
	virtual bool CenterDialog();
	virtual bool ResizeDialog( long nNewWidth, long nNewHeight );
	virtual bool CenterAndResizeDialog( long nNewWidth, long nNewHeight );
	virtual bool GetEffectiveWindowRect( CRect& rcDlg ) const;
	virtual bool GetEffectiveClientRect( CRect& rcDlg ) const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyResizable( TPropertyPtr pProp ); //Prop::Resizable
protected:
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
	virtual void OnFrameChanged(); //called by member functions that change the non-client size
	virtual void ApplyPosition(); //move control window to new position

protected:
friend class CAcadDockBarHost;
	virtual void GetClientArea(CRect &rect);
	virtual bool OnClosing();

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void PostNcDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
