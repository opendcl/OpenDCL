// AcadDocReactor.cpp : implementation file
//

#include "stdafx.h"
#include "AcadDocReactor.h"
#include "OdclListCtrl.h"
#include "InvokeMethod.h"
#include "GsPreviewCtrl.h"
#include "ArxWorkspace.h"

const TCHAR sSDI[] = _T("SDI");


/////////////////////////////////////////////////////////////////////////////
// CAcadDocReactor

CAcadDocReactor::CAcadDocReactor(CWnd *pBlockList, CWnd *pBlockView)
{
	m_pParentBlockList = pBlockList;
	m_pParentBlockView = pBlockView;
	m_EventDefun = CString();
}

CAcadDocReactor::~CAcadDocReactor()
{
}

void CAcadDocReactor::documentCreated(AcApDocument* pDocCreating)
{
	if(!pDocCreating)
		return;
}

void CAcadDocReactor::documentActivated(AcApDocument* pActivatedDoc)
{
	try
	{
	if(!pActivatedDoc)
		return;
	theArxWorkspace.UpdateGlobalLispSymbols();
	if (m_EventDefun.GetLength() > 0)
		// call methods to invoke the event
		InvokeMethod(m_EventDefun, true, pActivatedDoc);	
	
	if (m_pParentBlockList != NULL)
		((OdclListCtrl*)m_pParentBlockList)->RefreshBlockList();

	if (m_pParentBlockView != NULL)
		((CGsPreviewCtrl*)m_pParentBlockView)->UpdateBlock();
	}
	catch(...)
	{
	}
}

void CAcadDocReactor::documentBecameCurrent(AcApDocument* pDoc)
{
}

void CAcadDocReactor::documentDestroyed(const char* filename)
{
	try
	{
	if (m_pParentBlockList)
		((OdclListCtrl*)m_pParentBlockList)->RefreshBlockList();

	if (m_pParentBlockView != NULL)
		((CGsPreviewCtrl*)m_pParentBlockView)->UpdateBlock();
	}
	catch(...)
	{
	}
}
