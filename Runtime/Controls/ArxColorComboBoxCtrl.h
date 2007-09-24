// ArxColorComboBoxCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "ArxControlServices.h"


/////////////////////////////////////////////////////////////////////////////
// CArxArrowComboBoxCtrl window

class CArxColorComboBoxCtrl : public CAcUiColorComboBox, public CDialogControl
{
	CArxControlServices	mArxServices;

public:
	CArxColorComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxColorComboBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:
virtual void    DrawItemImage (CDC& dc, CRect& r, int cargo) { __super::DrawItemImage( dc, r, cargo ); }
virtual void    OnAddItems () { __super::OnAddItems(); }
virtual BOOL    OnSelectOther (BOOL isOther2, int curSel, int& newSel) { return __super::OnSelectOther( isOther2, curSel, newSel ); }

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSelchange();
	afx_msg void OnDropdown();
};
