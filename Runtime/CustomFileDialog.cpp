// CustomFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "CustomFileDialog.h"
#include "DialogObject.h"
#include "DclFormTypes.h"
#include "PropertyIds.h"
#include "Project.h"
#include "DialogControl.h"
#include "InvokeMethod.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "ControlTypes.h"
#include "Resource.h"


const UINT WM_FILEDLG_GETFILENAME = RegisterWindowMessage( _T("odcl.FileDialog.GetFileName") );
const UINT WM_FILEDLG_GETFILETITLE = RegisterWindowMessage( _T("odcl.FileDialog.GetFileTitle") );
const UINT WM_FILEDLG_GETFILEEXT = RegisterWindowMessage( _T("odcl.FileDialog.GetFileExt") );
const UINT WM_FILEDLG_GETFILEPATH = RegisterWindowMessage( _T("odcl.FileDialog.GetFilePath") );
const UINT WM_FILEDLG_GETFOLDERPATH = RegisterWindowMessage( _T("odcl.FileDialog.GetFolderPath") );
const UINT WM_FILEDLG_GETFOLDERNAME = RegisterWindowMessage( _T("odcl.FileDialog.GetFolderName") );
const UINT WM_FILEDLG_GETSELECTEDFILECOUNT = RegisterWindowMessage( _T("odcl.FileDialog.GetSelectedFileCount") );
const UINT WM_FILEDLG_GETSELECTEDFILES = RegisterWindowMessage( _T("odcl.FileDialog.GetSelectedFiles") );


class CArxFileDialogControl : public CArxDialogControl
{
public:
	CArxFileDialogControl( CDclControlObject* pTemplate, CControlPane* pPane, CWnd* pControl )
		: CArxDialogControl( pTemplate, pPane, pControl ) {}
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
};


// CFileDialogX interface implementation
CFileDialogX::CFileDialogX( CCustomFileDialog& Owner, CDclFormObject* pDclForm, FileDialogParams* pParams /*= NULL*/ )
: CArxDialogObject( pDclForm, &Owner )
, mpOwner( &Owner )
, mpParams( pParams )
{
}

CFileDialogX::~CFileDialogX()
{
}

DclFormType CFileDialogX::GetType() const
{
	return VdclFileDialog;
}

HWND CFileDialogX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

void CFileDialogX::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return; //already in the process of closing
	SetClosing();
	mpOwner->GetMainDialog().EndDialog( nStatus );
	mpOwner->GetMainDialog().SendMessage( WM_CLOSE, 0, 0 );
}

INT_PTR CFileDialogX::DoModal()
{
	INT_PTR nResult = mpOwner->DoModal();
	if( nResult == IDOK && mpParams )
	{
		mpParams->sFilename = mpOwner->GetPathName();
		mpParams->rsFilenames.RemoveAll();
		POSITION pos = mpOwner->GetStartPosition();
		while( pos )
			mpParams->rsFilenames.Add( mpOwner->GetNextPathName( pos ) );
	}
	delete mpOwner;
	return nResult;
}


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


static DWORD GetFileDlgFlags( CDclControlObject* pFileDlgProperties )
{
	assert (pFileDlgProperties != NULL);
	DWORD dwFlags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;

	if (pFileDlgProperties->GetLongProperty(nFileDlgStyle) == 1) // as open
	{
		if (!pFileDlgProperties->GetBoolProperty(nShowReadOnlyCheckBox))
			dwFlags |= OFN_HIDEREADONLY;
		if (pFileDlgProperties->GetBoolProperty(nAsReadOnly))
			dwFlags |= OFN_READONLY;
		if (pFileDlgProperties->GetBoolProperty(nMultiSelect))
			dwFlags |= OFN_ALLOWMULTISELECT;
		if (pFileDlgProperties->GetBoolProperty(nFileMustExist))
			dwFlags |= OFN_FILEMUSTEXIST;
		if (pFileDlgProperties->GetBoolProperty(nCreationPrompt))
			dwFlags |= OFN_CREATEPROMPT;
	}
	else if (pFileDlgProperties->GetLongProperty(nFileDlgStyle) == 0) // as save
	{
		dwFlags |= OFN_HIDEREADONLY;
		if (pFileDlgProperties->GetBoolProperty(nOverWritePrompt))
			dwFlags |= OFN_OVERWRITEPROMPT;	
	}
	if (pFileDlgProperties->GetBoolProperty(nShowHelp))
		dwFlags |= OFN_SHOWHELP;
	if (pFileDlgProperties->GetBoolProperty(nExtCanBeDiff))
		dwFlags |= OFN_EXTENSIONDIFFERENT;
	if (pFileDlgProperties->GetBoolProperty(nPathMustExist))
		dwFlags |= OFN_PATHMUSTEXIST;	
	return dwFlags;
}


/////////////////////////////////////////////////////////////////////////////
// CCustomFileDialog

CCustomFileDialog::CCustomFileDialog( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CFileDialog( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, pParent )
, mDialogX( *this, pSourceForm, pParams? (FileDialogParams*)pParams->lpData : NULL )
, mpParams( pParams? (FileDialogParams*)pParams->lpData : NULL )
, mMainFileDlg( pSourceForm, pParent, pParams )
, mpFileDlgCtrl( pSourceForm->FindFirstControlOfType(CtlFileDlgCtrl) )
{
	SetTemplate(IDD_CUSTOM_FILE_DIALOG, IDD_CUSTOM_FILE_DIALOG);
	OPENFILENAME& ofn = GetOFN();
	CDclControlObject* pProps = pSourceForm->GetControlProperties();
	if (pProps->GetBoolProperty(nResizable))
		ofn.Flags |= OFN_ENABLESIZING;
	if( pSourceForm )
	{
		if (mpFileDlgCtrl)
		{
			m_bOpenFileDialog = (mpFileDlgCtrl->GetLongProperty(nFileDlgStyle) != 0)? TRUE : FALSE;
			ofn.Flags |= GetFileDlgFlags(mpFileDlgCtrl);
			msTitle = pProps->GetStrProperty(nTitleBarText);
			if( !msTitle.IsEmpty() )
				ofn.lpstrTitle = msTitle.LockBuffer();
		}
		msFilterList = mpFileDlgCtrl->GetStrProperty(nFilter);
		if( mpParams && !mpParams->sFilterList.IsEmpty() )
			msFilterList = mpParams->sFilterList;
		if( !msFilterList.IsEmpty() )
		{
			msFilterList.Replace(_T('|'), _T('\0'));
			ofn.lpstrFilter = msFilterList.LockBuffer();
		}
		if( mpParams )
		{ //if we got params, add them to the design time properties
			//m_bOpenFileDialog = pFileDlgParams->bOpenFileDialog; //always use design time property
			mpParams->dwFlags |= ofn.Flags; //keep the flags we have so far, just add the new ones
			ofn.Flags = mpParams->dwFlags;
			msDefaultExtension = mpParams->sDefaultExtension;
			ofn.lpstrDefExt = msDefaultExtension.LockBuffer();
			DWORD cchBuffer = (ofn.Flags & OFN_ALLOWMULTISELECT)? 0x800 : MAX_PATH;
			ofn.lpstrFile = msResultBuf.GetBuffer( cchBuffer );
			memset( ofn.lpstrFile, 0, cchBuffer * sizeof(TCHAR) );
			if( !mpParams->sFilename.IsEmpty() && mpParams->sFilename.Right(1).SpanExcluding( _T("\\/") ).IsEmpty() )
			{
				msInitialDirectory = mpParams->sFilename;
				ofn.lpstrInitialDir = msInitialDirectory.LockBuffer();
			}
			else
				lstrcpyn( ofn.lpstrFile, mpParams->sFilename, cchBuffer );
			ofn.nMaxFile = cchBuffer;
		}
	}
}


CCustomFileDialog::~CCustomFileDialog()
{
}


BEGIN_MESSAGE_MAP(CCustomFileDialog, CFileDialog)
	ON_WM_HELPINFO()
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


void CCustomFileDialog::CtrlModifyStyle(int nCtrl) 
{	
	CWnd *pCtrl = GetParent()->GetDlgItem(nCtrl);
	if( pCtrl )
		pCtrl->ModifyStyle(NULL, WS_CLIPSIBLINGS, 0);
}


BOOL CCustomFileDialog::OnInitDialog() 
{
	__super::OnInitDialog();
	mMainFileDlg.SubclassWindow( ::GetParent( m_hWnd ) );

	// here we need to hide specified controls
	CDclControlObject* pCtrl = mDialogX.GetSourceForm()->FindFirstControlOfType( CtlFileDlgCtrl );
	if( pCtrl )
	{
		if (!pCtrl->GetBoolProperty(nShowOK))
			HideControl(IDOK);
		if (!pCtrl->GetBoolProperty(nShowCancel))
			HideControl(IDCANCEL);

		if(IsWindows98orLater())
		{
			//SetControlText(stc3, "");
			CWnd* pScrollBar = FindWindowEx(::GetParent(m_hWnd), NULL, _T("ScrollBar"), NULL);
			if(pScrollBar)
				::ShowWindow(pScrollBar->m_hWnd, SW_HIDE);
		}

		if (!pCtrl->GetBoolProperty(nShowTypeComboBox))
			HideControl(cmb1);
		if (!pCtrl->GetBoolProperty(nShowNameTextBox))
			HideControl(edt1);

		if (!pCtrl->GetBoolProperty(nShowTypeLabel))
			HideControl(stc2);
		if (!pCtrl->GetBoolProperty(nShowNameLabel))
			HideControl(stc3);

		CtrlModifyStyle(cmb1);
		CtrlModifyStyle(edt1);

		CtrlModifyStyle(stc2);
		CtrlModifyStyle(stc3);
		CtrlModifyStyle(IDOK);
		CtrlModifyStyle(IDCANCEL);

		//add right and bottom borders for custom controls by calculating what the right and left borders
		//are on the template, then add the same amount of border to this dialog.
		int nRightBorder =
			mDialogX.GetSourceForm()->GetControlProperties()->GetLongProperty( nWidth ) - pCtrl->GetLongProperty( nWidth );
		int nBottomBorder =
			mDialogX.GetSourceForm()->GetControlProperties()->GetLongProperty( nHeight ) - pCtrl->GetLongProperty( nHeight );

		CRect rectWindow;
		mMainFileDlg.GetWindowRect( &rectWindow );
		mMainFileDlg.GetParent()->ScreenToClient( &rectWindow );
		rectWindow.right += nRightBorder;
		rectWindow.bottom += nBottomBorder;
		mMainFileDlg.MoveWindow( &rectWindow, TRUE );

		CControlPane& CtrlPane = mDialogX.GetControlPane();
		CArxDialogControl *pCtrlObj = new CArxFileDialogControl( pCtrl, &CtrlPane, this );
		CtrlPane.AddControl( pCtrlObj );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

BOOL CCustomFileDialog::OnFileNameOK()
{
	__super::OnFileNameOK();
	return FALSE;
}

void CCustomFileDialog::OnFileNameChange()
{
	__super::OnFileNameChange();
	CWnd* pWnd = GetParent()->GetDlgItem(lst2);
	if (pWnd == NULL)
		return;
	if( !mpFileDlgCtrl )
		return;
	
	CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));
	
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	int nSelCount = wndLst1->GetSelectedCount();
	
	POSITION pos = wndLst1->GetFirstSelectedItemPosition();
	if (pos == NULL || nSelCount == 0)
	{
		// call methods to invoke the event
		InvokeMethodIntString(mpFileDlgCtrl->GetStrProperty(nEventSelChanged), -1, CString(), false);		
		return;
	}
	else
	{
		if (nSelCount == 1)
		{
			// call methods to invoke the event
			InvokeMethodIntString(mpFileDlgCtrl->GetStrProperty(nEventSelChanged), nSelCount, GetPathName(), false);			
		}
		else 
		{
			// call methods to invoke the event
			InvokeMethodIntString(mpFileDlgCtrl->GetStrProperty(nEventSelChanged), nSelCount, CString(), false);			
		}
	}
}

void CCustomFileDialog::OnTypeChange()
{
	__super::OnTypeChange();
	if( !mpFileDlgCtrl )
		return;
	CString sText;
	GetParent()->GetDlgItem(cmb1)->GetWindowText(sText);
	InvokeMethodString(mpFileDlgCtrl->GetStrProperty(nEventOnTypeChange), sText, false);
}

BOOL CCustomFileDialog::OnHelpInfo(HELPINFO* pHelpInfo)
{
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nEventOnHelp), false);
	return TRUE; 
}

void CCustomFileDialog::OnHelp()
{
	CDclControlObject* pProps = mDialogX.GetSourceForm()->GetControlProperties();
	InvokeMethod(pProps->GetStrProperty(nEventOnHelp), false);
}

void CCustomFileDialog::OnFolderChange()
{
	__super::OnFolderChange();
	if( !mpFileDlgCtrl )
		return;
	InvokeMethodString(mpFileDlgCtrl->GetStrProperty(nEventFolderChanged), GetFolderPath(), false);
}

void CCustomFileDialog::CloseNow() 
{
	mDialogX.CloseDialog( IDOK );
}

LRESULT CCustomFileDialog::OnGetFileName( WPARAM wParam, LPARAM lParam )
{
	CString sFileName = GetFileName();
	UINT_PTR cchFileName = sFileName.GetLength();
	if( wParam <= cchFileName )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFileName );
	return (LRESULT)cchFileName;
}

LRESULT CCustomFileDialog::OnGetFileTitle( WPARAM wParam, LPARAM lParam )
{
	CString sFileTitle = GetFileTitle();
	UINT_PTR cchFileTitle = sFileTitle.GetLength();
	if( wParam <= cchFileTitle )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFileTitle );
	return (LRESULT)cchFileTitle;
}

LRESULT CCustomFileDialog::OnGetFileExt( WPARAM wParam, LPARAM lParam )
{
	CString sFileExt = GetFileExt();
	UINT_PTR cchFileExt = sFileExt.GetLength();
	if( wParam <= cchFileExt )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFileExt );
	return (LRESULT)cchFileExt;
}

LRESULT CCustomFileDialog::OnGetFilePath( WPARAM wParam, LPARAM lParam )
{
	CString sFilePath = GetPathName();
	UINT_PTR cchFilePath = sFilePath.GetLength();
	if( wParam <= cchFilePath )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFilePath );
	return (LRESULT)cchFilePath;
}

LRESULT CCustomFileDialog::OnGetFolderPath( WPARAM wParam, LPARAM lParam )
{
	CString sFolderPath = GetFolderPath();
	UINT_PTR cchFolderPath = sFolderPath.GetLength();
	if( wParam <= cchFolderPath )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFolderPath );
	return (LRESULT)cchFolderPath;
}

LRESULT CCustomFileDialog::OnGetFolderName( WPARAM wParam, LPARAM lParam )
{
	CString sFolderName = GetFolderPath().MakeReverse().SpanExcluding(_T("\\/:")).MakeReverse();
	UINT_PTR cchFolderName = sFolderName.GetLength();
	if( wParam <= cchFolderName )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFolderName );
	return (LRESULT)cchFolderName;
}

LRESULT CCustomFileDialog::OnGetSelectedFileCount( WPARAM wParam, LPARAM lParam )
{
	UINT cFiles = 0;
	for( POSITION pos = GetStartPosition(); pos; GetNextPathName( pos ) )
		++cFiles;
	return (LRESULT)cFiles;
}

LRESULT CCustomFileDialog::OnGetSelectedFiles( WPARAM wParam, LPARAM lParam )
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
