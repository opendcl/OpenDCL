// FolderComboCtrl.h : header file
//

#pragma once

#include "FolderComboBox.h"
#include "DialogControl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CFolderComboCtrl window

class CFolderComboCtrl : public CFolderComboBox, public CDialogControl
{
	CAcadColorService mColorService;

public:
	CFolderComboCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CFolderComboCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	CRect GetWndRect() const override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }
	void OnThemeRequested( WndTheme& Theme ) const override { if( !Theme ) Theme.Attach( GetFolderComboBoxTheme(), GetHWnd() ); }

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam ) override;
	afx_msg void PostNcDestroy() override;
	BOOL PreTranslateMessage( MSG* pMsg ) override;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNMThemeChanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
