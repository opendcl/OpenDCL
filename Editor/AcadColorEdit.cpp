// AcadColorEdit.cpp : implementation file
//

#include "stdafx.h"
#include "AcadColorEdit.h"
#include "Resource.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CAcadColorEdit

CAcadColorEdit::CAcadColorEdit()
{
	m_sFilter = theWorkspace.LoadResourceString(IDS_LONGFILTER);
}

CAcadColorEdit::~CAcadColorEdit()
{
}


BEGIN_MESSAGE_MAP(CAcadColorEdit, CEdit)
	//{{AFX_MSG_MAP(CAcadColorEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAcadColorEdit message handlers

void CAcadColorEdit::OnBadInput()
{
	MessageBeep((UINT)nNotSet);
}


void CAcadColorEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{	
	if (nChar == nBackSpace)
 	{
		CEdit::OnChar(nChar, nRepCnt, nFlags);
		return;
	}
	
	if (nChar == nReturnChar)
	{
		return;
	}
	
	
	char sChar = nChar;
	CString strChar = sChar;
	if (m_sFilter.FindOneOf(strChar) > nNotSet)
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	else
		OnBadInput();
}
