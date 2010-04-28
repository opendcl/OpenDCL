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
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
