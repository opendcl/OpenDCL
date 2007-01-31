// FontBox.cpp : implementation file
//

#include "Stdafx.h"
#include "resource.h"
#include "FontCombo.h"
#include "ToolBox.h"
#include "ObjectDCLView.h"
#include "EditorWorkspace.h"


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
{
	// Load up glyphs
	m_img.Create(15,13,ILC_COLOR | ILC_MASK, 3, 1);

	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_TRUEFONT), RT_GROUP_ICON);

	
	HICON hIcon = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(IDI_TRUEFONT), IMAGE_ICON, 0, 0, 0);	
	m_img.Add(hIcon);
	DestroyIcon(hIcon);

	hIcon = (HICON)::LoadImage(hInstResource, MAKEINTRESOURCE(IDI_ACADFONT), IMAGE_ICON, 0, 0, 0);
	m_img.Add(hIcon);
	DestroyIcon(hIcon);

	
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
	ON_WM_DESTROY()
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetfocus)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_WM_TIMER()
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	ON_WM_MEASUREITEM()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontCombo message handlers
// Overridables for onwerdraw combos
void CFontCombo::DeleteItem(LPDELETEITEMSTRUCT) 
{
}
// Overridables for onwerdraw combos
void CFontCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = 15;
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
	int nOffset = 0;
	// Lets make a CDC for ease of use
	CDC *pDC = CDC::FromHandle(lpDIS->hDC);
	
	ASSERT(pDC); // Attached failed
	
	int nOffset2 = 1;
	if (lpDIS->rcItem.top > 3)
	{
		nOffset2 = 0;
	}

	CRect rc(lpDIS->rcItem);
	
	if (lpDIS->itemState > 4000)
		nOffset = 3;
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

	rc.top += nOffset;
	rc.bottom += nOffset;

	CString strCurFont,strNextFont;
	if (lpDIS->itemID >= 0 && lpDIS->itemID < UINT(GetCount()))
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
		m_img.Draw(pDC,0, CPoint(rc.left,rc.top-nOffset2),ILD_TRANSPARENT);

	else if (dwData == 6)
		m_img.Draw(pDC,1, CPoint(rc.left,rc.top-nOffset2),ILD_TRANSPARENT);
	
	int nX = rc.left; // Save for lines

	rc.left += GLYPH_WIDTH + 2; // Text Position
	
	pDC->TextOut(rc.left,rc.top-nOffset2,strCurFont);
	
	// GetItemData - return font in use
	if (GetItemData(lpDIS->itemID))
	{
		GetLBText(lpDIS->itemID+1,strNextFont);
		CFontObj* pFontObjNext;
		m_mapFonts.Lookup(strNextFont,pFontObjNext);
		
		if (!GetItemData(lpDIS->itemID+1) && lpDIS->itemState < 4000)
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
	CString sFontName;
	CString sThisFontName = sName;
	sThisFontName.MakeLower();
	// lets check to set if it's an autocad font first
	for (int i = 0; i< m_AcadFontFileList.GetSize(); i++)
	{
		sFontName = m_AcadFontFileList.GetAt(i);
		sFontName.MakeLower();
		if (sThisFontName == sFontName)
		{
			return 6;
		}
	}

	CString strKey;
	CFontObj* pFontObj = NULL;
	int nCount = m_mapFonts.GetCount();
	POSITION pos = m_mapFonts.GetStartPosition();
	while (pos != NULL)
	{
		pFontObj = NULL;
		m_mapFonts.GetNextAssoc(pos,strKey,pFontObj);		
		if (strKey == sName)
			return pFontObj->GetFlags();

	}

	
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
		{
			SetWindowText(m_strFontSave);
		}
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
	CFontObj* pFontObj;
	CString strKey,strComp;
	
	EnumerateFonts();
	
	// add a list of acad fonts
	m_AcadFontFileList.Add("bigfont");
	m_AcadFontFileList.Add("chineset");
	m_AcadFontFileList.Add("complex");
	m_AcadFontFileList.Add("extfont");
	m_AcadFontFileList.Add("gbcbig");
	m_AcadFontFileList.Add("gbeitc");
	m_AcadFontFileList.Add("gbenor");
	m_AcadFontFileList.Add("gothice");
	m_AcadFontFileList.Add("gothicg");
	m_AcadFontFileList.Add("gothici");
	m_AcadFontFileList.Add("greekc");
	m_AcadFontFileList.Add("greeks");
	m_AcadFontFileList.Add("isocp");
	m_AcadFontFileList.Add("isocp2");
	m_AcadFontFileList.Add("isocp3");
	m_AcadFontFileList.Add("isoct");
	m_AcadFontFileList.Add("isoct2");
	m_AcadFontFileList.Add("isoct3");
	m_AcadFontFileList.Add("italic");
	m_AcadFontFileList.Add("italicc");
	m_AcadFontFileList.Add("italict");
	m_AcadFontFileList.Add("monotxt");
	m_AcadFontFileList.Add("romanc");
	m_AcadFontFileList.Add("romand");
	m_AcadFontFileList.Add("romans");
	m_AcadFontFileList.Add("romant");
	m_AcadFontFileList.Add("scriptc");
	m_AcadFontFileList.Add("scripts");
	m_AcadFontFileList.Add("syastro");
	m_AcadFontFileList.Add("symap");
	m_AcadFontFileList.Add("symeteo");
	m_AcadFontFileList.Add("symusic");
	m_AcadFontFileList.Add("txt");
	m_AcadFontFileList.Add("whgdtxt");
	m_AcadFontFileList.Add("whgtxt");
	m_AcadFontFileList.Add("whtgtxt");
	m_AcadFontFileList.Add("whtmtxt");
	

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
	CString strDefault = "";
	
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
	CFontObj* pFontObj;	

	// Check fonts not aleady in the array
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
				int nThisHeight = itemHeight * ((nSel - FirstVis) + 1);
				
				CPoint pt(rc.right + 5,rc.top + nThisHeight);

				// Show tip in correct position
				CString str;
				GetLBText(nSel,str);
				
				m_wndTip.ShowTips(pt,str);
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
		dwData = TRUETYPE_FONTTYPE;			
		((CFontCombo*)pThis)->AddFont(pelf->elfLogFont.lfFaceName, dwData);
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
	
	if (theEditorWorkspace.GetToolBox()->m_pActiveView == NULL)
		return;
	if (((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->m_pThisDclForm == NULL)
		return;
	
	((CObjectDCLView*)theEditorWorkspace.GetToolBox()->m_pActiveView)->UpdateFont(sString);
}

BOOL CFontCombo::PreTranslateMessage(MSG* pMsg) 
{
	
	return CComboBox::PreTranslateMessage(pMsg);
}

void CFontCombo::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = 15;
	
	CComboBox::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
