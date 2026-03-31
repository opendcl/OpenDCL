// PaletteDlg.h : header file
//

#pragma once

#include "ArxDialogObject.h"
#include "AcadPaletteHost.h"
#include "AcadColorService.h"


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
	CAcadColorService* GetColorService() override { return &mColorService; }
	FormType GetType() const override { return FrmPaletteDlg; }
	CWnd* GetTopLevelWnd() override;
	bool IsModeless() const override { return true; }
	bool IsDockable() const override { return false; }
	bool IsResizable() const override { return mbResizable; }
	bool IsFloating() const override;
	bool CreateModeless( UINT nID ) override;
	void CloseDialog(int nStatus) override;
	bool CenterDialog() override;
	bool MoveDialog( long nNewLeft, long nNewTop ) override;
	bool ResizeDialog( long nNewWidth, long nNewHeight ) override;
	bool CenterAndResizeDialog( long nNewWidth, long nNewHeight ) override;
	CRect GetEffectiveWindowRect() const override;
	CRect GetEffectiveClientRect() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
protected:
	bool OnApplyCaption( TPropertyPtr pProp ) override; //Prop::Caption, Prop::TitleBarText
	bool OnApplyResizable( TPropertyPtr pProp ) override; //Prop::AllowResizing
	bool Create( CWnd* pParentWnd, UINT nID ) override { return false; }
	void OnFrameChanged() override; //called by member functions that change the non-client size
	void ApplyPosition() override; //move control window to new position
	bool OnApplyIcon( TPropertyPtr pProp ) override; //Prop::TitleBarIcon
	BOOL HandleEraseBkgnd( CDC* pDC ) override;

protected:
friend class CAcadPaletteHost;
	virtual bool OnClosing();
	virtual void OnMouseEnter();
	virtual void OnMouseLeave();

protected:
	DECLARE_MESSAGE_MAP()

	void PostNcDestroy() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnTimer( UINT_PTR nID );
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
