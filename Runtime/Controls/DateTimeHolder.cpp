// CDateTimeHolder.cpp : implementation file
//

#include "stdafx.h"
#include "DateTimeHolder.h"


/////////////////////////////////////////////////////////////////////////////
// CDateTimeHolder

CDateTimeHolder::CDateTimeHolder()
{
	
}

CDateTimeHolder::~CDateTimeHolder()
{
	
}


BEGIN_MESSAGE_MAP(CDateTimeHolder, CStatic)
	//{{AFX_MSG_MAP(CDateTimeHolder)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDateTimeHolder message handlers


BOOL CDateTimeHolder::CreateDate(CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal = FALSE;
	DWORD dwStyle;
	
	dwStyle = WS_VISIBLE|WS_CHILD|DTS_SHORTDATEFORMAT;

	dwStyle = dwStyle | WS_TABSTOP;
	dwStyle = dwStyle | WS_GROUP;

	if (CStatic::Create(CString(), WS_VISIBLE|WS_CHILD, rc, pParentWnd, nID) == TRUE)
	{
		RetVal = m_Child.Create( dwStyle, CRect(-1,-3,rc.Width()+3,rc.Height()+3), this, 100 );
	}
		
	m_bChildIsData = true;
	return RetVal;	
}

BOOL CDateTimeHolder::CreateTime(CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal = FALSE;
	DWORD dwStyle;
	
	dwStyle = WS_VISIBLE|WS_CHILD|DTS_TIMEFORMAT;

	dwStyle = dwStyle | WS_TABSTOP;
	dwStyle = dwStyle | WS_GROUP;

	if (CStatic::Create(CString(), WS_VISIBLE|WS_CHILD, rc, pParentWnd, nID) == TRUE)
	{
		RetVal = m_Child.Create( dwStyle, CRect(-1,-3,rc.Width()+2,rc.Height()+2), this, 100 );
	}

	m_bChildIsData = false;
	return RetVal;	
}


void CDateTimeHolder::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	if (m_bChildIsData)
	{
		m_Child.MoveWindow(-1,-3, cx+3, cy+3, TRUE);
	}
	else
	{
		m_Child.MoveWindow(-1,-3, cx+2, cy+2, TRUE);
	}
	
}
