// HtmlCtrl.h : header file
//

#pragma once

#include "HtmlBrowser.h"
#include "DialogControl.h"
#include "AcadColorService.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CHtmlCtrl window

class CHtmlCtrl : public CHtmlBrowser, public CDialogControl
{
	CAcadColorService mColorService;

// Construction
public:
	CHtmlCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CHtmlCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	
	virtual BOOL PreTranslateMessage( MSG* pMsg );	
	virtual afx_msg void PostNcDestroy();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};
