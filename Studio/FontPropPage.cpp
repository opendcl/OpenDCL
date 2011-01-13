// FontPropPage.cpp : implementation file
//

#include "stdafx.h"
#include "FontPropPage.h"
#include "PropertyObject.h"
#include "StudioWorkspace.h"
#include "OpenDCL.h"
#include "DclControlObject.h"
#include "StudioDialogControl.h"
#include "Resource.h"

#define nDeBoldWeight 700


/////////////////////////////////////////////////////////////////////////////
// CFontPropertyPage property page

IMPLEMENT_DYNCREATE(CFontPropertyPage, CPropertyPage)

CFontPropertyPage::CFontPropertyPage(TDclControlPtr pDclControl /*= NULL*/)
: CPropertyPage(CFontPropertyPage::IDD)
, mpDclControl( pDclControl )
{
	LOGFONT logfont;
	CWindowDC dc(GetDesktopWindow());
	CFont *pfont = dc.GetCurrentFont();
	pfont->GetLogFont( &logfont );

	m_bStrikeOut = logfont.lfStrikeOut;
	m_bUnderline = logfont.lfUnderline;
	m_sFont = logfont.lfFaceName;
	m_sSize.Format( _T("%d"),
									MulDiv(logfont.lfHeight < 0? -logfont.lfHeight : logfont.lfHeight,
												 GetDeviceCaps(dc, LOGPIXELSY),
												 72) );
	m_sStyle = theWorkspace.LoadResourceString(IDS_REGULAR);
	if( logfont.lfWeight >= nDeBoldWeight && logfont.lfItalic)
		m_sStyle = theWorkspace.LoadResourceString(IDS_BOLDITAL);
	else if( logfont.lfItalic )
		m_sStyle = theWorkspace.LoadResourceString(IDS_ITALIC);
	else if ( logfont.lfWeight >= nDeBoldWeight )
		m_sStyle = theWorkspace.LoadResourceString(IDS_BOLD);
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
		
		m_comboFont.AddString(theWorkspace.GetDefaultFontName());
		m_comboFont.AddString(_T("MS Serif"));
		m_comboFont.AddString(_T("System"));
		m_comboFont.AddString(_T("Terminal"));
		m_comboFont.AddString(_T("Small Fonts"));
		m_comboFont.AddString(_T("Fixedsys"));
		m_comboFont.AddString(_T("Courier"));
	}
	catch(...)
	{
		MessageBox(theWorkspace.LoadResourceString(IDS_FONTLISTLOADFAILURE), theWorkspace.GetAppKey(), MB_ICONEXCLAMATION);
	}


	// Fill Style combobox with "common" styles
	m_comboStyle.AddString( theWorkspace.LoadResourceString(IDS_REGULAR) );
	m_comboStyle.AddString( theWorkspace.LoadResourceString(IDS_BOLD) );
	m_comboStyle.AddString( theWorkspace.LoadResourceString(IDS_ITALIC) );
	m_comboStyle.AddString( theWorkspace.LoadResourceString(IDS_BOLDITAL) );

	FontSettings FS( mpDclControl );
	if( !FS )
		FS = theApp.GetDefaultFontSettings();
	if( FS )
		m_sFont = FS;

	int nSel = m_comboFont.FindStringExact(0, m_sFont);
	if (nSel > -1)
		m_comboFont.SetCurSel(nSel);


	m_sSize = FS.absoluteSize();
	
	if (FS.isScaled())
		m_ScaledOpt.SetCheck(BST_CHECKED);
	else
		m_PixelOpt.SetCheck(BST_CHECKED);

	long lSize = FS.size();
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

	((CButton*)pControl)->SetCheck(FS.isUnderlined()? BST_CHECKED : BST_UNCHECKED);

	if(FS.isBold() && FS.isItalic())
	{
		m_sStyle = theWorkspace.LoadResourceString(IDS_BOLDITAL);
		m_comboStyle.SetCurSel(3);
	}
	else if(!FS.isBold() && FS.isItalic())
	{
		m_sStyle = theWorkspace.LoadResourceString(IDS_ITALIC);
		m_comboStyle.SetCurSel(2);
	}
	else if(FS.isBold() && !FS.isItalic())
	{
		m_sStyle = theWorkspace.LoadResourceString(IDS_BOLD);
		m_comboStyle.SetCurSel(1);
	}
	else
	{
		m_sStyle = theWorkspace.LoadResourceString(IDS_REGULAR);
		m_comboStyle.SetCurSel(0);
	}
	
	OnSelChange();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

int CALLBACK CFontPropertyPage::FontEnumProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, 
		int FontType, CFontPropertyPage* pFontPage )
{
	CString sFontName = (LPCTSTR)lpelfe->elfFullName;
	if (sFontName.IsEmpty() || sFontName.Left(1) == _T("¨") || sFontName.GetLength() <= 2)
		return 1;

	if( pFontPage->m_comboFont.FindStringExact( 0, (LPCTSTR)lpelfe->elfFullName ) == CB_ERR )
		pFontPage->m_comboFont.AddString( lpelfe->elfFullName );

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
	if( nSel >= 0 )
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
			m_fontSample.Attach(hNewFont);
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
	FontSettings FS;

	CString sFontName;	
	int nSel = m_comboFont.GetCurSel();
	if (nSel > -1)		
		m_comboFont.GetLBText(nSel, sFontName);
	else
		m_comboFont.GetWindowText(sFontName);
	FS.setName( sFontName );

	// get the size
	CString sFontSize;	
	nSel = m_comboSize.GetCurSel();
	if (nSel > -1)		
		m_comboSize.GetLBText(nSel, sFontSize);
	else
		m_comboSize.GetWindowText(sFontSize);
	long lSize = _tstol( sFontSize );
	if( (m_PixelOpt.GetCheck() == BST_CHECKED) )
		lSize = -lSize;
	FS.setSize( lSize );

	FS.setUnderlined( (((CButton*)GetDlgItem(IDC_UNDERLINE))->GetCheck() == BST_CHECKED) );

	// set the bold and italic styles
	int nStyle = m_comboStyle.GetCurSel();
	switch( nStyle )
	{
	case 3:
		FS.setBold( true );
		FS.setItalic( true );
		break;
	case 2:
		FS.setBold( false );
		FS.setItalic( true );
		break;
	case 1:
		FS.setBold( true );
		FS.setItalic( false );
		break;
	default:
		FS.setBold( false );
		FS.setItalic( false );
		break;
	}

	if( mpDclControl )
	{
		mpDclControl->SetFontProperties( FS );
		CStudioDialogControl::UpdateProperty(mpDclControl,Prop::FontName);
	}
	else
		theApp.SetDefaultFontSettings( FS );

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
