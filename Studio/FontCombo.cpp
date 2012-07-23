// FontBox.cpp : implementation file
//

#include "Stdafx.h"
#include "FontCombo.h"
#include "Resource.h"
#include "ToolboxPane.h"
#include "StudioFrame.h"
#include "StudioWorkspace.h"


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
	m_img.Create( 15, 13, ILC_COLOR32 | ILC_MASK, 1, 1 );
	HMODULE hRes = theWorkspace.GetLocalResourceModule();
	m_img.Add( LoadIcon( hRes, MAKEINTRESOURCE( IDI_TTFONT ) ) );
	HKEY hkFontMgmt;
	if( ERROR_SUCCESS == RegOpenKey( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Font Management"), &hkFontMgmt ) )
	{
		DWORD dwType;
		DWORD cbVal;
		if( ERROR_SUCCESS == RegQueryValueEx( hkFontMgmt, _T("Inactive Fonts"), NULL, &dwType, NULL, &cbVal ) && dwType == REG_MULTI_SZ )
		{
			cbVal += sizeof(TCHAR);
			TCHAR* pszzVal = new TCHAR[cbVal / sizeof(TCHAR)];
			if( ERROR_SUCCESS == RegQueryValueEx( hkFontMgmt, _T("Inactive Fonts"), NULL, &dwType, (LPBYTE)pszzVal, &cbVal ) && dwType == REG_MULTI_SZ )
			{
				TCHAR* pch = pszzVal;
				while( *pch )
				{
					msetHiddenFonts.insert( pch );
					while( *++pch );
					++pch; //skip to next string
				}
			}
			delete[] pszzVal;
		}
		RegCloseKey( hkFontMgmt );
	}
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
	if (lpDIS->itemID < UINT(GetCount()))
	{
		GetLBText(lpDIS->itemID,strCurFont);
	}
	else
	{
		return;
	}

	CFontObj* pFontObj = NULL;
	m_mapFonts.Lookup(strCurFont,pFontObj);

//	ASSERT(pFontObj != NULL);
	DWORD dwData = GetFontTypeId(strCurFont);// GetItemData(lpDIS->itemID);//pFontObj->GetFlags();
	
	// Render Bitmaps
	if (dwData == 4 || dwData == 6)
		m_img.Draw(pDC,0, CPoint(rc.left,rc.top-nOffset2),ILD_TRANSPARENT);
	
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

DWORD CFontCombo::GetFontTypeId( LPCTSTR pszFont )
{
	CString strKey;
	CFontObj* pFontObj = NULL;
	POSITION pos = m_mapFonts.GetStartPosition();
	while (pos != NULL)
	{
		pFontObj = NULL;
		m_mapFonts.GetNextAssoc(pos,strKey,pFontObj);		
		if (strKey == pszFont)
			return pFontObj->GetFlags();
	}
	return 0;
}

// As it says...EnumerateFonts on the system
BOOL CFontCombo::EnumerateFonts()
{
	HDC hDC = ::GetWindowDC(NULL);
	LOGFONT lf;
	ZeroMemory(&lf,sizeof(lf));
	lf.lfCharSet = DEFAULT_CHARSET;

	int nResult = EnumFontFamiliesEx(
			hDC,	// handle to device context
			&lf,	// pointer to logical font information
			(FONTENUMPROC)EnumFamScreenCallBackEx,	// pointer to callback function
			(LPARAM) this,	// application-supplied data
			(DWORD) 0);
	::ReleaseDC(NULL,hDC);	

	if( nResult == 0 )
		return FALSE;

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

	POSITION pos = m_mapFonts.GetStartPosition();
	
	// add the sysytem fonts manually because they are not added otherwise
	AddString( _T("MS Shell Dlg"));
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
	SetTimer(1, 200, NULL);

	// Yep tip window is created here
	m_wndTip.Create(this);
	
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
void CFontCombo::AddFont(LPCTSTR pszFontName, DWORD dwFlags)
{
	CFontObj* pFontObj;	

	// Check fonts not aleady in the array
	if (!m_mapFonts.Lookup(pszFontName,pFontObj))
		m_mapFonts.SetAt(pszFontName,new CFontObj(dwFlags));
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
			int itemHeight = GetItemHeight( 0 );
			CRect rc;
			GetDroppedControlRect(&rc);
			int ctVisible = SendMessage( /*CB_GETMINVISIBLE*/ 0x1702, 0, 0 );
			if( ctVisible <= 0 )
				ctVisible = (rc.Height() / itemHeight);
			int lastVis = FirstVis + ctVisible - 1;
			if (nSel <= lastVis)
			{
				// Show tip in correct position
				int nThisHeight = itemHeight * ((nSel - FirstVis) + 1);
				CPoint pt(rc.right + 5,rc.top + nThisHeight);
				CString str;
				GetLBText(nSel,str);

				m_wndTip.SetWindowText( str );

				LOGFONT lfTip;
				ZeroMemory( &lfTip, sizeof(lfTip) );
				lfTip.lfHeight = 18;
				lstrcpyn( lfTip.lfFaceName, str, sizeof(lfTip.lfFaceName)/sizeof(lfTip.lfFaceName[0]) );
				CFont Font;
				Font.CreateFontIndirect( &lfTip );
				m_wndTip.SetFont( &Font, FALSE );

				CDC* pDC = GetDC();
				CFont* pOldFont = pDC->SelectObject( &Font );
				CSize sz = pDC->GetTextExtent( str );
				pDC->SelectObject( pOldFont );
				ReleaseDC( pDC );
				sz.cx += 8;
				sz.cy += 8;
				m_wndTip.Show( pt, sz );
			}
		}
	}
	CComboBox::OnTimer(nIDEvent);
}

bool CFontCombo::IsHidden( LPCTSTR pszFont )
{
	if( msetHiddenFonts.find( pszFont ) != msetHiddenFonts.end() )
		return true;
	if( *pszFont == _T('@') && msetHiddenFonts.find( pszFont + 1 ) != msetHiddenFonts.end() )
		return true;
	return false;
}

BOOL CALLBACK AFX_EXPORT CFontCombo::EnumFamScreenCallBackEx(ENUMLOGFONTEX* pelf, 
	NEWTEXTMETRICEX* lpntm, int FontType, CFontCombo* pThis)

{
	// don't put in non-printer raster fonts
	if (FontType & RASTER_FONTTYPE)
		return 1;
	
	if (FontType & TRUETYPE_FONTTYPE)
	{
		CFontCombo* pFontCombo = (CFontCombo*)pThis;
		CString sFont = pelf->elfLogFont.lfFaceName;
		if( pFontCombo->IsHidden( sFont ) )
			return 1;
		pFontCombo->AddFont(sFont, TRUETYPE_FONTTYPE);
	}
	
	return 1; // Call me back
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
