// ToolTipsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ToolTipsPage.h"
#include "ToolTips.h"
#include "AcadColorTable.h"
#include "Project.h"
#include "PictureObject.h"
#include "PropertyObject.h"
#include "SharedRes.h"


static CString LTOA(int nVal)
{
  CString sLong;
	sLong.Format(_T("%d"), nVal);
  return sLong;
}


const TCHAR s1[] = _T("\\");
const TCHAR s2[] = _T("/");
const TCHAR s3[] = _T(".avi");
const TCHAR s4[] = _T("><al_l>");
const TCHAR s5[] = _T("</b><br>");
const TCHAR s6[] = _T("<b><ct=0x");
const TCHAR s7[] = _T("</ct><br>");
const TCHAR s8[] = _T("<ct=0x000000><hr=100%>");
const TCHAR s9[] = _T("\\colortbl");
const TCHAR s10[] = _T("\\red");
const TCHAR s11[] = _T("\\green");
const TCHAR s12[] = _T("\\blue");
const TCHAR s13[] = _T(";");
const TCHAR s14[] = _T("\\viewkind");
const TCHAR s15[] = _T("\\lang");
const TCHAR s16[] = _T("\\f0");
const TCHAR s17[] = _T("\\fs");
const TCHAR s18[] = _T(" ");
const TCHAR s19[] = _T(">");
const TCHAR s20[] = _T(" \\b0");
const TCHAR s21[] = _T("</b>");
const TCHAR s22[] = _T(" \\b");
const TCHAR s23[] = _T("<b>");
const TCHAR s24[] = _T(" \\i0");
const TCHAR s25[] = _T("</i>");
const TCHAR s26[] = _T(" \\i");
const TCHAR s27[] = _T("<i>");
const TCHAR s28[] = _T(" \\ulnone");
const TCHAR s29[] = _T("</u>");
const TCHAR s30[] = _T(" \\ul0");
const TCHAR s31[] = _T(" \\cf");
const TCHAR s32[] = _T(" \\ul");
const TCHAR s33[] = _T("<u>");
const TCHAR s34[] = _T(" \\tab"); 
const TCHAR s35[] = _T("<t=2>");
const TCHAR s36[] = _T("\\b0");
const TCHAR s37[] = _T("<ct=0x");
const TCHAR s38[] = _T("\\b");
const TCHAR s39[] = _T("\\cf");
const TCHAR s40[] = _T("\\i0");
const TCHAR s41[] = _T("\\par }");
const TCHAR s42[] = _T("\\i");
const TCHAR s43[] = _T("\\par ");
const TCHAR s44[] = _T("\\ulnone");
const TCHAR s45[] = _T("\\par \\pard");
const TCHAR s46[] = _T("\\ul0");

const TCHAR s48[] = _T("\\ul");

const TCHAR s50[] = _T("\\tab"); 

const TCHAR s52[] = _T("\\cf0"); 
const TCHAR s53[] = _T("<ct=0x000000>");
const TCHAR s54[] = _T("\\pard");
const TCHAR s55[] = _T("\\ulnone ");
const TCHAR s56[] = _T("\\ul0 ");
const TCHAR s57[] = _T("\\ul ");
const TCHAR s58[] = _T("\\cf0 ");
const TCHAR s59[] = _T("\\par }\r\n");
const TCHAR s60[] = _T("\\par }");
const TCHAR s61[] = _T("\\par  ");
/////////////////////////////////////////////////////////////////////////////
// CToolTipsPage property page

IMPLEMENT_DYNCREATE(CToolTipsPage, CPropertyPage)

CToolTipsPage::CToolTipsPage() : CPropertyPage(CToolTipsPage::IDD)
{
}

CToolTipsPage::~CToolTipsPage()
{
}

void CToolTipsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToolTipsPage)
	DDX_Control(pDX, IDC_COLOR2, m_Color2);
	DDX_Control(pDX, IDC_UNDERLINE, m_UnderLine);
	DDX_Control(pDX, IDC_BOLD, m_Bold);
	DDX_Control(pDX, IDC_MAINTEXT, m_MainText);
	DDX_Control(pDX, IDC_ITALIC, m_Italic);
	DDX_Control(pDX, IDC_PICLIST, m_Pictures);
	DDX_Control(pDX, IDC_AVI, m_Avi);
	DDX_Control(pDX, IDC_TITLE, m_Title);
	DDX_Control(pDX, IDC_LINE, m_Line);
	DDX_Control(pDX, IDC_COLOR, m_Color);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CToolTipsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CToolTipsPage)
	ON_BN_CLICKED(IDC_PREVIEW, OnPreview)
	ON_BN_CLICKED(IDC_BOLD, OnBold)
	ON_BN_CLICKED(IDC_ITALIC, OnItalic)
	ON_BN_CLICKED(IDC_UNDERLINE, OnUnderline)
	ON_BN_CLICKED(IDC_COLOR2, OnColor2)
	ON_EN_CHANGE(IDC_AVI, OnChangeAvi)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	ON_BN_CLICKED(IDC_LINE, OnLine)
	ON_EN_CHANGE(IDC_MAINTEXT, OnChangeMaintext)
	ON_LBN_SELCHANGE(IDC_PICLIST, OnSelchangePiclist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolTipsPage message handlers

BOOL CToolTipsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Create the CPPToolTip object
	m_tooltip.Create(this);
	
	m_tooltip.AddTool(GetDlgItem(IDC_AVILABEL), _T("<b><ct=0x0000FF><al_c>AVI File Name</b><br><ct=0x00AA00><hr=100%></ct><br>The AVI file must be located in the same directory as the .odc or .ods file."));
	
	m_UnderLine.SetFlat();
	m_Bold.SetFlat();
	m_Italic.SetFlat();
	m_Color2.SetFlat();

	m_UnderLine.SetIcon(IDI_UNDERLINE);
	m_Bold.SetIcon(IDI_BOLD);
	m_Italic.SetIcon(IDI_ITALIC);
	m_Color2.SetIcon(IDI_TEXTCOLOR);

	//m_MainText.SetBackgroundColor(FALSE, ::GetSysColor(COLOR_INFOBK));

	CString sNone;
	sNone = theWorkspace.LoadResourceString(IDS_NONE);
	m_Pictures.AddString(sNone);
	//CString sAdd;
	//sAdd = theWorkspace.LoadResourceString(IDS_ADD);
	//m_Pictures.AddString(sAdd);
	m_Pictures.AddString(_T("< ? >"));
	m_Pictures.AddString(_T("<  i  >"));
	m_Pictures.AddString(_T("<  !  >"));
	m_Pictures.AddString(_T("< X >"));
	
	int nIndex=0;
	CProject *pProjectList = activeProject;
	int n = pProjectList->GetPictureList().GetCount();
	while(nIndex < pProjectList->GetPictureList().GetCount())
	{
		POSITION pos = pProjectList->GetPictureList().FindIndex(nIndex);
		if (pos != NULL)
		{
			CPictureObject *pPic = pProjectList->GetPictureList().GetAt(pos);
			TCHAR Value[80];
			_ltot(pPic->GetID(), Value, 10);
			int n = m_Pictures.AddString(Value);
			m_Pictures.SetItemData(n, pPic->GetID());
			if (m_SelectedPic == pPic->GetID())
				m_Pictures.SetCurSel(n);
			if (pPic->GetID() > m_nHighestId)
				m_nHighestId = pPic->GetID();
		}
		nIndex++;
	}

	switch (m_pToolTipPicture->GetLongValue())
	{
	case -1:
		m_Pictures.SetCurSel(1);
		break;
	case -2:
		m_Pictures.SetCurSel(2);
		break;
	case -3:
		m_Pictures.SetCurSel(3);
		break;
	case -4:
		m_Pictures.SetCurSel(4);
		break;
	default:
		{
		n = m_Pictures.FindStringExact(0, LTOA(m_pToolTipTitle->GetLongValue()));
		if (n != -1)
			m_Pictures.SetCurSel(n);
		else		
			m_Pictures.SetCurSel(0);
		}
	}
	m_Title.SetWindowText(m_pToolTipTitle->GetStringValue());	
	m_Line.SetCheck(m_pToolTipLine->GetBooleanValue());	
	m_MainText.SetWindowText(m_pToolTipBody->GetStringValue());	

	m_Color.SetColour((COLORREF)m_pToolTipTitleColor->GetLongValue());

	n = m_Pictures.FindStringExact(0, LTOA(m_pToolTipPicture->GetLongValue()));	
	if (n > -1)
		m_Pictures.SetCurSel(n);

	m_Avi.SetWindowText(m_pToolTipAvi->GetStringValue());

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

BOOL CToolTipsPage::PreTranslateMessage(MSG* pMsg) 
{
	m_tooltip.RelayEvent(pMsg); 
	return CPropertyPage::PreTranslateMessage(pMsg);
}


HBITMAP	GetBitmapFromProject(UINT nID, CSize &sz)
{
	return activeProject->CloneBitmap( nID, sz );
}

HICON GetIconFromProject(UINT nID)
{
	return activeProject->CloneIcon( nID );
}

void CToolTipsPage::OnPreview() 
{
	CString sTitle;
	m_Title.GetWindowText(sTitle);
	int nLine = m_Line.GetCheck();
	CString sBody;
	m_MainText.GetWindowText(sBody);
	CString sAviFile;
	m_Avi.GetWindowText(sAviFile);
	int nColor = m_Color.GetColour();
	CString sTooltipText = ConstructTooltipHtml( sTitle, GetRGBColor( nColor ), (nLine != 0), sBody );

	CRect rect;
	GetDlgItem(IDC_PREVIEW)->GetWindowRect(&rect);
	CPoint pt = rect.CenterPoint();

	CString sPic;
	if (m_Pictures.GetCurSel() != -1)
		m_Pictures.GetText(m_Pictures.GetCurSel(), sPic);
	if (sPic == _T("< ? >"))
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText, IDI_HELP);
	else if (sPic == _T("<  i  >"))
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText, IDI_INFO);
	else if (sPic == _T("<  !  >"))
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText, IDI_EXCLEMATION);
	else if (sPic == _T("< X >"))
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText, IDI_X);
	else if (sPic.Left(1) == _T("<"))	
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText);
	else if (sPic.GetLength() > 0)
	{
		int nId = _tstol(sPic);
		HICON hIcon = GetIconFromProject(nId);
		if (hIcon != NULL)
			m_tooltip.ShowHelpTooltip(&pt, sTooltipText, hIcon);
		else
			m_tooltip.ShowHelpTooltip(&pt, sTooltipText);
	}
	else
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText);
}

void CToolTipsPage::OnBold() 
{
	ModifySelection( _T("<b>"), _T("</b>") );
	SetModified();
}

void CToolTipsPage::OnItalic() 
{
	ModifySelection( _T("<i>"), _T("</i>") );
	SetModified();
}

void CToolTipsPage::OnUnderline() 
{
	ModifySelection( _T("<u>"), _T("</u>") );
	SetModified();
}

void CToolTipsPage::OnColor2() 
{
	CColorDialog dlg;
	// Get a color from the common color dialog.
	if( dlg.DoModal() != IDOK )
		return;
	COLORREF cr = dlg.GetColor();
	CString sColor;
	sColor.Format( _T("<font color=#%02X%02X%02X>"), GetRValue(cr), GetGValue(cr), GetBValue(cr) );
	ModifySelection( sColor, _T("</font>") );
	SetModified();
}

void CToolTipsPage::ModifySelection( LPCTSTR pszPrefix, LPCTSTR pszSuffix )
{
	CString sBody;
	m_MainText.GetWindowText(sBody);
	int nStart;
	int nEnd;
	m_MainText.GetSel( nStart, nEnd );
	CString sSelText = sBody.Mid( nStart, nEnd - nStart );
	m_MainText.ReplaceSel( CString( pszPrefix ) + sSelText + pszSuffix, TRUE );
	int nNewStart = nStart + lstrlen( pszPrefix );
	m_MainText.SetSel( nNewStart, nNewStart + nEnd - nStart );
	m_MainText.SetFocus();
}

void CToolTipsPage::Commit()
{
	CString sTitle;
	m_Title.GetWindowText(sTitle);
	m_pToolTipTitle->SetStringValue(sTitle);
	m_pToolTipLine->SetBooleanValue(m_Line.GetCheck() != 0);
	CString sBody;
	m_MainText.GetWindowText(sBody);
	m_pToolTipBody->SetStringValue(sBody);

	int n = m_Pictures.GetCurSel();
	if (n != -1)
	{
		CString sId;
		m_Pictures.GetText(n, sId);

		if (n < 1)
			m_pToolTipPicture->SetLongValue(0);
		else if (n == 1)
			m_pToolTipPicture->SetLongValue(-1);
		else if (n == 2)
			m_pToolTipPicture->SetLongValue(-2);
		else if (n == 3)
			m_pToolTipPicture->SetLongValue(-3);
		else if (n == 4)
			m_pToolTipPicture->SetLongValue(-4);
		else
			m_pToolTipPicture->SetLongValue(_tstol(sId));
	}

	CString sAviFile;
	m_Avi.GetWindowText(sAviFile);
	m_pToolTipAvi->SetStringValue(sAviFile);

	m_pToolTipTitleColor->SetLongValue(m_Color.GetColour());
}

BOOL CToolTipsPage::OnApply() 
{
	Commit();
	return CPropertyPage::OnApply();
}

void CToolTipsPage::OnOK() 
{
	Commit();
	CPropertyPage::OnOK();
}

void CToolTipsPage::OnChangeAvi() 
{
	SetModified();
}

void CToolTipsPage::OnColor() 
{
	SetModified();
}

void CToolTipsPage::OnLine() 
{
	SetModified();
}

void CToolTipsPage::OnChangeMaintext() 
{
	SetModified();
}

void CToolTipsPage::OnSelchangePiclist() 
{
	SetModified();
}
