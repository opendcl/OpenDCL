#pragma once

#include "ArxComboBoxCtrl.h"

class CDclControlObject;


/////////////////////////////////////////////////////////////////////////////
// CArxPaperComboBoxCtrl window

class CArxPaperComboBoxCtrl : public CArxComboBoxCtrl
{
// Construction
public:
	CArxPaperComboBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxPaperComboBoxCtrl();

// DialogControl Interface
public:
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};
