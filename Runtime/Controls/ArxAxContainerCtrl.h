#pragma once

#include "AxContainerCtrl.h"
#include "ArxControlServices.h"
#include "DclControlTemplate.h"

//export this function so it can be used elsewhere
resbuf* VariantArgToResbuf( const VARIANTARG& varArg, const GUID& guidType, resbuf*& prbTail );


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
	void HandleAxEvent( AFX_EVENT* pEvent );

protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};