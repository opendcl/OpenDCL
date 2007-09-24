// FontPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "FontPropPage.h"
#include "PropertyObject.h"
#include "EditorWorkspace.h"
#include "DclControlObject.h"
#include "ControlHolder.h"
#include "SharedRes.h"

#define nDeBoldWeight 700


/////////////////////////////////////////////////////////////////////////////
// CFontPropertyPage property page

IMPLEMENT_DYNCREATE(CFontPropertyPage, CPropertyPage)

CFontPropertyPage::CFontPropertyPage(TDclControlPtr pDclControl /*= NULL*/)
: CPropertyPage(CFontPropertyPage::IDD)
, mpDclControl( pDclControl )
{
	if( mpDclControl )
	{
		m_pFontName = mpDclControl->GetPropertyObject(Prop::FontName);
		m_pFontSize = mpDclControl->GetPropertyObject(Prop::FontSize);
		m_pFontStrikeOut = mpDclControl->GetPropertyObject(Prop::FontStrikeout);
		m_pFontUnderline = mpDclControl->GetPropertyObject(Prop::FontUnderline);
		m_pFontBold = mpDclControl->GetPropertyObject(Prop::FontBold);
		m_pFontItalic = mpDclControl->GetPropertyObject(Prop::FontItalic);
	}
	else
	{
		m_pFontName = NULL;
		m_pFontSize = NULL;
		m_pFontStrikeOut = NULL;
		m_pFontUnderline = NULL;
		m_pFontBold = NULL;
		m_pFontItalic = NULL;
	}

	LOGFONT* plogfont = NULL;
	static LOGFONT logfont;
	CWindowDC dc(GetDesktopWindow());
	m_cyPixelsPerInch = GetDeviceCaps(dc, LOGPIXELSY);
	if( plogfont == NULL )
	{
		CFont *pfont = dc.GetCurrentFont();
		pfont->GetLogFont( &logfont );
		plogfont = &logfont;
	}

	m_bStrikeOut = plogfont->lfStrikeOut;
	m_bUnderline = plogfont->lfUnderline;
	m_sFont = plogfont->lfFaceName;
	m_sSize.Format( _T("%d"),
									MulDiv(plogfont->lfHeight < 0? -plogfont->lfHeight : plogfont->lfHeight,
												 m_cyPixelsPerInch,
												 72) );
	m_sStyle = theWorkspace.LoadResourceString(IDS_REGULAR);
	if( plogfont->lfWeight >= nDeBoldWeight && plogfont->lfItalic)
		m_sStyle = theWorkspace.LoadResourceString(IDS_BOLDITAL);
	else if( plogfont->lfItalic )
		m_sStyle = theWorkspace.LoadResourceString(IDS_ITALIC);
	else if ( plogfont->lfWeight >= nDeBoldWeight )
		m_sStyle = theWorkspace.LoadResourceString(IDS_BOLD);

	sOpenDCLProf = theWorkspace.LoadResourceString(IDR_MAINFRAME);
}

CFontPropertyPage::~CFontPropertyPage()
{
}

void CFontPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, grp2, m_Frame);
	DDX_Control(pDX, IDC_SAMPLE, m_staticSample);
	DDX_Control(pDX, IDC_STYLE, m_comboStyle);
	DDX_Control(pDX, IDC_FONTSIZE, m_comboSize);
	DDX_Control(pDX, IDC_FONT, m_comboFont);
	DDX_Control(pDX, IDC_SCALEDSTYLE, m_ScaledOpt);
	DDX_Control(pDX, IDC_PIXELSTYLE, m_PixelOpt);
	DDX_Check(pDX, IDC_STRIKEOUT, m_bStrikeOut);
	DDX_Check(pDX, IDC_UNDERLINE, m_bUnderline);
	DDX_CBString(pDX, IDC_FONT, m_sFont);
	DDX_CBString(pDX, IDC_FONTSIZE, m_sSize);
	DDV_MaxChars(pDX, m_sSize, LF_FACESIZE);
	DDX_CBString(pDX, IDC_STYLE, m_sStyle);
}


BEGIN_MESSAGE_MAP(CFontPropertyPage, CPropertyPage)
	ON_CBN_SELCHANGE(IDC_FONT, OnSelChange)
	ON_CBN_SELENDOK(IDC_STYLE, OnSelendokStyle)
	ON_CBN_SELCHANGE(IDC_STYLE, OnSelChange)
	ON_BN_CLICKED(IDC_STRIKEOUT, OnSelChange)
	ON_BN_CLICKED(IDC_UNDERLINE, OnSelChange)
	ON_CBN_KILLFOCUS(IDC_FONT, OnSelChange)
	ON_CBN_KILLFOCUS(IDC_STYLE, OnSelChange)
	ON_CBN_KILLFOCUS(IDC_FONTSIZE, OnSelChange)
	ON_BN_CLICKED(IDC_PIXELSTYLE, OnSelChange)
	ON_BN_CLICKED(IDC_SCALEDSTYLE, OnSelChange)
	ON_CBN_SELCHANGE(IDC_FONTSIZE, OnSelchangeFontsize)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontPropertyPage message handlers

BOOL CFontPropertyPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	TEditorProjectPtr pProject = activeProject;

	try
	{
		m_comboFont.Clear();

		CWindowDC dc(this);
		LOGFONT logfont;
		memset(&logfont, 0, sizeof(LOGFONT));
		logfont.lfCharSet = DEFAULT_CHARSET;
		logfont.lfFaceName[0] = '\0';
		logfont.lfPitchAndFamily = 0;
		EnumFontFamiliesEx(
			dc.m_hDC,
			&logfont,
			(FONTENUMPROC)FontEnumProc, 
			(LPARAM) this,
			0 );
		
		m_comboFont.AddString(theWorkspace.LoadResourceString(IDS_DEFAULTFONT));
		m_comboFont.AddString(theWorkspace.LoadResourceString(IDS_MSSERIF));
		m_comboFont.AddString(theWorkspace.LoadResourceString(IDS_SYSTEM));
		m_comboFont.AddString(theWorkspace.LoadResourceString(IDS_TERMINAL));
		m_comboFont.AddString(theWorkspace.LoadResourceString(IDS_SMFONTS));
		m_comboFont.AddString(theWorkspace.LoadResourceString(IDS_FIXEDSYS));
		m_comboFont.AddString(theWorkspace.LoadResourceString(IDS_COURIER));
	}
	catch(...)
	{
		CString sProblems;
		CString sOpenDCL;

		sProblems = theWorkspace.LoadResourceString(IDS_PROBLEMS);
		sOpenDCL = theWorkspace.LoadResourceString(IDS_OBJECTDCL);

		MessageBox (sProblems, sOpenDCL, MB_ICONEXCLAMATION);
	}


	// Fill Style combobox with "common" styles
	m_comboStyle.AddString( theWorkspace.LoadResourceString(IDS_REGULAR) );
	m_comboStyle.AddString( theWorkspace.LoadResourceString(IDS_BOLD) );
	m_comboStyle.AddString( theWorkspace.LoadResourceString(IDS_ITALIC) );
	m_comboStyle.AddString( theWorkspace.LoadResourceString(IDS_BOLDITAL) );

	if (m_pFontName != NULL)
		m_sFont = m_pFontName->GetStringValue();
	else
		m_sFont = pProject->m_sDefaultFontName;

	int nSel;

	nSel = m_comboFont.FindStringExact(0, m_sFont);
	if (nSel > -1)
		m_comboFont.SetCurSel(nSel);


	// set the font size
	long lSize = m_pFontSize? m_pFontSize->GetLongValue() : pProject->m_nDefaultFontSize;
	
	if (lSize < 0)
	{
		m_sSize.Format(_T("%d"), -lSize);
		m_PixelOpt.SetCheck(BST_CHECKED);
	}
	else
	{
		m_sSize.Format(_T("%d"), lSize);
		m_ScaledOpt.SetCheck(BST_CHECKED);
	}

	long lAbsSize = (lSize < 0)? -lSize : lSize;
	for( int i = 1; i < m_comboSize.GetCount(); ++i )
	{
		CString sSize;
		m_comboSize.GetLBText( i, sSize );
		if( _tstol( sSize ) > lAbsSize )
		{
			m_comboSize.SetCurSel( i - 1 );
			break;
		}
	}
	

	// set the underline style
	CWnd *pControl = GetDlgItem(IDC_UNDERLINE);

	if (m_pFontUnderline != NULL)
		((CButton*)pControl)->SetCheck(m_pFontUnderline->GetBooleanValue());
	else
		((CButton*)pControl)->SetCheck(pProject->m_bDefaultFontUnderLine);

	if (m_pFontBold != NULL)
	{
		if (m_pFontBold->GetBooleanValue() == FALSE && m_pFontItalic->GetBooleanValue() == FALSE)
		{
			m_sStyle = theWorkspace.LoadResourceString(IDS_REGULAR);
			m_comboStyle.SetCurSel(0);
		}
		else if(m_pFontBold->GetBooleanValue() == TRUE && m_pFontItalic->GetBooleanValue() == TRUE)
		{
			m_sStyle = theWorkspace.LoadResourceString(IDS_BOLDITAL);
			m_comboStyle.SetCurSel(3);
		}
		else if(m_pFontBold->GetBooleanValue() == FALSE && m_pFontItalic->GetBooleanValue() == TRUE)
		{
			m_sStyle = theWorkspace.LoadResourceString(IDS_ITALIC);
			m_comboStyle.SetCurSel(2);
		}
		else if(m_pFontBold->GetBooleanValue() == TRUE && m_pFontItalic->GetBooleanValue() == FALSE)
		{
			m_sStyle = theWorkspace.LoadResourceString(IDS_BOLD);
			m_comboStyle.SetCurSel(1);
		}
	}
	else
	{
		if (pProject->m_bDefaultFontBold == FALSE && pProject->m_bDefaultFontItalic == FALSE)
		{
			m_sStyle = theWorkspace.LoadResourceString(IDS_REGULAR);
			m_comboStyle.SetCurSel(0);
		}
		else if(pProject->m_bDefaultFontBold == TRUE && pProject->m_bDefaultFontItalic == TRUE)
		{
			m_sStyle = theWorkspace.LoadResourceString(IDS_BOLDITAL);		
			m_comboStyle.SetCurSel(3);
		}
		else if(pProject->m_bDefaultFontBold == FALSE && pProject->m_bDefaultFontItalic == TRUE)
		{
			m_sStyle = theWorkspace.LoadResourceString(IDS_ITALIC);		
			m_comboStyle.SetCurSel(2);
		}
		else if(pProject->m_bDefaultFontBold == TRUE && pProject->m_bDefaultFontItalic == FALSE)
		{
			m_sStyle = theWorkspace.LoadResourceString(IDS_BOLD);
			m_comboStyle.SetCurSel(1);
		}
	}
	
	OnSelChange();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

int CALLBACK CFontPropertyPage::FontEnumProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, 
		int FontType, CFontPropertyPage* pFontPage )
{
	CString sFontName = (LPCTSTR)lpelfe->elfFullName;
	if (sFontName == _T("") ||
		sFontName.Left(1) == theWorkspace.LoadResourceString(IDS_WEIRDDOTDOT) ||
		sFontName.GetLength() <= 2)
		return 1;

	if( pFontPage->m_comboFont.FindStringExact( 0, (LPCTSTR)lpelfe->elfFullName ) == CB_ERR )
	{
		// Add to list
		pFontPage->m_comboFont.AddString( (LPCTSTR)lpelfe->elfFullName );
	}

 	return 1;
}

void CFontPropertyPage::OnSelChange() 
{
	// The selection hasn't changed yet, so change it
	if( IsChild( GetFocus() ) && 
			GetFocus()->GetParent()->IsKindOf( RUNTIME_CLASS( CComboBox ) ) )
	{
		CComboBox *cb = (CComboBox *)GetFocus()->GetParent();
		CString sText;
		if( cb->GetCurSel() != CB_ERR )
		{
			cb->GetLBText( cb->GetCurSel(), sText );
			cb->SetWindowText( sText );
		}
	}

	UpdateData(TRUE);

	LOGFONT logfont;
	memset(&logfont, 0, sizeof(LOGFONT));
	logfont.lfStrikeOut = m_bStrikeOut;
	logfont.lfUnderline = m_bUnderline;
	lstrcpyn( logfont.lfFaceName, m_sFont, LF_FACESIZE );
	int nSel = m_comboSize.GetCurSel();
	int n = m_comboSize.GetLBTextLen(nSel);      
	m_comboSize.GetLBText(nSel, m_sSize);

	if (m_ScaledOpt.GetCheck() == BST_CHECKED)
	{
		CDC* cdc = GetDC();	
		logfont.lfHeight = -::MulDiv(_tstol(m_sSize),	GetDeviceCaps(cdc->m_hDC, LOGPIXELSY), 72);
		ReleaseDC(cdc);
	}
	else
		logfont.lfHeight = -_tstol(m_sSize);		
	logfont.lfWeight = 400;	
	logfont.lfItalic = FALSE;
	if( m_sStyle.Find( theWorkspace.LoadResourceString(IDS_ITALIC) ) != -1 )
		logfont.lfItalic = TRUE;
	if( m_sStyle.Find( theWorkspace.LoadResourceString(IDS_BOLD) ) != -1 )
		logfont.lfWeight = nDeBoldWeight;
	m_fontSample.DeleteObject();

	if (_tcsicmp(logfont.lfFaceName, _T("kanji")) == 0)
		logfont.lfCharSet = SHIFTJIS_CHARSET;
	else
		logfont.lfCharSet = ANSI_CHARSET;

	try
	{
		HFONT hNewFont = ::CreateFontIndirect(&logfont);
		if (hNewFont != NULL)
		{
			m_fontSample.Attach(hNewFont);
		}
	}
	catch(...)
	{
	}
	
	m_staticSample.SetFont(&m_fontSample);
	m_staticSample.RedrawWindow();
	
	SetModified(TRUE);
}

void CFontPropertyPage::OnDestroy() 
{
	m_fontSample.DeleteObject();
	m_comboStyle.Clear();
	m_comboFont.Clear();
	m_comboSize.Clear();

	CPropertyPage::OnDestroy();
}


BOOL CFontPropertyPage::OnApply() 
{
	CWinApp* pApp = AfxGetApp();
	TEditorProjectPtr pProject = activeProject;
	
	// set the font name
	CString sFontName;	
	int nSel = m_comboFont.GetCurSel();
	if (nSel > -1)		
		m_comboFont.GetLBText(nSel, sFontName);
	else
		m_comboFont.GetWindowText(sFontName);

	// if the project list pointer is NULL then update the property
	if (m_pFontName != NULL) 
	{
		m_pFontName->SetStringValue(sFontName);
		theWorkspace.SetModified(true);
	}
	else // update the default setting
	{
		pProject->m_sDefaultFontName = sFontName;
		pApp->WriteProfileString(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontName), sFontName);		
	}

	
	// get the size
	CString sFontSize;	
	nSel = m_comboSize.GetCurSel();
	if (nSel > -1)		
		m_comboSize.GetLBText(nSel, sFontSize);
	else
		m_comboSize.GetWindowText(sFontSize);

	if (!sFontSize.IsEmpty())		
	{
		long lSize = _tstol(sFontSize);
		if (m_PixelOpt.GetCheck() == BST_CHECKED)
			lSize = -lSize;
		if (m_pFontSize != NULL) 
			m_pFontSize->SetLongValue(lSize);
		else // update the default setting
		{
			pProject->m_nDefaultFontSize = lSize;
			pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontSize), lSize);
			if( pApp->GetProfileInt( sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontSizeStyle), 0 ) != 0 )
				pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontSizeStyle), 0); //no longer used, must be set to zero
		}		
	}


	// set the underline style
	CWnd *pControl = GetDlgItem(IDC_UNDERLINE);
	bool bNewValue = (((CButton*)pControl)->GetCheck() == BST_CHECKED);

	// if the project list pointer is NULL then update the property
	if (m_pFontUnderline != NULL) 
		m_pFontUnderline->SetBooleanValue(bNewValue);
	else
	{
		// set the font Underline
		pProject->m_bDefaultFontUnderLine = bNewValue;
		pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontUnderLine), bNewValue? 1 : 0);	
	}

	// set the bold and italic styles
	int nStyle = m_comboStyle.GetCurSel();

	// if the project list pointer is NULL then update the property
	if (m_pFontBold != NULL) 		
	{
		if (nStyle == 0)
		{
			m_pFontBold->SetBooleanValue(false);
			m_pFontItalic->SetBooleanValue(false);
		}
		else if(nStyle == 3)
		{
			m_pFontBold->SetBooleanValue(true);
			m_pFontItalic->SetBooleanValue(true);
		}
		else if(nStyle == 2)
		{
			m_pFontBold->SetBooleanValue(false);
			m_pFontItalic->SetBooleanValue(true);
		}
		else if(nStyle == 1)
		{
			m_pFontBold->SetBooleanValue(true);
			m_pFontItalic->SetBooleanValue(false);
		}
	}
	else // update the default setting
	{
		if (nStyle == 0)
		{
			pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontBold), FALSE);
			pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontItalic), FALSE);
			pProject->m_bDefaultFontBold = FALSE;
			pProject->m_bDefaultFontItalic = FALSE;
		}
		else if(nStyle == 3)
		{
			pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontBold), TRUE);
			pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontItalic), TRUE);
			pProject->m_bDefaultFontBold = TRUE;
			pProject->m_bDefaultFontItalic = TRUE;
		}
		else if(nStyle == 2)
		{
			pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontBold), FALSE);
			pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontItalic), TRUE);
			pProject->m_bDefaultFontBold = FALSE;
			pProject->m_bDefaultFontItalic = TRUE;
		}
		else if(nStyle == 1)
		{
			pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontBold), TRUE);
			pApp->WriteProfileInt(sOpenDCLProf, theWorkspace.LoadResourceString(IDS_DefaultFontItalic), FALSE);
			pProject->m_bDefaultFontBold = TRUE;
			pProject->m_bDefaultFontItalic = FALSE;
		}
	}
	if( mpDclControl && mpDclControl->m_pCtrlHolder )
		((CControlHolder*)mpDclControl->m_pCtrlHolder)->UpdateProperty(Prop::FontName);

	return CPropertyPage::OnApply();
}



void CFontPropertyPage::OnSelendokStyle() 
{
	OnSelChange();
	
}

void CFontPropertyPage::OnSelchangeFontsize() 
{
	OnSelChange();	
}
