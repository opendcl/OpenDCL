// ParentFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ParentFileDialog.h"
#include "DialogObject.h"
#include "DclFormObject.h"
#include "PropertyIds.h"
#include "Project.h"
#include "DialogControl.h"
#include "InvokeMethod.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "ControlTypes.h"


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


static DWORD GetFileDlgFlags( CDclControlObject* pDclProperties, CDclControlObject* pFileDlgProperties )
{
	assert (pFileDlgProperties != NULL);
	DWORD dwFlags = OFN_EXPLORER| OFN_ENABLEHOOK|OFN_ENABLETEMPLATE;

	if (pFileDlgProperties->GetLngProperty(nFileDlgStyle) == 1) // as open
	{
		if (pFileDlgProperties->GetBoolProperty(nShowReadOnlyCheckBox) == FALSE)
			dwFlags |= OFN_HIDEREADONLY;

		if (pFileDlgProperties->GetBoolProperty(nAsReadOnly) == TRUE)
			dwFlags |= OFN_READONLY;

		if (pFileDlgProperties->GetBoolProperty(nMultiSelect) == TRUE)
			dwFlags |= OFN_ALLOWMULTISELECT;

		if (pFileDlgProperties->GetBoolProperty(nFileMustExist) == TRUE)
			dwFlags |= OFN_FILEMUSTEXIST;
		
		if (pFileDlgProperties->GetBoolProperty(nCreationPrompt) == TRUE)
			dwFlags |= OFN_CREATEPROMPT;
	}

	if (pFileDlgProperties->GetLngProperty(nFileDlgStyle) == 0) // as save
	{
		dwFlags |= OFN_HIDEREADONLY;

		if (pFileDlgProperties->GetBoolProperty(nOverWritePrompt) == TRUE)
			dwFlags |= OFN_OVERWRITEPROMPT;	
	}

	if (pFileDlgProperties->GetBoolProperty(nShowHelp) == TRUE)
		dwFlags |= OFN_SHOWHELP;

	if (pFileDlgProperties->GetBoolProperty(nExtCanBeDiff) == TRUE)
		dwFlags |= OFN_EXTENSIONDIFFERENT;

	if (pFileDlgProperties->GetBoolProperty(nPathMustExist) == TRUE)
		dwFlags |= OFN_PATHMUSTEXIST;	
		
	if (pDclProperties->GetBoolProperty(nResizable) == TRUE)
		dwFlags |= OFN_ENABLESIZING;

	return dwFlags;
}


/////////////////////////////////////////////////////////////////////////////
// CParentFileDialog

CParentFileDialog::CParentFileDialog( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CFileDialog( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, pParent )
, mParentDlg( pSourceForm, pParent )
, mpParams( pParams? (FileDialogParams*)pParams->lpData : NULL )
//, mprsFilenames( NULL )
{
	SetTemplate(IDD_CUSTOM_FILE_DIALOG, IDD_CUSTOM_FILE_DIALOG);
	InitializeFromParams( pSourceForm, pParams );
}

CParentFileDialog::CParentFileDialog(CDclFormObject* pSourceForm,
																		 BOOL bOpenFileDialog,
																		 LPCTSTR lpszDefExt,
																		 LPCTSTR lpszFileName,
																		 DWORD dwFlags,
																		 LPCTSTR lpszFilter,
																		 CWnd* pParentWnd)
:	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
, mParentDlg( pSourceForm, pParentWnd )
, mpParams( NULL )
//, mprsFilenames( NULL )
{
	SetTemplate(IDD_CUSTOM_FILE_DIALOG, IDD_CUSTOM_FILE_DIALOG);
	InitializeFromParams( pSourceForm );
}

void CParentFileDialog::InitializeFromParams( CDclFormObject* pSourceForm, DialogParams* pParams /*= NULL*/ )
{
	FileDialogParams* pFileDlgParams = pParams? (FileDialogParams*)pParams->lpData : NULL;
	OPENFILENAME& ofn = GetOFN();
	if( pSourceForm )
	{
		CDclControlObject* pFileDlgProperties = pSourceForm->FindFirstControlOfType(CtlFileDlgCtrl);
		if (pFileDlgProperties)
		{
			CDclControlObject* pDclProperties = pSourceForm->GetControlProperties();
			m_bOpenFileDialog = (pFileDlgProperties->GetLngProperty(nFileDlgStyle) != 0? TRUE : FALSE);
			ofn.Flags |= GetFileDlgFlags(pDclProperties, pFileDlgProperties);
			msTitle = pDclProperties->GetStrProperty(nTitleBarText);
			if( !msTitle.IsEmpty() )
				ofn.lpstrTitle = msTitle.LockBuffer();
		}
		msFilterList = pFileDlgProperties->GetStrProperty(nFilter);
		if( pFileDlgParams && !pFileDlgParams->sFilterList.IsEmpty() )
			msFilterList = pFileDlgParams->sFilterList;
		if( !msFilterList.IsEmpty() )
		{
			msFilterList.Replace(_T('|'), _T('\0'));
			ofn.lpstrFilter = msFilterList.LockBuffer();
		}
		if( pFileDlgParams )
		{ //if we got params, add them to the design time properties
			//m_bOpenFileDialog = pFileDlgParams->bOpenFileDialog; //always use design time property
			pFileDlgParams->dwFlags |= ofn.Flags; //keep the flags we have so far, just add the new ones
			ofn.Flags = pFileDlgParams->dwFlags;
			ofn.lpstrDefExt = pFileDlgParams->sDefaultExtension.LockBuffer();
			DWORD cchBuffer = (ofn.Flags & OFN_ALLOWMULTISELECT)? (0x8000 / sizeof(TCHAR)) : MAX_PATH;
			ofn.lpstrFile = pFileDlgParams->sFilename.GetBuffer( cchBuffer );
			ofn.nMaxFile = cchBuffer;
		}
	}
}


BEGIN_MESSAGE_MAP(CParentFileDialog, CFileDialog)
	ON_WM_HELPINFO()
	ON_WM_PAINT()
	ON_MESSAGE(WM_FILEDLG_GETFILENAME, OnGetFileName)
	ON_MESSAGE(WM_FILEDLG_GETFILETITLE, OnGetFileTitle)
	ON_MESSAGE(WM_FILEDLG_GETFILEEXT, OnGetFileExt)
	ON_MESSAGE(WM_FILEDLG_GETFILEPATH, OnGetFilePath)
	ON_MESSAGE(WM_FILEDLG_GETFOLDERPATH, OnGetFolderPath)
	ON_MESSAGE(WM_FILEDLG_GETFOLDERNAME, OnGetFolderName)
	ON_MESSAGE(WM_FILEDLG_GETSELECTEDFILECOUNT, OnGetSelectedFileCount)
	ON_MESSAGE(WM_FILEDLG_GETSELECTEDFILES, OnGetSelectedFiles)
	ON_COMMAND(ID_HELP, OnHelp)
END_MESSAGE_MAP()


void CParentFileDialog::CtrlModifyStyle(int nCtrl) 
{	
	CWnd *pCtrl = GetDlgItem(nCtrl);
	
	ModifyStyle(NULL, WS_CLIPSIBLINGS, 0);
}

BOOL CParentFileDialog::OnInitDialog() 
{
	mParentDlg.SubclassWindow(GetParent()->m_hWnd);

	CDclControlObject* pProps = mParentDlg.GetDialogObject().GetSourceForm()->GetControlProperties();

	// here we need to set the style of the dialog box according to it is to be resizable or not
	if (pProps->GetBoolProperty(nResizable) == FALSE)
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
	if (pProps->GetBoolProperty(nResizable) == FALSE)
		nHeightOffset = 10;
	

	CRect rcCtrls;
	pWnd->GetWindowRect(&rcCtrls);
	pWnd->SetWindowPos(NULL, 
		pProps->GetLngProperty(nLeft), 
		pProps->GetLngProperty(nTop), 
		rcCtrls.Width(),
		rcCtrls.Height(), 
		SWP_NOZORDER | SWP_NOACTIVATE);

	CRect rcRet;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = mParentDlg.GetDialogObject().GetSourceForm()->GetKeyPath();
    
    rcRet.left = pApp->GetProfileInt(sProfileName, _T("nTopLeftX"), -1);
    rcRet.top = pApp->GetProfileInt(sProfileName, _T("nTopLeftY"), -1);
	
	rcRet.right = rcRet.left + pApp->GetProfileInt(sProfileName, _T("sizeWidth"), -1);
	rcRet.bottom = rcRet.top + pApp->GetProfileInt(sProfileName, _T("sizeHeight"), -1);

	rcRet.right = -1;
	if (rcRet.right == -1)
	{
		int nScreenWidth = ::GetSystemMetrics(SM_CXSCREEN);
		int nScreenHeight = ::GetSystemMetrics(SM_CYSCREEN);

		int nParentWidth = pProps->GetLngProperty(nWidth);
		int nParentHeight = pProps->GetLngProperty(nHeight);			

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
	if (pProps->GetBoolProperty(nShowOK) == FALSE)
		HideControl(IDOK);
	if (pProps->GetBoolProperty(nShowCancel) == FALSE)
		HideControl(IDCANCEL);

	if(IsWindows98orLater())
	{
		//SetControlText(stc3, "");
		HWND hwndSB = FindWindowEx(GetParent()->m_hWnd, NULL, _T("ScrollBar"), NULL)->m_hWnd;
		if(hwndSB)
			::ShowWindow(hwndSB, SW_HIDE);
	}

	
	if (pProps->GetBoolProperty(nShowTypeComboBox) == FALSE)
		HideControl(cmb1);
	if (pProps->GetBoolProperty(nShowNameTextBox) == FALSE)
		HideControl(edt1);

	if (pProps->GetBoolProperty(nShowTypeLabel) == FALSE)
		HideControl(stc2);
	if (pProps->GetBoolProperty(nShowNameLabel) == FALSE)
		HideControl(stc3);

	CtrlModifyStyle(cmb1);
	CtrlModifyStyle(edt1);

	CtrlModifyStyle(stc2);
	CtrlModifyStyle(stc3);
	CtrlModifyStyle(IDOK);
	CtrlModifyStyle(IDCANCEL);

	CRect rcThis;
	mParentDlg.GetClientRect(&rcThis);

	CControlPane& CtrlPane = mParentDlg.GetDialogObject().GetControlPane();
	CtrlPane.GetPaneWindowRect() = rcThis;	

	// call method to create the controls
	UINT nID = 1000;
	CtrlPane.CreateControls(mParentDlg.GetDialogObject().GetSourceForm(), nID);

	// resize the control pane so all offsets are set correctly
	CtrlPane.SizeChanged(rcThis.Width(),rcThis.Height());	
	
	CArxDialogControl *pCtrlObj = new CArxDialogControl(pProps, this);
	mParentDlg.GetDialogObject().GetControlPane().AddControl(pCtrlObj);
	
	switch (pProps->GetLngProperty(nEventInvoke))
	{
	case 1:
		m_bInvokeWithSendString = true;
		break;
	default:
		m_bInvokeWithSendString = false;
		break;
	}

	
	if (pProps->GetBoolProperty(nResizable) == TRUE)
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
	InvokeMethod(pProps->GetStrProperty(nFormEventInitialize), m_bInvokeWithSendString);

	GetWindowRect(&rcThis);
	// call methods to invoke the event
	InvokeMethodIntInt(
		pProps->GetStrProperty(nFormEventSize), 
		rcThis.Width(),
		rcThis.Height(),
		false);	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
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
	return FALSE;   // not handled
}

void CParentFileDialog::OnSelectionChanged()
{
	CWnd* pWnd = GetParent()->GetDlgItem(lst2);
	if (pWnd == NULL)
		return;
	
	CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));
	
	CDclControlObject* pProps = mParentDlg.GetDialogObject().GetSourceForm()->GetControlProperties();
	int nSelCount = wndLst1->GetSelectedCount();
	
	POSITION pos = wndLst1->GetFirstSelectedItemPosition();
	if (pos == NULL || nSelCount == 0)
	{
		// call methods to invoke the event
		InvokeMethodIntString(pProps->GetStrProperty(nEventSelChanged), -1, CString(), m_bInvokeWithSendString);		
		return;
	}
	else
	{
		if (nSelCount == 1)
		{
			CString sItemText = GetPathName();
	
			// call methods to invoke the event
			InvokeMethodIntString(pProps->GetStrProperty(nEventSelChanged), nSelCount, sItemText, m_bInvokeWithSendString);			
		}
		else 
		{
			// call methods to invoke the event
			InvokeMethodIntString(pProps->GetStrProperty(nEventSelChanged), nSelCount, CString(), m_bInvokeWithSendString);			
		}
	}
}

BOOL CParentFileDialog::OnFileNameOK()
{	
	if( mpParams )
	{
		mpParams->sFilename = GetPathName();
		mpParams->rsFilenames.RemoveAll();
		POSITION pos = GetStartPosition();
		while( pos )
			mpParams->rsFilenames.Add( GetNextPathName( pos ) );
	}
	return FALSE;
}


void CParentFileDialog::OnTypeChange()
{
	CString sText;
	CWnd *pCmbo = GetParent()->GetDlgItem(cmb1);
	
	((CComboBox*)pCmbo)->GetWindowText(sText);
	CDclControlObject* pProps = mParentDlg.GetDialogObject().GetSourceForm()->GetControlProperties();
	InvokeMethodString(pProps->GetStrProperty(nEventOnTypeChange), sText, m_bInvokeWithSendString);
}

BOOL CParentFileDialog::OnHelpInfo(HELPINFO* pHelpInfo) 
{
	return TRUE; 
}

void CParentFileDialog::OnHelp()
{
	CDclControlObject* pProps = mParentDlg.GetDialogObject().GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nEventOnHelp), m_bInvokeWithSendString);
}

void CParentFileDialog::OnFolderChange()
{
	CDclControlObject* pProps = mParentDlg.GetDialogObject().GetSourceForm()->GetControlProperties();
	InvokeMethodString(pProps->GetStrProperty(nEventFolderChanged), GetFolderPath(), m_bInvokeWithSendString);
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

LRESULT CParentFileDialog::OnGetFileName( WPARAM wParam, LPARAM lParam )
{
	CString sFileName = GetFileName();
	UINT_PTR cchFileName = sFileName.GetLength();
	if( wParam <= cchFileName )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFileName );
	return (LRESULT)cchFileName;
}

LRESULT CParentFileDialog::OnGetFileTitle( WPARAM wParam, LPARAM lParam )
{
	CString sFileTitle = GetFileTitle();
	UINT_PTR cchFileTitle = sFileTitle.GetLength();
	if( wParam <= cchFileTitle )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFileTitle );
	return (LRESULT)cchFileTitle;
}

LRESULT CParentFileDialog::OnGetFileExt( WPARAM wParam, LPARAM lParam )
{
	CString sFileExt = GetFileExt();
	UINT_PTR cchFileExt = sFileExt.GetLength();
	if( wParam <= cchFileExt )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFileExt );
	return (LRESULT)cchFileExt;
}

LRESULT CParentFileDialog::OnGetFilePath( WPARAM wParam, LPARAM lParam )
{
	CString sFilePath = GetPathName();
	UINT_PTR cchFilePath = sFilePath.GetLength();
	if( wParam <= cchFilePath )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFilePath );
	return (LRESULT)cchFilePath;
}

LRESULT CParentFileDialog::OnGetFolderPath( WPARAM wParam, LPARAM lParam )
{
	CString sFolderPath = GetFolderPath();
	UINT_PTR cchFolderPath = sFolderPath.GetLength();
	if( wParam <= cchFolderPath )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFolderPath );
	return (LRESULT)cchFolderPath;
}

LRESULT CParentFileDialog::OnGetFolderName( WPARAM wParam, LPARAM lParam )
{
	CString sFolderName = GetFolderPath().MakeReverse().SpanExcluding(_T("\\/:")).MakeReverse();
	UINT_PTR cchFolderName = sFolderName.GetLength();
	if( wParam <= cchFolderName )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFolderName );
	return (LRESULT)cchFolderName;
}

LRESULT CParentFileDialog::OnGetSelectedFileCount( WPARAM wParam, LPARAM lParam )
{
	UINT cFiles = 0;
	for( POSITION pos = GetStartPosition(); pos; GetNextPathName( pos ) )
		++cFiles;
	return (LRESULT)cFiles;
}

LRESULT CParentFileDialog::OnGetSelectedFiles( WPARAM wParam, LPARAM lParam )
{
	LPTSTR pszCursor = (LPTSTR)lParam;
	POSITION pos = GetStartPosition();
	while( pos )
	{
		CString sPath = GetNextPathName( pos );
		UINT_PTR cchPath = sPath.GetLength();
		if( wParam <= cchPath )
			return 0; //not enough space
		lstrcpyn( pszCursor, sPath, cchPath );
		pszCursor += cchPath++;
		*pszCursor++ = _T('|'); //separator
		wParam -= cchPath;
	}
	*--pszCursor = _T('\0');
	return pszCursor - (LPTSTR)lParam; //return number of characters copied, not including trailing NULL
}
