// PrinterComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "PrinterComboBox.h"
#include "Resource.h"
#include "SharedRes.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "ToolTips.h"
#include "InvokeMethod.h"
#include "ArxGridCtrl.h"
#include "Workspace.h"
#include "ControlPane.h"
#include "ControlTypes.h"
#include "axlock.h"

// Constant and should not be changed unless 
// image IDB_GLYPH is changed
#define GLYPH_WIDTH 16 
#define dwFlags PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL


/////////////////////////////////////////////////////////////////////////////
// CPrinterComboBox

CPrinterComboBox::CPrinterComboBox( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID )
: CArxDialogControl( pTemplate, &Pane, this )
{
	m_img.Create(16,14,ILC_COLOR4 | ILC_MASK, 3, 1);
	HMODULE hRes = _hdllInstance;
	HICON hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_PRINTER), IMAGE_ICON, 0, 0, 0);	
	m_img.Add(hIcon);
	DestroyIcon(hIcon);
	hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_PLOTTER), IMAGE_ICON, 0, 0, 0);
	m_img.Add(hIcon);
	DestroyIcon(hIcon);
	Create( Pane.GetHostDialog(), nID );
}

CPrinterComboBox::CPrinterComboBox( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID, CRect rc )
: CArxDialogControl( pTemplate, &Pane, this )
{
	m_img.Create(16,14,ILC_COLOR4 | ILC_MASK, 3, 1);
	HMODULE hRes = _hdllInstance;
	HICON hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_PRINTER), IMAGE_ICON, 0, 0, 0);	
	m_img.Add(hIcon);
	DestroyIcon(hIcon);
	hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_PLOTTER), IMAGE_ICON, 0, 0, 0);
	m_img.Add(hIcon);
	DestroyIcon(hIcon);
	Create( Pane.GetHostDialog(), nID, rc );
}

CPrinterComboBox::~CPrinterComboBox()
{
}

CRect CPrinterComboBox::GetWndRect() const
{
	CRect rectCombo = CArxDialogControl::GetWndRect();
	long nListHeight = mpTemplate->GetLongProperty(Prop::DropDownHeight);
	if( nListHeight < 40 )
		nListHeight = 40;
	rectCombo.bottom += Prop::Height;
	return rectCombo;
}

DWORD CPrinterComboBox::GetWndStyle() const
{
	DWORD dwStyle = CArxDialogControl::GetWndStyle();
	dwStyle |= WS_BORDER | WS_VSCROLL | CBS_HASSTRINGS |
						CBS_AUTOHSCROLL | WS_CLIPCHILDREN | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;
	if (mpTemplate->GetBooleanProperty(Prop::Sorted) == TRUE)
		dwStyle |= CBS_SORT;		
	return dwStyle;
}

bool CPrinterComboBox::Create( CWnd* pParentWnd, UINT nID )
{
	return Create( pParentWnd, nID, GetWndRect() );
}

bool CPrinterComboBox::Create( CWnd* pParentWnd, UINT nID, CRect rc )
{
	bool bSuccess = (CComboBox::Create( GetWndStyle(), rc, pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	if( mpTemplate->GetLongProperty(Prop::EventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

CComboBox* CPrinterComboBox::FindPaperSizesCombo() const
{
	const CControlPane::TDialogControls& rControls = mpControlPane->GetControlsList();
	size_t ctMax = rControls.size();
	for( size_t idx = 0; idx < ctMax; ++idx )
	{
		TDialogControlPtr pControl = rControls[idx];
		if (pControl->GetControlType() == CtlComboBox)
		{
			if( pControl->GetTemplate()->GetLongProperty(Prop::ComboBoxStyle) == CmboStyle_PlotterPaperSizes )
				return (CComboBox*)pControl->GetControl();
		}
	}
	return NULL;
}


BEGIN_MESSAGE_MAP(CPrinterComboBox, CComboBox)
	//{{AFX_MSG_MAP(CPrinterComboBox)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrinterComboBox message handlers

int CPrinterComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	CreatePrinterList();
	return 0;
}

int CPrinterComboBox::CreatePrinterList()
{
	// get current document
	AcApDocument* pDoc = acDocManager->curDocument();

	Acad::ErrorStatus es = acDocManager->lockDocument(
								pDoc,
								AcAp::kWrite,
								NULL,
								NULL,
								false);

   
    if (es != Acad::eOk)
		return 0;

	// Get the plot settings validator
	AcDbPlotSettingsValidator *pPlotSettingsValidator = acdbHostApplicationServices()->plotSettingsValidator();
	assert( pPlotSettingsValidator != NULL );
		
	
	AcArray<const TCHAR*> mediaList;
	pPlotSettingsValidator->plotDeviceList(mediaList);
	
	ResetContent();
	
	CString sNone;
	sNone = theWorkspace.LoadResourceString(IDS_NONE);
	
	int nIndex = AddString (sNone);
	SetItemData(nIndex, 1);

	for (int i=1; i<mediaList.length(); i++)
	{
		CString value = mediaList[i];
		
		AddString(value);
		SetItemData(i, 1);
	}

	es = acDocManager->unlockDocument(pDoc);



	// do the second part of the list, we have to get the pc3 files and set the correct image id.

	CString sPlottersPath = GetPlottersPath();	

	// get the list of existing AutoCAD shx fonts
	CFileFind finder;
	BOOL bResult = finder.FindFile(sPlottersPath + "\\*.pc3");
	CString sPC3Name;

	while (bResult)
	{
		bResult = finder.FindNextFile();

		// skip . and .. files; otherwise, we'd
	    // recur infinitely!
		if (finder.IsDots())
			continue;
		
		sPC3Name = finder.GetFileName();
		int nFound = FindStringExact(0, sPC3Name);
		if (nFound > -1)
			SetItemData(nFound, 2);
	}
	finder.Close();
	
	return 0;
}

BOOL CPrinterComboBox::PreTranslateMessage(MSG* pMsg) 
{
	GetToolTipCtrl().RelayEvent(pMsg);	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (mpTemplate->GetBooleanProperty(Prop::ReturnAsTab) == TRUE)
			pMsg->wParam = VK_TAB;
	}
	return CComboBox::PreTranslateMessage(pMsg);
}


void CPrinterComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (mpTemplate)
		InvokeMethodIntIntInt(
			mpTemplate->GetStrProperty(Prop::EventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
	
	
	CComboBox::OnMouseMove(nFlags, point);
}

void CPrinterComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	if (mpTemplate)
		// call methods to invoke the event
		InvokeMethod(mpTemplate->GetStrProperty(Prop::EventSetFocus), m_bInvokeWithSendString);
	CComboBox::OnSetFocus(pOldWnd);	
}

void CPrinterComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	if (mpTemplate)
		// call methods to invoke the event
		InvokeMethod(mpTemplate->GetStrProperty(Prop::EventKillFocus), m_bInvokeWithSendString);
	CComboBox::OnKillFocus(pNewWnd);
}

void CPrinterComboBox::OnSelchange() 
{
	CString sString;
	int nSel = GetCurSel();
	int nTextLength = GetLBTextLen(nSel);      
	GetLBText(nSel, sString.GetBuffer(nTextLength));
	sString.ReleaseBuffer();

	if( mpTemplate )
	{
		CComboBox* pPaperSizesCombo = FindPaperSizesCombo();
		if( !pPaperSizesCombo )
		{
			InvokeMethodIntString(mpTemplate->GetStrProperty(Prop::EventSelChanged), nSel, sString, m_bInvokeWithSendString);
			mpTemplate->SetStringProperty(Prop::Text, sString);
			return;
		}

		BeginWaitCursor();
		// get current document
		AcAxDocLock CurDocWriteContext( acdbCurDwg() );

		// Get layout manager pointer
		AcApLayoutManager *pLayoutManager = (AcApLayoutManager*)acdbHostApplicationServices()->layoutManager();
		assert(pLayoutManager != NULL);

		CString sLayout = pLayoutManager->findActiveTab();
		   
		// Get the specific layout
		AcDbLayout* pLayoutObj = pLayoutManager->findLayoutNamed(sLayout,true);
		if (!pLayoutObj) 
			return;
		AcDbObjectPointer< AcDbLayout > pLayout;
		pLayout.acquire( pLayoutObj );

		// Get the plot settings validator
		AcDbPlotSettingsValidator *pPlotSettingsValidator = acdbHostApplicationServices()->plotSettingsValidator();
		assert( pPlotSettingsValidator != NULL );
		if (pPlotSettingsValidator == NULL) 
			return;

		if (nSel == 0)
		{
			pPlotSettingsValidator->setPlotCfgName(pLayout.object(),_T("None"),_T("none_user_media"));
			if (pPaperSizesCombo)
				pPaperSizesCombo->ResetContent();

			InvokeMethodIntString(mpTemplate->GetStrProperty(Prop::EventSelChanged), nSel, sString, m_bInvokeWithSendString);
			mpTemplate->SetStringProperty(Prop::Text, sString);
			return;
		}
		else
			pPlotSettingsValidator->setPlotCfgName(pLayout.object(),sString);

		// Refresh the layout lists in order to use it.
		pPlotSettingsValidator->refreshLists(pLayout.object());
	    
		AcArray<const TCHAR*> mediaList;
		Acad::ErrorStatus es = pPlotSettingsValidator->canonicalMediaNameList(pLayout.object(), mediaList);
		if (es != Acad::eOk)
			return;

		pPaperSizesCombo->ResetContent();
		for (int i=0; i<mediaList.length(); i++)
		{
			const ACHAR * pszPaperSize;
			pPlotSettingsValidator->getLocaleMediaName(pLayout.object(), i,pszPaperSize);
			pPaperSizesCombo->AddString(pszPaperSize);
		}

		EndWaitCursor();

		InvokeMethodIntString(mpTemplate->GetStrProperty(Prop::EventSelChanged), nSel, sString, m_bInvokeWithSendString);
		mpTemplate->SetStringProperty(Prop::Text, sString);
	}
	//else
	//{		
	//	// Send Notification to parent of ListView ctrl
	//	CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
	//	pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);
	//	pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, -1);
	//	// fire the on Grid edit cell event.
	//	pListCtrl->EndEditControls(pListCtrl);
	//}
}

void CPrinterComboBox::OnDestroy() 
{
	CComboBox::OnDestroy();
}


CString CPrinterComboBox::GetPlottersPath()
{
	CString sChar;
	CString sAcadPrefix;
	CString sCompile;
	struct resbuf rb; 

	acedGetVar(_T("ACADPREFIX"), &rb); 

	sAcadPrefix = rb.resval.rstring;

	for (int i=0; i<sAcadPrefix.GetLength(); i++)
	{
		sChar = sAcadPrefix.Mid(i,1);
		if (sChar != _T(";"))
			sCompile = sCompile + sChar;
		else
		{
			if (sCompile.Right(5) == _T("fonts"))
			{
				sCompile = sCompile.Left(sCompile.GetLength()-5) + _T("Plotters");
				return sCompile;
			}
			sCompile.Empty();
		}

	}
	return CString();
}


void CPrinterComboBox::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	ASSERT(lpDIS->CtlType == ODT_COMBOBOX); // We've gotta be a combo
	if( lpDIS->itemID == UINT(-1) )
		return;

	// Lets make a CDC for ease of use
	CDC *pDC = CDC::FromHandle(lpDIS->hDC);
	
	ASSERT(pDC); // Attached failed
	
	CRect rc(lpDIS->rcItem);
	
	// Draw focus rectangle
	if (lpDIS->itemState & ODS_FOCUS)
		pDC->DrawFocusRect(rc);
	
	// Save off context attributes
	int nIndexDC = pDC->SaveDC();

	CBrush brushFill;
	
	// Draw selection state
	if (lpDIS->itemState & ODS_SELECTED)
	{
		brushFill.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
		pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
	}
	else
		brushFill.CreateSolidBrush(pDC->GetBkColor());

	pDC->SetBkMode(TRANSPARENT);
	pDC->FillRect(rc, &brushFill);
	brushFill.DeleteObject();

	CString strCurFont,strNextFont;
	GetLBText(lpDIS->itemID,strCurFont);

	DWORD dwData = GetItemData(lpDIS->itemID);
	
	// Render Bitmaps
	if (dwData == 1)
		m_img.Draw(pDC,0, CPoint(rc.left,rc.top),ILD_TRANSPARENT);

	else if (dwData == 2)
		m_img.Draw(pDC,1, CPoint(rc.left,rc.top),ILD_TRANSPARENT);
	
	int nX = rc.left; // Save for lines

	rc.left += GLYPH_WIDTH + 2; // Text Position
	
	pDC->TextOut(rc.left,rc.top,strCurFont);
	
	// Restore State of context
	pDC->RestoreDC(nIndexDC);	
}

void CPrinterComboBox::PostNcDestroy() 
{
	CComboBox::PostNcDestroy();
	delete this;
}
