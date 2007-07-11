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
#include "OpenDCLView.h"
#include "Project.h"
#include "EditorWorkspace.h"
#include "MainFrm.h"
#include "LoadArgs.h"
#include "PropertyNames.h"

const int nTreeBottom = 150;
const int nLabelBottom = 135;
const int nEditBottom = 115;
const int nPreviewBottom = 75;
const int nDescBottom = 26;


/////////////////////////////////////////////////////////////////////////////
// CEventsTabPane dialog

CEventsTabPane::CEventsTabPane(CWnd* pParent /*=NULL*/)
	: CDialog(CEventsTabPane::IDD, pParent)
{
	m_bInitialized = false;
	m_pControl = NULL;
	m_pView = NULL;
}


void CEventsTabPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LABEL, m_Label);
	DDX_Control(pDX, IDC_EVENTDESC, m_EventDesc);
	DDX_Control(pDX, IDC_DEFUNPREVIEW, m_DefunPreview);
	DDX_Control(pDX, IDC_DEFUNEDIT, m_DefunEdit);
	DDX_Control(pDX, IDC_EVENTSTREE, m_EventsTree);
	DDX_Control(pDX, IDC_ADDTOLISP, m_AddToLisp);
	DDX_Control(pDX, IDC_ADDCANCEL, m_AddCancel);
}


BEGIN_MESSAGE_MAP(CEventsTabPane, CDialog)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_DEFUNEDIT, OnChangeDefunedit)
	ON_BN_CLICKED(IDC_ADDCANCEL, OnAddcancel)
	ON_BN_CLICKED(IDC_ADDTOLISP, OnAddtolisp)
	ON_LBN_SELCHANGE(IDC_EVENTSTREE, OnSelchangeEventstree)
	ON_CLBN_CHKCHANGE(IDC_EVENTSTREE, OnCheckChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventsTabPane message handlers

BOOL CEventsTabPane::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bInitialized = true;

	if (!AfxGetApp()->GetProfileInt(theWorkspace.LoadResourceString(IDR_MAINFRAME), _T("EventsWriteToLispFile"), FALSE))
		m_AddToLisp.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CEventsTabPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if (!m_bInitialized) 
		return;

	CRect rcEventTree (0,1,cx,cy-nTreeBottom);
	m_EventsTree.MoveWindow(rcEventTree, TRUE);
		
	CRect rcLabel (2,cy-nTreeBottom+1,cx,cy-nLabelBottom);
	m_Label.MoveWindow(rcLabel, TRUE);

	CRect rcDefunEdit (0,cy-nLabelBottom+1,cx,cy-nEditBottom);
	m_DefunEdit.MoveWindow(rcDefunEdit, TRUE);

	CRect rcDefunPreview (0,cy-nEditBottom+1,cx,cy-nPreviewBottom);
	m_DefunPreview.MoveWindow(rcDefunPreview, TRUE);	

	CRect rcEventDesc (0,cy-nPreviewBottom+1,cx,cy-nDescBottom);
	m_EventDesc.MoveWindow(rcEventDesc, TRUE);

	CRect rcAddCancel (0,cy-nDescBottom+3,70,cy-1);		
	m_AddCancel.MoveWindow(rcAddCancel, TRUE);
	
	CRect rcAddToLisp (75,cy-nDescBottom+3,cx+1,cy-1);
	m_AddToLisp.MoveWindow(rcAddToLisp, TRUE);
}


void CEventsTabPane::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	//if (bShow == TRUE && m_pDclForm != NULL)
	//	UpdateEvents(m_pControl);
}

void CEventsTabPane::UpdateEvents(CDclControlObject *pControl)
{
	m_pControl = pControl;
	
	if (pControl && pControl->GetOwnerForm()->GetType() == VdclDockable)
		m_AddCancel.ShowWindow(SW_SHOW);
	else
		m_AddCancel.ShowWindow(SW_HIDE);
	m_EventsTree.ResetContent();
	
	if (m_pControl == NULL)
		return;
	
	TryToAddEvent(nEventFolderChanged);
	TryToAddEvent(nEventOnHelp);
	TryToAddEvent(nEventOnTypeChange);
	TryToAddEvent(nEventBeginLabelEdit);
	TryToAddEvent(nEventBtnClicked);
	TryToAddEvent(nEventSplitterMoved);
	TryToAddEvent(nEventChanged);
	TryToAddEvent(nEventClicked);
	TryToAddEvent(nEventColumnClick);
	TryToAddEvent(nEventDblClicked);
	TryToAddEvent(nEventDeleteItem);
	TryToAddEvent(nEventDropDown);
	TryToAddEvent(nEventEditChanged);
	TryToAddEvent(nEventEndLabelEdit);
	TryToAddEvent(nEventGetDayState);
	TryToAddEvent(nEventItemExpanded);
	TryToAddEvent(nEventItemExpanding);
	TryToAddEvent(nEventMouseDown);
	TryToAddEvent(nEventMouseUp);
	TryToAddEvent(nEventMouseMove);
	TryToAddEvent(nEventMouseEntered);
	TryToAddEvent(nEventMouseMovedOff);
	TryToAddEvent(nEventMouseDblClick);
	TryToAddEvent(nEventMouseWheel);
	TryToAddEvent(nEventNavigateComplete);
	TryToAddEvent(nEventOutOfMemory);
	TryToAddEvent(nEventKeyDown);
	TryToAddEvent(nEventKeyUp);
	TryToAddEvent(nEventKillFocus);
	TryToAddEvent(nEventMaxText);
	TryToAddEvent(nOnLMouseEvent);
	TryToAddEvent(nOnMMouseEvent);
	TryToAddEvent(nEventPaint);
	TryToAddEvent(nOnRMouseEvent);
	TryToAddEvent(nEventReturn);
	TryToAddEvent(nEventRClick);
	TryToAddEvent(nEventRDblClick);
	TryToAddEvent(nEventReturnPressed);
	TryToAddEvent(nEventSelect);
	TryToAddEvent(nEventSelChanged);
	TryToAddEvent(nEventSelChanging);
	TryToAddEvent(nEventSetFocus);
	TryToAddEvent(nEventUpdate);
	TryToAddEvent(nEventScroll);
	TryToAddEvent(nEventScrolled);
	TryToAddEvent(nEventReleasedCapture);
	TryToAddEvent(nDragnDropBegin);
	TryToAddEvent(nDragnDropToAutoCAD);
	TryToAddEvent(nDragnDropFromControl);
	TryToAddEvent(nDragnDropFromAutoCAD);
	TryToAddEvent(nFormEventCancelClose);
	TryToAddEvent(nFormEventClose);
  TryToAddEvent(nFormEventOnOk);
	TryToAddEvent(nFormEventOnCancel);
	TryToAddEvent(nFormEventInitialize);
	TryToAddEvent(nFormEventSize);
	TryToAddEvent(nFormEventShow);
	TryToAddEvent(nDocEventActivated);
	TryToAddEvent(nCfgEventCancel);
	TryToAddEvent(nCfgEventOK);
	TryToAddEvent(nCfgEventHelp);
	TryToAddEvent(nCfgEventApply);
	
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
	INT_PTR idxProp = 0;
	POSITION pos = m_pControl->GetPropertyList().GetHeadPosition();
	while( pos )
	{
		RefCountedPtr< CPropertyObject > pProp = m_pControl->GetPropertyList().GetNext( pos );
		if( pProp && pProp->GetType() == PropActiveXEvent )
		{ //the event exists, so add it to the list
			CString sName = pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->GetName();
			if( !sName.IsEmpty() )
			{
				int idxEvent = m_EventsTree.AddString( sName );
				m_EventsTree.SetItemData( idxEvent, idxProp );
				if( !pProp->GetName().IsEmpty() ) // if the event has been set...
					m_EventsTree.SetCheck( idxEvent, BST_CHECKED ); // set the event tree item as checked.
			}
		}
		idxProp++;
	}
}

void CEventsTabPane::TryToAddEvent( PropertyId nEventId ) 
{
	if( m_pControl == NULL )
		return;

	RefCountedPtr< CPropertyObject > pProp = m_pControl->GetPropertyObject( nEventId );
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
	CString sDefunEditText;
	
	if (m_pControl == NULL)
		return;

	// get the current event selection
	int hItem = m_EventsTree.GetCurSel();
	
	if (hItem != -1)
	{
		// get the new defun name from the edit box
		m_DefunEdit.GetWindowText(sDefunEditText);

		// get the event id from the curent selection
		PropertyId nEventId = (PropertyId)m_EventsTree.GetItemData(hItem);

		// we need to know if the event has been checked
		bool bChecked = (m_EventsTree.GetCheck(hItem) == BST_CHECKED);

		// if the event is checked, we can update the property object
		if (bChecked)
			SetEvent(nEventId, sDefunEditText); // set the property to the new defun name
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
		PropertyId nEventId = (PropertyId)m_EventsTree.GetItemData( hItem );

		// get the defun name
		CString sEventDefun = GetEvent( nEventId );
		if( sEventDefun.IsEmpty() )
			m_DefunEdit.GetWindowText( sEventDefun );

		// if the ^C^C has been added, remove it for the defun preview
		if( sEventDefun.Left(4).CompareNoCase( _T("^c^c") ) == 0 )
			sEventDefun = sEventDefun.Mid( 4 );
		
		// get the arguments
		CString sArgs = GetDefunArguments();
		sArgs += _T(" /");

		CString sDefunBody;
		if (nEventId == nDragnDropFromAutoCAD && m_pControl->GetOwnerForm()->GetType() != VdclFileDialog)
			sDefunBody = _T("     (setq ssDragnDropSelectionSet (ssget \"P\"))"); //get the 'Previous' selection set
		else
		{
			// add the default message box to show the programmer the event has not been updated
			sDefunBody.Format( _T("     (dcl_MessageBox \"To Do: code must be added to event handler\\r\\n%s\" \"To do\")"),
												 sEventDefun );
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
		LoadArgsNDesc((PropertyId)nEventId, m_pControl, sArgs, strDesc);
	else
	{
		POSITION pos = m_pControl->GetPropertyList().FindIndex(nEventId);
		if (pos != NULL)
		{
			RefCountedPtr< CPropertyObject > pProp = m_pControl->GetPropertyList().GetAt(pos);
			strDesc = pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->GetDesc();
			size_t ctCallingArgs = pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->GetArgs().size();
			for (UINT_PTR i = 0; i < ctCallingArgs; i++)
			{
				sArgs += pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->GetArgs().at( i ).name;
				if (i < ctCallingArgs - 1)
					sArgs += " ";
			}
		}
	}

	// set the description
	m_EventDesc.SetWindowText(strDesc);
		
	return sArgs;
}

void CEventsTabPane::ClearEvents()
{
	m_AddToLisp.EnableWindow(FALSE);
	if (m_pControl == NULL)
		return;

	m_Label.SetWindowText(CString());
	m_EventDesc.SetWindowText(CString());
	m_EventsTree.ResetContent();
	m_DefunEdit.SetWindowText(CString());
	m_DefunPreview.SetWindowText(CString());
	m_pControl = NULL;
}

BOOL CEventsTabPane::PreTranslateMessage(MSG* pMsg) 
{
	//if( pMsg->message == WM_KEYDOWN ||  pMsg->message == WM_CHAR )
	//{
	//	// lets make sure the user can't press the [Esc] key and close the tab pane
	//	switch (pMsg->wParam)
	//	{			
	//		case VK_ESCAPE:
	//			{
	//				return 1;
	//			break;
	//			}
	//		case VK_DELETE:
	//			{
	//				m_pView->DeleteSelectedControls();
	//			break;
	//			}
	//		case 3:
	//			if (m_pView != NULL)
 //					m_pView->OnEditCopy();
	//			break;
	//		case 22:
	//			if (m_pView != NULL)
 //					m_pView->OnEditPaste();
	//			break;
	//		case 24:
	//			if (m_pView != NULL)
 //					m_pView->OnEditCut();
	//			break;
	//		case 26:
	//			if (m_pView != NULL)
 //					m_pView->OnEditUndo();
	//			break;
	//		default:
	//			if (m_pView != NULL)
 //					m_pView->PreTranslateMessage(pMsg);
	//			break;
	//	}
	//	
	//}	
	
	return CDialog::PreTranslateMessage(pMsg);
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
		PropertyId nEventId = (PropertyId)m_EventsTree.GetItemData(hItem);

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

void CEventsTabPane::CopyToClipboard() 
{
	CString source;

	// get the new defun name from the edit box
	m_DefunPreview.GetWindowText(source);

	//put your text in source
	if(OpenClipboard())
	{
		EmptyClipboard();
		HGLOBAL clipbuffer = GlobalAlloc(GMEM_DDESHARE, (source.GetLength() + 1) * sizeof(TCHAR));
		TCHAR* buffer = (TCHAR*)GlobalLock(clipbuffer);
		lstrcpyn(buffer, source, source.GetLength() + 1);
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
	PropertyId hItem = (PropertyId)m_EventsTree.GetCurSel();
	
	if (hItem != -1)
	{
		m_EventsTree.SetCheck(hItem, TRUE); // set the event tree item as checked.

		// get the event id from the curent selection
		PropertyId nEventId = (PropertyId)m_EventsTree.GetItemData(hItem);

		// get the new defun name from the edit box
		m_DefunPreview.GetWindowText(sDefunPreview);

		// lets get the defun it self
		m_DefunEdit.GetWindowText(sDefunEditText);

		// if the file name has not been set, prompt for it
		CProject* pProject = m_pControl->GetOwnerProject();
		CString sLispFileName = pProject->GetLispFileName();
		if (sLispFileName.IsEmpty())
		{
			CFileDialog Dlg( FALSE,
											 _T(".lsp"), 
											 sLispFileName, 
											 OFN_HIDEREADONLY | OFN_ENABLESIZING | OFN_EXPLORER,
											 theWorkspace.LoadResourceString(IDS_AUTOLISPFILE),
											 CWnd::GetActiveWindow() );
			CString sTitle = theWorkspace.LoadResourceString(IDS_SELECTPROJECTLISPFILE);
			Dlg.m_pOFN->lpstrTitle = sTitle.LockBuffer();
			if( Dlg.DoModal() != IDOK )
				return; //user cancelled
			sLispFileName = Dlg.GetPathName();
			if( sLispFileName.IsEmpty() )
				return;
			pProject->SetLispFileName( sLispFileName );
			theEditorWorkspace.GetProjectTreeCtrl()->SetAutoLispFilename(sLispFileName);
			CDocument* pDoc = m_pView->GetDocument();
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

		MessageBox(theWorkspace.LoadResourceString(IDS_FUNCADDED), theWorkspace.LoadResourceString(IDR_MAINFRAME), MB_OK);
	}
}

void CEventsTabPane::OnSelchangeEventstree() 
{
	m_AddToLisp.EnableWindow(FALSE);
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
	
	// update the defun edit box.
	m_DefunEdit.SetWindowText(GetEvent((PropertyId)m_EventsTree.GetItemData(m_EventsTree.GetCurSel())));
	m_AddToLisp.EnableWindow();
}

void CEventsTabPane::OnCheckChanged()
{
	PropertyId nEventId = (PropertyId)m_EventsTree.GetItemData(m_EventsTree.GetCurSel());
	CString sEventDefun;
	if (m_EventsTree.GetCheck(m_EventsTree.GetCurSel()) == BST_CHECKED)
		sEventDefun = GetEvent(nEventId);
	SetEvent(nEventId, sEventDefun);
}

void CEventsTabPane::SetEvent( PropertyId nEventId, CString sEventDefun )
{
	if (m_pControl->GetType() == CtlActiveX)
	{
		//if it's an ActiveX event, then nEventId is actually an index into the property list
		POSITION pos = m_pControl->GetPropertyList().FindIndex( nEventId );
		if (pos != NULL)
		{
			RefCountedPtr< CPropertyObject > pProp = m_pControl->GetPropertyList().GetAt( pos );
			pProp->SetStringValue( sEventDefun );
		}
	}
	else
		m_pControl->SetStringProperty( nEventId, sEventDefun ); 
	CString sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);
	if (AfxGetApp()->GetProfileInt(sProfileName, _T("EventsCopyToClipboard"), TRUE))
		CopyToClipboard();
}

CString CEventsTabPane::GetEvent( PropertyId nEventId )
{
	CString sEventSymbol;
	if (m_pControl->GetType() != CtlActiveX)
		sEventSymbol = m_pControl->GetStrProperty(nEventId); 
	if( sEventSymbol.IsEmpty() )
	{
		CString sEventName;
		if (m_pControl->GetType() != CtlForm)
		{
			sEventName = m_pControl->GetStrProperty( nGlobalVarName );
			if( sEventName.IsEmpty() )
				sEventName = m_pControl->GetKeyPath();
		}
		else
		{
			sEventName = m_pControl->GetStrProperty( nGlobalVarName );
			if( sEventName.IsEmpty() )
				sEventName = m_pControl->GetOwnerForm()->GetKeyPath();
		}
		CString sItemText;
		m_EventsTree.GetText(m_EventsTree.GetCurSel(), sItemText);
		sEventSymbol.Format( _T("c:%s_On%s"), (LPCTSTR)sEventName, (LPCTSTR)sItemText );
	}

	return sEventSymbol;
}
