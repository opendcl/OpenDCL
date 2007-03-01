#pragma once

//#include "PPToolTip.h"

#include "AxContainer.h"
#include "DclControlObject.h"
#include "DialogControl.h"

/////////////////////////////////////////////////////////////////////////////
// OdclActiveX window

class OdclActiveX : public CAxContainer
{
public:
	OdclActiveX(CDclFormObject* pParent);

	void TryToFireAxEvent(UINT idCtrl, AFX_EVENT* pEvent);
	void FireAxEvent(UINT idCtrl, AxEventDescriptor* pED, AFX_EVENT* pEvent);

	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
protected:
	DECLARE_MESSAGE_MAP()
};