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
, mbInitialized( false )
{
}

void CPropertiesTabPane::ActivateProperty( TPropertyPtr pProp )
{
	CString sName;
	CString sDesc;
	if( pProp )
	{
		sName = pProp->GetName();
		//Handle pseudo-properties manually
		switch( pProp->GetID() )
		{
		case Prop::ControlBrowser:
			sDesc = theWorkspace.LoadResourceString( IDS_PROPD_CONTROLBROWSER );
			break;
		case Prop::Custom:
			sDesc = theWorkspace.LoadResourceString( IDS_PROPD_CUSTOMWIZARD );
			break;
		case Prop::ActiveXPropPages:
			sDesc = theWorkspace.LoadResourceString( IDS_PROPD_AXPROPPAGES );
			break;
		default:
			CString sUnused;
			ParseHelpInfo( pProp, sUnused, sDesc );
			break;
		}
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
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesTabPane message handlers

void CPropertiesTabPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (!mbInitialized) 
		return;

	CRect rcPropertyTitle (0,cy-nPropertyListHeight,cx,cy - nPropertyDescWidth);
	GetDlgItem( IDC_PROPERTYTITLE )->MoveWindow(rcPropertyTitle, FALSE);	

	CRect rcPropertyDesc (0,cy-nPropertyDescWidth,cx,cy);
	GetDlgItem( IDC_PROPDESCEDIT )->MoveWindow(rcPropertyDesc, FALSE);
		
	CRect rcList (0,0,cx,cy-nPropertyListHeight);
	mPropGridCtrl.MoveWindow(rcList, FALSE);
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
	mFont.CreateFontIndirect(&lf);

	mbInitialized = true;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

BOOL CPropertiesTabPane::PreTranslateMessage(MSG* pMsg) 
{
	return CWnd::PreTranslateMessage(pMsg); //bypass CDialog
}
