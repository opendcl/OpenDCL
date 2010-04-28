// RectangleCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"

class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CRectangleCtrl window

class CRectangleCtrl : public CStatic, public CDialogControl
{
	CAcadColorService mColorService;

// Construction
public:
	CRectangleCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CRectangleCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
