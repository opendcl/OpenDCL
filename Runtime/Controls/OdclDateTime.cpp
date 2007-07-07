// OdclDateTime.cpp : implementation file
//

#include "stdafx.h"
#include "OdclDateTime.h"
#include "ArxGridCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// OdclDateTime

OdclDateTime::OdclDateTime()
{
	m_pStaticBrush = new CBrush();
	m_BackColor = ::GetSysColor(COLOR_BTNFACE);
	m_DefBackColor = m_BackColor;
	m_pStaticBrush->CreateSolidBrush(m_BackColor);
	m_ForeColor = ::GetSysColor(COLOR_WINDOWTEXT);
	m_DefForeColor = m_ForeColor;
	m_UseBackColor = true;

	m_DefaultBackColor = m_BackColor;
}

OdclDateTime::~OdclDateTime()
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
}


BEGIN_MESSAGE_MAP(OdclDateTime, CDateTimeCtrl)
	//{{AFX_MSG_MAP(OdclDateTime)
	ON_NOTIFY_REFLECT(DTN_DATETIMECHANGE, OnDatetimechange)
	ON_NOTIFY_REFLECT(DTN_WMKEYDOWN, OnWmkeydown)	
	ON_WM_CTLCOLOR_REFLECT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OdclDateTime message handlers

void OdclDateTime::SetBkColor(COLORREF lColor)
{
	if (m_pStaticBrush)
		delete m_pStaticBrush;
	m_pStaticBrush = new CBrush();
	m_pStaticBrush->CreateSolidBrush(lColor);
	m_BackColor = lColor;
	
	if (m_DefForeColor == m_DefForeColor && m_DefBackColor == m_BackColor)
		m_UseBackColor = false;
	else
		m_UseBackColor = true;
}

void OdclDateTime::SetForeColor(COLORREF lColor)
{
	m_ForeColor = lColor;
	if (m_DefForeColor == m_DefForeColor && m_DefBackColor == m_BackColor)
		m_UseBackColor = false;
	else
		m_UseBackColor = true;
}

HBRUSH OdclDateTime::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if (!IsWindowEnabled() || !m_UseBackColor)
	{		
		return NULL;
	}
	
	pDC->SetBkColor(m_BackColor);	
	pDC->SetTextColor(m_ForeColor);
	pDC->SelectObject(m_pStaticBrush);
	return (HBRUSH)(m_pStaticBrush->GetSafeHandle());
}

BOOL OdclDateTime::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message== WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();		
			pListCtrl->MoveDown();		
			return TRUE;				// DO NOT process further
		}
		if (pMsg->wParam == VK_ESCAPE)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			SetWindowText(m_strOldValue);
			
			// Send Notification to parent of ListView ctrl
			LV_DISPINFO dispinfo;
			dispinfo.hdr.hwndFrom = GetParent()->GetParent()->m_hWnd;
			dispinfo.hdr.idFrom = GetParent()->GetDlgCtrlID();
			dispinfo.hdr.code = LVN_ENDLABELEDIT;

			CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();

			dispinfo.item.mask = LVIF_TEXT;
			dispinfo.item.iItem = pListCtrl->m_nRowSelected;
			dispinfo.item.iSubItem = pListCtrl->m_nColSelected;
			dispinfo.item.pszText = LPTSTR((LPCTSTR)m_strOldValue);
			dispinfo.item.cchTextMax = m_strOldValue.GetLength();

			pListCtrl->SendMessage( WM_NOTIFY, GetDlgCtrlID(), (LPARAM)&dispinfo );
			pListCtrl->HideEditControls();
			return TRUE;				// DO NOT process further
		}
		
	}
	
	return CDateTimeCtrl::PreTranslateMessage(pMsg);
}
void OdclDateTime::OnDatetimechange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	CString sText;	
	GetWindowText(sText);
	
	// Send Notification to parent of ListView ctrl
	CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();

	//pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, GetCurrentItemColorIndex());
	pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sText);

	*pResult = 0;
}

void OdclDateTime::OnWmkeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}
