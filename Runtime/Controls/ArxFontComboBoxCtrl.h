#pragma once

#include "ArxComboBoxCtrl.h"
#include "TipWnd.h"


/////////////////////////////////////////////////////////////////////////////
// CArxFontComboBoxCtrl window

class CArxFontComboBoxCtrl : public CArxComboBoxCtrl
{
	HWND mhwndList;
	CTipWnd mwndTip;

// Construction
public:
	CArxFontComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxFontComboBoxCtrl();

// DialogControl Interface
public:
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	CRect GetWndRect() const override;
	DWORD GetWndStyle() const override;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
 
protected:
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	afx_msg void OnCbnCloseup();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnKillfocus();
};
