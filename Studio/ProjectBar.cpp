// mybar.cpp : implementation file
//

#include "stdafx.h"
#include "projectbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectBar

CProjectBar::CProjectBar()
{
}

CProjectBar::~CProjectBar()
{
}


BEGIN_MESSAGE_MAP(CProjectBar, baseCProjectBar)
	//{{AFX_MSG_MAP(CProjectBar)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CProjectBar message handlers

int CProjectBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (baseCProjectBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetSCBStyle(GetSCBStyle() | SCBS_SIZECHILD);

	if (!m_wndChild.Create(WS_CHILD|WS_VISIBLE|
			ES_MULTILINE|ES_WANTRETURN|ES_AUTOVSCROLL,
		CRect(0,0,0,0), this, 123))
		return -1;

	m_wndChild.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	// older versions of Windows* (NT 3.51 for instance)
	// fail with DEFAULT_GUI_FONT
	if (!m_font.CreateStockObject(DEFAULT_GUI_FONT))
		if (!m_font.CreatePointFont(80, _T("MS Sans Serif")))
			return -1;

	m_wndChild.SetFont(&m_font);

	return 0;
}
