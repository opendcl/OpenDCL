// OdclLayerCombo.cpp : implementation file
//

#include "stdafx.h"
#include "OdclLayerCombo.h"
#include "Resource.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "AcadColorTable.h"
#include "InvokeMethod.h"
#include "SpreadSheet.h"
#include "ToolTips.h"


/////////////////////////////////////////////////////////////////////////////
// OdclLayerCombo

OdclLayerCombo::OdclLayerCombo()
{
	m_Images.Create(16,16,ILC_COLOR8 | ILC_MASK, 3, 1);
		
	HMODULE hRes = _hdllInstance;

	HICON hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_LAYERON), IMAGE_ICON, 0, 0, 0);	
	m_Images.Add(hIcon);
	DestroyIcon(hIcon);

	hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_LAYEROFF), IMAGE_ICON, 0, 0, 0);
	m_Images.Add(hIcon);
	DestroyIcon(hIcon);

	hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_LAYERTHAW), IMAGE_ICON, 0, 0, 0);	
	m_Images.Add(hIcon);
	DestroyIcon(hIcon);

	hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_LAWERFROZEN), IMAGE_ICON, 0, 0, 0);
	m_Images.Add(hIcon);
	DestroyIcon(hIcon);

	m_bESC = false;

}

OdclLayerCombo::~OdclLayerCombo()
{
}


BEGIN_MESSAGE_MAP(OdclLayerCombo, CComboBox)
	//{{AFX_MSG_MAP(OdclLayerCombo)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// OdclLayerCombo message handlers
BOOL OdclLayerCombo::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	if (rc.Height() < 100)
		rc.bottom = rc.top + 100;

	// set the arx control pointer
	dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS | CBS_SORT | ES_AUTOHSCROLL
			  | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |WS_GROUP |WS_TABSTOP;
	
	RetVal = CComboBox::Create( dwStyle, rc, pParentWnd, nID );

	return RetVal;
}

BOOL OdclLayerCombo::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
{
	BOOL RetVal;
	DWORD dwStyle;
	CRect ArxRect;

	// set the arx control pointer
    m_ArxControl = pControl;
	
	// get the rectangle of the new control
	ArxRect.top = pControl->m_pTop->GetLongValue();
	ArxRect.left = pControl->m_pLeft->GetLongValue();

	if (pControl->GetLngProperty(nComboBoxStyle) != 1)
	{
		pControl->SetLongProperty(nHeight, (pControl->GetLngProperty(nHeight) + pControl->GetLngProperty(nDropDownHeight)));
	}

	ArxRect.bottom = pControl->m_pHeight->GetLongValue() + ArxRect.top;
	ArxRect.right = pControl->m_pWidth->GetLongValue() + ArxRect.left;
	
	if (ArxRect.Height() < 40)
	{
		ArxRect.bottom = ArxRect.top + 60;
	}
	
	dwStyle = WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL
			  | CBS_HASSTRINGS | CBS_SORT | WS_VSCROLL | WS_CLIPSIBLINGS
			  | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED |WS_GROUP |WS_TABSTOP;
	
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	RetVal = CComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );

	VERIFY(CComboBox::SubclassDlgItem(nID, pParentWnd));

	InitToolTip();
	SetToolTipEx(this, m_ToolTip, pControl);

	SetExtendedUI(TRUE);
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

void OdclLayerCombo::DrawColor(CDC* pDC, CRect rc, int nColor)
{
	// setup the CRect for FillRect	
	CPoint point;
	
	// move the color splotch down by one.
	rc.top++;
	rc.bottom++;

	HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, RGB(0,0,0));
	HGDIOBJ OldPen = SelectObject(pDC->m_hDC, pen);
		
	::Rectangle(pDC->m_hDC, rc.left, rc.top, rc.right, rc.bottom);

	SelectObject(pDC->m_hDC, OldPen);			
	DeleteObject(pen);

	rc.DeflateRect(1,1);

	// draw the solid rectangle
	COLORREF rgb;
	
	if (nColor == 256 || nColor == 0)					
		rgb = ::SetBkColor(pDC->m_hDC, RGB(255,255,255));
	else
		rgb = ::SetBkColor(pDC->m_hDC, GetRGBColor(nColor));

	::ExtTextOut(pDC->m_hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	::SetBkColor(pDC->m_hDC, rgb);

	
}

void OdclLayerCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	// Lets make a CDC for ease of use
	CDC *pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
	
	ASSERT(pDC); // Attached failed
	
	CRect rc(lpDrawItemStruct->rcItem);
	
	// Draw focus rectangle
	if (lpDrawItemStruct->itemState & ODS_FOCUS)
		pDC->DrawFocusRect(rc);
	
	// Save off context attributes
	int nIndexDC = pDC->SaveDC();

	CBrush brushFill;
	
	// Draw selection state
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		brushFill.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
		brushFill.CreateSolidBrush(pDC->GetBkColor());

	pDC->SetBkMode(TRANSPARENT);
	pDC->FillRect(rc, &brushFill);

	CString strCurFont, strNextFont;

	if (lpDrawItemStruct->itemID >= 0 && lpDrawItemStruct->itemID < GetCount())
	{
		GetLBText(lpDrawItemStruct->itemID,strCurFont);
	}
	else
	{
		return;
	}

	CRect rcImage(rc);

	rcImage.left += 2;
	rcImage.right = rcImage.left + 13;
	rcImage.bottom = rcImage.top + 13;

	DrawColor(pDC, rcImage, lpDrawItemStruct->itemData);
	
	bool bFrozen, bOn;

	GetVisibility(lpDrawItemStruct->itemID, bFrozen, bOn);

	if (bOn)
		m_Images.Draw(pDC,0, CPoint(rcImage.left+14,rcImage.top),ILD_TRANSPARENT);
	else
		m_Images.Draw(pDC,1, CPoint(rcImage.left+14,rcImage.top),ILD_TRANSPARENT);

	if (!bFrozen)
		m_Images.Draw(pDC,2, CPoint(rcImage.left+28,rcImage.top),ILD_TRANSPARENT);
	else
		m_Images.Draw(pDC,3, CPoint(rcImage.left+28,rcImage.top),ILD_TRANSPARENT);

	int nX = rc.left; // Save for lines

	rc.left += 48; // Text Position
	rc.top++;
	
	pDC->TextOut(rc.left,rc.top,strCurFont);
	
	// Restore State of context
	pDC->RestoreDC(nIndexDC);
	
}

void OdclLayerCombo::OnSelchange() 
{
	CString sString;
	int nSel = GetCurSel();
	int nTextLength = GetLBTextLen(nSel);      
	GetLBText(nSel, sString.GetBuffer(nTextLength));
	sString.ReleaseBuffer();

	if (m_ArxControl)
	{
		InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nSel, sString, m_bInvokeWithSendString);
		m_ArxControl->SetStringProperty(nText, sString);
	}

	if (m_ArxControl == NULL)
	{			
		// Send Notification to parent of ListView ctrl
		CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent()->GetParent();
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);		
		pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, GetItemData(nSel));
		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}
}
void OdclLayerCombo::OnDropdown() 
{
	ResetContent();

	AcDbLayerTable *pLayerTable;
    acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTable, AcDb::kForRead);

    AcDbLayerTableIterator* pIterator = NULL;
    
	pLayerTable->newIterator(pIterator, true, true);

	// loop thru the layers
	for (; !pIterator->done(); pIterator->step())
	{
		AcDbLayerTableRecord *pLayerTableRecord = NULL;

		// open the entity for read
		if (pIterator->getRecord(pLayerTableRecord, AcDb::kForRead) != Acad::eOk)
			continue;

		// get the layer name
		const TCHAR* pName;
		pLayerTableRecord->getName(pName);
		int nItem = AddString(pName);

		// get the layer color
		AcCmColor clr = pLayerTableRecord->color();
		SetItemData(nItem, clr.colorIndex());

		// of course we must close it
	    pLayerTableRecord->close();
	}
	
	delete pIterator;

    pLayerTable->close();
	
	
}

void OdclLayerCombo::GetVisibility(int nItem, bool &bFrozen, bool &bOn) 
{
	CString sLayerName;
	int nTextLength = GetLBTextLen(nItem);      
	GetLBText(nItem, sLayerName.GetBuffer(nTextLength));
	sLayerName.ReleaseBuffer();

	AcDbLayerTable *pLayerTable;
    acdbHostApplicationServices()->workingDatabase()->getSymbolTable(pLayerTable, AcDb::kForRead);

	AcDbLayerTableRecord *pLayerTableRecord = NULL;

	Acad::ErrorStatus es = pLayerTable->getAt(sLayerName, pLayerTableRecord, AcDb::kForRead);

    pLayerTable->close();

	if (es != Acad::eOk)
	{
		bFrozen = false;
		bOn = true;
		return;
	}
	
	// get the layer info
	bOn = !pLayerTableRecord->isOff();
	bFrozen = pLayerTableRecord->isFrozen();
	
	// of course we must close it
	pLayerTableRecord->close();

	
	
}

void OdclLayerCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    lpMeasureItemStruct->itemHeight = 16;
}
void OdclLayerCombo::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = 16;
		
	CComboBox::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}


void OdclLayerCombo::OnMouseMove(UINT nFlags, CPoint point) 
{
	CComboBox::OnMouseMove(nFlags, point);

	if (m_ArxControl)
		InvokeMethodIntIntInt(
			m_ArxControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
		
	

}

void OdclLayerCombo::OnDestroy() 
{
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	

	CComboBox::OnDestroy();
	
}

void OdclLayerCombo::OnKillfocus() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);

}

void OdclLayerCombo::OnSetfocus() 
{
	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

}

void OdclLayerCombo::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip

BOOL OdclLayerCombo::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);
	
	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN && m_ArxControl)
	{
		if (m_ArxControl->GetBoolProperty(nReturnAsTab) == TRUE)
			pMsg->wParam = VK_TAB;		
	}
	if (pMsg->message== WM_KEYDOWN && m_ArxControl == NULL)
	{
		if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
	    {
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			
			m_bESC = pMsg->wParam == VK_ESCAPE;
			return TRUE;				// DO NOT process further
		}
	}

	return CComboBox::PreTranslateMessage(pMsg);
}

