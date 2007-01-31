// ParentFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ParentFileDialog.h"
#include "DclFormObject.h"
#include "PropertyIds.h"
#include "Project.h"
#include "DialogControl.h"
#include "InvokeMethod.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"


bool IsWindows98orLater()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	return 
		(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
		( (osvi.dwMajorVersion > 4) ||
		( (osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0) ) );
}

/////////////////////////////////////////////////////////////////////////////
// CParentFileDialog

IMPLEMENT_DYNAMIC(CParentFileDialog, CFileDialog)

CParentFileDialog::CParentFileDialog(CDclFormObject* pSourceForm,
																		 BOOL bOpenFileDialog,
																		 LPCTSTR lpszDefExt,
																		 LPCTSTR lpszFileName,
																		 DWORD dwFlags,
																		 LPCTSTR lpszFilter,
																		 CWnd* pParentWnd)
:	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
, mpSourceForm( pSourceForm )
, mControlPane( pSourceForm )
, mpControl( NULL )
, mParentDlg( pSourceForm )
{
	m_pParent = pParentWnd;
	SetTemplate(IDD_CUSTOM_FILE_DIALOG, IDD_CUSTOM_FILE_DIALOG);
}


BEGIN_MESSAGE_MAP(CParentFileDialog, CFileDialog)
	//{{AFX_MSG_MAP(CParentFileDialog)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()

//
void CParentFileDialog::SetDclForm(CDclFormObject *mpSourceFormObject)
{
	// set the internal pointer to equal the object passed
	mpSourceForm = mpSourceFormObject;
}

void CParentFileDialog::CtrlModifyStyle(int nCtrl) 
{	
	CWnd *pCtrl = GetDlgItem(nCtrl);
	
	ModifyStyle(NULL, WS_CLIPSIBLINGS, 0);
}
BOOL CParentFileDialog::OnInitDialog() 
{
	mParentDlg.SubclassWindow(GetParent()->m_hWnd);

	// here we need to set the style of the dialog box according to it is to be resizable or not
	if (mpSourceForm->GetControlProperties()->GetBoolProperty(nResizable) == FALSE)
		// set as not resizable
		mParentDlg.ModifyStyle(WS_THICKFRAME, DS_MODALFRAME, 0);
	else
		// set as resizable
		mParentDlg.ModifyStyle(DS_MODALFRAME, WS_THICKFRAME, 0);

	mParentDlg.m_pMainChild = this;
	CFileDialog::OnInitDialog();
	
	
	// Here's one of the big differences from pre MFC 4.0
	// customizations. All of the controls on the Explorer
	// dialog are now on a dialog which is the PARENT of
	// the CFileDialog. That's right; you'll need to do a
	// GetParent() first, before using GetDlgItem().
	
	// Overall, this removes the edit control and its static
	// text and then moves the filetypes combo and static 
	// text up into the same spot.
	CRect rcWndEdit, rcWndStatic;

	CWnd* pWnd =GetDlgItem(1119);

	int nHeightOffset = 0;
	if (mpSourceForm->GetControlProperties()->GetBoolProperty(nResizable) == FALSE)
		nHeightOffset = 10;
	

	CRect rcCtrls;
	pWnd->GetWindowRect(&rcCtrls);
	pWnd->SetWindowPos(NULL, 
		m_pFileDlgProps->GetLngProperty(nLeft), 
		m_pFileDlgProps->GetLngProperty(nTop), 
		rcCtrls.Width(),
		rcCtrls.Height(), 
		SWP_NOZORDER | SWP_NOACTIVATE);

	CRect rcRet;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = m_sProjectName + "_" + m_sDialogName; 
    
    rcRet.left = pApp->GetProfileInt(sProfileName, _T("nTopLeftX"), -1);
    rcRet.top = pApp->GetProfileInt(sProfileName, _T("nTopLeftY"), -1);
	
	rcRet.right = rcRet.left + pApp->GetProfileInt(sProfileName, _T("sizeWidth"), -1);
	rcRet.bottom = rcRet.top + pApp->GetProfileInt(sProfileName, _T("sizeHeight"), -1);

	rcRet.right = -1;
	if (rcRet.right == -1)
	{
		int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		int nParentWidth = mParentDlg.GetSourceForm()->GetControlProperties()->GetLngProperty(nWidth);
		int nParentHeight = mParentDlg.GetSourceForm()->GetControlProperties()->GetLngProperty(nHeight);			

		CPoint pt;
		// get the left and top values to center the form on the screen	
		pt.y =  (nScreenHeight - nParentHeight) / 2;
		pt.x =  (nScreenWidth - nParentWidth) / 2;

		GetParent()->SetWindowPos(NULL, pt.x, pt.y, nParentWidth, nParentHeight, 
			SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else	
	{
		GetParent()->SetWindowPos(NULL, rcRet.left, rcRet.top, rcRet.Width(), rcRet.Height(), 
			SWP_NOZORDER | SWP_NOACTIVATE);
	}
	
	

	pWnd = GetParent()->GetDlgItem(edt1);
	pWnd->GetWindowRect(&rcWndEdit);
	GetParent()->ScreenToClient(&rcWndEdit);
	pWnd = GetParent()->GetDlgItem(stc3);
	pWnd->GetWindowRect(&rcWndStatic);
	GetParent()->ScreenToClient(&rcWndStatic);

	
	// here we need to hide specified controls
	if (m_pFileDlgProps->GetBoolProperty(nShowOK) == FALSE)
		HideControl(IDOK);
	if (m_pFileDlgProps->GetBoolProperty(nShowCancel) == FALSE)
		HideControl(IDCANCEL);

	if(IsWindows98orLater())
	{
		//SetControlText(stc3, "");
		HWND hwndSB = FindWindowEx(GetParent()->m_hWnd, NULL, _T("ScrollBar"), NULL)->m_hWnd;
		if(hwndSB)
			::ShowWindow(hwndSB, SW_HIDE);
	}

	
	if (m_pFileDlgProps->GetBoolProperty(nShowTypeComboBox) == FALSE)
		HideControl(cmb1);
	if (m_pFileDlgProps->GetBoolProperty(nShowNameTextBox) == FALSE)
		HideControl(edt1);

	if (m_pFileDlgProps->GetBoolProperty(nShowTypeLabel) == FALSE)
		HideControl(stc2);
	if (m_pFileDlgProps->GetBoolProperty(nShowNameLabel) == FALSE)
		HideControl(stc3);

	CtrlModifyStyle(cmb1);
	CtrlModifyStyle(edt1);

	CtrlModifyStyle(stc2);
	CtrlModifyStyle(stc3);
	CtrlModifyStyle(IDOK);
	CtrlModifyStyle(IDCANCEL);

	CRect rcThis;
	mParentDlg.GetClientRect(&rcThis);

	mParentDlg.GetControlPane().m_pControlCol = &mParentDlg.m_ControlCol;
	mParentDlg.GetControlPane().m_pFontCollection = m_pFontCollection;
	mParentDlg.GetControlPane().m_PanePos = rcThis;	
	mParentDlg.GetControlPane().m_pParentDlg = &mParentDlg;

	mParentDlg.GetControlPane().m_sProjectName = m_sProjectName;
	mParentDlg.GetControlPane().m_sDialogName = m_sDialogName;	
	// call method to create the controls
	mParentDlg.GetControlPane().CreateControls(mpSourceForm, 1000);

	// resize the control pane so all offsets are set correctly
	mParentDlg.GetControlPane().SizeChanged(rcThis.Width(),rcThis.Height());	
	
	CArxDialogControl *pCtrlObj = new CArxDialogControl(m_pFileDlgProps, this);
	mParentDlg.m_ControlCol.AddTail(pCtrlObj);
	
	switch (mParentDlg.GetSourceForm()->GetControlProperties()->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}

	
	if (mpSourceForm->GetControlProperties()->GetBoolProperty(nResizable) == TRUE)
		mParentDlg.m_bShowGrip = true;
	else
		mParentDlg.m_bShowGrip = false;

	// gets the template size as the min track size
	CRect rc;
	GetWindowRect(&rc);
	mParentDlg.m_ptMinTrackSize.x = rc.Width();
	mParentDlg.m_ptMinTrackSize.y = rc.Height();

	mParentDlg.m_bInitDone = TRUE;

	//mParentDlg.UpdateGripPos();

	// call methods to invoke the OnInitDialog event
	InvokeMethod(mParentDlg.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventInitialize), m_bInvokeWithSendString);

	GetWindowRect(&rcThis);
	// call methods to invoke the event
	InvokeMethodIntInt(
		mParentDlg.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventSize), 
		rcThis.Width(),
		rcThis.Height(),
		false);	


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}


BOOL CParentFileDialog::ReadListViewNames()
{
	// Okay, this is the big hack of the sample, I admit it.
	// With some creative use of the Spy++ utility, you will
	// find that the listview is not actually ID = lst1 as
	// documented in some references, but is actually a child
	// of dlg item ID = lst2
	
	// WARNING! Although this is a non-intrusive customization,
	// it does rely on unpublished (but easily obtainable)
	// information. The Windows common file dialog box implementation
	// may be subject to change in future versions of the
	// operating systems, and may even be modified by updates of
	// future Microsoft applications. This code could break in such
	// a case. It is intended to be a demonstration of one way of
	// extending the standard functionality of the common dialog boxes.

	CWnd* pWnd = GetParent()->GetDlgItem(lst2);
	if (pWnd == NULL)
		return FALSE;
	
	CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));

	UINT nSelected = wndLst1->GetSelectedCount();
	if (!nSelected)
		// nothing selected -- don't retrieve list
		return FALSE;

	CString strDirectory = GetFolderPath();

	// reset the list
	m_pStrList->RemoveAll();
	m_pStrList->SetSize(0,1);

	// /\ 4.2: undocumented, but non-implementation
	CString strItemText;
	// Could this iteration code be cleaner?
	for (int nItem = wndLst1->GetNextItem(-1,LVNI_SELECTED); 
			nSelected-- > 0; nItem = wndLst1->GetNextItem(nItem, LVNI_SELECTED))
	{
		strItemText = wndLst1->GetItemText(nItem,0);

		m_pStrList->Add(strItemText);
	}	
	return TRUE;
}


BOOL CParentFileDialog::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(pResult != NULL);

	// allow message map to override
	if (CCommonDialog::OnNotify(wParam, lParam, pResult))
		return TRUE;

	OFNOTIFY* pNotify = (OFNOTIFY*)lParam;
	switch(pNotify->hdr.code)
	{		
	case CDN_HELP:
		OnHelp();
		return TRUE;
		break;
	case CDN_INITDONE:
		OnInitDone();
		return TRUE;
		break;
	case CDN_SELCHANGE:
		OnFileNameChange();
		OnSelectionChanged();
		return TRUE;
		break;
	case CDN_FOLDERCHANGE:
		OnFolderChange();
		OnSelectionChanged();
		return TRUE;
		break;
	case CDN_SHAREVIOLATION:
		*pResult = OnShareViolation(pNotify->pszFile);
		return TRUE;
		break;
	case CDN_FILEOK:
		*pResult = OnFileNameOK();
		return TRUE;
		break;
	case CDN_TYPECHANGE:
		OnTypeChange();
		OnSelectionChanged();
		return TRUE;
		break;
	}
	//return TRUE;   // not handled
	return FALSE;   // not handled
}

void CParentFileDialog::OnSelectionChanged()
{
	CWnd* pWnd = GetParent()->GetDlgItem(lst2);
	if (pWnd == NULL)
		return;
	
	CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));
	
	int nSelCount = wndLst1->GetSelectedCount();
	
	POSITION pos = wndLst1->GetFirstSelectedItemPosition();
	if (pos == NULL || nSelCount == 0)
	{
		// call methods to invoke the event
		InvokeMethodIntString(m_pFileDlgProps->GetStrProperty(nEventSelChanged), -1, CString(), m_bInvokeWithSendString);		
		return;
	}
	else
	{
		if (nSelCount == 1)
		{
			CString sItemText = GetPathName();
	
			// call methods to invoke the event
			InvokeMethodIntString(m_pFileDlgProps->GetStrProperty(nEventSelChanged), nSelCount, sItemText, m_bInvokeWithSendString);			
		}
		else 
		{
			// call methods to invoke the event
			InvokeMethodIntString(m_pFileDlgProps->GetStrProperty(nEventSelChanged), nSelCount, CString(), m_bInvokeWithSendString);			
		}
	}
}

BOOL CParentFileDialog::OnFileNameOK()
{	
	CString sFileName;
	
	m_sFileName = GetFileName();
	m_sPathName = GetPathName();
	m_sFileTitle = GetFileTitle();
	m_sFolderPath = GetFolderPath();
	StoreFileList();

	return FALSE;
}


void CParentFileDialog::OnTypeChange()
{
	CString sText;
	CWnd *pCmbo = GetParent()->GetDlgItem(cmb1);
	
	((CComboBox*)pCmbo)->GetWindowText(sText);
	InvokeMethodString(m_pFileDlgProps->GetStrProperty(nEventOnTypeChange), sText, m_bInvokeWithSendString);
}

BOOL CParentFileDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	return TRUE; 
}
void CParentFileDialog::OnHelp()
{
	InvokeMethod(m_pFileDlgProps->GetStrProperty(nEventOnHelp), m_bInvokeWithSendString);
	
}

void CParentFileDialog::OnFolderChange()
{
	m_sFolderPath = GetFolderPath();
	
	InvokeMethodString(m_pFileDlgProps->GetStrProperty(nEventFolderChanged), m_sFolderPath, m_bInvokeWithSendString);

}

void CParentFileDialog::StoreFileList()
{
	ReadListViewNames();
}


void CParentFileDialog::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (mParentDlg.m_bShowGrip)
	{
		// draw size-grip
		dc.DrawFrameControl(&mParentDlg.m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}
}


void CParentFileDialog::CloseNow() 
{
	OnOK();
	//mParentDlg.EndDialog(IDOK);
}
