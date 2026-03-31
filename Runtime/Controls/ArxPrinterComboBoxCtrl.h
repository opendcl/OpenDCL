#pragma once

#include "ArxComboBoxCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPrinterComboBoxCtrl window

class CArxPrinterComboBoxCtrl : public CArxComboBoxCtrl
{
	TDclControlPtr mpPaperCombo;

public:
	CArxPrinterComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
													 TDclControlPtr pPaperCombo = NULL, bool bCreate = true );
	virtual ~CArxPrinterComboBoxCtrl();

// DialogControl Interface
public:
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;

public:
	void SetPaperSizeCombo( TDclControlPtr pPaperCombo );

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	afx_msg void OnCbnSelchange();
};
