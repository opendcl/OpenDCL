// EventsTabPane.cpp : implementation file
//

#include "stdafx.h"
#include "EventsTabPane.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "DclControlObject.h"
#include "DclFormObject.h"
#include "Resource.h"
#include "PropertyObject.h"
#include "AxEventDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "OpenDCLDoc.h"
#include "Project.h"
#include "StudioWorkspace.h"
#include "StudioFrame.h"
#include "LoadArgs.h"
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
	m_bInitialized = false;
	m_pControl = NULL;
}


void CEventsTabPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVENTDESC, m_EventDesc);
	DDX_Control(pDX, IDC_DEFUNPREVIEW, m_DefunPreview);
	DDX_Control(pDX, IDC_DEFUNEDIT, m_DefunEdit);
	DDX_Control(pDX, IDC_EVENTSTREE, m_EventsTree);
}


BEGIN_MESSAGE_MAP(CEventsTabPane, CDialog)
	ON_WM_SIZE()
	ON_LBN_SELCHANGE(IDC_EVENTSTREE, OnSelchangeEventstree)
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
	m_bInitialized = true;

	if (!AfxGetApp()->GetProfileInt(theWorkspace.GetAppKey(), _T("EventsCopyToClipboard"), TRUE))
		GetDlgItem(IDC_COPYTOCLIPBOARD)->ShowWindow(SW_HIDE);
	if (!AfxGetApp()->GetProfileInt(theWorkspace.GetAppKey(), _T("EventsWriteToLispFile"), FALSE))
		GetDlgItem(IDC_ADDTOLISP)->ShowWindow(SW_HIDE);

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

void CEventsTabPane::UpdateEvents(TDclControlPtr pControl)
{
	m_pControl = pControl;
	
	if (pControl && pControl->GetOwnerForm()->GetType() == FrmDockableDlg)
		GetDlgItem(IDC_ADDCANCEL)->ShowWindow(SW_SHOW);
	else
		GetDlgItem(IDC_ADDCANCEL)->ShowWindow(SW_HIDE);
	m_EventsTree.ResetContent();
	
	if (m_pControl == NULL)
		return;
	
	TryToAddEvent(Prop::EventFolderChanged);
	TryToAddEvent(Prop::EventOnHelp);
	TryToAddEvent(Prop::EventOnTypeChange);
	TryToAddEvent(Prop::EventBeginLabelEdit);
	TryToAddEvent(Prop::EventBtnClicked);
	TryToAddEvent(Prop::EventSplitterMoved);
	TryToAddEvent(Prop::EventChanged);
	TryToAddEvent(Prop::EventClicked);
	TryToAddEvent(Prop::EventColumnClick);
	TryToAddEvent(Prop::EventDblClicked);
	TryToAddEvent(Prop::EventDeleteItem);
	TryToAddEvent(Prop::EventDropDown);
	TryToAddEvent(Prop::EventEditChanged);
	TryToAddEvent(Prop::EventEndLabelEdit);
	TryToAddEvent(Prop::EventGetDayState);
	TryToAddEvent(Prop::EventItemExpanded);
	TryToAddEvent(Prop::EventItemExpanding);
	TryToAddEvent(Prop::EventMouseDown);
	TryToAddEvent(Prop::EventMouseUp);
	TryToAddEvent(Prop::EventMouseMove);
	TryToAddEvent(Prop::EventMouseEntered);
	TryToAddEvent(Prop::EventMouseMovedOff);
	TryToAddEvent(Prop::EventMouseDblClick);
	TryToAddEvent(Prop::EventMouseWheel);
	TryToAddEvent(Prop::EventNavigateComplete);
	TryToAddEvent(Prop::EventOutOfMemory);
	TryToAddEvent(Prop::EventKeyDown);
	TryToAddEvent(Prop::EventKeyUp);
	TryToAddEvent(Prop::EventKillFocus);
	TryToAddEvent(Prop::EventMaxText);
	TryToAddEvent(Prop::OnLMouseEvent);
	TryToAddEvent(Prop::OnMMouseEvent);
	TryToAddEvent(Prop::EventPaint);
	TryToAddEvent(Prop::OnRMouseEvent);
	TryToAddEvent(Prop::EventReturn);
	TryToAddEvent(Prop::EventRClick);
	TryToAddEvent(Prop::EventRDblClick);
	TryToAddEvent(Prop::EventReturnPressed);
	TryToAddEvent(Prop::EventSelect);
	TryToAddEvent(Prop::EventSelChanged);
	TryToAddEvent(Prop::EventSelChanging);
	TryToAddEvent(Prop::EventSetFocus);
	TryToAddEvent(Prop::EventUpdate);
	TryToAddEvent(Prop::EventScroll);
	TryToAddEvent(Prop::EventScrolled);
	TryToAddEvent(Prop::EventReleasedCapture);
	TryToAddEvent(Prop::DragnDropBegin);
	TryToAddEvent(Prop::DragnDropToAutoCAD);
	TryToAddEvent(Prop::DragnDropFromControl);
	TryToAddEvent(Prop::DragnDropFromAutoCAD);
	TryToAddEvent(Prop::FormEventCancelClose);
	TryToAddEvent(Prop::FormEventClose);
  TryToAddEvent(Prop::FormEventOnOk);
	TryToAddEvent(Prop::FormEventOnCancel);
	TryToAddEvent(Prop::FormEventInitialize);
	TryToAddEvent(Prop::FormEventSize);
	TryToAddEvent(Prop::FormEventShow);
	TryToAddEvent(Prop::DocEventActivated);
	TryToAddEvent(Prop::DocEventEnteringNoDocState);
	TryToAddEvent(Prop::CfgEventCancel);
	TryToAddEvent(Prop::CfgEventOK);
	TryToAddEvent(Prop::CfgEventHelp);
	TryToAddEvent(Prop::CfgEventApply);
	
	// call the method to add any ActiveX events.
	AddAnyActiveXEvents();

	if (IsWindowVisible() && m_EventsTree.GetCount() > 0)
		m_EventsTree.SetFocus();
	
	m_DefunEdit.SetWindowText(NULL);
}


void CEventsTabPane::AddAnyActiveXEvents() 
{
	if( m_pControl == NULL )
		return;

	//The ActiveX event item data is the property item index because a control can have multiple 
	//ActiveX 'Event' properties, so the index must be used to determine which property object 
	//the event is associated with.
	size_t idxProp = 0;
	const TPropertyList& Props = m_pControl->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter, ++idxProp )
	{
		if( (*iter)->GetType() == PropActiveXEvent )
		{
			const AxInterfaceDescriptor* pDescriptor = (*iter)->GetConstAxInterfaceDescriptorPtr();
			CString sName = pDescriptor->GetEvent()->GetName();
			if( !sName.IsEmpty() )
			{
				int idxEvent = m_EventsTree.AddString( sName );
				m_EventsTree.SetItemData( idxEvent, idxProp );
				if( !(*iter)->GetName().IsEmpty() ) // if the event has been set...
					m_EventsTree.SetCheck( idxEvent, BST_CHECKED ); // set the event tree item as checked.
			}
		}
	}
}

void CEventsTabPane::TryToAddEvent( Prop::Id nEventId ) 
{
	if( m_pControl == NULL )
		return;

	TPropertyPtr pProp = m_pControl->GetPropertyObject( nEventId );
	if( pProp )
	{ //the event exists, so add it to the list
		int idxEvent = m_EventsTree.AddString( GetPropertyName( nEventId ) );
		m_EventsTree.SetItemData( idxEvent, nEventId );
		if( !pProp->GetStringValue().IsEmpty() )
			m_EventsTree.SetCheck( idxEvent, TRUE ); //if the event has been set, set the event tree item as checked.
	} 
}

void CEventsTabPane::OnChangeDefunedit() 
{
	if (m_pControl == NULL)
		return;

	int hItem = m_EventsTree.GetCurSel();
	if (hItem != -1)
	{
		CString sDefunEditText;
		if (m_EventsTree.GetCheck(hItem) == BST_CHECKED)
			m_DefunEdit.GetWindowText(sDefunEditText); // get the new defun name from the edit box
		SetEvent((Prop::Id)m_EventsTree.GetItemData(hItem), sDefunEditText); // set the property to the new defun name
		SetDefunPreview();
	}
}

void CEventsTabPane::SetDefunPreview()
{
	if( !m_pControl )
		return;

	CString sPreview;

	// get the current event selection
	int hItem = m_EventsTree.GetCurSel();
	if( hItem != -1 )
	{
		// get the event id from the curent selection
		Prop::Id nEventId = (Prop::Id)m_EventsTree.GetItemData( hItem );

		// get the defun name
		CString sEventDefun;
		m_DefunEdit.GetWindowText( sEventDefun );
		if( sEventDefun.IsEmpty() )
			sEventDefun = GetEvent( nEventId );

		// if the ^C^C has been added, remove it for the defun preview
		if( sEventDefun.Left(4).CompareNoCase( _T("^c^c") ) == 0 )
			sEventDefun = sEventDefun.Mid( 4 );
		
		// get the arguments
		CString sArgs = GetDefunArguments();
		sArgs += _T(" /");

		CString sDefunBody;
		if (nEventId == Prop::DragnDropFromAutoCAD && m_pControl->GetOwnerForm()->GetType() != FrmFileDlg)
			sDefunBody = _T("     (setq ssDragnDropSelectionSet (ssget \"P\"))"); //get the 'Previous' selection set
		else
		{
			// add the default message box to show the programmer the event has not been updated
			CString sToDoMsgBox;
			sToDoMsgBox.Format( theWorkspace.LoadResourceString( IDS_MSG_TODOLISPFUNC ), (LPCTSTR)sEventDefun );
			sDefunBody.Format( _T("     %s"), (LPCTSTR)sToDoMsgBox );
		}

		sPreview.Format( _T("(defun %s (%s)\r\n%s\r\n)\r\n"), (LPCTSTR)sEventDefun, (LPCTSTR)sArgs, (LPCTSTR)sDefunBody );
	}

	m_DefunPreview.SetWindowText( sPreview ); // set the preview control's text
}

CString CEventsTabPane::GetDefunArguments()
{
	CString sArgs;
	CString strDesc;
	
	// get the event id from the curent selection
	UINT_PTR nEventId = m_EventsTree.GetItemData(m_EventsTree.GetCurSel());

	if (m_pControl->GetType() != CtlActiveX)
		LoadArgsNDesc((Prop::Id)nEventId, m_pControl, sArgs, strDesc);
	else
	{
		TPropertyList::const_iterator iter = m_pControl->GetPropertyList().begin();
		while( nEventId-- > 0 ) ++iter;
		strDesc = (*iter)->GetConstAxInterfaceDescriptorPtr()->GetEvent()->GetDesc();
		size_t ctCallingArgs = (*iter)->GetConstAxInterfaceDescriptorPtr()->GetEvent()->GetArgs().size();
		for (UINT_PTR i = 0; i < ctCallingArgs; i++)
		{
			sArgs += (*iter)->GetConstAxInterfaceDescriptorPtr()->GetEvent()->GetArgs().at( i ).name;
			if (i < ctCallingArgs - 1)
				sArgs += _T(" ");
		}
	}

	// set the description
	m_EventDesc.SetWindowText(strDesc);
		
	return sArgs;
}

void CEventsTabPane::ClearEvents()
{
	GetDlgItem(IDC_ADDCANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_COPYTOCLIPBOARD)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADDTOLISP)->EnableWindow(FALSE);
	if (m_pControl == NULL)
		return;

	m_EventDesc.SetWindowText(CString());
	m_EventsTree.ResetContent();
	m_DefunEdit.SetWindowText(CString());
	m_DefunPreview.SetWindowText(CString());
	m_pControl = NULL;
}

BOOL CEventsTabPane::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST )
	{
		if( pMsg->message == WM_KEYDOWN )
		{
			if( pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN )
				return TRUE; //prevent dialog from closing
			if( pMsg->wParam == VK_CONTROL || pMsg->wParam == 'C' )
				return __super::PreTranslateMessage(pMsg); //allow Ctrl-C to copy to clipboard
		}
		return AfxGetMainWnd()->PreTranslateMessage( pMsg );
	}
	return CWnd::PreTranslateMessage(pMsg);
}

void CEventsTabPane::OnAddcancel() 
{
	if (m_pControl == NULL)
		return;
	
	CString sDefunEditText;
	
	// get the current event selection
	int hItem = m_EventsTree.GetCurSel();
	
	if (hItem != -1)
	{
		// get the new defun name from the edit box
		m_DefunEdit.GetWindowText(sDefunEditText);

		// get the event id from the curent selection
		Prop::Id nEventId = (Prop::Id)m_EventsTree.GetItemData(hItem);

		// add the ^C^C to the defun name
		sDefunEditText = CString("^c^c") + sDefunEditText;

		// we need to know if the event has been checked
		BOOL bChecked = m_EventsTree.GetCheck(hItem);

		// if the event is checked, we can update the property object
		if (bChecked == TRUE)
			// set the property to the new defun name
			SetEvent(nEventId, sDefunEditText);
	}
}

void CEventsTabPane::OnCopytoclipboard() 
{
	CString sourceT;
	m_DefunPreview.GetWindowText(sourceT);
	CStringA source( sourceT );

	//put your text in source
	if(OpenClipboard())
	{
		EmptyClipboard();
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
	CString sDefunPreview;
	CString sDefunEditText;

	if (m_pControl == NULL)
		return;

	// get the current event selection
	Prop::Id hItem = (Prop::Id)m_EventsTree.GetCurSel();
	
	if (hItem != -1)
	{
		m_EventsTree.SetCheck(hItem, TRUE); // set the event tree item as checked.

		// get the event id from the curent selection
		Prop::Id nEventId = (Prop::Id)m_EventsTree.GetItemData(hItem);

		// get the new defun name from the edit box
		m_DefunPreview.GetWindowText(sDefunPreview);

		// lets get the defun it self
		m_DefunEdit.GetWindowText(sDefunEditText);

		// if the file name has not been set, prompt for it
		TProjectPtr pProject = m_pControl->GetOwnerProject();
		CString sLispFileName = pProject->GetLispFileName();
		if (sLispFileName.IsEmpty())
		{
			CFileDialog Dlg( FALSE,
											 _T(".lsp"), 
											 sLispFileName, 
											 OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
											 theWorkspace.LoadResourceString(IDS_FILTERAUTOLISPFILE),
											 CWnd::GetActiveWindow() );
			CString sTitle = theWorkspace.LoadResourceString(IDS_SELECTPROJECTLISPFILE);
			Dlg.m_pOFN->lpstrTitle = sTitle.LockBuffer();
			if( Dlg.DoModal() != IDOK )
				return; //user cancelled
			sLispFileName = Dlg.GetPathName();
			if( sLispFileName.IsEmpty() )
				return;
			pProject->SetLispFileName( sLispFileName );
			theStudioWorkspace.GetProjectPane()->SetAutoLispFilename(sLispFileName);
			CDocument* pDoc = theStudioWorkspace.GetActiveDocument();
			if( pDoc )
				pDoc->SetModifiedFlag();
		}

		// set the property to the new defun name
		SetEvent(nEventId, sDefunEditText);

		// here we are going to write out to the lisp file.
		CStdioFile fout(m_pControl->GetOwnerProject()->GetLispFileName(),
										CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite);
		
		fout.SeekToEnd();
		fout.WriteString(_T("\r\n"));
		fout.WriteString(sDefunPreview);
		fout.WriteString(_T("\r\n"));
		
		fout.Close();

		MessageBox(theWorkspace.LoadResourceString(IDS_FUNCADDED), theWorkspace.GetAppKey(), MB_OK);
	}
}

void CEventsTabPane::OnSelchangeEventstree() 
{
	GetDlgItem(IDC_ADDCANCEL)->EnableWindow(FALSE);
	if (m_pControl == NULL)
		return;
	
	int hItem = m_EventsTree.GetCurSel();
	if (hItem == -1)
		return;

	// get the selected item's text
	CString sItemText;
	m_EventsTree.GetText(hItem, sItemText);   		
	if (sItemText.IsEmpty())
		return;

	BOOL bChecked = (m_EventsTree.GetCheck(hItem) == BST_CHECKED);
	
	// update the defun edit box.
	m_DefunEdit.SetWindowText(GetEvent((Prop::Id)m_EventsTree.GetItemData(m_EventsTree.GetCurSel())));
	GetDlgItem(IDC_ADDCANCEL)->EnableWindow(bChecked);
	GetDlgItem(IDC_COPYTOCLIPBOARD)->EnableWindow(bChecked);
	GetDlgItem(IDC_ADDTOLISP)->EnableWindow(bChecked);
}

void CEventsTabPane::OnCheckChanged()
{
	Prop::Id nEventId = (Prop::Id)m_EventsTree.GetItemData(m_EventsTree.GetCurSel());
	CString sEventDefun;
	if (m_EventsTree.GetCheck(m_EventsTree.GetCurSel()) == BST_CHECKED)
		sEventDefun = GetEvent(nEventId);
	SetEvent(nEventId, sEventDefun);
}

void CEventsTabPane::SetEvent( Prop::Id nEventId, CString sEventDefun )
{
	if (m_pControl->GetType() == CtlActiveX)
	{
		//if it's an ActiveX event, then nEventId is actually an index into the property list
		TPropertyList::iterator iterProp = m_pControl->GetPropertyList().begin();
		size_t idxProp = nEventId;
		while( idxProp-- > 0 )
			++iterProp;
		(*iterProp)->SetStringValue( sEventDefun );
	}
	else
		m_pControl->SetStringProperty( nEventId, sEventDefun ); 
}

CString CEventsTabPane::GetEvent( Prop::Id nEventId )
{
	CString sEventSymbol;
	if (m_pControl->GetType() != CtlActiveX)
		sEventSymbol = m_pControl->GetStringProperty(nEventId); 
	if( sEventSymbol.IsEmpty() )
	{
		CString sEventName = m_pControl->GetVarName();
		sEventSymbol.Format( _T("c:%s_On%s"), (LPCTSTR)sEventName, GetPropertyName( nEventId ) );
	}

	return sEventSymbol;
}
