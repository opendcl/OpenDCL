// Colors.cpp : implementation file
//

#include "stdafx.h"
#include "ColorsPage.h"
#include "StudioWorkspace.h"
#include "StudioDialogControl.h"
#include "PropertyObject.h"
#include "AcadColorTable.h"
#include "IntegerFilter.h"
#include "DclFormView.h"
#include "TrueColorDialog.h"


static UINT GetDialogResourceId( Prop::Id idProp )
{
	switch( idProp )
	{
	case Prop::BackgroundColor: return IDD_BACKCOLORS;
	case Prop::AlternatingColor: return IDD_ALTCOLORS;
	case Prop::ForegroundColor: return IDD_FORECOLORS;
	}
	return (UINT)-1;
}

static UINT GetTitleResource( Prop::Id idProp )
{
	switch( idProp )
	{
	case Prop::BackgroundColor: return IDS_BGCOLOR;
	case Prop::AlternatingColor: return IDS_ALTCOLOR;
	case Prop::ForegroundColor: return IDS_FGCOLOR;
	}
	return (UINT)-1;
}

/////////////////////////////////////////////////////////////////////////////
// CColorsPage property page


CColorsPage::CColorsPage(Prop::Id idProp, TDclControlPtr pControl)
: CPropertyPage( GetDialogResourceId( idProp ) )
, midProp( idProp )
, mpControl( pControl )
, msTitle( theWorkspace.LoadResourceString( GetTitleResource( idProp ) ) )
, mpColor( pControl->GetPropertyObject( idProp ) )
, m_Edit( new CIntegerFilter )
{
}

CColorsPage::~CColorsPage()
{
}

void CColorsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COLORCOMBO, m_ColorCB);
	DDX_Control(pDX, IDC_EDIT, m_Edit);
	DDX_Control(pDX, IDC_SYSTEMCOLORLIST, m_SystemColors);
	DDX_Control(pDX, IDC_COLORPATCH, m_Color);
}


BEGIN_MESSAGE_MAP(CColorsPage, CPropertyPage)
	ON_LBN_SELCHANGE(IDC_SYSTEMCOLORLIST, OnSelchangeSystemcolorlist)	
	ON_CBN_SELCHANGE(IDC_COLORCOMBO, OnSelchangeColorcombo)
	ON_EN_CHANGE(IDC_EDIT, OnChangeEdit)
	ON_BN_CLICKED(IDC_TRUEBTN, OnTruebtn)
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorsPage message handlers

BOOL CColorsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	SetWindowText(msTitle);
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_SCROLLBARCOLOR ), GetSysColor( COLOR_SCROLLBAR ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_DESKTOP ), GetSysColor( COLOR_DESKTOP ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_ACTCAPTION ), GetSysColor( COLOR_ACTIVECAPTION ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_INACTCAPT ), GetSysColor( COLOR_INACTIVECAPTION ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_MENU ), GetSysColor( COLOR_MENU ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_WINDOW ), GetSysColor( COLOR_WINDOW ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_WINFRAME ), GetSysColor( COLOR_WINDOWFRAME ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_MENUTEXT ), GetSysColor( COLOR_MENUTEXT ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_WINTEXT ), GetSysColor( COLOR_WINDOWTEXT ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_CAPTEXT ), GetSysColor( COLOR_CAPTIONTEXT ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_ACTBORDER ), GetSysColor( COLOR_ACTIVEBORDER ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_INACTBORDER ), GetSysColor( COLOR_INACTIVEBORDER ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_APPWORK ), GetSysColor( COLOR_APPWORKSPACE ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_HIGHLIGHT ), GetSysColor( COLOR_HIGHLIGHT ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_HIGHLIGHTTEXT ), GetSysColor( COLOR_HIGHLIGHTTEXT ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_BTNFACE ), GetSysColor( COLOR_BTNFACE ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_BTNSHDOW ), GetSysColor( COLOR_BTNSHADOW ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_GREYTEXT ), GetSysColor( COLOR_GRAYTEXT ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_BTNTEXT ), GetSysColor( COLOR_BTNTEXT ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_INACTCAPTEXT ), GetSysColor( COLOR_INACTIVECAPTIONTEXT ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_BTNHLIGHT ), GetSysColor( COLOR_BTNHIGHLIGHT ) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_MSPACECOLOR ), RGB(0,0,0) );
	m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_PSPACECOLOR ), RGB(255,255,255) );
	if( midProp == Prop::BackgroundColor )
		m_SystemColors.AddColor( theWorkspace.LoadResourceString( IDS_TRANSPARENT ), RGB(255,255,255) );

	m_ColorCB.ResetContent();
	m_ColorCB.AddColor( theWorkspace.LoadResourceString( IDS_COLOR_BLACK ), RGB(0,0,0) );
	m_ColorCB.AddColor( theWorkspace.LoadResourceString( IDS_COLOR_RED ), RGB(255,0,0) );
	m_ColorCB.AddColor( theWorkspace.LoadResourceString( IDS_COLOR_YELLOW ), RGB(255,255,0) );
	m_ColorCB.AddColor( theWorkspace.LoadResourceString( IDS_COLOR_GREEN ), RGB(0,255,0) );
	m_ColorCB.AddColor( theWorkspace.LoadResourceString( IDS_COLOR_CYAN ), RGB(0,255,255) );
	m_ColorCB.AddColor( theWorkspace.LoadResourceString( IDS_COLOR_BLUE ), RGB(0,0,255) );
	m_ColorCB.AddColor( theWorkspace.LoadResourceString( IDS_COLOR_MAGENTA ), RGB(255,0,255) );
	m_ColorCB.AddColor( theWorkspace.LoadResourceString( IDS_COLOR_WHITE ), RGB(255,255,255) );

	m_Edit.SetWindowText( mpColor->GetStringValue() );
	long lColor = mpColor->GetLongValue();
	if( lColor < 0 )
		m_SystemColors.SetCurSel( (-lColor) - 1 );
	else
		m_ColorCB.SetCurSel( lColor );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

BOOL CColorsPage::OnApply() 
{
	CString sValue;
	m_Edit.GetWindowText( sValue );
	long nColor = _tstol( sValue );
	mpColor->SetLongValue( nColor );
	CStudioDialogControl::UpdateProperty( mpControl, mpColor->GetID() );
	if( mpControl == theStudioWorkspace.GetActiveDclControl() )
		theStudioWorkspace.ActivateDclControl( mpControl );
	return CPropertyPage::OnApply();
}

void CColorsPage::OnSelchangeSystemcolorlist() 
{
	int nCurSel = m_SystemColors.GetCurSel();
	long nColor = (-nCurSel) - 1;
	CString sVal;
	sVal.Format( _T("%d"), nColor );
	m_Edit.SetWindowText( sVal );
	SetModified( TRUE );

	m_ColorCB.SetCurSel( -1 );
	DisplayColor();
}

void CColorsPage::OnSelchangeColorcombo() 
{
	CString sVal;
	sVal.Format( _T("%d"), m_ColorCB.GetCurSel() );
	m_Edit.SetWindowText( sVal );
	SetModified( TRUE );

	m_SystemColors.SetCurSel( -1 );
	DisplayColor();
}

void CColorsPage::OnChangeEdit() 
{
	CString sValue;
	m_Edit.GetWindowText( sValue );
	long nColor = _tstol( sValue );
	if( nColor < 0 )
	{
		m_SystemColors.SetCurSel( (-nColor) - 1 );
		m_ColorCB.SetCurSel( -1 );
	}
	else if( nColor >= 0 && nColor < 8 )
	{
		m_ColorCB.SetCurSel( nColor );
		m_SystemColors.SetCurSel( -1 );
	}
	else
	{
		m_ColorCB.SetCurSel( -1 );
		m_SystemColors.SetCurSel( -1 );
	}
	
	DisplayColor();
	SetModified( TRUE );
}

void CColorsPage::OnTruebtn() 
{
	CTrueColorDialog colorDlg;
	if( colorDlg.DoModal() == IDOK ) // The user selected the "OK" button
	{
		COLORREF crColor = colorDlg.GetColor(); 
		crColor |= 0x01000000;
		CString sColor;
		sColor.Format( _T("%d"), crColor );
		m_Edit.SetWindowText( sColor );
	}
	DisplayColor();
}

void CColorsPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
}

void CColorsPage::DisplayColor() 
{
	CString sValue;
	m_Edit.GetWindowText(sValue);
	m_Color.m_color = GetRGBColor( _tstol( sValue ) );
	m_Color.Invalidate();
}
