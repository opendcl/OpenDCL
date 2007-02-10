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
#include "ObjectDCLView.h"
#include "Project.h"
#include "EditorWorkspace.h"
#include "MainFrm.h"
#include "LoadArgs.h"

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
	//{{AFX_DATA_INIT(CEventsTabPane)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bInitialized = false;
	m_pControl = NULL;
	m_pDclForm = NULL;
	m_pView = NULL;
	
}


void CEventsTabPane::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventsTabPane)
	DDX_Control(pDX, IDC_LABEL, m_Label);
	DDX_Control(pDX, IDC_EVENTDESC, m_EventDesc);
	DDX_Control(pDX, IDC_DEFUNPREVIEW, m_DefunPreview);
	DDX_Control(pDX, IDC_DEFUNEDIT, m_DefunEdit);
	DDX_Control(pDX, IDC_EVENTSTREE, m_EventsTree);
	DDX_Control(pDX, IDC_ADDTOLISP, m_AddToLisp);
	DDX_Control(pDX, IDC_ADDCANCEL, m_AddCancel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventsTabPane, CDialog)
	//{{AFX_MSG_MAP(CEventsTabPane)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_EN_CHANGE(IDC_DEFUNEDIT, OnChangeDefunedit)
	ON_BN_CLICKED(IDC_ADDCANCEL, OnAddcancel)
	ON_BN_CLICKED(IDC_ADDTOLISP, OnAddtolisp)
	ON_LBN_SELCHANGE(IDC_EVENTSTREE, OnSelchangeEventstree)
	ON_CLBN_CHKCHANGE(IDC_EVENTSTREE, OnCheckChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventsTabPane message handlers

BOOL CEventsTabPane::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_bInitialized = true;

	CString sText;
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);

	BOOL bUsesOn = pApp->GetProfileInt(sProfileName, _T("EventsCopyToClipboard"), TRUE);
			
	if (bUsesOn)
	{
		sText = theWorkspace.LoadResourceString(IDS_COPYTOCLIPBOARD);
		m_AddToLisp.SetWindowText(sText);
	}
	else
	{
		sText = theWorkspace.LoadResourceString(IDS_ADDTOSTRING);
		m_AddToLisp.SetWindowText(sText);	
	}

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

	if (bShow == TRUE && m_pDclForm != NULL)
		UpdateEvents(m_pDclForm, m_pControl);
}

void CEventsTabPane::UpdateEvents(CDclFormObject *pDclForm, CDclControlObject *pControl)
{
	m_pControl = pControl;
	m_pDclForm = pDclForm;

	if (m_pDclForm == NULL)
		return;
	
	if (m_pDclForm->GetType() == VdclDockable)
		m_AddCancel.ShowWindow(TRUE);
	else
		m_AddCancel.ShowWindow(FALSE);

	CString sEventDeclaration;
	m_EventsTree.ResetContent();
	
	if (m_pDclForm == NULL || m_pControl == NULL)
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
	{
		m_EventsTree.SetFocus();
	}
	
	m_DefunEdit.SetWindowText(CString());
}


void CEventsTabPane::AddAnyActiveXEvents() 
{
	if (m_pDclForm == NULL)
		return;
	
	CString DefaultEventDeclaration;

	
	for (int i = 0; i < m_pControl->GetPropertyList().GetCount(); i++)
	{
		POSITION pos = m_pControl->GetPropertyList().FindIndex(i);

		if (pos != NULL)
		{
			// get the event property object
			RefCountedPtr< CPropertyObject > pProp = m_pControl->GetPropertyList().GetAt(pos);
			
			// if the event exists, add it
			if (pProp != NULL && pProp->GetType() == PropActiveXEvent)
			{
				CString sName = pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->Name;
				if (!sName.IsEmpty())
				{
					int hItem = m_EventsTree.AddString(sName);
					m_EventsTree.SetItemData(hItem, i);
					if (pProp->GetSubtype() & 1) // if the event has been set...
						m_EventsTree.SetCheck(hItem, TRUE); // set the event tree item as checked.
				}
			}
		}
	}    
}

void CEventsTabPane::TryToAddEvent(PropertyId nEventId) 
{
	if (m_pDclForm == NULL)
		return;
	
	UINT ResStringId = nEventId + 210;	
	// there is a special case for grid cell editing.
	if (nEventId == nEventEndLabelEdit)
		ResStringId = 5226;
	   
	CString DefaultEventDeclaration;	
	// get the event property object
	RefCountedPtr< CPropertyObject > pProp = m_pControl->GetPropertyObject(nEventId);
	
	// if the event exists, add it
	if (pProp != NULL)
	{
		CString EventName;
    if (nEventId != nFormEventOnOk) {
      EventName = theWorkspace.LoadResourceString(ResStringId);
    } else {
      EventName = "Ok";
    }
		
		CWinApp* pApp = AfxGetApp();
		CString sLoadText;

		BOOL bUsesOn = pApp->GetProfileInt(sLoadText, _T("EventPrefixUsesON"), TRUE);
    
		if (bUsesOn == TRUE)
		{
			sLoadText = theWorkspace.LoadResourceString(IDS_EVENTPREFIXON);
			EventName = sLoadText + EventName;
		}	

		int hItem = m_EventsTree.AddString(EventName);
		m_EventsTree.SetItemData(hItem, nEventId);

		// if the event has been set...
		if (pProp->GetStringValue().GetLength() > 0)
			// set the event tree item as checked.
			m_EventsTree.SetCheck(hItem, TRUE);		
	} 
}
void CEventsTabPane::OnChangeDefunedit() 
{
	CString sDefunEditText;
	
	if (m_pDclForm == NULL)
	{		
		return;
	}
	

	// get the current event selection
	int hItem = m_EventsTree.GetCurSel();
	
	if (hItem != -1)
	{
		// get the new defun name from the edit box
		m_DefunEdit.GetWindowText(sDefunEditText);

		// get the event id from the curent selection
		PropertyId nEventId = (PropertyId)m_EventsTree.GetItemData(hItem);

		// we need to know if the event has been checked
		BOOL bChecked = m_EventsTree.GetCheck(hItem);

		// if the event is checked, we can update the property object
		if (bChecked == TRUE)
			// set the property to the new defun name
			SetEvent(nEventId, sDefunEditText);


		SetDefunPreview();
	}
}

void CEventsTabPane::SetDefunPreview()
{
	CString sStringPreview = "(defun ";

	if (m_pDclForm == NULL)
		return;
	

	// get the current event selection
	int hItem = m_EventsTree.GetCurSel();
	
	if (hItem != -1)
	{
		// get the event id from the curent selection
		PropertyId nEventId = (PropertyId)m_EventsTree.GetItemData(hItem);

		// get the defun name
		CString sEventDefun = GetEvent(nEventId);

		if (sEventDefun.IsEmpty())
		{
			m_DefunEdit.GetWindowText(sEventDefun);
		}
		// if the ^C^C has been added, remove it for the defun preview
		if (sEventDefun.Left(4) == "^c^c" || sEventDefun.Left(4) == "^C^C")
			sEventDefun = sEventDefun.Right(sEventDefun.GetLength() - 4);

		// add the defun name
		sStringPreview = sStringPreview + sEventDefun + " ("; 
		
		// get the arguments
		CString sArgs = GetDefunArguments();

		if (!sArgs.IsEmpty())
			sArgs += " /";

		// if arguments are found add them to the defun
		if (sArgs.GetLength() > 0)
		{
			sStringPreview = sStringPreview + sArgs;
		}

		// close the argument bracket.
		sStringPreview = sStringPreview + ")";

		// start the next line
		sStringPreview = sStringPreview + "\r\n";

		if (nEventId == nDragnDropFromAutoCAD && m_pDclForm->GetType() != VdclFileDialog)
		{
			// add the code to get the current AutoCAD selection set in AutoLisp
			sStringPreview = sStringPreview 
                +"     (setq ssDragnDropSelectionSet (ssget \"P\"))"
				// start the next line
				+ "\r\n"
				// end the setq bracket
				+ ")";
		}
		else
		{
			// add the default message box to show the programmer the event has not been updated
			sStringPreview = sStringPreview  
				+ "     (Odcl_MessageBox \"To Do: code must be added to " 
				// add the control name here
				+ m_pControl->GetStrProperty(nName) + "\" \"To do\")";
		}

		// start the next line
		sStringPreview = sStringPreview + "\r\n";

		// close the defun bracket.
		sStringPreview = sStringPreview + ")";


		// set the preview control's text
		m_DefunPreview.SetWindowText(sStringPreview);
	}
}

CString CEventsTabPane::GetDefunArguments()
{
	CString sArgs = CString();
	CString strDesc= CString();
	
	// get the event id from the curent selection
	int nEventId = m_EventsTree.GetItemData(m_EventsTree.GetCurSel());

	// get the arx control Id
	int nArxControlType = m_pControl->GetType();

	if (nArxControlType != CtlActiveX)
		LoadArgsNDesc(nEventId, nArxControlType, nArxControlType, sArgs, strDesc, m_pControl);
	else
	{
		POSITION pos = m_pControl->GetPropertyList().FindIndex(nEventId);
		if (pos != NULL)
		{
			RefCountedPtr< CPropertyObject > pProp = m_pControl->GetPropertyList().GetAt(pos);
			strDesc = pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->DocumentationDesc;
			int ctCallingArgs = pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->nArgs;
			for (int i = 0; i < ctCallingArgs; i++)
			{
				sArgs += pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->CallingArgNames[i];
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
	if (m_pDclForm == NULL)
		return;

	m_Label.SetWindowText(CString());
	m_EventDesc.SetWindowText(CString());
	m_EventsTree.ResetContent();
	m_DefunEdit.SetWindowText(CString());
	m_DefunPreview.SetWindowText(CString());
	m_pControl = NULL;
	m_pDclForm = NULL;
	
}

BOOL CEventsTabPane::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN ||  pMsg->message == WM_CHAR )
	{
		// lets make sure the user can't press the [Esc] key and close the tab pane
		switch (pMsg->wParam)
		{			
			case VK_ESCAPE:
				{
					return 1;
				break;
				}
			case VK_DELETE:
				{
					m_pView->DeleteSelectedControls();
				break;
				}
			case 3:
				if (m_pView != NULL)
 					m_pView->OnEditCopy();
				break;
			case 22:
				if (m_pView != NULL)
 					m_pView->OnEditPaste();
				break;
			case 24:
				if (m_pView != NULL)
 					m_pView->OnEditCut();
				break;
			case 26:
				if (m_pView != NULL)
 					m_pView->OnEditUndo();
				break;
			default:
				if (m_pView != NULL)
 					m_pView->PreTranslateMessage(pMsg);
				break;
		}
		
	}	
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CEventsTabPane::OnAddcancel() 
{
	CString sDefunEditText;
	
	if (m_pDclForm == NULL)
	{		
		return;
	}
	
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
		HGLOBAL clipbuffer;
		TCHAR * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, (source.GetLength()+1) * sizeof(TCHAR));
		buffer = (TCHAR*)GlobalLock(clipbuffer);
		lstrcpy(buffer, LPCTSTR(source));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();


	}
}

void CEventsTabPane::OnAddtolisp() 
{
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);

	BOOL bUsesOn = pApp->GetProfileInt(sProfileName, _T("EventsCopyToClipboard"), TRUE);
	
	
	CString sDefunPreview;
	CString sDefunEditText;
	CDclControlObject *pControl = m_pControl;

	if (m_pDclForm == NULL || m_pControl == NULL)
	{		
		return;
	}

	// get the current event selection
	PropertyId hItem = (PropertyId)m_EventsTree.GetCurSel();
	
	if (hItem != -1)
	{
		// get the event id from the curent selection
		PropertyId nEventId = (PropertyId)m_EventsTree.GetItemData(hItem);

		// get the new defun name from the edit box
		m_DefunPreview.GetWindowText(sDefunPreview);

		// lets get the defun it self
		m_DefunEdit.GetWindowText(sDefunEditText);

		if (bUsesOn)
		{
			CopyToClipboard();

			// set the event tree item as checked.
			m_EventsTree.SetCheck(hItem, TRUE);

			if (pControl != NULL)
				// set the property to the new defun name
				SetEvent(nEventId, sDefunEditText);

			return;
		}

		// if the file name has not been set.
		if (activeProject->m_LispFileName.GetLength() == 0)
		{
			activeProject->m_LispFileName =
				theEditorWorkspace.GetMainFrame()->m_PropertyTabPane.m_PropertiesTabPane.GetPropertiesCtrl().QueryForLispFileName();
			// set the lisp file name
			if (activeProject->m_LispFileName.GetLength() == 0)
				// if the user canceled. Exit here
				return;

		}
		
		
		
		// set the event tree item as checked.
		m_EventsTree.SetCheck(hItem, TRUE);

		if (pControl != NULL)
			// set the property to the new defun name
			SetEvent(nEventId, sDefunEditText);

		// here we are going to write out to the lisp file.
		CStdioFile fout(activeProject->m_LispFileName,
			CFile::modeCreate|CFile::modeNoTruncate|CFile::modeWrite);
		
		fout.SeekToEnd();
		fout.WriteString(_T("\r\n"));
		fout.WriteString(sDefunPreview);
		fout.WriteString(_T("\r\n"));
		
		fout.Close();

		// update the project tree's autolisp file name
		theEditorWorkspace.GetProjectTreeCtrl()->SetAutoLispFilename(activeProject->m_LispFileName);

		CString sText;
		CString sTitle;
		sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
		sText = theWorkspace.LoadResourceString(IDS_FUNCADDED);		
		MessageBox(sText, sTitle, MB_OK);
	}
}

void CEventsTabPane::OnSelchangeEventstree() 
{
	CString sItemText;
	CString sDclFormName;
	BOOL bUsesOn;

	if (m_pDclForm == NULL)
	{		
		return;
	}
	
	
	int hItem = m_EventsTree.GetCurSel();

	if (hItem == -1)
		return;

	// get the selected item's text
	m_EventsTree.GetText(hItem, sItemText);   		
		
	int nCheck = m_EventsTree.GetCheck(hItem);
	
	if (sItemText.GetLength() == 0)
		return;

	// create the defun now
	CString sEventDeclaration = theWorkspace.LoadResourceString(IDS_C);
	try
	{
		if (m_pDclForm->GetType() != VdclTabForm)
		{
			sDclFormName = m_pDclForm->GetKeyName();
		}
		else
		{
			m_pDclForm = activeProject->GetParentDclForm(m_pDclForm->GetParentName());
			sDclFormName = m_pDclForm->GetKeyName();
		}
	}
	catch(...)
	{
		m_pDclForm = NULL;
		m_EventsTree.ResetContent();
		return;
	}

	if (sDclFormName.GetLength() == 0)
		return;

	CString sUnderscore;
	sUnderscore = theWorkspace.LoadResourceString(IDS_UNDERSCORE);
	
	sEventDeclaration = sEventDeclaration + sDclFormName + sUnderscore;
	
	if (m_pDclForm->GetControlProperties() != m_pControl)
	{
		CWinApp* pApp = AfxGetApp();
		CString sLoadText;
		sLoadText = theWorkspace.LoadResourceString(IDR_MAINFRAME);

		bUsesOn = pApp->GetProfileInt(sLoadText, _T("EventPrefixUsesON"), TRUE);
    
		if (bUsesOn == TRUE)
		{
			sEventDeclaration = sEventDeclaration + m_pControl->GetStrProperty(nName) + sUnderscore;
		}		
		else			
			sEventDeclaration = sEventDeclaration + m_pControl->GetStrProperty(nName) + sUnderscore;
	}
	else
	{
		CWinApp* pApp = AfxGetApp();
		CString sLoadText;
		sLoadText = theWorkspace.LoadResourceString(IDR_MAINFRAME);

		bUsesOn = pApp->GetProfileInt(sLoadText, _T("EventPrefixUsesON"), TRUE);
    
		if (bUsesOn == TRUE)
		{
			sEventDeclaration = sEventDeclaration;
		}
	}
	
	sEventDeclaration = sEventDeclaration + sItemText;

	// get the posible event defun string from the property
	CString sEventDefun = GetEvent((PropertyId)m_EventsTree.GetItemData(m_EventsTree.GetCurSel())); 

	// if the property was not previously set, set it to the default name
	if (sEventDefun.GetLength() == 0)
		sEventDefun = sEventDeclaration;
	
	// update the defun edit box.
	m_DefunEdit.SetWindowText(sEventDefun);
	
}
void CEventsTabPane::OnCheckChanged()
{
	CString sDefunEditText;
	CString sDclFormName;
	CString sEventDefun;
	BOOL bUsesOn;

	int nCheck = m_EventsTree.GetCheck(m_EventsTree.GetCurSel());

	PropertyId nEventId = (PropertyId)m_EventsTree.GetItemData(m_EventsTree.GetCurSel());
	if (nCheck == 1)
	{	
		// create the defun now
		CString sItemText;
		CString sEventDeclaration = theWorkspace.LoadResourceString(IDS_C);
		try
		{
			sDclFormName = m_pDclForm->GetKeyName();
		}
		catch(...)
		{
			m_pDclForm = NULL;
			m_EventsTree.ResetContent();
			return;
		}

		if (sDclFormName.GetLength() == 0)
			return;

		CString sUnderscore;
		sUnderscore = theWorkspace.LoadResourceString(IDS_UNDERSCORE);

		sEventDeclaration = sEventDeclaration + sDclFormName + sUnderscore;
		
		if (m_pDclForm->GetControlProperties() != m_pControl)
		{
			CWinApp* pApp = AfxGetApp();
			CString sLoadText;
			sLoadText = theWorkspace.LoadResourceString(IDR_MAINFRAME);

			bUsesOn = pApp->GetProfileInt(sLoadText, _T("EventPrefixUsesON"), TRUE);
    
			if (bUsesOn == TRUE)
			{
				//sLoadText = theWorkspace.LoadResourceString(IDS_EVENTPREFIXON);
				sEventDeclaration = sEventDeclaration + m_pControl->GetStrProperty(nName) + sUnderscore;
			}
			else
				sEventDeclaration = sEventDeclaration + m_pControl->GetStrProperty(nName) + sUnderscore;
		}

		
		// get the selected item's text
		m_EventsTree.GetText(m_EventsTree.GetCurSel(), sItemText);   		
	
		sEventDeclaration = sEventDeclaration + sItemText;

		// get the posible event defun string from the property
		sEventDefun = GetEvent((PropertyId)m_EventsTree.GetItemData(m_EventsTree.GetCurSel())); 

		// if the property was not previously set, set it to the default name
		if (sEventDefun.GetLength() == 0)
			sEventDefun = sEventDeclaration;
	}
	SetEvent(nEventId, sEventDefun);

	
}

void CEventsTabPane::SetEvent(PropertyId nEventId, CString sEventDefun)
{
	if (m_pControl->GetType() == CtlActiveX)
	{
		POSITION pos = m_pControl->GetPropertyList().FindIndex(nEventId);
		if (pos != NULL)
		{
			RefCountedPtr< CPropertyObject > pProp = m_pControl->GetPropertyList().GetAt(pos);
			pProp->SetStringValue( sEventDefun );
		}
	}
	else
		// set the property to the new defun name
		m_pControl->SetStrProperty(nEventId, sEventDefun); 
}
CString CEventsTabPane::GetEvent(PropertyId nEventId)
{
	if (m_pControl->GetType() == CtlActiveX)
	{
		POSITION pos = m_pControl->GetPropertyList().FindIndex(nEventId);
		if (pos != NULL)
		{
			RefCountedPtr< CPropertyObject > pProp = m_pControl->GetPropertyList().GetAt(pos);
			return pProp->GetStringValue();
		}
	}
	else
		// set the property to the new defun name
		return m_pControl->GetStrProperty(nEventId); 

	return CString();
}
