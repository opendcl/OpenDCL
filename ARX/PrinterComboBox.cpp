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
#include "SpreadSheet.h"
#include "Workspace.h"
#include "ControlPane.h"
#include "ControlTypes.h"

// Constant and should not be changed unless 
// image IDB_GLYPH is changed
#define GLYPH_WIDTH 16 
#define dwFlags PRINTER_ENUM_CONNECTIONS | PRINTER_ENUM_LOCAL


CRect CPrinterComboControlX::GetWndRect() const
{
	CRect rectCombo = CArxDialogControl::GetWndRect();
	long nListHeight = mpTemplate->GetLngProperty(nDropDownHeight);
	if( nListHeight < 40 )
		nListHeight = 40;
	rectCombo.bottom += nHeight;
	return rectCombo;
}

DWORD CPrinterComboControlX::GetWndStyle() const
{
	DWORD dwStyle = CArxDialogControl::GetWndStyle();
	dwStyle |= WS_BORDER | WS_VSCROLL | WS_EX_CLIENTEDGE | CBS_HASSTRINGS |
						ES_AUTOHSCROLL | WS_CLIPCHILDREN | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED;
	if (mpTemplate->GetBoolProperty(nSorted) == TRUE)
		dwStyle |= CBS_SORT;		
	return dwStyle;
}


/////////////////////////////////////////////////////////////////////////////
// CPrinterComboBox

CPrinterComboBox::CPrinterComboBox( CControlPane& Pane, CDclControlObject* pTemplate, UINT nID )
: mControlX( pTemplate, &Pane, this )
, mpSourceControl( pTemplate )
, mpControlPane( &Pane )
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

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
: mControlX( pTemplate, &Pane, this )
, mpSourceControl( pTemplate )
, mpControlPane( &Pane )
{
	// No tooltip created
	m_ToolTip.m_hWnd = NULL;

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

CComboBox* CPrinterComboBox::FindPaperSizesCombo() const
{
	const CControlPane::TDialogControls& rControls = mpControlPane->GetControlsList();
	size_t ctMax = rControls.size();
	for( size_t idx = 0; idx < ctMax; ++idx )
	{
		TDialogControlPtr pControl = rControls[idx];
		if (pControl->GetControlType() == CtlComboBox)
		{
			if( pControl->GetTemplate()->GetLngProperty(nComboBoxStyle) == CmboStyle_PlotterPaperSizes )
				return (CComboBox*)&*pControl;
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

bool CPrinterComboBox::Create( CWnd* pParentWnd, UINT nID )
{
	return Create( pParentWnd, nID, mControlX.GetWndRect() );
}

bool CPrinterComboBox::Create( CWnd* pParentWnd, UINT nID, CRect rc )
{
	bool bSuccess =
		CComboBox::Create( mControlX.GetWndStyle(),
											 rc,
											 pParentWnd,
											 nID );
	VERIFY(CWnd::SubclassDlgItem(nID, pParentWnd));

	if( bSuccess && !mControlX.ApplyPropertiesEnum() )
		bSuccess = false;

	InitToolTip();
	SetToolTipEx(this, m_ToolTip, mpSourceControl);

	if( mpSourceControl->GetLngProperty(nEventInvoke) == 1 )
		m_bInvokeWithSendString = true;
	else
		m_bInvokeWithSendString = false;

	return bSuccess;
}

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


void CPrinterComboBox::SetTooltipText(CString* spText, BOOL bActivate)
{
} // End of SetTooltipText

void CPrinterComboBox::InitToolTip()
{
	if (m_ToolTip.m_hWnd == NULL)
	{
		// Create ToolTip control
		m_ToolTip.Create(this);
		// Create inactive
		m_ToolTip.Activate(FALSE);
	}
} // End of InitToolTip

BOOL CPrinterComboBox::PreTranslateMessage(MSG* pMsg) 
{
	InitToolTip();
	m_ToolTip.RelayEvent(pMsg);	
	
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (mpSourceControl->GetBoolProperty(nReturnAsTab) == TRUE)
			pMsg->wParam = VK_TAB;
		
	}

	return CComboBox::PreTranslateMessage(pMsg);
}


void CPrinterComboBox::OnMouseMove(UINT nFlags, CPoint point) 
{

	if (mpSourceControl)
		InvokeMethodIntIntInt(
			mpSourceControl->GetStrProperty(nEventMouseMove),
			nFlags,
			point.x,
			point.y,
			m_bInvokeWithSendString);
	
	
	CComboBox::OnMouseMove(nFlags, point);
}

void CPrinterComboBox::OnSetFocus(CWnd* pOldWnd) 
{
	if (mpSourceControl)
		// call methods to invoke the event
		InvokeMethod(mpSourceControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);

	CComboBox::OnSetFocus(pOldWnd);	
	
}

void CPrinterComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	if (mpSourceControl)
		// call methods to invoke the event
		InvokeMethod(mpSourceControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);



	CComboBox::OnKillFocus(pNewWnd);
	

	
}

void CPrinterComboBox::OnSelchange() 
{
	CString sString;
	int nSel = GetCurSel();
	int nTextLength = GetLBTextLen(nSel);      
	GetLBText(nSel, sString.GetBuffer(nTextLength));
	sString.ReleaseBuffer();

	if( mpSourceControl )
	{
		CComboBox* pPaperSizesCombo = FindPaperSizesCombo();
		if( !pPaperSizesCombo )
		{
			InvokeMethodIntString(mpSourceControl->GetStrProperty(nEventSelChanged), nSel, sString, m_bInvokeWithSendString);
			mpSourceControl->SetStringProperty(nText, sString);
			return;
		}

		BeginWaitCursor();
		// get current document
		AcApDocument* pDoc = acDocManager->curDocument();
		Acad::ErrorStatus es = acDocManager->lockDocument( pDoc, AcAp::kWrite, NULL, NULL, false);

		// Get layout manager pointer
		AcApLayoutManager *pLayoutManager = (AcApLayoutManager*)acdbHostApplicationServices()->layoutManager();
		assert(pLayoutManager != NULL);

		CString sLayout = pLayoutManager->findActiveLayout(true);
		   
		// Get the specific layout
		AcDbLayout *pLayout = pLayoutManager->findLayoutNamed(sLayout,true);
		if (pLayout == NULL) 
			return;
		   
		es = pLayout->setPlotSettingsName(sString);
		if (es != Acad::eOk)
			return;

		// Get the plot settings validator
		AcDbPlotSettingsValidator *pPlotSettingsValidator = acdbHostApplicationServices()->plotSettingsValidator();
		assert( pPlotSettingsValidator != NULL );

		
		if (nSel == 0)
		{
			pPlotSettingsValidator->setPlotCfgName(pLayout,_T("None"),_T("none_user_media"));
			if (pPaperSizesCombo)
				pPaperSizesCombo->ResetContent();
			pLayout->close();
			es = acDocManager->unlockDocument(pDoc);
			EndWaitCursor();
			if (mpSourceControl)
			{
				InvokeMethodIntString(mpSourceControl->GetStrProperty(nEventSelChanged), nSel, sString, m_bInvokeWithSendString);
				mpSourceControl->SetStringProperty(nText, sString);
			}
			return;
		}
		else
		{
			pPlotSettingsValidator->setPlotCfgName(pLayout,sString);
		}

		// Refresh the layout lists in order to use it.
		pPlotSettingsValidator->refreshLists(pLayout);
	    
		AcArray<const TCHAR*> mediaList;
		pPlotSettingsValidator->canonicalMediaNameList(pLayout, mediaList);

		pPaperSizesCombo->ResetContent();
		for (int i=0; i<mediaList.length(); i++)
		{
			CString value = mediaList[i];
			const TCHAR * Value;
			pPlotSettingsValidator->getLocaleMediaName(pLayout, i,Value);
			pPaperSizesCombo->AddString(Value);
		}

		pLayout->close();

		es = acDocManager->unlockDocument(pDoc);

		EndWaitCursor();

		InvokeMethodIntString(mpSourceControl->GetStrProperty(nEventSelChanged), nSel, sString, m_bInvokeWithSendString);
		mpSourceControl->SetStringProperty(nText, sString);
	}
	else
	{		
		// Send Notification to parent of ListView ctrl
		CSpreadSheet *pListCtrl = (CSpreadSheet*)GetParent()->GetParent();
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);
		pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, -1);
		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}
}

void CPrinterComboBox::OnDestroy() 
{
	CComboBox::OnDestroy();
	
	// delete the tool tip text control object
	m_ToolTip.DelTool(this, 1);
	
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
