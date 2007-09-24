// FolderComboCtrl.h : header file
//

#pragma once

#include "FolderComboBox.h"
#include "DialogControl.h"
#include "AcadColorService.h"

class CArxDwgListCtrl;


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
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

protected:
	virtual afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage( MSG* pMsg );
};
