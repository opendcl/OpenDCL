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
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );

public:
	void SetPaperSizeCombo( TDclControlPtr pPaperCombo );

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	afx_msg void OnCbnSelchange();
};
