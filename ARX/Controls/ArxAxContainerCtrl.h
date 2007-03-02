#pragma once

//#include "PPToolTip.h"

#include "AxContainerCtrl.h"
#include "DclControlObject.h"
#include "DialogControl.h"

/////////////////////////////////////////////////////////////////////////////
// CArxAxContainerCtrl window

class CArxAxContainerCtrl : public CAxContainerCtrl
{
public:
	CArxAxContainerCtrl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true);
	virtual ~CArxAxContainerCtrl();

	CString	msLispSymbolName;
	bool m_bInvokeWithSendString;

	void SetLispSymbol( bool bResetToNil = false ) const;
	virtual bool Create( CWnd* pParentWnd, UINT nID );

	void TryToFireAxEvent(UINT idCtrl, AFX_EVENT* pEvent);
	void FireAxEvent(UINT idCtrl, CPropertyObject* pProp, AFX_EVENT* pEvent);

	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
	DECLARE_MESSAGE_MAP()
};