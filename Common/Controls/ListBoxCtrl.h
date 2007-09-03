// ListBoxCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include "PPToolTip.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl window

class CListBoxCtrl : public CListBox, public CDialogControl
{
	CAcadColorService mAcadColorService;

// Construction
public:
	CListBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CListBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mAcadColorService; }

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void PostNcDestroy();
};
