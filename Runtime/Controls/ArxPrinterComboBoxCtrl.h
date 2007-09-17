#pragma once

#include "ArxComboBoxCtrl.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CArxPrinterComboBoxCtrl window

class CArxPrinterComboBoxCtrl : public CArxComboBoxCtrl
{
	CDclControlObject* mpPaperCombo;

public:
	CArxPrinterComboBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID,
													 CDclControlObject* pPaperCombo = NULL, bool bCreate = true );
	virtual ~CArxPrinterComboBoxCtrl();

// DialogControl Interface
public:
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:
	void SetPaperSizeCombo( CDclControlObject* pPaperCombo );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
 
protected:
	afx_msg void OnSelchange();
};
