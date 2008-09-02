// AcadBlockReactor.cpp : implementation file
//

#include "stdafx.h"
#include "AcadBlockReactor.h"
#include "ArxListViewCtrl.h"
#include "ArxGsViewCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CAcadDocReactor

CAcadBlockReactor::CAcadBlockReactor(CWnd *pBlockList, CWnd *pBlockView)
{
	m_pParentBlockList = pBlockList;	
	m_pParentBlockView = pBlockView;
}

CAcadBlockReactor::~CAcadBlockReactor()
{
	m_pParentBlockList = NULL;
}

void CAcadBlockReactor::endInsert(AcDbDatabase* pTo)
{
	if(!pTo)
		return;

	if (m_pParentBlockList)
		((CArxListViewCtrl*)m_pParentBlockList)->RefreshBlockList();

	if (m_pParentBlockView != NULL)
		((CArxGsViewCtrl*)m_pParentBlockView)->UpdateBlock();
}

void CAcadBlockReactor::endDeepClone(AcDbIdMapping& idMap)
{
	if (m_pParentBlockList)
		((CArxListViewCtrl*)m_pParentBlockList)->RefreshBlockList();

	if (m_pParentBlockView != NULL)
		((CArxGsViewCtrl*)m_pParentBlockView)->UpdateBlock();
}


void CAcadBlockReactor::commandEnded(const char * cmdStr)
{
	CString sCommand = cmdStr;
	if (sCommand == _T("PURGE"))
	{
		if (m_pParentBlockList)
			((CArxListViewCtrl*)m_pParentBlockList)->RefreshBlockList();

		if (m_pParentBlockView != NULL)
			((CArxGsViewCtrl*)m_pParentBlockView)->UpdateBlock();
	}
}
