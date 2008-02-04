// ToolTipsPage.cpp : implementation file
//

#include "stdafx.h"
#include "ToolTipsPage.h"
#include "ToolTips.h"
#include "AcadColorTable.h"
#include "StudioWorkspace.h"
#include "StudioDialogControl.h"
#include "PictureObject.h"
#include "PropertyObject.h"
#include "Resource.h"


///////////////////////////////////////////////////////////////////////////////
// CToolTipsPage property page

CToolTipsPage::CToolTipsPage( TDclControlPtr pDclControl )
: CPropertyPage(CToolTipsPage::IDD)
, mpDclControl( pDclControl )
{
	m_pToolTipBalloon = pDclControl->GetPropertyObject(Prop::ToolTipBalloon);	
	m_pToolTipTitle = pDclControl->GetPropertyObject(Prop::ToolTipTitle);	
	m_pToolTipLine = pDclControl->GetPropertyObject(Prop::ToolTipLine);	
	m_pToolTipBody = pDclControl->GetPropertyObject(Prop::ToolTipBody);	
	m_pToolTipPicture = pDclControl->GetPropertyObject(Prop::ToolTipPicture);	
	m_pToolTipAvi = pDclControl->GetPropertyObject(Prop::ToolTipAviFileName);
	m_pToolTipTitleColor = pDclControl->GetPropertyObject(Prop::ToolTipTitleColor);
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
	DDX_Control(pDX, IDC_BALLOON, m_Balloon);
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
	ON_BN_CLICKED(IDC_BALLOON, OnBalloon)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToolTipsPage message handlers

BOOL CToolTipsPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// Create the CPPToolTip object
	m_tooltip.Create(this);
	
	//m_tooltip.AddTool(GetDlgItem(IDC_AVILABEL), _T("<b><ct=0x0000FF><al_c>AVI File Name</b><br><ct=0x00AA00><hr=100%></ct><br>The AVI file must be located in the same directory as the .odcl file."));
	
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
	TStudioProjectPtr pProjectList = activeProject;
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
		m_Pictures.SelectString(0, m_pToolTipPicture->GetStringValue());
		break;
	}
	m_Balloon.SetCheck(m_pToolTipBalloon->GetBooleanValue());	
	m_Title.SetWindowText(m_pToolTipTitle->GetStringValue());	
	m_Line.SetCheck(m_pToolTipLine->GetBooleanValue());	
	m_MainText.SetWindowText(m_pToolTipBody->GetStringValue());	
	m_Color.SetColour((COLORREF)m_pToolTipTitleColor->GetLongValue());
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
	COLORREF crTitle = m_Color.GetColour();
	CString sTooltipText = ConstructTooltipHtml( sTitle, crTitle, (nLine != 0), sBody );

	CRect rect;
	GetDlgItem(IDC_PREVIEW)->GetWindowRect(&rect);
	CPoint pt = rect.CenterPoint();

	m_tooltip.SetDefaultSizes( (m_Balloon.GetCheck() != 0) );
	CString sPic;
	if (m_Pictures.GetCurSel() != -1)
		m_Pictures.GetText(m_Pictures.GetCurSel(), sPic);
	if (sPic == _T("< ? >"))
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText, IDI_HELP);
	else if (sPic == _T("<  i  >"))
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText, IDI_INFO);
	else if (sPic == _T("<  !  >"))
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText, IDI_WARN);
	else if (sPic == _T("< X >"))
		m_tooltip.ShowHelpTooltip(&pt, sTooltipText, IDI_ERR);
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
	m_pToolTipBalloon->SetBooleanValue(m_Balloon.GetCheck() != 0);
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

	CStudioDialogControl::UpdateProperty(mpDclControl, m_pToolTipBalloon->GetID());
	CStudioDialogControl::UpdateProperty(mpDclControl, m_pToolTipTitle->GetID());
	CStudioDialogControl::UpdateProperty(mpDclControl, m_pToolTipLine->GetID());
	CStudioDialogControl::UpdateProperty(mpDclControl, m_pToolTipPicture->GetID());
	CStudioDialogControl::UpdateProperty(mpDclControl, m_pToolTipAvi->GetID());
	CStudioDialogControl::UpdateProperty(mpDclControl, m_pToolTipTitleColor->GetID());
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

void CToolTipsPage::OnBalloon() 
{
	SetModified();
}
