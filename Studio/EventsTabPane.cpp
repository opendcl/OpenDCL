// EventsTabPane.cpp : implementation file
//

#include "stdafx.h"
#include "EventsTabPane.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "DclControlTemplate.h"
#include "DclFormTemplate.h"
#include "Resource.h"
#include "PropertyObject.h"
#include "ControlBrowser.h"
#include "AxEventDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "OpenDCLDoc.h"
#include "Project.h"
#include "StudioWorkspace.h"
#include "StudioFrame.h"
#include "StdioUnicodeFile.h"
#include "ParseFuncHelp.h"
#include "PropertyNames.h"
#include "ProjectPane.h"

const int nTreeBottom = 150;
const int nLabelBottom = 135;
const int nEditBottom = 115;
const int nPreviewBottom = 75;
const int nDescBottom = 26;


/////////////////////////////////////////////////////////////////////////////
// CEventsTabPane dialog

CEventsTabPane::CEventsTabPane(CWnd* pParent /*=NULL*/)
: CDialog(CEventsTabPane::IDD, pParent)
, mszDlg( 0, 0 )
{
}


void CEventsTabPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISABLED, mStatusMsg);
	DDX_Control(pDX, IDC_EVENTDESC, mEventDesc);
	DDX_Control(pDX, IDC_DEFUNPREVIEW, mDefunPreview);
	DDX_Control(pDX, IDC_DEFUNEDIT, mDefunEdit);
	DDX_Control(pDX, IDC_EVENTSTREE, mEventsList);
}


BEGIN_MESSAGE_MAP(CEventsTabPane, CDialog)
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_EVENTSTREE, OnSelchangeEventstree)
	ON_LBN_DBLCLK(IDC_EVENTSTREE, OnDblClkEventstree)
	ON_CLBN_CHKCHANGE(IDC_EVENTSTREE, OnCheckChanged)
	ON_EN_CHANGE(IDC_DEFUNEDIT, OnChangeDefunedit)
	ON_BN_CLICKED(IDC_ADDCANCEL, OnAddcancel)
	ON_BN_CLICKED(IDC_COPYTOCLIPBOARD, OnCopytoclipboard)
	ON_BN_CLICKED(IDC_ADDTOLISP, OnAddtolisp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventsTabPane message handlers

BOOL CEventsTabPane::OnInitDialog() 
{
	CDialog::OnInitDialog();

	mStatusMsg.ShowWindow( SW_HIDE );
	if (!AfxGetApp()->GetProfileInt(theWorkspace.GetAppKey(), _T("EventsCopyToClipboard"), TRUE))
		GetDlgItem(IDC_COPYTOCLIPBOARD)->ShowWindow( SW_HIDE );
	if (!AfxGetApp()->GetProfileInt(theWorkspace.GetAppKey(), _T("EventsWriteToLispFile"), FALSE))
		GetDlgItem(IDC_ADDTOLISP)->ShowWindow( SW_HIDE );
	ClearEvents();

	CRect rectDlg;
	GetWindowRect( &rectDlg );
	mszDlg.SetSize( rectDlg.Width(), rectDlg.Height() );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CEventsTabPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if( mszDlg.cx == 0 && mszDlg.cy == 0 )
		return; //no-op

	CSize vecOffset( cx - mszDlg.cx, cy - mszDlg.cy );
	mszDlg.SetSize( cx, cy );
	if( vecOffset.cx == 0 && vecOffset.cy == 0 )
		return; //no-op

	CRect rcLabel;
	GetDlgItem(IDC_LABEL)->GetWindowRect( &rcLabel );

	CRect rcEventsTree;
	GetDlgItem(IDC_EVENTSTREE)->GetWindowRect( &rcEventsTree );
	static int nBottomOffset = rcLabel.top - rcEventsTree.bottom;
	rcEventsTree.bottom = rcLabel.top + vecOffset.cy - nBottomOffset;
	rcEventsTree.right += vecOffset.cx;
	ScreenToClient( &rcEventsTree );
	rcEventsTree.top = 1;
	GetDlgItem(IDC_EVENTSTREE)->MoveWindow(rcEventsTree, TRUE);
	GetDlgItem(IDC_DISABLED)->MoveWindow(rcEventsTree, TRUE);

	rcLabel.top += vecOffset.cy;
	rcLabel.bottom += vecOffset.cy;
	rcLabel.right += vecOffset.cx;
	ScreenToClient( &rcLabel );
	GetDlgItem(IDC_LABEL)->MoveWindow(rcLabel, TRUE);

	CRect rcDefunEdit;
	GetDlgItem(IDC_DEFUNEDIT)->GetWindowRect( &rcDefunEdit );
	rcDefunEdit.top += vecOffset.cy;
	rcDefunEdit.bottom += vecOffset.cy;
	rcDefunEdit.right += vecOffset.cx;
	ScreenToClient( &rcDefunEdit );
	GetDlgItem(IDC_DEFUNEDIT)->MoveWindow(rcDefunEdit, TRUE);

	CRect rcDefunPreview;
	GetDlgItem(IDC_DEFUNPREVIEW)->GetWindowRect( &rcDefunPreview );
	rcDefunPreview.top += vecOffset.cy;
	rcDefunPreview.bottom += vecOffset.cy;
	rcDefunPreview.right += vecOffset.cx;
	ScreenToClient( &rcDefunPreview );
	GetDlgItem(IDC_DEFUNPREVIEW)->MoveWindow(rcDefunPreview, TRUE);	

	CRect rcEventDesc;
	GetDlgItem(IDC_EVENTDESC)->GetWindowRect( &rcEventDesc );
	rcEventDesc.top += vecOffset.cy;
	rcEventDesc.bottom += vecOffset.cy;
	rcEventDesc.right += vecOffset.cx;
	ScreenToClient( &rcEventDesc );
	GetDlgItem(IDC_EVENTDESC)->MoveWindow(rcEventDesc, TRUE);

	CRect rcAddCancel (0,cy-nDescBottom+3,70,cy-1);		
	GetDlgItem(IDC_ADDCANCEL)->GetWindowRect( &rcAddCancel );
	rcAddCancel.top += vecOffset.cy;
	rcAddCancel.bottom += vecOffset.cy;
	ScreenToClient( &rcAddCancel );
	GetDlgItem(IDC_ADDCANCEL)->MoveWindow(rcAddCancel, TRUE);
	
	CRect rcCopyToClipboard;
	GetDlgItem(IDC_COPYTOCLIPBOARD)->GetWindowRect( &rcCopyToClipboard );
	rcCopyToClipboard.top += vecOffset.cy;
	rcCopyToClipboard.bottom += vecOffset.cy;
	ScreenToClient( &rcCopyToClipboard );
	GetDlgItem(IDC_COPYTOCLIPBOARD)->MoveWindow(rcCopyToClipboard, TRUE);
	
	CRect rcAddToLisp;
	GetDlgItem(IDC_ADDTOLISP)->GetWindowRect( &rcAddToLisp );
	rcAddToLisp.top += vecOffset.cy;
	rcAddToLisp.bottom += vecOffset.cy;
	ScreenToClient( &rcAddToLisp );
	GetDlgItem(IDC_ADDTOLISP)->MoveWindow(rcAddToLisp, TRUE);
}

void CEventsTabPane::UpdateEvents( TDclControlPtr pControl )
{
	mpDclControl = pControl;
	mEventsList.ResetContent();
	mStatusMsg.ShowWindow( SW_HIDE );

	if( !mpDclControl )
		return;

	AddEvents();
	if( mEventsList.GetCount() > 0 )
	{
		mEventsList.EnableWindow( TRUE );
		if( pControl && pControl->GetOwnerForm()->IsModeless() )
			GetDlgItem( IDC_ADDCANCEL )->ShowWindow( SW_SHOW );
		mEventsList.EnableWindow( TRUE );
		GetDlgItem( IDC_LABEL )->EnableWindow( TRUE );
		mDefunEdit.EnableWindow( TRUE );
	}
	mDefunEdit.SetWindowText( NULL );
}

void CEventsTabPane::AddEvents() 
{
	if( !mpDclControl )
		return;

	size_t idxProp = 0;
	const TPropertyList& Props = mpDclControl->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter, ++idxProp )
	{
		CString sEventName;
		switch( (*iter)->GetType() )
		{
		case PropEvent:
		case PropActiveXEvent:
			sEventName = (*iter)->GetApiName();
			break;
		default:
			continue;
		}
		if( !sEventName.IsEmpty() )
		{
			int idxEvent = mEventsList.AddString( sEventName );
			mEventsList.SetItemData( idxEvent, idxProp );
			if( !(*iter)->GetStringValue().IsEmpty() ) // if the event has been set...
				mEventsList.SetCheck( idxEvent, BST_CHECKED ); // set the event tree item as checked.
		}
	}
}

void CEventsTabPane::OnChangeDefunedit()
{
	if( !mpDclControl )
		return;

	int nCurSel = mEventsList.GetCurSel();
	if( nCurSel < 0 )
		return;

	CString sDefunEditText;
	mDefunEdit.GetWindowText( sDefunEditText ); // get the new defun name from the edit box
	bool bChecked = (mEventsList.GetCheck( nCurSel ) == BST_CHECKED);
	if( bChecked )
		SetEvent( mEventsList.GetItemData( nCurSel ), sDefunEditText );
	SetDefunPreview();
	if( sDefunEditText.Left( 4 ).CompareNoCase( _T("^C^C") ) == 0 )
		GetDlgItem(IDC_ADDCANCEL)->EnableWindow( FALSE );
	else
		GetDlgItem(IDC_ADDCANCEL)->EnableWindow( TRUE );
}

void CEventsTabPane::SetDefunPreview()
{
	if( !mpDclControl )
		return;

	CString sPreview;
	CString sDesc;

	int nCurSel = mEventsList.GetCurSel();
	if( nCurSel >= 0 )
	{
		size_t idxProp = mEventsList.GetItemData( nCurSel );

		CString sEventDefun;
		mDefunEdit.GetWindowText( sEventDefun );
		if( sEventDefun.IsEmpty() )
			sEventDefun = GetEvent( idxProp );
		if( sEventDefun.Left( 4 ).CompareNoCase( _T("^C^C") ) == 0 )
			sEventDefun = sEventDefun.Mid( 4 );

		TPropertyList::iterator iterProp = mpDclControl->GetPropertyList().begin();
		while( idxProp-- > 0 )
			++iterProp;

		ParseHelpInfo( *iterProp, sPreview, sDesc );

		sPreview.Replace( _T("<FUNCTION-NAME>"), sEventDefun );

		CString sFunctionBody;
		if( (*iterProp)->GetID() == Prop::DragnDropFromOther )
			sFunctionBody += _T("  (setq ssDragnDropSelectionSet (ssget \"_P\")) ; Get selected entities\r\n");
		//add code to remind the programmer that the event handler isn't finished
		CString sToDoMsgBox;
		sToDoMsgBox.Format( theWorkspace.LoadResourceString( IDS_MSG_TODOLISPFUNC ), (LPCTSTR)sEventDefun );
		sFunctionBody += _T("  ");
		sFunctionBody += sToDoMsgBox;
		sFunctionBody += _T("\r\n");
		sPreview.Replace( _T("<FUNCTION-BODY>"), sFunctionBody );
	}
	mDefunPreview.SetWindowText( sPreview + _T("\r\n") );
	mEventDesc.SetWindowText( sDesc );
	return;
}

void CEventsTabPane::ClearEvents( LPCTSTR pszErrMsg /*= NULL*/ )
{
	mpDclControl = NULL;
	mEventsList.EnableWindow( FALSE );
	mEventsList.ResetContent();
	GetDlgItem( IDC_LABEL )->EnableWindow( FALSE );
	mDefunEdit.EnableWindow( FALSE );
	GetDlgItem( IDC_ADDCANCEL )->EnableWindow( FALSE );
	GetDlgItem( IDC_COPYTOCLIPBOARD )->EnableWindow( FALSE );
	GetDlgItem( IDC_ADDTOLISP )->EnableWindow( FALSE );
	GetDlgItem( IDC_ADDCANCEL )->ShowWindow( SW_HIDE );

	mEventDesc.SetWindowText( NULL );
	mDefunEdit.SetWindowText( NULL );
	mDefunPreview.SetWindowText( NULL );

	if( pszErrMsg )
		mStatusMsg.SetWindowText( pszErrMsg );
	mStatusMsg.ShowWindow( pszErrMsg? SW_SHOW : SW_HIDE );
}

BOOL CEventsTabPane::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST )
	{
		if( pMsg->message == WM_KEYDOWN )
		{
			switch( pMsg->wParam )
			{
			case VK_ESCAPE:
			case VK_RETURN:
				return TRUE; //prevent dialog from closing
			case VK_CONTROL:
			case VK_DELETE:
			case 'C':
			case 'V':
				return __super::PreTranslateMessage(pMsg); //allow dialog to process these keystrokes
			}
		}
		return AfxGetMainWnd()->PreTranslateMessage( pMsg );
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CEventsTabPane::OnAddcancel() 
{
	if( !mpDclControl )
		return;
	
	int nCurSel = mEventsList.GetCurSel();
	if( nCurSel >= 0 )
	{
		CString sEventCommand;
		mDefunEdit.GetWindowText( sEventCommand );

		// add the ^C^C to the defun name
		sEventCommand = CString("^C^C") + sEventCommand;
		mDefunEdit.SetWindowText( sEventCommand );
		mEventsList.SetCheck( nCurSel, BST_CHECKED ); // set the event tree item as checked.
		SetEvent( mEventsList.GetItemData( nCurSel ), sEventCommand );
		OnChangeDefunedit();
	}
}

void CEventsTabPane::OnCopytoclipboard() 
{
	if( OpenClipboard() )
	{
		EmptyClipboard();
		CString sourceT;
		mDefunPreview.GetWindowText( sourceT );
		CStringA source( sourceT );
		HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength() + 1);
		CHAR* buffer = (CHAR*)GlobalLock(clipbuffer);
		lstrcpynA(buffer, source, source.GetLength() + 1);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
}

void CEventsTabPane::OnAddtolisp() 
{

	if( !mpDclControl )
		return;
	int nCurSel = mEventsList.GetCurSel();
	if( nCurSel < 0 )
		return;
	
	mEventsList.SetCheck( nCurSel, BST_CHECKED ); // set the event tree item as checked.
	size_t idxProp = mEventsList.GetItemData( nCurSel );

	CString sDefunPreview;
	mDefunPreview.GetWindowText( sDefunPreview );

	CString sDefunEditText;
	mDefunEdit.GetWindowText( sDefunEditText );

	// if the file name has not been set, prompt for it
	TProjectPtr pProject = mpDclControl->GetOwnerProject();
	CString sLispFileName = pProject->GetLispFileName();
	if( sLispFileName.IsEmpty() )
	{
		CFileDialog Dlg( FALSE,
										 _T(".lsp"), 
										 sLispFileName, 
										 OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
										 theWorkspace.LoadResourceString( IDS_FILTERAUTOLISPFILE ),
										 this );
		CString sTitle = theWorkspace.LoadResourceString( IDS_SELECTPROJECTLISPFILE );
		Dlg.m_pOFN->lpstrTitle = sTitle;
		if( Dlg.DoModal() != IDOK )
			return; //user cancelled
		sLispFileName = Dlg.GetPathName();
		if( sLispFileName.IsEmpty() )
			return;
		pProject->SetLispFileName( sLispFileName );
		theStudioWorkspace.GetProjectPane()->SetAutoLispFilename( sLispFileName );
		CDocument* pDoc = theStudioWorkspace.GetActiveDocument();
		if( pDoc )
			pDoc->SetModifiedFlag();
	}

	SetEvent( idxProp, sDefunEditText );

	try
	{
		CStdioUnicodeFile File( sLispFileName,
														CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareDenyWrite | CFile::typeText | CFile::osSequentialScan );
		CString sContent;
		CString sLine;
		bool bInserted = false;
		while( File.ReadString( sLine ) )
		{
			if( !bInserted && sLine.Find( _T(";|«Visual LISP© Format Options»") ) >= 0 )
			{
				sContent += _T("\r\n");
				sContent += sDefunPreview;
				sContent += _T("\r\n");
				bInserted = true;
			}
			sContent += sLine;
			sContent += _T("\r\n");
			if( !bInserted && sLine.Find( _T(";|«OpenDCL Event Handlers»") ) >= 0 )
			{
				sContent += _T("\r\n");
				sContent += sDefunPreview;
				sContent += _T("\r\n");
				bInserted = true;
			}
		}
		if( !bInserted )
		{
			sContent += _T("\r\n");
			sContent += sDefunPreview;
			sContent += _T("\r\n");
		}
		File.SeekToBegin();
		File.WriteString( sContent );
		File.SetLength( File.GetPosition() );
	}
	catch( CFileException* e )
	{
		e->ReportError();
		e->Delete();
	}

	//try
	//{
	//	// here we are going to write out to the lisp file.
	//	CStdioUnicodeFile fout( mpDclControl->GetOwnerProject()->GetLispFileName(),
	//									 CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite );
	//	fout.SeekToEnd();
	//	fout.WriteString( _T("\r\n") );
	//	fout.WriteString( sDefunPreview );
	//	fout.WriteString( _T("\r\n") );
	//	fout.Close();
	//}
	//catch( CException* e )
	//{
	//	e->ReportError();
	//	e->Delete();
	//}

	theWorkspace.DisplayAlert( IDS_FUNCADDED );
}

void CEventsTabPane::OnDblClkEventstree() 
{
	if( !mpDclControl )
		return;
	
	int nCurSel = mEventsList.GetCurSel();
	if( nCurSel < 0 )
		return;

	CString sItemText;
	mEventsList.GetText( nCurSel, sItemText );   		
	if( sItemText.IsEmpty() )
		return;

	TPropertyPtr pProp = mpDclControl->FindPropertyObject( sItemText );
	if( !pProp )
		return;

	CControlBrowser ControlBrowserDlg( pProp );
	ControlBrowserDlg.DoModal();
}

void CEventsTabPane::OnSelchangeEventstree() 
{
	if( !mpDclControl )
		return;
	
	int nCurSel = mEventsList.GetCurSel();
	if( nCurSel < 0 )
		return;

	CString sItemText;
	mEventsList.GetText( nCurSel, sItemText );   		
	if( sItemText.IsEmpty() )
		return;

	bool bChecked = (mEventsList.GetCheck( nCurSel ) == BST_CHECKED);
	
	// update the defun edit box.
	mDefunEdit.SetWindowText( GetEvent( mEventsList.GetItemData( nCurSel ) ) );
	GetDlgItem(IDC_COPYTOCLIPBOARD)->EnableWindow( bChecked? TRUE : FALSE );
	GetDlgItem(IDC_ADDTOLISP)->EnableWindow( bChecked? TRUE : FALSE );
}

void CEventsTabPane::OnCheckChanged()
{
	int nCurSel = mEventsList.GetCurSel();
	size_t idxProp = mEventsList.GetItemData( nCurSel );
	CString sEventDefun;
	if( mEventsList.GetCheck( nCurSel ) == BST_CHECKED )
		sEventDefun = GetEvent( idxProp );
	SetEvent( idxProp, sEventDefun );
}

void CEventsTabPane::SetEvent( size_t idxProp, LPCTSTR pszEventDefun )
{
	if( idxProp >= mpDclControl->GetPropertyList().size() )
		return;
	TPropertyList::iterator iterProp = mpDclControl->GetPropertyList().begin();
	while( idxProp-- > 0 )
		++iterProp;
	(*iterProp)->SetStringValue( pszEventDefun );
}

CString CEventsTabPane::GetEvent( size_t idxProp )
{
	if( idxProp >= mpDclControl->GetPropertyList().size() )
		return NULL;
	TPropertyList::iterator iterProp = mpDclControl->GetPropertyList().begin();
	while( idxProp-- > 0 )
		++iterProp;
	CString sEventName = (*iterProp)->GetStringValue();
	if( sEventName.IsEmpty() )
		sEventName.Format( _T("c:%s#On%s"), mpDclControl->GetVarName(), (*iterProp)->GetName() );
	return sEventName;
}
