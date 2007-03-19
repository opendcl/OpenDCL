// FontBox.cpp : implementation file
//

#include "Stdafx.h"
#include "FontCombo.h"
#include "SharedRes.h"
#include "DclControlObject.h"
#include "PropertyObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"
#include "ArxGridCtrl.h"


// Constant and should not be changed unless 
// image IDB_GLYPH is changed
#define GLYPH_WIDTH 15 
const int SHX_FONTTYPE = 6;
/////////////////////////////////////////////////////////////////////////////
// CFontCombo

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo
//
// DESCRIPTION:	Constructor
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
CFontCombo::CFontCombo()
: m_ArxControl( NULL )
{
	// Load up glyphs
	m_img.Create(15,13,ILC_COLOR4 | ILC_MASK, 3, 1);

	HMODULE hRes = _hdllInstance;
	HICON hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_TRUEFONT), IMAGE_ICON, 0, 0, 0);	
	m_img.Add(hIcon);
	DestroyIcon(hIcon);

	hIcon = (HICON)::LoadImage(hRes, MAKEINTRESOURCE(IDI_ACADFONT), IMAGE_ICON, 0, 0, 0);
	m_img.Add(hIcon);
	DestroyIcon(hIcon);
}
		 
BOOL CFontCombo::Create(CDclControlObject* pControl, CWnd* pParentWnd, UINT nID ) 
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

	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_EX_CLIENTEDGE | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS | ES_AUTOHSCROLL | WS_CLIPCHILDREN | CBS_SORT | CBS_OWNERDRAWFIXED;
	
	if (pControl->GetBoolProperty(nIsTabStop) != FALSE)
		dwStyle = dwStyle | WS_TABSTOP;
	else
		dwStyle = dwStyle | WS_GROUP;

	switch (pControl->GetLngProperty(nComboBoxStyle))
	{
	case 8:
		{
			pControl->SetLongProperty(nHeight, (pControl->GetLngProperty(nHeight) + pControl->GetLngProperty(nDropDownHeight)));
			ArxRect.bottom = ArxRect.bottom  + pControl->GetLngProperty(nDropDownHeight);
			dwStyle = dwStyle | CBS_DROPDOWNLIST;
			break;
		}
	case 9:
		{
			dwStyle = dwStyle | CBS_SIMPLE;
			break;
		}	
	}

	RetVal = CComboBox::Create( dwStyle, ArxRect, pParentWnd, nID );

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



BOOL CFontCombo::Create(int nStyle, CRect rc, CWnd* pParentWnd, UINT nID) 
{
	BOOL RetVal;
	DWORD dwStyle;

	if (rc.Height() < 100)
		rc.bottom = rc.top + 100;

	// set the arx control pointer
    m_ArxControl = NULL;
		
	//dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_CLIPSIBLINGS
	//		  | CBS_HASSTRINGS | ES_AUTOHSCROLL | WS_CLIPCHILDREN;
	
	dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_EX_CLIENTEDGE | WS_CLIPSIBLINGS
			  | CBS_HASSTRINGS | ES_AUTOHSCROLL | WS_CLIPCHILDREN | CBS_SORT | CBS_OWNERDRAWFIXED;
	dwStyle = dwStyle | CBS_NOINTEGRALHEIGHT;
	

	dwStyle = dwStyle | CBS_SORT;		

	dwStyle = dwStyle | WS_GROUP;

	dwStyle = dwStyle | CBS_DROPDOWNLIST;
	
	RetVal = CComboBox::Create( dwStyle, rc, pParentWnd, nID );
	
	return RetVal;
}
////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::~CFontCombo
//
// DESCRIPTION:	Constructor
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
CFontCombo::~CFontCombo()
{
}

BEGIN_MESSAGE_MAP(CFontCombo, CComboBox)
	//{{AFX_MSG_MAP(CFontCombo)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_WM_TIMER()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropdown)
	ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontCombo message handlers
// Overridables for onwerdraw combos
void CFontCombo::DeleteItem(LPDELETEITEMSTRUCT) 
{
}

// Initialize fonts for combo
int CFontCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	Initialize();

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::DrawItem
//
// DESCRIPTION:	Owner draw to render bitmap and font name
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
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
	if (lpDIS->itemID >= 0 && lpDIS->itemID < (UINT)GetCount())
	{
		GetLBText(lpDIS->itemID,strCurFont);
	}
	else
	{
		return;
	}


	CFontObj* pFontObj = NULL;
	int nCount = m_mapFonts.GetCount();
	m_mapFonts.Lookup(strCurFont,pFontObj);

//	ASSERT(pFontObj != NULL);
	DWORD dwData = GetFontTypeId(strCurFont);// GetItemData(lpDIS->itemID);//pFontObj->GetFlags();
	
	// Render Bitmaps
	if (dwData == 4)
		m_img.Draw(pDC,0, CPoint(rc.left,rc.top),ILD_TRANSPARENT);

	else if (dwData == 6)
		m_img.Draw(pDC,1, CPoint(rc.left,rc.top),ILD_TRANSPARENT);
	
	int nX = rc.left; // Save for lines

	rc.left += GLYPH_WIDTH + 2; // Text Position
	
	pDC->TextOut(rc.left,rc.top,strCurFont);
	
	// GetItemData - return font in use
	if (GetItemData(lpDIS->itemID))
	{
		GetLBText(lpDIS->itemID+1,strNextFont);
		CFontObj* pFontObjNext;
		m_mapFonts.Lookup(strNextFont,pFontObjNext);
		
		if (!GetItemData(lpDIS->itemID+1))
		{
			// Draw font MRU separator ==============
			//                         
			TEXTMETRIC tm;
			pDC->GetTextMetrics(&tm);

			pDC->MoveTo(nX,rc.top+tm.tmHeight);
			pDC->LineTo(rc.right,rc.top+tm.tmHeight);

			pDC->MoveTo(nX,rc.top+tm.tmHeight+2);
			pDC->LineTo(rc.right,rc.top+tm.tmHeight+2);
		}
	}
	// Restore State of context
	pDC->RestoreDC(nIndexDC);
}

DWORD CFontCombo::GetFontTypeId(CString sName)
{
	CFontObj* pFontObj;
	if( m_mapFonts.Lookup( sName, pFontObj ) )
		return pFontObj->GetFlags();
	return 0;
}

// As it says...EnumerateFonts on the system
BOOL CFontCombo::EnumerateFonts()
{
	HDC hDC;
	
	// Get screen fonts
	hDC = ::GetWindowDC(NULL);
	
	LOGFONT lf;
	
	ZeroMemory(&lf,sizeof(lf));
	lf.lfCharSet = DEFAULT_CHARSET;

	if (!EnumFontFamiliesEx(
			hDC,	// handle to device context
			&lf,	// pointer to logical font information
			(FONTENUMPROC)EnumFamScreenCallBackEx,	// pointer to callback function
			(LPARAM) this,	// application-supplied data
			(DWORD) 0))
		return FALSE;

	::ReleaseDC(NULL,hDC);	
	
	return TRUE; // All's ok
}


////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::OnDestroy
//
// DESCRIPTION:	Destroys font objects
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::OnDestroy() 
{
	POSITION pos = m_mapFonts.GetStartPosition();
	while (pos)
	{
		CString strKey;
		CFontObj* pFontObj;
		m_mapFonts.GetNextAssoc(pos,strKey,pFontObj);
		delete pFontObj;
	}
	CComboBox::OnDestroy();
}

void CFontCombo::OnMouseMove(UINT nFlags, CPoint point) 
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

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::SetFontInUse
//
// DESCRIPTION:	Flags a font that is in use, similar to MS Words font MRU list
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::SetFontInUse(const CString& strFont)
{
	CFontObj *pFontObj;
	CString strFontDesc;
	
	// Find the desired font
	if (m_mapFonts.Lookup(strFont,pFontObj))
	{
		// Check the font is not already in use
		if (!pFontObj->GetFontInUse())
		{
			// Set the font as the top most and selected font
			pFontObj->SetFontInUse(TRUE);
			InsertString(0,strFont);
			SetItemData(0,DWORD(1));

			// Only hold six MRU'ed fonts
			// so drop the 7th.
			if (GetItemData(6))
			{
				GetLBText(6,strFontDesc);
				if (!strFontDesc.IsEmpty())
				{
					if (m_mapFonts.Lookup(strFontDesc,pFontObj))
					{
						pFontObj->SetFontInUse(FALSE);
						DeleteString(6);
					}
				}
			}
		}
		else
		{
			// If font is aleady MRU'ed position it as the first font
			int nSel = FindString(-1,strFont);
	
			if (nSel != CB_ERR)
			{
				// Remove from original position
				DeleteString(nSel);
				// Restore to begining of list
			
				InsertString(0,strFont);
				SetItemData(0,TRUE); // Mark as selected
				SetCurSel(0);	// Bring as current selection
			}
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::OnKillfocus
//
// DESCRIPTION:	Set up the current font
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::OnKillfocus() 
{
	SetCurrentFont();
	m_wndTip.ShowWindow(SW_HIDE);

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventKillFocus), m_bInvokeWithSendString);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::OnSetfocus
//
// DESCRIPTION:	Save current font
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::OnSetfocus() 
{
	// Save off original font
	GetWindowText(m_strFontSave);

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventSetFocus), m_bInvokeWithSendString);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::SetCurrentFont
//
// DESCRIPTION:	Set up the current font
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::SetCurrentFont()
{
	CString strSelFont;

	// Change font in edit box is a known font
	// otherwise reject and restore orginal
	int nSel = GetCurSel();

	if (nSel == CB_ERR)
	{
		GetWindowText(strSelFont);
		nSel = FindStringExact(-1,strSelFont);
		
		if (nSel == CB_ERR)
			SetWindowText(m_strFontSave);
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::OnCloseUp
//
// DESCRIPTION:	Hide tip window and set font name
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::OnCloseUp()
{
	int nSel;	

	CString strFont;
	// Set Face Name
	SetCurrentFont();
	nSel = GetCurSel();
	if (nSel != CB_ERR)
		GetLBText(nSel,strFont);

	SetFontInUse(strFont);
	m_wndTip.ShowWindow(SW_HIDE);
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::Initialize
//
// DESCRIPTION:	Initials combo box
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::Initialize()
{
	// get the list of existing AutoCAD shx fonts
	CString sFontPath = GetFontPath();	
	CFileFind finder;
	BOOL bResult = finder.FindFile(sFontPath + "\\*.shx");
	while (bResult)
	{
		bResult = finder.FindNextFile();
		if (finder.IsDots()) // skip . and .. files
			continue;
		m_AcadFontFileList.Add(finder.GetFileName().MakeLower());
	}
	finder.Close();

	CFontObj* pFontObj;
	CString strKey,strComp;
	EnumerateFonts();
	
	POSITION pos = m_mapFonts.GetStartPosition();
	
	// add the sysytem fonts manually because they are not added otherwise
	AddString( _T("MS Sans Serif"));
	AddString( _T("MS Serif"));
	AddString( _T("System"));
	AddString( _T("Terminal"));
	AddString( _T("Small Fonts"));
	AddString( _T("Fixedsys"));
	AddString( _T("Courier"));

	while (pos)
	{
		m_mapFonts.GetNextAssoc(pos,strKey,pFontObj);
		AddString(strKey);
	}
	
	// We set the timer because its the only way we know when a selection
	// has changed - use for tip window
	SetTimer(1, 500, NULL);

	// Yep tip window is created here
	m_wndTip.Create(this);

	// Set default font name
	CString strDefault;
}

CString CFontCombo::GetFontPath()
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
				return sCompile;
			sCompile = _T("");
		}
	}
	return CString();
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::AddFont
//
// DESCRIPTION:	Adds a font to the internal array
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::AddFont(CString strName, DWORD dwFlags)
{
	// Check fonts not aleady in the array
	CFontObj* pFontObj;	
	if (!m_mapFonts.Lookup(strName,pFontObj))
		m_mapFonts.SetAt(strName,new CFontObj(dwFlags));
}

////////////////////////////////////////////////////////////////////////////////
//
// FUNCTION:	CFontCombo::OnTimer
//
// DESCRIPTION:	Positions tip window against selected font
//
// INPUTS:		
//
// RETURNS:     
//
// NOTES:       
//
// MODIFICATIONS:
//
// Name			Date      Version	Comments
// N T ALMOND   25/09/98  1.0		Origin
//
////////////////////////////////////////////////////////////////////////////////
void CFontCombo::OnTimer(UINT nIDEvent) 
{
	// Is combo open
	if (GetDroppedState( ))
	{
		int nSel = GetCurSel();
		int FirstVis = GetTopIndex();
		
		// Selected
		if (nSel != -1 && FirstVis <= nSel)
		{
			CRect rc;
			GetDroppedControlRect(rc);
			int itemHeight = GetItemHeight(0);
			int lastVis = FirstVis + ((rc.Height()-itemHeight)/itemHeight);
			if (nSel <= lastVis)
			{
				int nHeight = itemHeight * ((nSel - FirstVis) + 1);
				
				CPoint pt(rc.right + 5,rc.top + nHeight);

				// Show tip in correct position
				CString str;
				GetLBText(nSel,str);
				if (m_ArxControl)
				{
					//if (m_ArxControl->GetLngProperty(nComboBoxStyle) == 8)
						m_wndTip.ShowTips(pt,str);
				}
				else
				{
					//m_wndTip.ShowTips(pt,str);
				}
			}
		}
	}
	CComboBox::OnTimer(nIDEvent);
}

BOOL CALLBACK AFX_EXPORT CFontCombo::EnumFamScreenCallBackEx(ENUMLOGFONTEX* pelf, 
	NEWTEXTMETRICEX* lpntm, int FontType, LPVOID pThis)

{
	// don't put in non-printer raster fonts
	if (FontType & RASTER_FONTTYPE)
		return 1;
	
	DWORD dwData;
	
	if (FontType & TRUETYPE_FONTTYPE)
	{
		CString sFontName = pelf->elfLogFont.lfFaceName;
		dwData = TRUETYPE_FONTTYPE;			
		//check whether it's an SHX font
		CString sShxName = sFontName;
		sShxName.MakeLower() += _T(".shx");
		for (int i = 0; i< ((CFontCombo*)pThis)->m_AcadFontFileList.GetSize(); i++)
		{
			if (sShxName == ((CFontCombo*)pThis)->m_AcadFontFileList.GetAt(i))
			{
				sFontName += _T(".shx");
				dwData = 6;
				break;
			}
		}
		((CFontCombo*)pThis)->AddFont(sFontName, dwData);
	}
	return 1; // Call me back
}

void CFontCombo::OnSelchange() 
{
	CString sString;
	int nSel = GetCurSel();
	int nTextLength = GetLBTextLen(nSel);      
	GetLBText(nSel, sString.GetBuffer(nTextLength));
	sString.ReleaseBuffer();

	if (m_ArxControl)
		InvokeMethodIntString(m_ArxControl->GetStrProperty(nEventSelChanged), nSel, sString, m_bInvokeWithSendString);
	
	if (m_ArxControl == NULL)
	{		
		// Send Notification to parent of ListView ctrl
		CArxGridCtrl *pListCtrl = (CArxGridCtrl*)GetParent()->GetParent();
		pListCtrl->SetItemText(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, sString);
		DWORD nData = GetFontTypeId(sString);

		if (nData == 4)
			nData = 0;
		else if (nData == 6)
			nData = 1;
		else 
			nData = -1;
		
		pListCtrl->SetItemImage(pListCtrl->m_nRowSelected, pListCtrl->m_nColSelected, nData);

		// fire the on Grid edit cell event.
		pListCtrl->EndEditControls(pListCtrl);
	}
}

BOOL CFontCombo::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message== WM_KEYDOWN && pMsg->wParam==VK_RETURN)
	{
		if (m_ArxControl)
		{
			if (m_ArxControl->GetBoolProperty(nReturnAsTab) == TRUE)
				pMsg->wParam = VK_TAB;
		}
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

void CFontCombo::OnDropdown() 
{

	if (m_ArxControl)
		// call methods to invoke the event
		InvokeMethod(m_ArxControl->GetStrProperty(nEventDropDown), m_bInvokeWithSendString);	
}

void CFontCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 16;
}

void CFontCombo::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = 16;
	CComboBox::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}