// PictureFolder.cpp : implementation file
//

#include "stdafx.h"
#include "PictureFolder.h"
#include "Project.h"
#include "PictureObject.h"
#include "PreviewFileDlg.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"


static const TCHAR sPictureFilter[] = _T("Picture Files|*.bmp;*.wmf;*.jpg;*.ico;*.dib|Bitmaps (*.bmp,*.dib)|*.bmp;*.dib|JPEG Files (*.jpg)|*.jpg|Icon Files (*.ico)|*.ico|Wmf FIle (*.wmf)|*.wmf|");


static void HIMETRICtoDP(HDC hdc, LPSIZE lpSize)
{
	// map against logical inch for non-constrained mapping modes
	int cxPerInch, cyPerInch;
	cxPerInch = GetDeviceCaps(hdc, LOGPIXELSX);
	cyPerInch = GetDeviceCaps(hdc, LOGPIXELSY);

	ASSERT(cxPerInch != 0 && cyPerInch != 0);
	lpSize->cx = MulDiv(lpSize->cx, cxPerInch, 2540);
	lpSize->cy = MulDiv(lpSize->cy, cyPerInch, 2540);
}


static bool FindPictureId(int nId)
{
	// if the count is 0, then return a false to indicate, not found
	if (activeProject->GetPictureList().GetCount() == 0)
		return false;

	// do loop to find a matching id
	for (int i=0; i<activeProject->GetPictureList().GetCount(); i++)
	{
		POSITION pos = activeProject->GetPictureList().FindIndex(i);
		if (pos != NULL)
		{
			CPictureObject *pPic = activeProject->GetPictureList().GetAt(pos);
			if (pPic != NULL)
			{
				// if this is the same id then return a true to indicate, found	
				if (pPic->GetID() == nId)
					return true;
			}
		}
	}
	// if made it this far then return a false to indicate, not found
	return false;
}


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


static void UpdatePicture(short nID, LPPICTUREDISP NewPicture, CPictureObject *pPicture) 
{
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
}


static void LoadPictureFile(LPCTSTR szFile, int nID, CPictureObject *pPicture = NULL)
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

	if (pPicture == NULL)		
		AddPicture(nID, phPicture.GetPictureDispatch());
	else
		UpdatePicture(nID, phPicture.GetPictureDispatch(), pPicture);
	phPicture.m_pPict = ipOld;
	pstm->Release();
}


/////////////////////////////////////////////////////////////////////////////
// CPictureFolder dialog

CPictureFolder::CPictureFolder(CWnd* pParent /*=NULL*/)
	: CDialog(CPictureFolder::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPictureFolder)
	//}}AFX_DATA_INIT
}


void CPictureFolder::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPictureFolder)
	DDX_Control(pDX, IDC_PICTURELIST, m_PictureList);
	DDX_Control(pDX, IDC_PICTURE, m_PictureBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPictureFolder, CDialog)
	//{{AFX_MSG_MAP(CPictureFolder)
	ON_BN_CLICKED(IDADD, OnAdd)
	ON_BN_CLICKED(IDDELETE, OnDelete)
	ON_LBN_SELCHANGE(IDC_PICTURELIST, OnSelchangePicturelist)
	ON_BN_CLICKED(IDUPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPictureFolder message handlers

void CPictureFolder::OnAdd() 
{
	MultiFileDialog();
	
}

void CPictureFolder::OnDelete() 
{
	int nCurSel = m_PictureList.GetCurSel();

	// if nothing selected
	if (nCurSel == -1)
		return;

	// set the picture box blank
	m_PictureBox.SetPictureBlank();

	CProject *pProjectList = activeProject;
	
	int nCurSelId = m_PictureList.GetItemData(nCurSel);

	for (int i = 0; i<pProjectList->GetPictureList().GetCount(); i++)
	{
		POSITION pos = pProjectList->GetPictureList().FindIndex(i);
		if (pos!= NULL)
		{
			CPictureObject *pPicture = pProjectList->GetPictureList().GetAt(pos);
			if (pPicture != NULL)
			{
				if (pPicture->GetID() == nCurSelId)
				{
					// set the delete flag on so it will be deleted when the dialog box is closed
					pPicture->SetToBeDeleted();
					m_PictureList.DeleteString(nCurSel);
					if (m_PictureList.GetCount() > nCurSel)
					{
						m_PictureList.SetCurSel(nCurSel);
						m_PictureBox.SetPictureID(m_PictureList.GetItemData(nCurSel));
					}
					else if (m_PictureList.GetCount() > 0)
					{
						m_PictureList.SetCurSel(nCurSel-1);
						m_PictureBox.SetPictureID(m_PictureList.GetItemData(nCurSel-1));
					}
					else
					{
						m_PictureList.SetCurSel(-1);
						m_PictureBox.SetPictureBlank();
					}
				}
			}
		}
	}
	
}

void CPictureFolder::OnOK() 
{
	m_PictureBox.SetPictureBlank();
	
	CProject *pProjectList = activeProject;
	
	for (int i = pProjectList->GetPictureList().GetCount()-1; i>=0; i--)
	{
		POSITION pos = pProjectList->GetPictureList().FindIndex(i);
		if (pos!= NULL)
		{
			CPictureObject *pPicture = pProjectList->GetPictureList().GetAt(pos);
			if (pPicture != NULL)
			{
				if (pPicture->GetToBeDeleted() == true)
				{
					delete pPicture;
					pProjectList->GetPictureList().RemoveAt(pos);
				}
			}
		}
	}
	CheckPictureRefs();

	CDialog::OnOK();
}

void CPictureFolder::OnCancel() 
{
	m_PictureBox.SetPictureBlank();
	
	CProject *pProjectList = activeProject;
	
	for (int i = pProjectList->GetPictureList().GetCount()-1; i>=0; i--)
	{
		POSITION pos = pProjectList->GetPictureList().FindIndex(i);
		if (pos!= NULL)
		{
			CPictureObject *pPicture = pProjectList->GetPictureList().GetAt(pos);
			if (pPicture != NULL)
			{
				if (pPicture->GetToBeAdded())
				{
					delete pPicture;
					pProjectList->GetPictureList().RemoveAt(pos);
				}
			}
		}
	}


	CDialog::OnCancel();
}

BOOL CPictureFolder::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CProject *pProjectList = activeProject;
	
	for (int i = 0; i<pProjectList->GetPictureList().GetCount(); i++)
	{
		POSITION pos = pProjectList->GetPictureList().FindIndex(i);
		if (pos!= NULL)
		{
			CPictureObject *pPicture = pProjectList->GetPictureList().GetAt(pos);
			if (pPicture != NULL)
			{
				pPicture->SetToBeAdded(false);
				pPicture->SetToBeDeleted(false);
				TCHAR Value[80];
				_ltot(pPicture->GetID(), Value, 10);
				m_PictureList.SetItemData(m_PictureList.AddString(Value), pPicture->GetID());
			}
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}
void CPictureFolder::MultiFileDialog() 
{
	CStringArray m_FileList;
	m_FileList.RemoveAll();

	// create the open dialog box
	CPreviewFileDlg BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_ALLOWMULTISELECT | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		sPictureFilter,
		CWnd::GetActiveWindow());

	// proceed to setup the file buffer size
	BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
	TCHAR* pc = new TCHAR[MAX_PATH];
	BrowseWnd.m_ofn.lpstrFile = pc;
    BrowseWnd.m_ofn.lpstrFile[0] = _T('\0');

	// call method to invoke the file dialog box
	int iReturn = BrowseWnd.DoModal();
		
	
	if(iReturn == IDOK)   
	{
		CString szPathName;
		POSITION pos;

		// do loop to get all selected files
		for (pos = BrowseWnd.GetStartPosition(); pos != NULL; )
		{
			// get the file name 
			szPathName = BrowseWnd.GetNextPathName(pos);
	
			// add the file name to the file list
			m_FileList.Add(szPathName);				
		}
	}

	delete [] pc; 
	
	for (int i=0; i<m_FileList.GetSize(); i++)
	{
		int nLargestId;
		// change this line later for smallest available id
		if (m_PictureList.GetCount() > 0)
			nLargestId = m_PictureList.GetItemData(m_PictureList.GetCount()-1) + 1;
		else
			nLargestId = 100;
		
		// load the picture into the picture list collection
		LoadPictureFile(m_FileList.GetAt(i), nLargestId);
		
		TCHAR Value[80];
		_ltot(nLargestId, Value, 10);
		m_PictureList.SetItemData(m_PictureList.AddString(Value), nLargestId);	
	}
	
	m_FileList.RemoveAll();
}

void CPictureFolder::UpdateSingleFileDialog() 
{
	int nCurSel = m_PictureList.GetCurSel();

	// create the open dialog box
	CPreviewFileDlg BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		sPictureFilter,
		CWnd::GetActiveWindow());

	// proceed to setup the file buffer size
	BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
	TCHAR* pc = new TCHAR[MAX_PATH];
	BrowseWnd.m_ofn.lpstrFile = pc;
    BrowseWnd.m_ofn.lpstrFile[0] = _T('\0');

	// call method to invoke the file dialog box
	int iReturn = BrowseWnd.DoModal();
		
	CString szPathName;
	if(iReturn == IDOK)   
	{		
		POSITION pos;

		// do loop to get all selected files
		for (pos = BrowseWnd.GetStartPosition(); pos != NULL; )
		{
			// get the file name 
			szPathName = BrowseWnd.GetNextPathName(pos);

			int nId = m_PictureList.GetItemData(nCurSel);
			// load the picture into the picture list collection
			LoadPictureFile(szPathName, nId, GetSelectedPictureObject());
			
		}
	}

	delete [] pc; 
	
}


CPictureObject * CPictureFolder::GetSelectedPictureObject() 
{
	CProject *pProjectList = activeProject;
	
	if (m_PictureList.GetCurSel() == -1)
		return NULL;

	int nCurSel = m_PictureList.GetItemData(m_PictureList.GetCurSel());

	for (int i = 0; i<pProjectList->GetPictureList().GetCount(); i++)
	{
		POSITION pos = pProjectList->GetPictureList().FindIndex(i);
		if (pos!= NULL)
		{
			CPictureObject *pPicture = pProjectList->GetPictureList().GetAt(pos);
			if (pPicture != NULL)
			{
				if (pPicture->GetID() == nCurSel)
				{
					return pPicture;
				}
			}
		}
	}
	return NULL;	
}

void CPictureFolder::OnSelchangePicturelist() 
{
	CProject *pProjectList = activeProject;
	
	if (m_PictureList.GetCurSel() == -1)
		return;

	int nCurSel = m_PictureList.GetItemData(m_PictureList.GetCurSel());

	for (int i = 0; i<pProjectList->GetPictureList().GetCount(); i++)
	{
		POSITION pos = pProjectList->GetPictureList().FindIndex(i);
		if (pos!= NULL)
		{
			CPictureObject *pPicture = pProjectList->GetPictureList().GetAt(pos);
			if (pPicture != NULL)
			{
				if (pPicture->GetID() == nCurSel)
				{
					m_PictureBox.SetPictureID(nCurSel);
				}
			}
		}
	}
	
}

// search every picture id property and ensure it's id is still valid
void CPictureFolder::CheckPictureRefs() 
{
	CProject *pProjectList = activeProject;

	// do loops to search every picture id property and ensure it's id is still valid
	for (int i=0; i<pProjectList->GetDclFormList().GetCount(); i++)
	{
		POSITION pos = pProjectList->GetDclFormList().FindIndex(i);
		if (pos != NULL)
		{
			CDclFormObject *pDclObject = pProjectList->GetDclFormList().GetAt(pos);
			if (pDclObject != NULL)
			{
				// call method search this dcl form's controls for invalide picture id's
				SearchPictureRefs(pDclObject);
			}
		}	
	}
	
}

void CPictureFolder::SearchPictureRefs(CDclFormObject *pDclObject) 
{
	for (int i=0; i<pDclObject->GetControlList().GetCount(); i++)
	{
		POSITION pos = pDclObject->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pControl = pDclObject->GetControlList().GetAt(pos);
			int nPictureId = pControl->GetLngProperty(nPicture);
			if (nPictureId > -1)
			{
				if (!FindPictureId(nPictureId))
				{
					pControl->SetLngProperty(nPicture, 0);
				}
			}
			int nPressedPictureId = pControl->GetLngProperty(nPressedPicture);
			if (nPressedPictureId > -1)
			{
				if (!FindPictureId(nPressedPictureId))
				{
					pControl->SetLngProperty(nPressedPicture, 0);
				}
			}
			int nIconId = pControl->GetLngProperty(nIcon);
			if (nIconId > -1)
			{
				if (!FindPictureId(nIconId))
				{
					pControl->SetLngProperty(nIcon, 0);
				}
			}
		}
	}
	
}

void CPictureFolder::OnUpdate() 
{
	UpdateSingleFileDialog();
	OnSelchangePicturelist();
}
