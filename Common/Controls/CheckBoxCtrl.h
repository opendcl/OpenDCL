// CheckBoxCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CCheckBoxCtrl window

class CCheckBoxCtrl : public CButton, public CDialogControl
{
	CAcadColorService mAcadColorService;

// Construction
public:
	CCheckBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CCheckBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mAcadColorService; }

// Overrides
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void PostNcDestroy();
};
