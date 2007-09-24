// UndoActions.cpp : implementation file
//

#include "stdafx.h"
#include "UndoActions.h"


/////////////////////////////////////////////////////////////////////////////
// CUndoActions

CUndoActions::CUndoActions(int nAction, TDclControlPtr pArxObject, CWnd *pControl)
{
	m_nAction = nAction;
	m_pArxObject = pArxObject;
	m_pControl = pControl;
}

CUndoActions::~CUndoActions()
{
	m_pControl = NULL;
	m_pArxObject = NULL;
}


