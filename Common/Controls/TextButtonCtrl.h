// TextButtonCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CTextButtonCtrl window

class CTextButtonCtrl : public CButton, public CDialogControl
{
	CAcadColorService mColorService;

// Construction
public:
	CTextButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CTextButtonCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnNcPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
