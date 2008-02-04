// ScrollBarCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CScrollBarCtrl window

class CScrollBarCtrl : public CScrollBar, public CDialogControl
{
	CAcadColorService mAcadColorService;

public:
	CScrollBarCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CScrollBarCtrl();

protected:
	virtual void OnScroll(UINT nSBCode, UINT nPos);

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyForegroundColor( TPropertyPtr pProp ); //Prop::ForegroundColor
	virtual CAcadColorService* GetColorService() { return &mAcadColorService; }

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

	virtual afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void HScroll(UINT nSBCode, UINT nPos);
	afx_msg void VScroll(UINT nSBCode, UINT nPos);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};
