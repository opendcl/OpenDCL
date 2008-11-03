// PropertiesTabPane.cpp : implementation file
//

#include "stdafx.h"
#include "PropertiesTabPane.h"
#include "ParseFuncHelp.h"
#include "Workspace.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertiesTabPane dialog


CPropertiesTabPane::CPropertiesTabPane( const std::vector< TDclControlPtr >& ActiveControls, CWnd* pParent /*=NULL*/ )
: CDialog(CPropertiesTabPane::IDD, pParent)
, mPropGridCtrl( this, ActiveControls )
{
	//{{AFX_DATA_INIT(CPropertiesTabPane)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInitialized = false;
}

void CPropertiesTabPane::ActivateProperty( TPropertyPtr pProp )
{
	CString sName;
	CString sDesc;
	if( pProp )
	{
		sName = pProp->GetName();
		CString sUnused;
		ParseHelpInfo( pProp, sUnused, sDesc );
		if( sDesc.IsEmpty() )
			sDesc = theWorkspace.LoadResourceString( IDS_PROPD_NAME + pProp->GetID() - 1 );
	}
	GetDlgItem( IDC_PROPERTYTITLE )->SetWindowText( sName );
	GetDlgItem( IDC_PROPDESCEDIT )->SetWindowText( sDesc );
}

void CPropertiesTabPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertiesTabPane, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesTabPane message handlers

void CPropertiesTabPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (!m_bInitialized) 
		return;

	CRect rcPropertyTitle (0,cy-nPropertyListHeight,cx,cy - nPropertyDescWidth);
	GetDlgItem( IDC_PROPERTYTITLE )->MoveWindow(rcPropertyTitle, FALSE);	

	CRect rcPropertyDesc (0,cy-nPropertyDescWidth,cx,cy);
	GetDlgItem( IDC_PROPDESCEDIT )->MoveWindow(rcPropertyDesc, FALSE);
		
	CRect rcList (0,0,cx,cy-nPropertyListHeight);
	mPropGridCtrl.MoveWindow(rcList, FALSE);
}

int CPropertiesTabPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

BOOL CPropertiesTabPane::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CRect rc(0,0,2,2);	

	mPropGridCtrl.Create(this, rc, nPropertyListID);

	// set the font	
	LOGFONT	lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lstrcpyn(lf.lfFaceName, theWorkspace.GetDefaultFontName(), _elements(lf.lfFaceName));
	CDC *pDC = mPropGridCtrl.GetDC();
	lf.lfHeight = -::MulDiv(nDeFontSize, pDC->GetDeviceCaps(LOGPIXELSY), nDePixels); // create font size as scaled
	lf.lfQuality = PROOF_QUALITY;
	lf.lfWeight = FW_BOLD;
	m_font.CreateFontIndirect(&lf);

	m_bInitialized = true;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CPropertiesTabPane::OnDestroy() 
{
	CDialog::OnDestroy();
	
	m_font.DeleteObject();	
}

BOOL CPropertiesTabPane::PreTranslateMessage(MSG* pMsg) 
{
	return CWnd::PreTranslateMessage(pMsg); //bypass CDialog
}
