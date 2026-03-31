#pragma once

#include "ArxComboBoxCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPaperComboBoxCtrl window

class CArxPaperComboBoxCtrl : public CArxComboBoxCtrl
{
// Construction
public:
	CArxPaperComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxPaperComboBoxCtrl();

// DialogControl Interface
public:
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	bool ApplyProperty( TPropertyPtr pProp ) override;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
