// ArxRadioButtonCtrl.h : header file
//

#pragma once

#include "RadioButtonCtrl.h"
#include "ArxControlServices.h"

class CDclControlObject;
class CPropertyObject;


/////////////////////////////////////////////////////////////////////////////
// CArxRadioButtonCtrl window

class CArxRadioButtonCtrl : public CRadioButtonCtrl
{
	CArxControlServices	mArxServices;

	bool m_bInvokeWithSendString;

// Construction
public:
	CArxRadioButtonCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxRadioButtonCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};
