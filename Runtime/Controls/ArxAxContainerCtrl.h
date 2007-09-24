#pragma once

#include "AxContainerCtrl.h"
#include "ArxControlServices.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// CArxAxContainerCtrl window

class CArxAxContainerCtrl : public CAxContainerCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxAxContainerCtrl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true);
	virtual ~CArxAxContainerCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );

protected:
	void TryToFireAxEvent(UINT idCtrl, AFX_EVENT* pEvent);
	void FireAxEvent(UINT idCtrl, const CPropertyObject* pProp, AFX_EVENT* pEvent);

protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};