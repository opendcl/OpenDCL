// PropertiesTabPane.cpp : implementation file
//

#include "stdafx.h"
#include "PropertiesTabPane.h"
#include "SharedRes.h"


/////////////////////////////////////////////////////////////////////////////
// CPropertiesTabPane dialog


CPropertiesTabPane::CPropertiesTabPane(CWnd* pParent /*=NULL*/)
	: CDialog(CPropertiesTabPane::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPropertiesTabPane)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInitialized = false;
}


void CPropertiesTabPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPropertiesTabPane)
	DDX_Control(pDX, IDC_PROPDESCEDIT, m_PropertyDesc);
	DDX_Control(pDX, IDC_PROPERTYTITLE, m_PropertyTitle);
	DDX_Control(pDX, IDC_CTRL_DESC, m_ControlDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPropertiesTabPane, CDialog)
	//{{AFX_MSG_MAP(CPropertiesTabPane)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertiesTabPane message handlers

void CPropertiesTabPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (!m_bInitialized) 
		return;

	CRect rcTitle (2,0,cx,nTitleHeight);
	m_ControlDesc.MoveWindow(rcTitle, TRUE);
		
	CRect rcList (0,nTitleHeight,cx,cy-nPropertyListHeight);
	m_PropertyList.MoveWindow(rcList, TRUE);

	CRect rcPropertyTitle (0,cy-nPropertyListHeight - 1,cx,cy);
	m_PropertyTitle.MoveWindow(rcPropertyTitle, TRUE);	

	CRect rcPropertyDesc (2,cy-nPropertyDescWidth,cx-2,cy-2);
	m_PropertyDesc.MoveWindow(rcPropertyDesc, TRUE);

}

int CPropertiesTabPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == nNotSet)
		return nNotSet;
	
	// TODO: Add your specialized creation code here
	
	return 0;
}

BOOL CPropertiesTabPane::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_bInitialized = true;
	
	CRect rc(0,0,2,2);	
	
	m_PropertyList.Create(rc, this, nPropertyListID);
	
	m_ControlDesc.SetWindowText(CString());
	m_PropertyTitle.SetWindowText(CString());

	m_PropertyList.m_pPropTitle = &m_PropertyTitle;
	m_PropertyList.m_pPropDesc = &m_PropertyDesc;

	m_PropertyDesc.SetFont(m_ControlDesc.GetFont());


	// set the font	
	LOGFONT	lf;
	memset(&lf, 0, sizeof(LOGFONT));
	CString sFont;
	sFont = theWorkspace.LoadResourceString(IDS_DEFAULTFONT);
	lstrcpy(lf.lfFaceName, sFont);
	CDC *pDC = m_PropertyList.GetDC();

	// create font size as scaled
	lf.lfHeight = -::MulDiv(nDeFontSize, pDC->GetDeviceCaps(LOGPIXELSY), nDePixels);

	lf.lfQuality = PROOF_QUALITY;

	lf.lfWeight = FW_BOLD;

	HFONT hNewFont = ::CreateFontIndirect(&lf);
	if (hNewFont != NULL)
	{
		m_font.Attach(hNewFont);
		m_PropertyTitle.SetFont(&m_font);	
	}
	
	//m_font.CreateFontIndirect(&lf);
		
	
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
	if (pMsg->message== WM_KEYDOWN )
	{
		if (pMsg->wParam==VK_RETURN && !m_PropertyList.m_Edit.IsWindowVisible())
		{
			pMsg->wParam = NULL;
			pMsg->message = NULL;
		}
	}		
		
	return CDialog::PreTranslateMessage(pMsg);
}
