// ButtonStyles.cpp : implementation file
//

#include "stdafx.h"
#include "ButtonStyles.h"
#include "PreviewFileDlg.h"
#include "PictureObject.h"
#include "Workspace.h"
#include "Project.h"
#include "PropertyObject.h"
#include "SharedRes.h"


static short AddPicture(short nID, LPPICTUREDISP NewPicture) 
{
	//create new picture object
	CPictureObject *pPicture = new CPictureObject;
	
	pPicture->SetID(nID);
	pPicture->GetPicture().SetPictureDispatch(NewPicture);
	
	HDC hdc = ::GetDC(GetDesktopWindow());
	CDC * cdc = CDC::FromHandle(hdc);

	// assign picture object values	
	CSize sizePic;
	long lPicWidth;
	long lPicHeight;

	// get dimensions of bitmap
	pPicture->GetPicture().m_pPict->get_Width(&lPicWidth);
	pPicture->GetPicture().m_pPict->get_Height(&lPicHeight);

	sizePic.cx = (int)lPicWidth;
	sizePic.cy = (int)lPicHeight;

	// convert coordinates from units to logical units
	cdc->HIMETRICtoLP(&sizePic);
	cdc->Detach();
	pPicture->SetWidth(sizePic.cx);
	pPicture->SetHeight(sizePic.cy);

	// add the new picture object
	activeProject->GetPictureList().AddTail(pPicture);
	
	int nCount = activeProject->GetPictureList().GetCount();
	// return the index that this new picture object was inserted at
	return nCount - 1;
}


static void LoadPictureFile(LPCTSTR szFile, int nID)
{
	LPPICTURE		lpPicture;
	lpPicture		= NULL;
	CPictureHolder	phPicture;
	
	// open file
	HANDLE hFile = CreateFile(szFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	_ASSERTE(INVALID_HANDLE_VALUE != hFile);

	// get file size
	DWORD dwFileSize = GetFileSize(hFile, NULL);
	_ASSERTE(-1 != dwFileSize);

	LPVOID pvData = NULL;
	// alloc memory based on file size
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);
	_ASSERTE(NULL != hGlobal);

	pvData = GlobalLock(hGlobal);
	_ASSERTE(NULL != pvData);

	DWORD dwBytesRead = 0;
	// read file and store in global memory
	BOOL bRead = ReadFile(hFile, pvData, dwFileSize, &dwBytesRead, NULL);
	_ASSERTE(FALSE != bRead);
	GlobalUnlock(hGlobal);
	CloseHandle(hFile);

	LPSTREAM pstm = NULL;
	// create IStream* from global memory
	HRESULT hr = CreateStreamOnHGlobal(hGlobal, TRUE, &pstm);
	_ASSERTE(SUCCEEDED(hr) && pstm);
	
	// Create IPicture from image file
	if (lpPicture)
		lpPicture->Release();
	hr = ::OleLoadPicture(pstm, dwFileSize, FALSE, IID_IPicture, (LPVOID *)&lpPicture);
	_ASSERTE(SUCCEEDED(hr) && lpPicture);	

	IPicture *ipOld = phPicture.m_pPict;
	phPicture.m_pPict = lpPicture;

	AddPicture(nID, phPicture.GetPictureDispatch());
	phPicture.m_pPict = ipOld;
	pstm->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CButtonStyles property page

IMPLEMENT_DYNCREATE(CButtonStyles, CPropertyPage)

CButtonStyles::CButtonStyles() : CPropertyPage(CButtonStyles::IDD)
{
	//{{AFX_DATA_INIT(CButtonStyles)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nHighestId = 99;
}

CButtonStyles::~CButtonStyles()
{
}

void CButtonStyles::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CButtonStyles)
	DDX_Control(pDX, IDC_PICTURE, m_Picture);
	DDX_Control(pDX, IDC_PICLIST, m_PicList);
	DDX_Control(pDX, IDC_DESC, m_Desc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CButtonStyles, CPropertyPage)
	//{{AFX_MSG_MAP(CButtonStyles)
	ON_BN_CLICKED(IDC_STYLE0, OnStyle0)
	ON_BN_CLICKED(IDC_STYLE1, OnStyle1)
	ON_BN_CLICKED(IDC_STYLE2, OnStyle2)
	ON_BN_CLICKED(IDC_STYLE3, OnStyle3)
	ON_BN_CLICKED(IDC_STYLE4, OnStyle4)
	ON_LBN_SELCHANGE(IDC_PICLIST, OnSelchangePiclist)
	ON_BN_CLICKED(IDC_STYLE5, OnStyle5)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonStyles message handlers

void CButtonStyles::OnStyle0() 
{
	DisplayDesc(0);
	SetModified();	
}

void CButtonStyles::OnStyle1() 
{
	DisplayDesc(1);
	SetModified();
	
}

void CButtonStyles::OnStyle2() 
{
	DisplayDesc(2);
	SetModified();
	
}

void CButtonStyles::OnStyle3() 
{
	DisplayDesc(3);
	SetModified();
	
}

void CButtonStyles::OnStyle4() 
{
	DisplayDesc(4);
	SetModified();
	
}

void CButtonStyles::OnSelchangePiclist() 
{
	int nSel = m_PicList.GetCurSel();
	if (nSel == 1)
	{
		CString sFilter;

		sFilter = theWorkspace.LoadResourceString(IDS_FILTER);
	
		// create the open dialog box
		CPreviewFileDlg BrowseWnd(
			TRUE, 
			NULL,
			NULL, 
			OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
			sFilter,
			CWnd::GetActiveWindow());

		// proceed to setup the file buffer size
		BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
		TCHAR* pc = new TCHAR[MAX_PATH];
		BrowseWnd.m_ofn.lpstrFile = pc;
		BrowseWnd.m_ofn.lpstrFile[0] = _T('\0');

		// call method to invoke the file dialog box
		if (BrowseWnd.DoModal() == IDOK)
		{
			CString sFile = BrowseWnd.GetPathName();
			m_nHighestId++;
			LoadPictureFile(sFile, m_nHighestId);
			TCHAR Value[80];
			_ltot(m_nHighestId, Value, 10);
			int n = m_PicList.AddString(Value);
			m_PicList.SetItemData(n, m_nHighestId);
			m_PicList.SetCurSel(n);
			m_SelectedPic = m_nHighestId;
		}
		delete[] pc;
	}
	else if (nSel == 0)
	{
		m_SelectedPic = 0;
	}
	else
	{
		m_SelectedPic = m_PicList.GetItemData(nSel);
	}
	
	if (m_SelectedPic > 0)
	{
		m_Picture.SetPictureID(m_SelectedPic);
		m_Picture.Invalidate();
	}
	SetModified(TRUE);
}

BOOL CButtonStyles::OnApply() 
{
	m_pStyle->SetLongValue(m_SelectedStyle);
	m_pPicture->SetLongValue(m_SelectedPic);
	
	return CPropertyPage::OnApply();
}



void CButtonStyles::DisplayDesc(int nSetting)
{
	CString sDesc;
	
	switch (nSetting)
	{
	case 0:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE0);
		m_PicList.ShowWindow(TRUE);
		m_Picture.ShowWindow(TRUE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	case 1:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE1);
		m_PicList.ShowWindow(TRUE);
		m_Picture.ShowWindow(TRUE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	case 2:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE2);
		m_PicList.ShowWindow(FALSE);
		m_Picture.ShowWindow(FALSE);
		GetDlgItem(IDC_PICK)->ShowWindow(TRUE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	case 3:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE3);
		m_PicList.ShowWindow(FALSE);
		m_Picture.ShowWindow(FALSE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(TRUE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
		break;
	case 4:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE4);
		m_PicList.ShowWindow(FALSE);
		m_Picture.ShowWindow(FALSE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(TRUE);
		break;
	case 5:
		sDesc = theWorkspace.LoadResourceString(IDS_BUTTONSTYLE5);
		m_PicList.ShowWindow(TRUE);
		m_Picture.ShowWindow(TRUE);
		GetDlgItem(IDC_PICK)->ShowWindow(FALSE);
		GetDlgItem(IDC_SELECT)->ShowWindow(FALSE);
		GetDlgItem(IDC_FILTER)->ShowWindow(FALSE);
	}

	m_SelectedStyle = nSetting;
	m_Desc.SetWindowText(sDesc);
	SetModified(TRUE);
}

void CButtonStyles::OnStyle5() 
{
	DisplayDesc(5);	
	SetModified();
}


BOOL CButtonStyles::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	CButton *pButton = NULL;
	m_SelectedStyle = m_pStyle->GetLongValue();
	m_SelectedPic = m_pPicture->GetLongValue();
	switch (m_pStyle->GetLongValue())
	{
	case 0:
		pButton = (CButton*)GetDlgItem(IDC_STYLE0);
		break;
	case 1:
		pButton = (CButton*)GetDlgItem(IDC_STYLE1);
		break;
	case 2:
		pButton = (CButton*)GetDlgItem(IDC_STYLE2);
		break;
	case 3:
		pButton = (CButton*)GetDlgItem(IDC_STYLE3);
		break;
	case 4:
		pButton = (CButton*)GetDlgItem(IDC_STYLE4);
		break;
	case 5:
		pButton = (CButton*)GetDlgItem(IDC_STYLE5);
		break;
	}
	pButton->SetCheck(TRUE);

	DisplayDesc(m_pStyle->GetLongValue());

	CString sNone;
	sNone = theWorkspace.LoadResourceString(IDS_NONE);
	m_PicList.AddString(sNone);
	CString sAdd;
	sAdd = theWorkspace.LoadResourceString(IDS_ADD);
	m_PicList.AddString(sAdd);
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
			int n = m_PicList.AddString(Value);
			m_PicList.SetItemData(n, pPic->GetID());
			if (m_SelectedPic == pPic->GetID())
				m_PicList.SetCurSel(n);
			if (pPic->GetID() > m_nHighestId)
				m_nHighestId = pPic->GetID();
		}
		nIndex++;
	}
	OnSelchangePiclist();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}
