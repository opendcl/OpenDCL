// CustomFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CustomFileDlg.h"
#include "DialogObject.h"
#include "FormTypes.h"
#include "PropertyIds.h"
#include "Workspace.h"
#include "DialogControl.h"
#include "InvokeMethod.h"
#include "ArxWorkspace.h"
#include "ArxDialogControl.h"
#include "DclControlTemplate.h"
#include "ControlTypes.h"
#include "Resource.h"
#include "MultiMon.h"
#include <set>


const UINT WM_FILEDLG_GETFILENAME = RegisterWindowMessage( _T("OpenDCL.FileDialog.GetFileName") );
const UINT WM_FILEDLG_GETFILETITLE = RegisterWindowMessage( _T("OpenDCL.FileDialog.GetFileTitle") );
const UINT WM_FILEDLG_GETFILEEXT = RegisterWindowMessage( _T("OpenDCL.FileDialog.GetFileExt") );
const UINT WM_FILEDLG_GETFILEPATH = RegisterWindowMessage( _T("OpenDCL.FileDialog.GetFilePath") );
const UINT WM_FILEDLG_GETFOLDERPATH = RegisterWindowMessage( _T("OpenDCL.FileDialog.GetFolderPath") );
const UINT WM_FILEDLG_GETFOLDERNAME = RegisterWindowMessage( _T("OpenDCL.FileDialog.GetFolderName") );
const UINT WM_FILEDLG_GETSELECTEDFILECOUNT = RegisterWindowMessage( _T("OpenDCL.FileDialog.GetSelectedFileCount") );
const UINT WM_FILEDLG_GETSELECTEDFILES = RegisterWindowMessage( _T("OpenDCL.FileDialog.GetSelectedFiles") );


//class CArxFileDialogControl : public CArxDialogControl
//{
//public:
//	CArxFileDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControl )
//		: CArxDialogControl( pTemplate, pPane, pControl ) {}
//	bool Create( CWnd* pParentWnd, UINT nID ) override { return false; }
//	CRect GetWndRect() const override
//		{
//			return CRect( 0, 0,
//										mpTemplate->GetLongProperty(Prop::Width),
//										mpTemplate->GetLongProperty(Prop::Height) );
//		}
//};


static HHOOK& GetHook()
{
	static HHOOK hkCallWnd = NULL;
	return hkCallWnd;
}


static std::set< CControlPane* >& GetHookedPanes()
{
	static std::set< CControlPane* > setHookedPanes;
	return setHookedPanes;
}


static LRESULT CALLBACK GetMessageHook( int nCode, WPARAM wParam, LPARAM lParam )
{
	LPMSG pMSG = (LPMSG)lParam;
	if( wParam == PM_REMOVE && pMSG->hwnd )
	{
		for( std::set< CControlPane* >::const_iterator iter = GetHookedPanes().begin();
				 iter != GetHookedPanes().end();
				 ++iter )
		{
			TDialogControlPtr pDlgControl = (*iter)->FindControl( pMSG->hwnd );
			if( pDlgControl )
			{
				if( pDlgControl->GetControlWnd()->PreTranslateMessage( pMSG ) )
				{
					pMSG->hwnd = NULL;
					pMSG->message = -1;
					return 0;
				}
				break;
			}
		}
	}
	return CallNextHookEx( GetHook(), nCode, wParam, lParam );
}


static void SetHook( CControlPane* pControlPane, bool bAddRef = true )
{
	if( bAddRef )
	{
		GetHookedPanes().insert( pControlPane );
		if( !GetHook() )
			GetHook() = SetWindowsHookEx( WH_GETMESSAGE, &GetMessageHook, NULL, GetCurrentThreadId() );
	}
	else
	{
		GetHookedPanes().erase( pControlPane );
		if( GetHookedPanes().empty() )
		{
			UnhookWindowsHookEx( GetHook() );
			GetHook() = NULL;
		}
	}
}


static bool IsWindows98orLater()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx (&osvi);
	return
		(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) &&
		( (osvi.dwMajorVersion > 4) ||
		( (osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0) ) );
}


static DWORD GetFileDlgFlags( TDclControlPtr pFileDlgProperties )
{
	assert (pFileDlgProperties != NULL);
	DWORD dwFlags = OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;

	if (pFileDlgProperties->GetLongProperty(Prop::FileDlgStyle) == 1) // as open
	{
		if (!pFileDlgProperties->GetBooleanProperty(Prop::ShowReadOnlyCheckBox))
			dwFlags |= OFN_HIDEREADONLY;
		if (pFileDlgProperties->GetBooleanProperty(Prop::AsReadOnly))
			dwFlags |= OFN_READONLY;
		if (pFileDlgProperties->GetBooleanProperty(Prop::MultiSelect))
			dwFlags |= OFN_ALLOWMULTISELECT;
		if (pFileDlgProperties->GetBooleanProperty(Prop::FileMustExist))
			dwFlags |= OFN_FILEMUSTEXIST;
		if (pFileDlgProperties->GetBooleanProperty(Prop::CreationPrompt))
			dwFlags |= OFN_CREATEPROMPT;
	}
	else if (pFileDlgProperties->GetLongProperty(Prop::FileDlgStyle) == 0) // as save
	{
		dwFlags |= OFN_HIDEREADONLY;
		if (pFileDlgProperties->GetBooleanProperty(Prop::OverWritePrompt))
			dwFlags |= OFN_OVERWRITEPROMPT;
	}
	if (pFileDlgProperties->GetBooleanProperty(Prop::ShowHelp))
		dwFlags |= OFN_SHOWHELP;
	if (pFileDlgProperties->GetBooleanProperty(Prop::ExtCanBeDiff))
		dwFlags |= OFN_EXTENSIONDIFFERENT;
	if (pFileDlgProperties->GetBooleanProperty(Prop::PathMustExist))
		dwFlags |= OFN_PATHMUSTEXIST;
	return dwFlags;
}


__if_exists(CFileDialog::m_bVistaStyle)
{
	CNonVistaFileDialog::CNonVistaFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt /*= NULL*/,
		LPCTSTR lpszFileName /*= NULL*/, DWORD dwFlags /*= 0UL*/, LPCTSTR lpszFilter /*= NULL*/,
		CWnd* pParentWnd /*= NULL*/)
	: CFileDialog( bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd, 0UL, FALSE )
	{
	}
}
__if_not_exists(CFileDialog::m_bVistaStyle)
{
	CNonVistaFileDialog::CNonVistaFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt /*= NULL*/,
		LPCTSTR lpszFileName /*= NULL*/, DWORD dwFlags /*= 0UL*/, LPCTSTR lpszFilter /*= NULL*/,
		CWnd* pParentWnd /*= NULL*/)
		: CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
	{
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCustomFileDlg

CCustomFileDlg::CCustomFileDlg( TDclFormPtr pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CNonVistaFileDialog( TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, pParent )
, CArxDialogObject( pSourceForm, this )
, mpParams( pParams? (FileDialogParams*)pParams->lpData : NULL )
, mptInitial( pParams? pParams->position : CPoint(INT_MIN, INT_MIN) )
, msizeInitial( pParams? pParams->size : CSize(0, 0) )
, mMainFileDlg( this, pParent )
, mpFileDlgCtrl( pSourceForm->FindFirstControlOfType(CtlFileExplorer) )
, mnRightBorder( 0 )
, mnBottomBorder( 0 )
{
	SetTemplate(IDD_CUSTOM_FILE_DIALOG, IDD_CUSTOM_FILE_DIALOG);
	OPENFILENAME& ofn = GetOFN();
	TDclControlPtr pProps = pSourceForm->GetControlProperties();
	if (pProps->GetBooleanProperty(Prop::AllowResizing))
		ofn.Flags |= OFN_ENABLESIZING;
	if( pSourceForm )
	{
		if (mpFileDlgCtrl)
		{
			m_bOpenFileDialog = (mpFileDlgCtrl->GetLongProperty(Prop::FileDlgStyle) != 0)? TRUE : FALSE;
			ofn.Flags |= GetFileDlgFlags(mpFileDlgCtrl);
			msTitle = pProps->GetStringProperty(Prop::TitleBarText);
			if( !msTitle.IsEmpty() )
				ofn.lpstrTitle = msTitle.LockBuffer();
		}
		msFilterList = mpFileDlgCtrl->GetStringProperty(Prop::Filter);
		if( mpParams && !mpParams->sFilterList.IsEmpty() )
			msFilterList = mpParams->sFilterList;
		CStringArray rsFilters;
		if( !msFilterList.IsEmpty() )
		{
			int nStart = 0;
			while( nStart >= 0 )
			{
				CString sDesc = msFilterList.Tokenize( _T("|"), nStart );
				if( nStart < 0 )
					break;
				rsFilters.Add( msFilterList.Tokenize( _T("|"), nStart ) );
			}
			msFilterList += _T('\0');
			msFilterList.Replace(_T('|'), _T('\0'));
			ofn.lpstrFilter = msFilterList.LockBuffer();
		}
		if( mpParams )
		{ //if we got params, add them to the design time properties
			//m_bOpenFileDialog = pFileDlgParams->bOpenFileDialog; //always use design time property
			mpParams->dwFlags |= ofn.Flags; //keep the flags we have so far, just add the new ones
			ofn.Flags = mpParams->dwFlags;
			msDefaultExtension = mpParams->sDefaultExtension;
			msDefaultExtension.TrimLeft( _T(".") );
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
			{
				lstrcpyn( ofn.lpstrFile, mpParams->sFilename, cchBuffer );
				if( !rsFilters.IsEmpty() )
				{ //search the filter list for a match
					CString sFilename = mpParams->sFilename;
					sFilename = sFilename.MakeReverse().SpanExcluding( _T("\\/:") );
					sFilename.MakeReverse();
					sFilename.MakeUpper();
					INT_PTR ctFilter = rsFilters.GetCount();
					for( INT_PTR idx = 0; idx < ctFilter; ++idx )
					{
						CString sFilter = rsFilters.GetAt( idx );
						int nStart = 0;
						while( nStart >= 0 )
						{
							CString sPattern = sFilter.Tokenize( _T(";"), nStart );
							if( sPattern.IsEmpty() )
								continue;
							sPattern.MakeUpper();
							sPattern.Replace( _T("."), _T("`.") );
							if( acutWcMatch( sFilename, sPattern ) == RTNORM )
							{
								ofn.nFilterIndex = idx + 1;
								idx = ctFilter;
								break;
							}
						}
					}
				}
			}
			ofn.nMaxFile = cchBuffer;
		}
	}
}


CCustomFileDlg::~CCustomFileDlg()
{
	SetHook( mpControlPane, false );
	theArxWorkspace.ResetLispSymbol( mpFileDlgCtrl->GetVarName() );
	mpFileDlgCtrl->SetControlInstance( NULL );
}

void CCustomFileDlg::CloseDialog(int nStatus)
{
	if( IsClosing() )
		return; //already in the process of closing
	SetClosing();
	GetMainDialog().EndDialog( nStatus );
	GetMainDialog().SendMessage( WM_CLOSE, 0, 0 );
}

INT_PTR CCustomFileDlg::DoModal()
{
	IgnoreSizing();
	INT_PTR nResult = CFileDialog::DoModal();
	if( nResult == IDOK && mpParams )
	{
		mpParams->sFilename = GetPathName();
		mpParams->rsFilenames.RemoveAll();
		POSITION pos = GetStartPosition();
		while( pos )
			mpParams->rsFilenames.Add( GetNextPathName( pos ) );
	}
	delete this;
	return nResult;
}

bool CCustomFileDlg::OnApplyResizable( TPropertyPtr pProp )
{
	GetOFN().Flags |= OFN_ENABLESIZING;
	return true;
}

void CCustomFileDlg::SavePosition()
{
	if( !IsWindow( m_hWnd ) )
		return;
	CRect rcWnd;
	mMainFileDlg.GetWindowRect( &rcWnd );
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpSourceForm->GetKeyPath();
	pApp->WriteProfileInt( sProfileName, _T("Width"), mpTemplate->GetLongProperty( Prop::Width ) );
	pApp->WriteProfileInt( sProfileName, _T("Height"), mpTemplate->GetLongProperty( Prop::Height ) );
	pApp->WriteProfileInt( sProfileName, _T("TopLeftX"), rcWnd.left );
	pApp->WriteProfileInt( sProfileName, _T("TopLeftY"), rcWnd.top );
}

void CCustomFileDlg::ReadPosition(POINT& ptTopLeft, SIZE& size) const
{
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = theWorkspace.GetUserProfilePrefix() + _T("Dialogs\\") + mpSourceForm->GetKeyPath();
	ptTopLeft.x = pApp->GetProfileInt( sProfileName, _T("TopLeftX"), INT_MIN );
	ptTopLeft.y = pApp->GetProfileInt( sProfileName, _T("TopLeftY"), INT_MIN );
	size.cx = pApp->GetProfileInt( sProfileName, _T("Width"), -1 );
	size.cy = pApp->GetProfileInt( sProfileName, _T("Height"), -1 );
}

void CCustomFileDlg::OnInitializationComplete()
{
	CPoint ptSaved( INT_MIN, INT_MIN );
	CSize sizeSaved( -1, -1 );
	ReadPosition(ptSaved, sizeSaved); //get the saved position before it gets overwritten during SetWindowPos()

	CRect rectWindow;
	mMainFileDlg.GetWindowRect( &rectWindow );
	CRect rectClient;
	mMainFileDlg.GetClientRect( &rectClient );
	SetNCWidth( rectWindow.Width() - rectClient.Width() );
	SetNCHeight( rectWindow.Height() - rectClient.Height() );
	mpTemplate->SetLongProperty( Prop::Width, ToDIP( rectClient.Width() ) );
	mpTemplate->SetLongProperty( Prop::Height, ToDIP( rectClient.Height() ) );
	MoveWindow( &rectClient );
	ApplyPropertiesEnum();

	//create the control pane and the design time controls
	UINT nID = 1000;
	GetControlPane()->CreateControls( nID );
	mpFileDlgCtrl->SetControlInstance( this );
	theArxWorkspace.SetLispSymbol( mpFileDlgCtrl->GetVarName(), (const CDclControlTemplate*)mpFileDlgCtrl, odcl::ptrDclControl );
	GetControlPane()->RecalcLayout();

	CRect rcDesktop;
	GetDesktopWindow()->GetClientRect( &rcDesktop );
#if defined(SM_CXVIRTUALSCREEN)
	int nVirtualScreenWidth = ::GetSystemMetrics( SM_CXVIRTUALSCREEN );
	if( nVirtualScreenWidth > 0 ) //We have virtual screen support
	{
		rcDesktop.left = ::GetSystemMetrics( SM_XVIRTUALSCREEN );
		rcDesktop.top = ::GetSystemMetrics( SM_YVIRTUALSCREEN );
		rcDesktop.right = rcDesktop.left + nVirtualScreenWidth;
		rcDesktop.bottom = rcDesktop.top + ::GetSystemMetrics( SM_CYVIRTUALSCREEN );
	}
#endif //SM_CXVIRTUALSCREEN
	CRect rectParent;
	::GetWindowRect( ::GetParent(mMainFileDlg.m_hWnd), &rectParent );
	if( mptInitial.x > INT_MIN )
		rectWindow.MoveToX( FromDIP( mptInitial.x ) );
	else if( ptSaved.x >= rcDesktop.left - 10 && ptSaved.x < rcDesktop.right - 10 )
		rectWindow.MoveToX( ptSaved.x );
	else
		rectWindow.MoveToX( rectParent.left + (rectParent.Width() - rectWindow.Width()) / 2 );
	if( mptInitial.y > INT_MIN )
		rectWindow.MoveToY( FromDIP( mptInitial.y ) );
	else if( ptSaved.y >= rcDesktop.top - 10 && ptSaved.y < rcDesktop.right - 10 )
		rectWindow.MoveToY( ptSaved.y );
	else
		rectWindow.MoveToY( rectParent.top + (rectParent.Height() - rectWindow.Height()) / 2 );
	if( IsResizable() )
	{
		if( msizeInitial.cx > 0 )
			rectWindow.right = rectWindow.left + FromDIP( msizeInitial.cy ) + GetNCWidth();
		else if( sizeSaved.cx > 0 )
			rectWindow.right = rectWindow.left + FromDIP( sizeSaved.cx ) + GetNCWidth();
		if( msizeInitial.cy > 0 )
			rectWindow.bottom = rectWindow.top + FromDIP( msizeInitial.cy ) + GetNCHeight();
		else if( sizeSaved.cy > 0 )
			rectWindow.bottom = rectWindow.top + FromDIP( sizeSaved.cy ) + GetNCHeight();
	}
	if( mMainFileDlg.GetStyle() & WS_CHILD )
		mMainFileDlg.GetParent()->ScreenToClient( &rectWindow );

	GetArxServices()->HandleEvent( Prop::FormEventInitialize, false );
	mMainFileDlg.MoveWindow( &rectWindow, FALSE );
	mpControlPane->CheckDpiChanged();
	IgnoreSizing( false );
}


BEGIN_MESSAGE_MAP(CCustomFileDlg, CFileDialog)
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
	ON_WM_WINDOWPOSCHANGED()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CCustomFileDlg::OnDpiChanged)
END_MESSAGE_MAP()


LRESULT CCustomFileDlg::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

void CCustomFileDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);
	if( ((lpwndpos->flags & (SWP_NOSIZE | SWP_NOMOVE)) != (SWP_NOSIZE | SWP_NOMOVE)) )
	{
		CRect rcDlg = GetEffectiveClientRect();
		int nNewWidth = rcDlg.Width();
		int nNewHeight = rcDlg.Height();
		mpFileDlgCtrl->SetLongProperty( Prop::Width, nNewWidth - mnRightBorder );
		mpFileDlgCtrl->SetLongProperty( Prop::Height, nNewHeight - mnBottomBorder );
	}
}


void CCustomFileDlg::CtrlModifyStyle(int nCtrl)
{
	CWnd *pCtrl = GetParent()->GetDlgItem(nCtrl);
	if( pCtrl )
		pCtrl->ModifyStyle(NULL, WS_CLIPSIBLINGS, 0);
}


BOOL CCustomFileDlg::OnInitDialog()
{
	__super::OnInitDialog();
	mMainFileDlg.SubclassWindow( ::GetParent( m_hWnd ) );

	// here we need to hide specified controls
	if( mpFileDlgCtrl )
	{
		if (!mpFileDlgCtrl->GetBooleanProperty(Prop::ShowOK))
			HideControl(IDOK);
		if (!mpFileDlgCtrl->GetBooleanProperty(Prop::ShowCancel))
			HideControl(IDCANCEL);

		if(IsWindows98orLater())
		{
			//SetControlText(stc3, "");
			CWnd* pScrollBar = FindWindowEx(::GetParent(m_hWnd), NULL, _T("ScrollBar"), NULL);
			if(pScrollBar)
				::ShowWindow(pScrollBar->m_hWnd, SW_HIDE);
		}

		if (!mpFileDlgCtrl->GetBooleanProperty(Prop::ShowTypeComboBox))
			HideControl(cmb1);
		if (!mpFileDlgCtrl->GetBooleanProperty(Prop::ShowNameTextBox))
			HideControl(edt1);

		if (!mpFileDlgCtrl->GetBooleanProperty(Prop::ShowTypeLabel))
			HideControl(stc2);
		if (!mpFileDlgCtrl->GetBooleanProperty(Prop::ShowNameLabel))
			HideControl(stc3);

		CtrlModifyStyle(cmb1);
		CtrlModifyStyle(edt1);

		CtrlModifyStyle(stc2);
		CtrlModifyStyle(stc3);
		CtrlModifyStyle(IDOK);
		CtrlModifyStyle(IDCANCEL);

		//add right and bottom borders for custom controls by calculating what the right and left borders
		//are on the template, then add the same amount of border to this dialog.
		mnRightBorder =
			FromDIP(mpTemplate->GetLongProperty( Prop::Width ) - mpFileDlgCtrl->GetLongProperty( Prop::Width ));
		mnBottomBorder =
			FromDIP(mpTemplate->GetLongProperty( Prop::Height ) - mpFileDlgCtrl->GetLongProperty( Prop::Height ));

		CRect rectWindow;
		mMainFileDlg.GetWindowRect( &rectWindow );
		mMainFileDlg.GetParent()->ScreenToClient( &rectWindow );
		rectWindow.right += mnRightBorder;
		rectWindow.bottom += mnBottomBorder;
		mMainFileDlg.MoveWindow( &rectWindow, TRUE );
		SetHook( mpControlPane, true );

		//CControlPane* pCtrlPane = GetControlPane();
		//CArxDialogControl* pCtrlObj = new CArxFileDialogControl( mpFileDlgCtrl, pCtrlPane, NULL );
		//pCtrlPane->AddControl( pCtrlObj );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

BOOL CCustomFileDlg::OnFileNameOK()
{
	if( __super::OnFileNameOK() )
		return TRUE;
	if( IsClosing() || IsCloseAllowed( false ) )
	{
		SetClosing();
		return FALSE;
	}
	SetClosing( false );
	return TRUE;
}

void CCustomFileDlg::OnFileNameChange()
{
	__super::OnFileNameChange();
	CWnd* pWnd = GetParent()->GetDlgItem(lst2);
	if (pWnd == NULL)
		return;
	if( !mpFileDlgCtrl )
		return;

	CListCtrl* wndLst1 = (CListCtrl*)(pWnd->GetDlgItem(1));

	CString sEventSelChanged = mpFileDlgCtrl->GetStringProperty( Prop::EventSelChanged );
	int nSelCount = wndLst1->GetSelectedCount();
	POSITION pos = wndLst1->GetFirstSelectedItemPosition();
	if( pos == NULL || nSelCount == 0 )
		CArxControlServices::HandleEventGeneric( false, sEventSelChanged, args_NS( -1, _T("") ) );
	else if (nSelCount == 1)
		CArxControlServices::HandleEventGeneric( false, sEventSelChanged, args_NS( nSelCount, GetPathName() ) );
	else
		CArxControlServices::HandleEventGeneric( false, sEventSelChanged, args_NS( nSelCount, _T("") ) );
}

void CCustomFileDlg::OnTypeChange()
{
	__super::OnTypeChange();
	if( !mpFileDlgCtrl )
		return;
	CString sText;
	GetParent()->GetDlgItem(cmb1)->GetWindowText( sText );
	CString sEventTypeChanged = mpFileDlgCtrl->GetStringProperty( Prop::EventTypeChanged );
	CArxControlServices::HandleEventGeneric( false, sEventTypeChanged, args_S( sText ) );
}

BOOL CCustomFileDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	GetArxServices()->HandleEvent( Prop::EventHelp );
	return TRUE;
}

void CCustomFileDlg::OnHelp()
{
	GetArxServices()->HandleEvent( Prop::EventHelp );
}

void CCustomFileDlg::OnFolderChange()
{
	__super::OnFolderChange();
	if( !mpFileDlgCtrl )
		return;
	CString sEventFolderChanged = mpFileDlgCtrl->GetStringProperty( Prop::EventFolderChanged );
	CArxControlServices::HandleEventGeneric( false, sEventFolderChanged, args_S( GetFolderPath() ) );
}

LRESULT CCustomFileDlg::OnGetFileName( WPARAM wParam, LPARAM lParam )
{
	CString sFileName = GetFileName();
	UINT_PTR cchFileName = sFileName.GetLength();
	if( wParam <= cchFileName )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFileName );
	return (LRESULT)cchFileName;
}

LRESULT CCustomFileDlg::OnGetFileTitle( WPARAM wParam, LPARAM lParam )
{
	CString sFileTitle = GetFileTitle();
	UINT_PTR cchFileTitle = sFileTitle.GetLength();
	if( wParam <= cchFileTitle )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFileTitle );
	return (LRESULT)cchFileTitle;
}

LRESULT CCustomFileDlg::OnGetFileExt( WPARAM wParam, LPARAM lParam )
{
	CString sFileExt = GetFileExt();
	UINT_PTR cchFileExt = sFileExt.GetLength();
	if( wParam <= cchFileExt )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFileExt );
	return (LRESULT)cchFileExt;
}

LRESULT CCustomFileDlg::OnGetFilePath( WPARAM wParam, LPARAM lParam )
{
	CString sFilePath = GetPathName();
	UINT_PTR cchFilePath = sFilePath.GetLength();
	if( wParam <= cchFilePath )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFilePath );
	return (LRESULT)cchFilePath;
}

LRESULT CCustomFileDlg::OnGetFolderPath( WPARAM wParam, LPARAM lParam )
{
	CString sFolderPath = GetFolderPath();
	UINT_PTR cchFolderPath = sFolderPath.GetLength();
	if( wParam <= cchFolderPath )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFolderPath );
	return (LRESULT)cchFolderPath;
}

LRESULT CCustomFileDlg::OnGetFolderName( WPARAM wParam, LPARAM lParam )
{
	CString sFolderName = GetFolderPath().MakeReverse().SpanExcluding(_T("\\/:")).MakeReverse();
	UINT_PTR cchFolderName = sFolderName.GetLength();
	if( wParam <= cchFolderName )
		return 0;
	lstrcpy( (LPTSTR)lParam, sFolderName );
	return (LRESULT)cchFolderName;
}

LRESULT CCustomFileDlg::OnGetSelectedFileCount( WPARAM wParam, LPARAM lParam )
{
	UINT cFiles = 0;
	for( POSITION pos = GetStartPosition(); pos; GetNextPathName( pos ) )
		++cFiles;
	return (LRESULT)cFiles;
}

LRESULT CCustomFileDlg::OnGetSelectedFiles( WPARAM wParam, LPARAM lParam )
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

LRESULT CCustomFileDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return __super::WindowProc(message, wParam, lParam);
}
