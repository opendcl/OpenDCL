#pragma once

#include "ArxComboBoxCtrl.h"
#include "TipWnd.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CArxFontComboBoxCtrl window

class CArxFontComboBoxCtrl : public CArxComboBoxCtrl
{
	HWND mhwndList;
	CTipWnd mwndTip;

// Construction
public:
	CArxFontComboBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxFontComboBoxCtrl();

// DialogControl Interface
public:
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
 
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnCloseUp();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKillfocus();
};
