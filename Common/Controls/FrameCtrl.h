// FrameCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CFrameCtrl window

class CFrameCtrl : public CButton, public CDialogControl
{
	CAcadColorService mColorService;

// Construction
public:
	CFrameCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CFrameCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual CAcadColorService* GetColorService() { return &mColorService; }

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};
