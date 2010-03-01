// PaletteDlg.h : header file
//

#pragma once

#include "ArxDialogObject.h"
#include "AcadPaletteHost.h"
#include "AcadColorService.h"


#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CPaletteDlg dialog

class CPaletteDlg : public CDialog, public CArxDialogObject
{
	CWnd* mpParent;
	CAcadPaletteHost& mHostPaletteSet;
	CPoint mptInitPos;
	bool mbKeepFocus;
	bool mbResizable;
	CAcadColorService mColorService;

// Construction
public:
	CPaletteDlg( TDclFormPtr pSourceForm, CWnd* pParent = NULL, DialogParams* pParams = NULL );
	virtual ~CPaletteDlg();

// Atributes
public:
	bool IsKeepFocus() const { return mbKeepFocus; }

// CDialogObject overrides
public:
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual FormType GetType() const { return FrmPaletteDlg; }
	virtual CWnd* GetTopLevelWnd();
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return false; }
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
	virtual bool OnApplyProperty( TPropertyPtr pProp );
protected:
	virtual bool OnApplyCaption( TPropertyPtr pProp ); //Prop::Caption, Prop::TitleBarText
	virtual bool OnApplyResizable( TPropertyPtr pProp ); //Prop::AllowResizing
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
	virtual void OnFrameChanged(); //called by member functions that change the non-client size
	virtual void ApplyPosition(); //move control window to new position
	virtual bool OnApplyIcon( TPropertyPtr pProp ); //Prop::TitleBarIcon

protected:
friend class CAcadPaletteHost;
	virtual bool OnClosing();
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();

protected:
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
