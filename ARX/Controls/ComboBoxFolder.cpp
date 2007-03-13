// ComboBoxFolder.cpp : implementation file
//

#include "stdafx.h"
#include "ComboBoxFolder.h"
#include "DclControlObject.h"
#include "InvokeMethod.h"
#include "DwgDirList.h"
#include "ArxGridCtrl.h"
#include "PropertyIds.h"
#include "PropertyObject.h"
#include "ToolTips.h"


#define ID_FOLDER_TREECTRL	1235632

/////////////////////////////////////////////////////////////////////////////
// CComboBoxFolder

CComboBoxFolder::CComboBoxFolder()
{
	m_iconWidth = 16;
	m_droppedHeight = 100;
	m_pDwgList = NULL;
}

CComboBoxFolder::~CComboBoxFolder()
{
}


BEGIN_MESSAGE_MAP(CComboBoxFolder, CComboBox)
	//{{AFX_MSG_MAP(CComboBoxFolder)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SELECTITEM_CHANGE, OnSelectItemChange)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComboBoxFolder message handlers

void CComboBoxFolder::DrawItem(LPDRAWITEMSTRUCT pDIStruct)
{
	CDC dc;

	if(!dc.Attach(pDIStruct -> hDC ))
		return;

	CRect rc(pDIStruct->rcItem);
	CRect rc2;
	GetClientRect(rc2);
	CPoint p;
	p.x = 4;
	p.y = (rc2.bottom - m_iconWidth)/2 ;

	HTREEITEM item = m_treeCtrl.GetSelectedItem();
	if (item != NULL)
	{
		CFolder* folder = (CFolder*) m_treeCtrl.GetItemData(item);
		if (folder != NULL)
		{
			CImageList* imageList = m_treeCtrl.GetImageList(TVSIL_NORMAL);
			if (imageList)
			{
				if (folder->m_imageIndex == 0)
					imageList->Draw(&dc, 1, p, ILD_NORMAL);
				else
					imageList->Draw(&dc, folder->m_imageIndex, p, ILD_NORMAL);
			}			
			rc.top += 1;
			rc.bottom -= 1;
			rc.left += m_iconWidth + 2;
			CSize size = dc.GetTextExtent(folder->m_pathDescription);
			rc.right = rc.left + size.cx + 4;
			if(pDIStruct -> itemState & ODS_SELECTED)
			{	dc.FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT) );
				dc.DrawFocusRect(rc);
				dc.SetTextColor((0x00FFFFFF & ~(GetSysColor(COLOR_WINDOWTEXT))));
			}
			else
				dc.SetTextColor( GetSysColor(COLOR_WINDOWTEXT) );
			rc.left += 2;
			dc.DrawText(folder->m_pathDescription, rc, DT_SINGLELINE | DT_VCENTER);
		}
	}

	dc.Detach();
}


// No need the MeasureItem to do anything. Whatever the system says, it stays

void CComboBoxFolder::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
}

void CComboBoxFolder::Init(int w, int h)
{
	CRect rc(0, 0, 10, 10);
	UINT style =  WS_POPUP | WS_BORDER | WS_VSCROLL | TVS_DISABLEDRAGDROP|TVS_SHOWSELALWAYS;
	
	//m_treeCtrl.CreateEx(0, WC_TREEVIEW, NULL, style, rc, GetParent(), 0, NULL);
	m_treeCtrl.Create(style, rc, GetParent(), 0);
	
	m_treeCtrl.CreateImageList();
	m_treeCtrl.Init(this);
	
	GetClientRect(rc);
	if (w <= 0)
		w = rc.right;
	if (h <= 0)
		h = 100;
	SetDroppedWidth(w);
	SetDroppedHeight(h);
}

BOOL CComboBoxFolder::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	if (rc.Height() < 100)
		rc.bottom = rc.top + 100;

	// set the arx control pointer
    m_ArxControl = NULL;
		
	dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS | ES_AUTOHSCROLL | WS_CLIPCHILDREN;
	
	dwStyle = dwStyle | CBS_SORT;		

	dwStyle = dwStyle | WS_GROUP;

	dwStyle = dwStyle | CBS_DROPDOWNLIST;
	
	RetVal = CComboBox::Create( dwStyle, rc, pParentWnd, nID );

	VERIFY(CComboBox::SubclassDlgItem(nID, pParentWnd));
	
	return RetVal;
}

BOOL CComboBoxFolder::GetDroppedState( )
{
	return m_treeCtrl.IsWindowVisible();
}

void CComboBoxFolder::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (m_treeCtrl.IsWindowVisible())
		m_treeCtrl.ShowWindow(SW_HIDE);
	else
		DisplayTree ();

}

void CComboBoxFolder::AddPath(CString path)
{
	m_treeCtrl.AddPath(path);
	Invalidate();
}

void CComboBoxFolder::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	OnLButtonDown(nFlags, point);

}

CString CComboBoxFolder::GetSelectedPath()
{	
	return m_treeCtrl.GetSelectedPath();
}

void CComboBoxFolder::DisplayTree()
{
	int nDropHeight = m_treeCtrl.GetItemHeight() * m_treeCtrl.GetCount() + 3;
	CRect rc;
	GetWindowRect(rc);
	rc.top = rc.bottom + 1;
	// we are actually setting the width and height here for simplicity
	rc.bottom = nDropHeight;

	if (m_ArxControl == NULL)
		rc.right = 216;
	else
		rc.right = GetDroppedWidth();
	
	m_treeCtrl.Display(rc);
}

int CComboBoxFolder::GetDroppedHeight()
{	return m_droppedHeight;
}

void CComboBoxFolder::SetDroppedHeight(int height)
{	m_droppedHeight = height;
}

int CComboBoxFolder::GetDroppedWidth2()
{	return m_droppedWidth;
}

void CComboBoxFolder::SetDroppedWidth(int width)
{	m_droppedWidth = width;
}

BOOL CComboBoxFolder::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);

	if (pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_DOWN || pMsg->wParam==VK_RIGHT))
	{	m_treeCtrl.SelectNextItem(TRUE);
		Invalidate();
		return TRUE;
	}
	else if (pMsg->message==WM_KEYDOWN && (pMsg->wParam==VK_UP || pMsg->wParam==VK_LEFT))
	{	m_treeCtrl.SelectNextItem(FALSE);
		Invalidate();
		return TRUE;
	}
	else if (pMsg->message==WM_SYSKEYDOWN && pMsg->wParam==VK_DOWN)
	{	
		DisplayTree();
		return TRUE;
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

void CComboBoxFolder::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
	
	
	CComboBox::OnMouseMove(nFlags, point);
}
void CComboBoxFolder::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText
void CComboBoxFolder::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
		m_ToolTip.Create(this);
} // End of InitToolTip




BOOL CComboBoxFolder::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();
	pControl->SetLongProperty(nHeight, (pControl->GetLngProperty(nHeight) + pControl->GetLngProperty(nDropDownHeight)));
	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	ArxRect.bottom = 300 + ArxRect.top;

	if (ArxRect.Height() < 40)
	{
		ArxRect.bottom = ArxRect.top + 60;
	}

	dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | CBS_OWNERDRAWVARIABLE | CBS_DROPDOWNLIST;	
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	ArxRect.bottom = ArxRect.bottom  + pControl->GetLngProperty(nDropDownHeight);
	
	RetVal = CComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );


	VERIFY(CComboBox::SubclassDlgItem(nID, pParentWnd));
	InitToolTip();
	SetToolTipEx(this, m_ToolTip, pControl);


	switch (m_ArxControl->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}
	return RetVal;
}

const TCHAR sDivider1[] = _T("\\");
const TCHAR sDivider2[] = _T("\\\\");
const TCHAR sDivider3[] = _T("//");
const TCHAR sDivider4[] = _T("/");

long CComboBoxFolder::OnSelectItemChange(WPARAM w, LPARAM l)
{
	CString path;
	CFolder* folder = (CFolder*) w;
	if (folder != NULL)
		path = folder->m_path;

	CString sLispSafePath;
	CString sChar;
	for (int i=0; i<path.GetLength(); i++)
	{
		sChar = path.Mid(i, 1);
		if (sChar == sDivider1)
			sLispSafePath += sDivider2;
		else if (sChar == sDivider4)
			sLispSafePath += sDivider3;
		else
			sLispSafePath += sChar;
	}

	if (m_pDwgList != NULL)
	{
		m_pDwgList->Dir(path);
	}
	if (m_ArxControl)
	{
		InvokeMethodString(m_ArxControl->GetStrProperty(nEventSelChanged), sLispSafePath, m_bInvokeWithSendString);

		m_ArxControl->SetStringProperty(nText, path);
	}

	// now lets clear the control and repopulate it so the old path is removed correctly
	m_treeCtrl.FreeMemory(m_treeCtrl.GetRootItem());
	m_treeCtrl.DeleteAllItems();
	m_treeCtrl.Init(this);
	m_treeCtrl.AddPath(path);

	if (m_ArxControl == NULL)
	{		
		// Send Notification to parent of ListView ctrl
		LV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = GetParent()->GetParent()->m_hWnd;
		dispinfo.hdr.idFrom = GetDlgCtrlID();
		dispinfo.hdr.code = LVN_ENDLABELEDIT;

		CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, path);
		pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, -1);
		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}

	return 0;
}


void CComboBoxFolder::OnKillFocus(CWnd* pNewWnd) 
{
	CComboBox::OnKillFocus(pNewWnd);
	
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);

}

void CComboBoxFolder::OnSetFocus(CWnd* pOldWnd) 
{
	CComboBox::OnSetFocus(pOldWnd);
	
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

	
}

void CComboBoxFolder::OnDestroy() 
{
	m_treeCtrl.FreeMemory(m_treeCtrl.GetRootItem());
	m_treeCtrl.DeleteAllItems();
	m_treeCtrl.DestroyWindow();	
	CComboBox::OnDestroy();
	
	
}

void CComboBoxFolder::OnDropdown() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDropDown), m_bInvokeWithSendString);	
}
