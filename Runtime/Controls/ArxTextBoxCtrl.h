// ArxTextBoxCtrl.h : header file
//

#pragma once

#include "TextBoxCtrl.h"
#include "ArxControlServices.h"
#include "OleOdcDropTarget.h"


/////////////////////////////////////////////////////////////////////////////
// CArxTextBoxCtrl window

class CArxTextBoxCtrl : public CTextBoxCtrl
{
	CArxControlServices	mArxServices;

	COleOdcDropTarget m_DropTarget;
	bool m_bInvokeWithSendString;

public:
	CArxTextBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID,
									 CInputFilter* pFilter = NULL, DWORD dwAcUiStyle = 0, bool bCreate = true );
	virtual ~CArxTextBoxCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( TPropertyPtr pProp );

// Operations
public:
	void SetDragnDrop(BOOL bRegister);

protected:
	DECLARE_MESSAGE_MAP();

protected:
};
