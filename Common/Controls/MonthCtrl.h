// MonthCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CMonthCtrl window

class CMonthCtrl : public CMonthCalCtrl, public CDialogControl
{
	CAcadColorService mAcadColorService;

// Construction
public:
	CMonthCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CMonthCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mAcadColorService; }

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
