// ObjectBrowser.cpp : implementation file
//

#include "stdafx.h"
#include "ObjectBrowser.h"
#include "EventsTabPane.h"
#include "PropertyIds.h"
#include "VarUtils.h"
#include "Resource.h"
#include "DclControlObject.h"
#include "ControlTypes.h"
#include "DclFormObject.h"
#include "ControlName.h"
#include "Project.h"
#include "EditorWorkspace.h"
#include "PropertyNames.h"
#include "PropertyObject.h"
#include "AxMethodDescriptor.h"
#include "AxPropertyDescriptor.h"
#include "AxEventDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "AxContainerCtrl.h"
#include "ControlHolder.h"
#include "LoadArgs.h"
#include "SharedRes.h"


/////////////////////////////////////////////////////////////////////////////
// CObjectBrowser dialog

CObjectBrowser::CObjectBrowser(CWnd* pParent /*=NULL*/)
	//: CDialog(CObjectBrowser::IDD, pParent)
	: CResizableDialog(CObjectBrowser::IDD, pParent)
{
	
	//{{AFX_DATA_INIT(CObjectBrowser)
	//}}AFX_DATA_INIT
	m_pControl = NULL;
	nMethodIndex = 0;
	m_sDclFormName = theWorkspace.LoadResourceString(IDS_DclFormName);
	m_sClipBoardDefun1;
	m_sClipBoardDefun2;
}


void CObjectBrowser::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CObjectBrowser)
	DDX_Control(pDX, IDC_COPY3, m_Copy3);
	DDX_Control(pDX, IDC_METHDEF, m_MefDef);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_COPY2, m_Copy2);
	DDX_Control(pDX, IDC_COPY1, m_Copy1);
	DDX_Control(pDX, IDC_RICHEDIT, m_RichBox);
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CObjectBrowser, CResizableDialog)
	//{{AFX_MSG_MAP(CObjectBrowser)
	ON_NOTIFY(NM_CLICK, IDC_LISTBOX, OnClickListbox)
	ON_NOTIFY(NM_RETURN, IDC_LISTBOX, OnReturnListbox)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTBOX, OnDblclkListbox)
	ON_NOTIFY(TVN_SELCHANGED, IDC_LISTBOX, OnSelchangedListbox)
	ON_BN_CLICKED(IDC_COPY2, OnCopy2)
	ON_BN_CLICKED(IDC_COPY1, OnCopy1)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_COPY3, OnCopy3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectBrowser message handlers

BOOL CObjectBrowser::OnInitDialog() 
{
	CResizableDialog::OnInitDialog();
	
	m_ImageList.Create(nDeIconSize,nDeIconSize, ILC_COLOR | ILC_MASK, 1, 1);
	
	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ENUMS), RT_GROUP_ICON);

	HICON hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_ENUMS));
  	m_ImageList.Add(hicon);
	DestroyIcon(hicon);
	
	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_ENUM));
	m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_EVENT));
	m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_METHOD));
	m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_PROPERTY));
	m_ImageList.Add(hicon);
	DestroyIcon(hicon);

	hicon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_OLEOBJECT));
	m_ImageList.Add(hicon);
	DestroyIcon(hicon);
	
	m_ListBox.SetImageList(&m_ImageList, TVSIL_NORMAL);
    m_ImageList.SetBkColor(m_ListBox.GetBkColor());

	Setup();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}

void CObjectBrowser::Setup() 
{
	
	m_ListBox.DeleteAllItems();
	CString sBrowser;
	sBrowser = theWorkspace.LoadResourceString(IDS_ObjectBrowser);
	CString sPropSize;
	sPropSize = theWorkspace.LoadResourceString(IDS_PROP_FORMEVENTSIZE);
	EnableSaveRestore(sBrowser, sPropSize);
	
	if (m_pControl == NULL)
		return;

	
	// begin loading the main OleControl into the tree control
	HTREEITEM hMainParent = LoadOleObjectIntoTree(m_pControl);
	LoadAllAssociatedOleObjects(m_pControl);

	if (hMainParent != NULL)
		m_ListBox.Expand(hMainParent, TVE_EXPAND);

	ShowSizeGrip(TRUE);
	

	//ResizeControls(m_ptMinTrackSize.x, m_ptMinTrackSize.y);
	ResizeControls();

	
}

HTREEITEM CObjectBrowser::LoadOleObjectIntoTree(RefCountedPtr< COleControlObject > pControl) 
{
	std::vector< RefCountedPtr< COleControlObject > >::const_iterator pos = m_OleObjectList.begin();
	// lets make sure we do not insert any OleObjects more than once.
	while (pos != m_OleObjectList.end())
	{
		RefCountedPtr< COleControlObject > pObject = *pos++;
		if (pObject == pControl)
			return NULL;
	}

	// create the parent item.
	HTREEITEM hItem;

	if (pControl->GetType() == CtlActiveX)
	{
		CString sName = pControl->GetActiveXTypeName();
		hItem = m_ListBox.InsertItem(sName, TVI_ROOT, TVI_SORT);
	}
	else
	{
		if (pControl->GetType() < CtlLabel || pControl->GetType() > CtlFileDlgCtrl)		
		{
			switch (m_pDclForm->GetType())
			{
			case -2:
				hItem = m_ListBox.InsertItem(_T("Additional Useful Functions"), TVI_ROOT, TVI_SORT);		
				break;
			case VdclModal:
				hItem = m_ListBox.InsertItem(theWorkspace.LoadResourceString(IDS_MODALFORM2), TVI_ROOT, TVI_SORT);		
				break;
			case VdclModeless:
				hItem = m_ListBox.InsertItem(theWorkspace.LoadResourceString(IDS_MODELESSFORM2), TVI_ROOT, TVI_SORT);		
				break;
			case VdclDockable:
				hItem = m_ListBox.InsertItem(theWorkspace.LoadResourceString(IDS_DOCKABLEFORM2), TVI_ROOT, TVI_SORT);		
				break;
			case VdclConfigTab:
				hItem = m_ListBox.InsertItem(theWorkspace.LoadResourceString(IDS_CONFIGTAB2), TVI_ROOT, TVI_SORT);		
				break;
			case VdclFileDialog:
				hItem = m_ListBox.InsertItem(theWorkspace.LoadResourceString(IDS_FILEDLG), TVI_ROOT, TVI_SORT);		
				break;			
			}
		}
		else
			hItem = m_ListBox.InsertItem(GetControlName(pControl), TVI_ROOT, TVI_SORT);
	}
		
	size_t nCount = m_OleObjectList.size();
	// set the item data to the index where the main m_pControl object goes.
	m_ListBox.SetItemData(hItem, nCount);

	// here we are going to hold the pointers to the ole object's arx control object info holders
	m_OleObjectList.push_back(pControl);
	
	// set the image index as the OleObject icon
	m_ListBox.SetItemImage(hItem, 5, 5);
	
	// call the method to load all the object's info.
	LoadInfoTree(pControl, hItem, nCount);	

	return hItem;
}

void CObjectBrowser::LoadAllAssociatedOleObjects(CDclControlObject *pControl) 
{
	POSITION pos = pControl->GetPropertyList().GetHeadPosition();

	// do loop to inspect all property objects
	while (pos != NULL)
	{
		RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyList().GetNext(pos);
		if (pProp != NULL)
		{
			AxInterfaceDescriptor* pAxDesc = pProp->GetAxInterfaceDescriptorPtr();
			if( pAxDesc )
			{
				SearchMethods(pProp);
				RefCountedPtr< COleControlObject > pOleObject;
				// check events
				pOleObject = activeProject->GetOleObject(pAxDesc->GetEvent());
				if (pOleObject != NULL)
					LoadOleObjectIntoTree(pOleObject);
				
				// check each property pointer type
				pOleObject = activeProject->GetOleObject(pAxDesc->GetProp());
				if (pOleObject != NULL)
					LoadOleObjectIntoTree(pOleObject);
				
				pOleObject = activeProject->GetOleObject(pAxDesc->GetPropGet());
				if (pOleObject != NULL)
					LoadOleObjectIntoTree(pOleObject);
				
				pOleObject = activeProject->GetOleObject(pAxDesc->GetPropPut());
				if (pOleObject != NULL)
					LoadOleObjectIntoTree(pOleObject);
				
				pOleObject = activeProject->GetOleObject(pAxDesc->GetPropPutRef());
				if (pOleObject != NULL)
					LoadOleObjectIntoTree(pOleObject);
			}
		}
	}
}


void CObjectBrowser::SearchMethods(RefCountedPtr< CPropertyObject > pProp) 
{
	size_t idx = pProp->size();
	while( idx-- > 0 )
	{
		RefCountedPtr< COleControlObject > pOleObject = activeProject->GetOleObject(pProp->GetAxInterfaceDescriptorPtr()->GetMethods()->at(idx));
		if (pOleObject != NULL)
			LoadOleObjectIntoTree(pOleObject);			
	}
}

void CObjectBrowser::LoadInfoTree(RefCountedPtr< COleControlObject > pControl, HTREEITEM hParentItem, int nIndex) 
{
	int nCount = pControl->GetPropertyList().GetCount();
	for (int i = 0; i<nCount; i++)
	{
		POSITION pos = pControl->GetPropertyList().FindIndex(i);
		if (pos != NULL)
		{			
			RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyList().GetAt(pos);
			if (pProp != NULL)
			{
				switch (pProp->GetType())
				{
				case PropCustom:
				case PropImageList:
				case PropStringArray:
				case PropIntArray:
				case PropActiveXPropPages:
					break;
				case PropActiveXMethods:
					{
						int nCount = pProp->size();
						if (nCount > 0)
						{
							nMethodIndex = i;
							for (int j=0; j<nCount; j++)
							{
								CString sTitle = pProp->GetAxInterfaceDescriptorPtr()->GetMethods()->at(j)->GetName();								
								HTREEITEM hItem = m_ListBox.InsertItem(sTitle, hParentItem, TVI_SORT);
								m_ListBox.SetItemData(hItem, j);
								m_ListBox.SetItemImage(hItem, 3, 3);
							}
						}
					break;
					}
				default:
					if (pProp->GetID() != nName && pProp->GetID() != nGlobalVarName)
					{
						CString sTitle = pProp->GetName();
						if (sTitle.GetLength() > 0)
						{
							HTREEITEM hItem = m_ListBox.InsertItem(sTitle, hParentItem, TVI_SORT);
							m_ListBox.SetItemData(hItem, i);
							switch (pProp->GetType())
							{
							case PropActiveXEvent:
								m_ListBox.SetItemImage(hItem, 2, 2);
								// if this is not the main OleObject, don't both showning events because they
								// can't see them or access them in the events tab pane
								//if (nIndex > 0)
								//	return;
								break;
							case PropActiveXProp:
							case PropActiveXRunTime:
							case PropActiveXEnum:
								m_ListBox.SetItemImage(hItem, 4, 4);							
								break;
							case PropLong:
							case PropString:
							case PropDouble:
							case PropBool:
							case PropEnum:
							case PropPicture:
								m_ListBox.SetItemImage(hItem, 4, 4);
								break;
							}						
						}
					}
					break;
				}
			}
		}
	}

	if (nIndex == 0)
	{
		if( m_pDclForm->GetType() == VdclFileDialog )
		{
			LoadMethods( theWorkspace.LoadResourceString( IDS_FILEDLGMTH ), hParentItem );
			if( pControl->GetType() == CtlFileDlgCtrl )
				LoadMethods(theWorkspace.LoadResourceString(IDS_FILEDLGMTH2), hParentItem);
			return;
		}
		else if (m_pDclForm->GetType() == -2)
		{
			LoadMethods(_T("BonusFunctions.mth"), hParentItem);
			return;
		}
		
		if (pControl->GetType() != CtlForm &&
				pControl->GetType() != CtlInvalid &&
				pControl->GetType() != CtlFileDlgCtrl)
			LoadMethods(theWorkspace.LoadResourceString(IDS_ALLCTRLMTH), hParentItem);
		
		// here we need to see which control it being displayed to load the correct method info
		switch (pControl->GetType())
		{
		//case VdclFileDialog:
		//	LoadMethods(theWorkspace.LoadResourceString(IDS_FILEDLGMTH2), hParentItem);
		//	break;
		case CtlTabStrip:
			LoadMethods(theWorkspace.LoadResourceString(IDS_TABMTH), hParentItem);
			break;
		case CtlListBox:
			LoadMethods(theWorkspace.LoadResourceString(IDS_LISTBOXMTH), hParentItem);
			break;
		case CtlListView:
			LoadMethods(theWorkspace.LoadResourceString(IDS_LISTCTRLMTH), hParentItem);
			break;
		case CtlForm: // a form
			LoadMethods(theWorkspace.LoadResourceString(IDS_FORMSMTH), hParentItem);
			break;
			
		case -2:
			
			break;
		case CtlGrid:
			LoadMethods(_T("Grid.mth"), hParentItem);
			break;
		case CtlImageComboBox:
			LoadMethods(_T("ImageComboBox.mth"), hParentItem);
			break;
		case CtlAnimate:
			LoadMethods(_T("AnimationCtrl.mth"), hParentItem);
			break;	
		case CtlDwgList:
			LoadMethods(theWorkspace.LoadResourceString(IDS_DWGLISTMTH), hParentItem);
			break;	
		case CtlBlockList:
			LoadMethods(theWorkspace.LoadResourceString(IDS_BLOCKLISTMTH), hParentItem);
			break;	
		case CtlPictureBox:
			LoadMethods(theWorkspace.LoadResourceString(IDS_PICBOXMTH), hParentItem);
			break;
		case CtlOptionList:
			LoadMethods(theWorkspace.LoadResourceString(IDS_OPTLISTMTH), hParentItem);
			break;
		case CtlBlockView:
			LoadMethods(theWorkspace.LoadResourceString(IDS_BLOCKVIEWMTH), hParentItem);
			break;
		case CtlHatch:
			LoadMethods(theWorkspace.LoadResourceString(IDS_HATCHMTH), hParentItem);
			break;
		case CtlSlideView:
			LoadMethods(theWorkspace.LoadResourceString(IDS_SLIDEVIEWMTH), hParentItem);
			break;
		case CtlTree:
			LoadMethods(theWorkspace.LoadResourceString(IDS_TREEMTH), hParentItem);
			break;
		case CtlComboBox:
			LoadMethods(theWorkspace.LoadResourceString(IDS_COMBOMTH), hParentItem);
			break;
		case CtlTextBox:
			LoadMethods(theWorkspace.LoadResourceString(IDS_TEXTBOXMTH), hParentItem);
			break;	
		case CtlDwgPreview:
			LoadMethods(theWorkspace.LoadResourceString(IDS_DWGPREVIEWMTH), hParentItem);
			break;
		case CtlActiveX:
			LoadMethods(theWorkspace.LoadResourceString(IDS_AXCTRLSMTH), hParentItem);
			break;	
		case CtlMonth:
			LoadMethods(theWorkspace.LoadResourceString(IDS_MONTHMTH), hParentItem);
			break;	
		case CtlGraphicButton:
			LoadMethods(theWorkspace.LoadResourceString(IDS_GRPBTNMTH), hParentItem);
			break;	
		case CtlHtmlCtrl:
			LoadMethods(theWorkspace.LoadResourceString(IDS_HTMLMTH), hParentItem);
			break;				
		}	
	}
	else
	{
		LoadMethods(theWorkspace.LoadResourceString(IDS_AXOBJMTH), hParentItem);
		return;
	}
}

void CObjectBrowser::LoadMethods(CString sFileName, HTREEITEM hParentItem)
{
	CString sMethodFile = theWorkspace.FindFile( sFileName );
	if( sMethodFile.IsEmpty() )
		//try it without path (probably won't help, but at least there will be a filename in the error message)
		sMethodFile = sFileName;

	try
	{
		CStdioFile fMthFile(sMethodFile, CFile::shareDenyWrite|CFile::modeRead);
		
		CString sLine;
		fMthFile.ReadString(sLine);
		CString sMethod2 = theWorkspace.LoadResourceString(IDS_METHOD2);
		CString sEOF = theWorkspace.LoadResourceString(IDS_EOF);
		
		// do loop until the end is found.
		while (sLine != sEOF && fMthFile.GetPosition() < fMthFile.GetLength())
		{
			if (sLine == sMethod2)
			{		
				fMthFile.ReadString(sLine);	
				HTREEITEM hItem = m_ListBox.InsertItem(sLine, hParentItem, TVI_SORT);
				// set item data to -1 to indicate the info must be loaded from file.
				m_ListBox.SetItemData(hItem, nNotSet); 
				m_ListBox.SetItemImage(hItem, 3, 3);							
			}
			fMthFile.ReadString(sLine);
		}
	}
	catch( CFileException* e )
	{
		e->ReportError();
		e->Delete();
	}
}

CString CObjectBrowser::StripMethodNameOfBrackets(CString sMethodName)
{
	CString sCompile;
	CString sChar;
	for (int i=0; i<sMethodName.GetLength(); i++)
	{
		sChar = sMethodName.Mid(i, 1);
		if (sChar != theWorkspace.LoadResourceString(IDS_BRACKETOPEN))
			sCompile += sChar;
		else
			return sCompile;
	}
	return sCompile;
}

bool CObjectBrowser::LoadFullMethod(CString sFileName, CString sMethodName, CString &sTitle, CString &sDesc, CString &sDefun1)
{
	CString sFuncName;
	CString sMethodFile = theWorkspace.FindFile( sFileName );
	if( sMethodFile.IsEmpty() )
		//try it without path (probably won't help, but at least there will be a filename in the error message)
		sMethodFile = sFileName;

	try
	{
		CStdioFile fMthFile(sMethodFile, CFile::shareDenyWrite|CFile::modeRead);
		
		CString sLine;
		fMthFile.ReadString(sLine);

		CString sMethod2 = theWorkspace.LoadResourceString(IDS_METHOD2);
		CString sEOF = theWorkspace.LoadResourceString(IDS_EOF);
		
		// do loop until the end is found.
		while (sLine != sEOF)
		{
			if (sLine == sMethod2)
			{		
				fMthFile.ReadString(sLine);	
				// if this is the method we are looking for.
				if (sMethodName == sLine)
				{
					fMthFile.ReadString(sLine);	// get past the [Prefix]
					fMthFile.ReadString(sLine);	
					sFuncName = sLine + StripMethodNameOfBrackets(sMethodName);
					fMthFile.ReadString(sLine);	// get past the [Desc]
					while (sLine.Left(nDeOffset) != theWorkspace.LoadResourceString(IDS_ARGS) && 
								lstrcmpi(sLine, _T("[Argument]")) != 0 &&
								fMthFile.GetPosition() < fMthFile.GetLength())
					{
						fMthFile.ReadString(sLine);	
						if (sLine.Left(nDeOffset) != theWorkspace.LoadResourceString(IDS_ARGS) && 
								lstrcmpi(sLine, _T("[Argument]")) != 0)
							sDesc += sLine;			
					}
					// clear the clipboard string holder
					m_sClipBoardDefun2.Empty();					

					sDefun1 = theWorkspace.LoadResourceString(IDS_PAR1);
					bool bHasReturn = false;
					// setup the return type
					if (sLine.Left(18) == theWorkspace.LoadResourceString(IDS_ARGS2))
					{
						// here we must setup the sTitle (first line) so it will show the Method + function short name + as something for return.
						sTitle = theWorkspace.LoadResourceString(IDS_BOLDMETHOD) + sMethodName + theWorkspace.LoadResourceString(IDS_B0CF1)
							+ sLine.Mid(19) + theWorkspace.LoadResourceString(IDS_CF0);
						sDefun1 += theWorkspace.LoadResourceString(IDS_SETQVALUE) + sFuncName + theWorkspace.LoadResourceString(IDS_CF02);
						m_sClipBoardDefun2 = theWorkspace.LoadResourceString(IDS_SETQVALUE2) + sFuncName + theWorkspace.LoadResourceString(IDS_SPACE);
						bHasReturn = true;
					}
					else
					{
						sTitle = theWorkspace.LoadResourceString(IDS_METHOD);
						sDefun1 += theWorkspace.LoadResourceString(IDS_CF2) + sFuncName + theWorkspace.LoadResourceString(IDS_CF02);
						m_sClipBoardDefun2 = theWorkspace.LoadResourceString(IDS_OPENBRACKET) + sFuncName + theWorkspace.LoadResourceString(IDS_SPACE);
					}
					

					fMthFile.ReadString(sLine);	// now we can get past the [Arguments] 
					// here we loop through all the remaining arguments and add to them to the tail of the autolisp syntax structure
					while (sLine != sMethod2 && !sLine.IsEmpty() && sLine != sEOF)
					{
						if (sLine == theWorkspace.LoadResourceString(IDS_ProjectFileName))
						{
							sDefun1 += theWorkspace.LoadResourceString(IDS_CF3) + m_pControl->GetKeyPath() + theWorkspace.LoadResourceString(IDS_CF0);
							m_sClipBoardDefun2 += m_pControl->GetKeyPath();
						}						
						else if (sLine == theWorkspace.LoadResourceString(IDS_ProjectName))
						{
							sDefun1 += theWorkspace.LoadResourceString(IDS_CF3) + theEditorWorkspace.GetActiveProjectName() + theWorkspace.LoadResourceString(IDS_CF0);
							m_sClipBoardDefun2 += theEditorWorkspace.GetActiveProjectName();
						}						
						else if (sLine.Left(3) == theWorkspace.LoadResourceString(IDS_AS))
						{
							sDefun1 += theWorkspace.LoadResourceString(IDS_CF5I) + sLine + theWorkspace.LoadResourceString(IDS_I0CF0);
							m_sClipBoardDefun2 += sLine;
						}
						else if (_tcsicmp(sLine, theWorkspace.LoadResourceString(IDS_BEGINLIST)) == 0)
						{
							sDefun1 += theWorkspace.LoadResourceString(IDS_CF05) + "list ";
							m_sClipBoardDefun2 += " (list ";
						}
						else if (_tcsicmp(sLine, theWorkspace.LoadResourceString(IDS_ENDLIST)) == 0)
						{
							sDefun1 += theWorkspace.LoadResourceString(IDS_CF04);
							m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_CLOSEBRACKET);
						}
						else if (!sLine.IsEmpty() && sLine != theWorkspace.LoadResourceString(IDS_SPACE) && sLine != theWorkspace.LoadResourceString(IDS_DOUBLESPACE) && sLine != theWorkspace.LoadResourceString(IDS_TRIPLESPACE)) 
						{
							sDefun1 += theWorkspace.LoadResourceString(IDS_CF1) + sLine ;
							// if the last char was not a ( add a space before we add the argument
							if (m_sClipBoardDefun2.Right(1) != theWorkspace.LoadResourceString(IDS_OPENBRACKET))
								m_sClipBoardDefun2 += _T(' ');
							m_sClipBoardDefun2 += sLine + _T(' ');
						}
						if( fMthFile.GetPosition() >= fMthFile.GetLength() )
							break;
						fMthFile.ReadString(sLine);	// now we can get past the [Arguments] 				
					}

					if (bHasReturn)
					{
						sDefun1 += theWorkspace.LoadResourceString(IDS_CF03);
						m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_DOUBLECLOSEBRACKET);
					}
					else
					{
						sDefun1 += theWorkspace.LoadResourceString(IDS_CF04);
						m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_CLOSEBRACKET);
					}

					sDefun1 += theWorkspace.LoadResourceString(IDS_PAR1);
					sDefun1 += theWorkspace.LoadResourceString(IDS_PAR1);
					break;
				}

				// hide the first copy button
				m_Copy1.ShowWindow(FALSE);

				// set the second copy button
				CString sCopy2Text;
				sCopy2Text = theWorkspace.LoadResourceString(IDS_COPYTOCLIP);			
				m_Copy2.SetWindowText(sCopy2Text);
				m_Copy2.ShowWindow(TRUE);
			}
			if( fMthFile.GetPosition() >= fMthFile.GetLength() )
				break;
			fMthFile.ReadString(sLine);
		}
	}
	catch( CFileException* e )
	{
		e->ReportError();
		e->Delete();
	}
	m_sClipBoardDefun2.Replace( _T("\\i0"), _T("") );
	m_sClipBoardDefun2.Replace( _T("\\i"), _T("") );
	m_sClipBoardDefun2.Replace( _T("\\par"), _T("\r\n") ); //change RTF paragraph breaks to ASCII linefeeds
	m_sClipBoardDefun2.Replace( _T("\\tab"), _T("\t") ); //change RTF tabs to ASCII tabs

	return true;
}

void CObjectBrowser::SelectionChanged(HTREEITEM hItem) 
{
	int nImage;
	int nSelImage;
	POSITION pos = NULL;
	CString sExtraText;
	CString sOleObject;
	sOleObject = theWorkspace.LoadResourceString(IDS_Odcl_AxControl_);
	CString sArgType;
	CString sGlobalVarName;
	hItem = m_ListBox.GetSelectedItem();

	if (hItem == NULL)
	{
		CString sRtf;
		m_RichBox.SetRTF(sRtf);			
		return;
	}

	m_Copy1.ShowWindow(FALSE);
	m_Copy2.ShowWindow(FALSE);
	m_Copy3.ShowWindow(FALSE);
		
	HTREEITEM hParent = m_ListBox.GetParentItem(hItem);
	if (hParent == NULL)
	{
		CString sRtf;
		m_RichBox.SetRTF(sRtf);					
		ResizeControls();
		return;
	}

	int nThisItemData = m_ListBox.GetItemData(hParent);

	RefCountedPtr< COleControlObject > pControl = m_OleObjectList.at(nThisItemData);
	if (pControl == NULL)
		return;

	sGlobalVarName = pControl->GetKeyPath();
	
	if (pControl != m_pControl)
	{
		sOleObject = theWorkspace.LoadResourceString(IDS_Odcl_AxObject_);
		sGlobalVarName = theWorkspace.LoadResourceString(IDS_OleObject);
	}
	
	m_ListBox.GetItemImage(hItem, nImage, nSelImage);
	nThisItemData = m_ListBox.GetItemData(hItem);

	if (nImage == 3/* a method*/)
		pos = pControl->GetPropertyList().FindIndex(nMethodIndex);
	else
		pos = pControl->GetPropertyList().FindIndex(nThisItemData);

	//if (pos == NULL && pControl->GetType() == -2)
	//{			
	//	CString sItemText;
	//	CString sTitle;
	//	CString sDesc;
	//	CString sVarType;
	//	CString sDefun1;
	//	CString sDefun2;
	//}

	if (pos != NULL)
	{			
		RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyList().GetAt(pos);
		if (pProp != NULL)
		{
			CString sTitle;
			CString sDesc;
			CString sVarType;
			CString sDefun1;
			CString sDefun2;
			if (nImage == 3/* a method*/)
			{
				if (nThisItemData == nNotSet)
				{
					CString sItemText = m_ListBox.GetItemText(hItem);
					
					if (pControl->GetType() == CtlActiveX)
					{
						LoadFullMethod(theWorkspace.LoadResourceString(IDS_AXCTRLSMTH), sItemText, sTitle, sDesc, sDefun1);
						// call the method to load info from the appropriate method info files.
						LoadFullMethod(theWorkspace.LoadResourceString(IDS_ALLCTRLMTH), sItemText, sTitle, sDesc, sDefun1);					
					}
					else if (pControl->GetID() == -1)
					{
						if (m_pDclForm->GetType() == VdclFileDialog)
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_FILEDLGMTH), sItemText, sTitle, sDesc, sDefun1);	
						else if (pControl->GetType() == -2)
							LoadFullMethod(_T("BonusFunctions.mth"), sItemText, sTitle, sDesc, sDefun1);	
						else
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_FORMSMTH), sItemText, sTitle, sDesc, sDefun1);	
					}
					else
					{
						if (m_pDclForm->GetType() == VdclFileDialog && pControl->GetType() != CtlFileDlgCtrl)
							// call the method to load info from the appropriate method info files.
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_FILEDLGMTH), sItemText, sTitle, sDesc, sDefun1);					
						
						else if (pControl->GetType() != CtlFileDlgCtrl)
							// call the method to load info from the appropriate method info files.
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_ALLCTRLMTH), sItemText, sTitle, sDesc, sDefun1);					

						// here we need to see which control it being displayed to load the correct method info
						switch (pControl->GetType())
						{
						
						case CtlFileDlgCtrl:
							LoadFullMethod(_T("FileDlgCtrl.mth"), sItemText, sTitle, sDesc, sDefun1);
							break;

						case CtlTabStrip:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_TABMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
		
						case CtlListBox:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_LISTBOXMTH), sItemText, sTitle, sDesc, sDefun1);
							break;

						case CtlListView:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_LISTCTRLMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
	
						case CtlGrid:
							LoadFullMethod(_T("Grid.mth"), sItemText, sTitle, sDesc, sDefun1);
							break;
	
						case CtlForm: // a form
							if (m_pDclForm->GetType() == VdclFileDialog && pControl->GetType() != CtlFileDlgCtrl)
								LoadFullMethod(_T("FileDlg.mth"), sItemText, sTitle, sDesc, sDefun1);
							else
								LoadFullMethod(_T("Forms.mth"), sItemText, sTitle, sDesc, sDefun1);
							break;

						case -2: // bonus functions
							LoadFullMethod(_T("BonusFunctions.mth"), sItemText, sTitle, sDesc, sDefun1);
							break;

						case CtlImageComboBox:
							LoadFullMethod(_T("ImageComboBox.mth"), sItemText, sTitle, sDesc, sDefun1);
							break;
	
						case CtlAnimate:
							LoadFullMethod(_T("AnimationCtrl.mth"), sItemText, sTitle, sDesc, sDefun1);
							break;
	
						case CtlDwgList:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_DWGLISTMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
	
						case CtlBlockList:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_BLOCKLISTMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
	
						case CtlPictureBox:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_PICBOXMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
	
						case CtlOptionList:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_OPTLISTMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
	
						case CtlBlockView:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_BLOCKVIEWMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
						
						case CtlHatch:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_HATCHMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
						
						case CtlSlideView:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_SLIDEVIEWMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
						
						case CtlTree:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_TREEMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
						
						case CtlComboBox:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_COMBOMTH), sItemText, sTitle, sDesc, sDefun1);
							break;

						case CtlTextBox:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_TEXTBOXMTH), sItemText, sTitle, sDesc, sDefun1);
							break;

						case CtlDwgPreview:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_DWGPREVIEWMTH), sItemText, sTitle, sDesc, sDefun1);
							break;

						case CtlActiveX:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_AXCTRLSMTH), sItemText, sTitle, sDesc, sDefun1);
							break;
	
						case CtlMonth:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_MONTHMTH), sItemText, sTitle, sDesc, sDefun1);
							break;	

						case CtlGraphicButton:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_GRPBTNMTH), sItemText, sTitle, sDesc, sDefun1);
							break;	

						case CtlHtmlCtrl:
							LoadFullMethod(theWorkspace.LoadResourceString(IDS_HTMLMTH), sItemText, sTitle, sDesc, sDefun1);
							break;	
						}
					}
					
				}
				if (nThisItemData > nNotSet)
				{
					sTitle = theWorkspace.LoadResourceString(IDS_METHOD);
					if (pProp->GetAxInterfaceDescriptorPtr() != NULL) {
						sDesc = pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodDesc(nThisItemData);
						sVarType = GetTypeName(pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodReturnType(nThisItemData),
																	 pProp->GetAxInterfaceDescriptorPtr()->GetAxMethod(nThisItemData));
						
						// here we need to put in OleObject closing instructions if required.
						if (pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodReturnType(nThisItemData) == VT_DISPATCH ||
							pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodReturnType(nThisItemData) == VT_UNKNOWN)
						{
							if (!sDesc.IsEmpty())
								sDesc += theWorkspace.LoadResourceString(IDS_PARPAR);						
							if (sVarType == theWorkspace.LoadResourceString(IDS_OleObject) || sVarType == CString())
								sDesc += theWorkspace.LoadResourceString(IDS_OLENOTE1);
							else					
							{
								sDesc += theWorkspace.LoadResourceString(IDS_OLENOTE2) + sVarType + theWorkspace.LoadResourceString(IDS_OLENOTE3);

							}
						}
					}
					sDefun1 = theWorkspace.LoadResourceString(IDS_PAR1);
					
					// clear the clipboard string holder
					m_sClipBoardDefun2;
					// setup the defun is it is to return a value
					if (!sVarType.IsEmpty())
					{
						sDefun1 += theWorkspace.LoadResourceString(IDS_SETQVALUE3);
						m_sClipBoardDefun2 = theWorkspace.LoadResourceString(IDS_SETQVALUE2) + theWorkspace.LoadResourceString(IDS_SPACE);						
					}
					// add the DoMethod
					sDefun1 += theWorkspace.LoadResourceString(IDS_CF22) + sOleObject + theWorkspace.LoadResourceString(IDS_DOMETHOD) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_CF0CF3B);
					m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_OPENBRACKET) + sOleObject + theWorkspace.LoadResourceString(IDS_DOMETHOD2) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_QUOTE);
					// add the method name
					sDefun1 += m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_CF0B0);
					m_sClipBoardDefun2 += m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_QUOTE);
					
					if (pProp->GetAxInterfaceDescriptorPtr() != NULL) {
						size_t nCount = pProp->GetAxInterfaceDescriptorPtr()->CountAxMethodParams(nThisItemData);
						for (size_t i = 0; i < nCount; ++i)
						{
							// add the argument name
							sDefun1 += theWorkspace.LoadResourceString(IDS_CF12) + pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodParamName(nThisItemData, i);
							m_sClipBoardDefun2 += CString(theWorkspace.LoadResourceString(IDS_SPACE)) + pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodParamName(nThisItemData, i); 
							// add the [as ...]
							sArgType = pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodParamVarType(nThisItemData, i);
					
							if (sArgType == CString())
							{
								sArgType = theWorkspace.LoadResourceString(IDS_OPTIONALNIL);
								sDefun1 += theWorkspace.LoadResourceString(IDS_CF5I2) + sArgType + theWorkspace.LoadResourceString(IDS_I);
								m_sClipBoardDefun2 += sArgType;
							}
							else if (!sArgType.IsEmpty())
							{
								sDefun1 += theWorkspace.LoadResourceString(IDS_CF5IAS) + sArgType + theWorkspace.LoadResourceString(IDS_CLOSEBRACKET2) + theWorkspace.LoadResourceString(IDS_I);
								m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_OPENAS) + sArgType + theWorkspace.LoadResourceString(IDS_CLOSEBRACKET2);
							}
							else
								sDefun1 += theWorkspace.LoadResourceString(IDS_CF5);
							// if it is the second to last argument add close the color.
							if (i < nCount - 1)
							{
								sDefun1 += theWorkspace.LoadResourceString(IDS_CF0) + theWorkspace.LoadResourceString(IDS_SPACE);
								m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_SPACE);
							}
						}
					}

					// close the brackets
					if (!sVarType.IsEmpty())
					{
						sDefun1 += theWorkspace.LoadResourceString(IDS_CF0PAR);
						m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_DOUBLECLOSEBRACKET);
					}
					else
					{
						sDefun1 += theWorkspace.LoadResourceString(IDS_CF0PAR2);
						m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_CLOSEBRACKET);
					}

					// add a more info disclaimer.
					sDefun1 += theWorkspace.LoadResourceString(IDS_FORMOREINFO);
					// add a microsoft disclaimer.
					if (pControl->IsMicrosoftActiveXCtrl() == TRUE)
						sDefun1 += theWorkspace.LoadResourceString(IDS_TOFINDIT);
							
					
					// set the second copy button
					CString sCopy2Text = theWorkspace.LoadResourceString(IDS_COPYTOCLIP);			
					m_Copy2.SetWindowText(sCopy2Text);
					m_Copy2.ShowWindow(TRUE);
				}
			}
			else if (pProp->GetType() == PropActiveXEvent)
			{
				sTitle = theWorkspace.LoadResourceString(IDS_EVENTTITLE);
				sDesc = pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->GetDesc();

				if (!pProp->GetStringValue().IsEmpty())
				{
					sDefun1 = pProp->GetStringValue();
					sDefun1 += theWorkspace.LoadResourceString(IDS_PAR);
				}				
			}
			else if (pProp->GetType() == PropEvent)
			{
				sTitle = theWorkspace.LoadResourceString(IDS_EVENTTITLE);

				// load the event name
				CString sEventName = GetPropertyName(pProp->GetID());

				// get the args and desc
				CString sEventArgs;
				LoadArgsNDesc(pProp->GetID(), pControl, sEventArgs, sDesc);
				
				sDefun1 = theWorkspace.LoadResourceString(IDS_PAR);
				if (!pProp->GetStringValue().IsEmpty())
				{
					if (!sEventArgs.IsEmpty())
						sDefun1 += pProp->GetStringValue() + theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_BRACKETOPEN) + sEventArgs + theWorkspace.LoadResourceString(IDS_CLOSEBRACKET3);						
					else
						sDefun1 += pProp->GetStringValue() + theWorkspace.LoadResourceString(IDS_DOUBLEBRACKET);						

					sDefun1 += theWorkspace.LoadResourceString(IDS_PAR);
				}
				else
				{
					sDefun1 += theWorkspace.LoadResourceString(IDS_C);
					sDefun1 += pControl->GetKeyPath() + _T("_On") + sEventName;
					if (!sEventArgs.IsEmpty())
						sDefun1 += theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_OPENBRACKET) + sEventArgs + theWorkspace.LoadResourceString(IDS_CLOSEBRACKET3);
					else
						sDefun1 += theWorkspace.LoadResourceString(IDS_DOUBLEBRACKET);
					sDefun1 += theWorkspace.LoadResourceString(IDS_PAR);
				}
				
			}
			else if (pProp->GetType() == PropActiveXProp || pProp->GetType() == PropActiveXRunTime)
			{
				// clear the clipboard string holder2
				m_sClipBoardDefun1;
				m_sClipBoardDefun2;
				
				AxPropertyDescriptor* pAxPropGet = pProp->GetAxInterfaceDescriptorPtr()->GetGetDescriptor();
				AxPropertyDescriptor* pAxPropPut = pProp->GetAxInterfaceDescriptorPtr()->GetPutDescriptor();
				sDesc = pProp->GetAxInterfaceDescriptorPtr()->GetDesc();
				sTitle = theWorkspace.LoadResourceString(IDS_THEPROPERTY);
				
				// do a special override for the color set properties
				if (pAxPropPut && pAxPropPut->GetGuid() == GUID_COLOR)
				{
					sDefun1 = theWorkspace.LoadResourceString(IDS_PARCF2) + sOleObject + theWorkspace.LoadResourceString(IDS_SETCOLOR) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_CF0CF3B1) + m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_QUOTEB0);
					m_sClipBoardDefun1 = theWorkspace.LoadResourceString(IDS_OPENBRACKET) + sOleObject + theWorkspace.LoadResourceString(IDS_SETCOLOR2) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_QUOTE) + m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_QUOTE) + theWorkspace.LoadResourceString(IDS_SPACE);
		
					sVarType = GetTypeName(pProp->GetAxInterfaceDescriptorPtr()->GetType(), NULL, pAxPropPut);
		
					sDefun1 += theWorkspace.LoadResourceString(IDS_REDCOLORDESC2);
					m_sClipBoardDefun1 += theWorkspace.LoadResourceString(IDS_REDCOLORDESC);
					sDefun1 += theWorkspace.LoadResourceString(IDS_CLOSEBRACKET) + theWorkspace.LoadResourceString(IDS_PAR);
					m_sClipBoardDefun1 += theWorkspace.LoadResourceString(IDS_CLOSEBRACKET);

					if (sDesc.IsEmpty())
						sDesc = theWorkspace.LoadResourceString(IDS_DESCNOTSET);

					sDesc += theWorkspace.LoadResourceString(IDS_SETCOLORDESC);
						
					// show the first copy button
					m_Copy1.ShowWindow(TRUE);
				}
					
				// if a put property
				if (pAxPropPut && pAxPropPut->GetGuid() != GUID_COLOR)
				{
					// lets set the put property
					sDefun1 = theWorkspace.LoadResourceString(IDS_PARCF2) + sOleObject + theWorkspace.LoadResourceString(IDS_SETPROP3) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_CF0CF3BQ);
					m_sClipBoardDefun1 = theWorkspace.LoadResourceString(IDS_OPENBRACKET) + sOleObject + theWorkspace.LoadResourceString(IDS_SETPROP4) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_QUOTE);

					sVarType = GetTypeName(pProp->GetAxInterfaceDescriptorPtr()->GetType(), NULL, pAxPropPut);
		
					if (sVarType == theWorkspace.LoadResourceString(IDS_PROP_PICTURE))
					{
						if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IPictureDisp)
						{
							CString sLoad;
							sExtraText = theWorkspace.LoadResourceString(IDS_PICTURESHORTCUT);
							sExtraText += CString() + sOleObject + theWorkspace.LoadResourceString(IDS_LOADPIC) + sGlobalVarName;
							sLoad = theWorkspace.LoadResourceString(IDS_PICTURESHORTCUT2);
							m_sClipBoardDefun3 = theWorkspace.LoadResourceString(IDS_OPENBRACKET) + sOleObject + theWorkspace.LoadResourceString(IDS_LOADPIC2) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_PICDESC);
							sExtraText += sLoad;
							m_Copy3.ShowWindow(TRUE);
		
						}
					}
					
					// here we need to put in OleObject closing instructions if required.
					if (pProp->GetAxInterfaceDescriptorPtr()->GetType() == VT_DISPATCH ||
							pProp->GetAxInterfaceDescriptorPtr()->GetType() == VT_UNKNOWN)
					{
						if (!sDesc.IsEmpty())
							sDesc += theWorkspace.LoadResourceString(IDS_PARPAR);
						if (sVarType == theWorkspace.LoadResourceString(IDS_OleObject) || sVarType == CString())
							sDesc += theWorkspace.LoadResourceString(IDS_WHENFIN);
						else						
							sDesc += theWorkspace.LoadResourceString(IDS_WHENFIN2) + sVarType + theWorkspace.LoadResourceString(IDS_WHENFIN3);
					}
					
					// if the set property does not have arguments
					if (pAxPropPut == NULL || pAxPropPut->GetArgs().empty())
					{
						sDefun1 += m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_NEWVAL2);
						m_sClipBoardDefun1 += m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_NEWVAL);
					}
					// if it does have arguments then we must add them
					else
					{
						sDefun1 += m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_CF0B0);
						m_sClipBoardDefun1 += m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_QUOTE);

						size_t nCount = pAxPropPut->GetArgs().size();
						for (size_t i=0; i<nCount; i++)
						{
							CString sArg = pAxPropPut->GetArgs()[i].name;
							if (sArg.IsEmpty())
								sArg = theWorkspace.LoadResourceString(IDS_NEWVAL3);
							sDefun1 += theWorkspace.LoadResourceString(IDS_CF12) + sArg;
							m_sClipBoardDefun1 += theWorkspace.LoadResourceString(IDS_SPACE) + sArg;
							sArgType = GetTypeName(pAxPropPut->GetArgs()[i].vt, NULL, pAxPropPut);
							if (sArgType == CString())
							{
								sArgType = theWorkspace.LoadResourceString(IDS_OPTIONALNILASB);
								sDefun1 += theWorkspace.LoadResourceString(IDS_CF5IB) + sArgType + theWorkspace.LoadResourceString(IDS_CBI0);
								m_sClipBoardDefun1 += theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_OPENBRACKET2) + sArgType + theWorkspace.LoadResourceString(IDS_CLOSEBRACKET2);
							}
							else if (!sArgType.IsEmpty())
							{						
								sDefun1 += theWorkspace.LoadResourceString(IDS_CF5IAS2) + sArgType + theWorkspace.LoadResourceString(IDS_CBI0);
								m_sClipBoardDefun1 += theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_OPENAS) + sArgType + theWorkspace.LoadResourceString(IDS_CLOSEBRACKET2);
							}
							else
								sDefun1 += theWorkspace.LoadResourceString(IDS_CF5);
							if (i < nCount-1)
							{
								sDefun1 += theWorkspace.LoadResourceString(IDS_CF02);
								m_sClipBoardDefun1 += theWorkspace.LoadResourceString(IDS_SPACE);
							}
						}							
					}
					sDefun1 += theWorkspace.LoadResourceString(IDS_CLOSEBRACKET) + theWorkspace.LoadResourceString(IDS_PAR);
					m_sClipBoardDefun1 += theWorkspace.LoadResourceString(IDS_CLOSEBRACKET);
					// show the first copy button
					m_Copy1.ShowWindow(TRUE);
				}
				else
				{
					// hide the first copy button
					m_Copy1.ShowWindow(TRUE);
				}

				// if a get property
				if (pAxPropGet != NULL)
				{
					sVarType = GetTypeName(pProp->GetAxInterfaceDescriptorPtr()->GetType(), NULL, pAxPropGet);

					// lets set the get property
					sDefun2 = theWorkspace.LoadResourceString(IDS_PARSETVALUE) + sOleObject + theWorkspace.LoadResourceString(IDS_GETPROPCF0CF3) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_CF0CF3B2);
					m_sClipBoardDefun2 = theWorkspace.LoadResourceString(IDS_SETQVAL) + sOleObject + theWorkspace.LoadResourceString(IDS_GETPROP) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_QUOTE);
					sDefun2 += m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_CF0B0);
					m_sClipBoardDefun2 += m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_QUOTE);

					// lets add the arguments
					if (pAxPropGet != NULL && !pAxPropGet->GetArgs().empty())
					{					
						size_t nCount = pAxPropGet->GetArgs().size();
						for (size_t i=0; i<nCount; i++)
						{
							CString sArg = pAxPropGet->GetArgs()[i].name;
							if (sArg.IsEmpty())
								sArg = theWorkspace.LoadResourceString(IDS_NEWVAL3);
							sDefun2 += theWorkspace.LoadResourceString(IDS_CF12) + sArg;
							m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_SPACE) + sArg;
							sArgType = GetTypeName(pAxPropGet->GetArgs()[i].vt, NULL, pAxPropGet);
							if (sArgType == CString())
							{
								sArgType = theWorkspace.LoadResourceString(IDS_OPTIONALNILASB);
								sDefun2 += theWorkspace.LoadResourceString(IDS_CF5IB) + sArgType + theWorkspace.LoadResourceString(IDS_CBI0);
								m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_OPENBRACKET2) + sArgType + theWorkspace.LoadResourceString(IDS_CLOSEBRACKET2);
							}
							else if (!sArgType.IsEmpty())
							{														
								sDefun2 += theWorkspace.LoadResourceString(IDS_CF5IAS2) + sArgType + theWorkspace.LoadResourceString(IDS_CBI0);
								m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_OPENAS) + sArgType + theWorkspace.LoadResourceString(IDS_CLOSEBRACKET2);
							}
							if (i < nCount - 1)
							{
								sDefun2 += theWorkspace.LoadResourceString(IDS_CF02);
								m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_SPACE);
							}
						}
					}
					sDefun2 += theWorkspace.LoadResourceString(IDS_DQPAR);
					m_sClipBoardDefun2 += theWorkspace.LoadResourceString(IDS_DOUBLECLOSEBRACKET);	

					// set the second copy button
					CString sCopy2Text;
					sCopy2Text = theWorkspace.LoadResourceString(IDS_COPYGETTOCLIP);			
					m_Copy2.SetWindowText(sCopy2Text);
					m_Copy2.ShowWindow(TRUE);
				}
				else
				{					
					m_Copy2.ShowWindow(FALSE);
				}

				
			}
			else
			{
				if (m_pControl->GetType() == CtlFileDlgCtrl || m_pControl->GetType() == nNotSet)
				{

					sTitle = theWorkspace.LoadResourceString(IDS_THEPROPERTY);
					sDesc = pProp->GetDocumentationDesc();
					sDesc += theWorkspace.LoadResourceString(IDS_PARPAR) + theWorkspace.LoadResourceString(IDS_DESIGNTIMEONLY);

				}
				else
				{	
					sDesc = pProp->GetDocumentationDesc();
						
					switch (pProp->GetID())
					{
					case nComboBoxStyle:
					case nButtonStyle:
					case nFilterStyle:
					case nMultiRow:
					case nOrientation:
					case nRectStyle:
					case nJustification:
					case nAutoHScroll:
					case nAutoVScroll:
					case nDropDownHeight:
					case nMultiColumn:
					case nSelectStyle:
					case nNoIntegralHeight:
					case nSorted:
					case nUseTabStops:
					case nDisableNoScroll:
					case nSmoothProgress:
					case nAutoWrap:
					case nMinTabWidth:
					case nResizable:
					case nShowSelectAlways:
					case nHasLines:
					case nLinesAtRoot:
					case nHasButtons:
					case nEditLabels:
					case nCheckBoxes:
					case nIcon:
					case nShowEditBox:
					case nInputFilter:
					case nIsTabStop:
					case nBeginGroup:
					case nListViewStyle:
					case nListViewSort:
					case nListViewIconAlign:
					case nBlockListStyle:
					case nAutoArrange:
					case nColHeader:
					case nLabelWrap:
					case nAllowScrolling:
					case nIconXSpacing:
					case nIconYSpacing:
					case nGridLines:
					case nFullRowSelect:
					case nFontSizeStyle:
					case nReturnAsTab:
					case nSingleExpanded:
					case nRowHeight:
					case nDefSelIndex:
					case nDockableSides:
					case nDefaultDockedSide:
					case nTitleBarText:
					case nMinDialogWidth:
					case nMaxDialogWidth:
					case nMinDialogHeight:
					case nMaxDialogHeight:
						{
							sTitle = theWorkspace.LoadResourceString(IDS_THEPROPERTY);
							sDesc = pProp->GetDocumentationDesc();
							CString sMsg;
							if (m_pControl->GetType() < CtlLabel || m_pControl->GetType() > CtlFileDlgCtrl)
								sMsg = theWorkspace.LoadResourceString(IDS_RUNTIMENOTALLOWEDDLG);
							else
								sMsg = theWorkspace.LoadResourceString(IDS_RUNTIMENOTALLOWEDCTRL);

							sDesc += theWorkspace.LoadResourceString(IDS_PARPAR) + sMsg;
							break;
						}
					default:
						{
							if ((pProp->GetID() == nWidth || pProp->GetID() == nHeight) &&
									pControl->GetID() == -1)
							{
								sTitle = theWorkspace.LoadResourceString(IDS_THEPROPERTY);
								sDesc = pProp->GetDocumentationDesc();
								CString sMsg;
								sMsg = theWorkspace.LoadResourceString(IDS_DESIGNTIMEONLY);
								sDesc += theWorkspace.LoadResourceString(IDS_PARPAR) + sMsg;
							}
							else
							{
								switch(pProp->GetType())
								{
									case PropLong:
										sVarType = theWorkspace.LoadResourceString(IDS_LONG);
										break;
									case PropEnum:
									case PropPicture:
										sVarType = theWorkspace.LoadResourceString(IDS_INT);
										break;
									case PropString:
										sVarType = theWorkspace.LoadResourceString(IDS_STRING);
										break;
									case PropDouble:
										sVarType = theWorkspace.LoadResourceString(IDS_REAL);
										break;
									case PropBool:
										sVarType = theWorkspace.LoadResourceString(IDS_BOOLEAN);
										break;					
								}		

								// lets set the Put property
								//sDefun1 = theWorkspace.LoadResourceString(IDS_SETPROP) + sGlobalVarName + theWorkspace.LoadResourceString(IDS_CF0CF3B2);
								sDefun1 = CString("\\par (\\cf2 Odcl_Control_Set") + m_ListBox.GetItemText(hItem) + " \\cf0 \\cf3" + sGlobalVarName + " ";
								m_sClipBoardDefun1 = CString("(Odcl_Control_Set") + m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_SPACE);
								sDefun1 += "\\cf1 newValue [as " + sVarType + "]\\cf0 ) " + theWorkspace.LoadResourceString(IDS_PAR);
								m_sClipBoardDefun1 += sGlobalVarName + " newValue [as " + sVarType + theWorkspace.LoadResourceString(IDS_CLOSEBRACKETS);
								
								// lets set the get property
								sDefun2 = CString("\\par (\\cf2 Setq \\cf1 Value \\cf0 (\\cf2 Odcl_Control_Get") + m_ListBox.GetItemText(hItem) + " \\cf0 \\cf3";
								m_sClipBoardDefun2 = "(Setq Value (Odcl_Control_Get" + m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_SPACE);
								sDefun2 += sGlobalVarName + "\\cf0))";
								m_sClipBoardDefun2 += sGlobalVarName + theWorkspace.LoadResourceString(IDS_DOUBLECLOSEBRACKET);
			
								// set the title.
								sTitle = theWorkspace.LoadResourceString(IDS_THEPROPERTY);
								
								// hide the first copy button
								m_Copy1.ShowWindow(TRUE);
			
								// set the second copy button
								CString sCopy2Text;
								sCopy2Text = theWorkspace.LoadResourceString(IDS_COPYGETTOCLIP);			
								m_Copy2.SetWindowText(sCopy2Text);
								m_Copy2.ShowWindow(TRUE);
							}
						}
					}
				}
			}
			
			if (sDesc.IsEmpty())
				sDesc = theWorkspace.LoadResourceString(IDS_DESCNOTSET);

			CHARFORMAT cf;

			// set the first default char settings
			cf.dwMask = CFM_STRIKEOUT|CFM_BOLD;
			cf.dwEffects = NULL;
			m_RichBox.SetDefaultCharFormat(cf);

			CString sRtf;
			CString sRtfHeader;

			// add the headers to the RTF string
			sRtfHeader = theWorkspace.LoadResourceString(IDS_RTFHEADER1);
			sRtf = sRtfHeader + theWorkspace.LoadResourceString(IDS_RN);
			sRtfHeader = theWorkspace.LoadResourceString(IDS_RTFHEADER2);
			sRtf += sRtfHeader + theWorkspace.LoadResourceString(IDS_RN);
			sRtfHeader = theWorkspace.LoadResourceString(IDS_RTFHEADER3);
			sRtf += sRtfHeader + theWorkspace.LoadResourceString(IDS_SPACE);
			// set the title then start the bold statment
			sRtf += sTitle;
			
			if (sTitle.GetLength() < nTitleLength)
				// add the title name and close the bold
				sRtf += theWorkspace.LoadResourceString(IDS_B) + m_ListBox.GetItemText(hItem) + theWorkspace.LoadResourceString(IDS_B0CF0);
				
			//if the var type has been set
			if (!sVarType.IsEmpty())
			{
				// add it to the RTF text as the color red.
				sRtf += theWorkspace.LoadResourceString(IDS_ISA) + sVarType + theWorkspace.LoadResourceString(IDS_CF02);
			}
			sRtf += theWorkspace.LoadResourceString(IDS_PAR);
			sRtf += theWorkspace.LoadResourceString(IDS_PAR);
			// add the description
			sRtf += sDesc + theWorkspace.LoadResourceString(IDS_SPACE) + theWorkspace.LoadResourceString(IDS_PAR);		
			
			// close the RTF statement
			if (!sDefun1.IsEmpty() && sDefun2.IsEmpty())
			{
				sRtf += theWorkspace.LoadResourceString(IDS_ALISPSYN);
				// add it to the RTF text as the color red.
				sRtf += sDefun1;
			}
			else if (!sDefun1.IsEmpty() && !sDefun2.IsEmpty())
			{
				sRtf += theWorkspace.LoadResourceString(IDS_ALSPS);
				// add it to the RTF text as the color red.
				sRtf += sDefun1;
				sRtf += theWorkspace.LoadResourceString(IDS_ALGPS);
				// add it to the RTF text as the color red.
				sRtf += sDefun2;
			}			
			else if (!sDefun2.IsEmpty())
			{
				m_Copy1.ShowWindow(FALSE);
				m_Copy2.ShowWindow(TRUE);
				sRtf += theWorkspace.LoadResourceString(IDS_ALGPS);
				// add it to the RTF text as the color red.
				sRtf += sDefun2;
			}
			
			sRtf += sExtraText;
			sRtf += theWorkspace.LoadResourceString(IDS_PARRN);
			// add any extra text if any extists.
			
			m_RichBox.SetRTF(sRtf);			
		}
	}
	ResizeControls();
}


void CObjectBrowser::OnClickListbox(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// get the selected item
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
		
	// get the selected item
	TV_ITEM SelectedItem = pNMTreeView->itemNew;

	if (SelectedItem.hItem !=NULL)
		SelectionChanged(SelectedItem.hItem);
	else
	{
		HTREEITEM hItem = m_ListBox.GetSelectedItem();
		SelectionChanged(SelectedItem.hItem);
	}
	

	*pResult = 0;
}

void CObjectBrowser::OnReturnListbox(NMHDR* pNMHDR, LRESULT* pResult) 
{
	
	*pResult = 0;
}

void CObjectBrowser::OnDblclkListbox(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	
	*pResult = 0;
}

void CObjectBrowser::OnSelchangedListbox(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	// get the selected item
	TV_ITEM SelectedItem = pNMTreeView->itemNew;

	if (SelectedItem.hItem !=NULL)
		SelectionChanged(SelectedItem.hItem);
	else
	{
		HTREEITEM hItem = m_ListBox.GetSelectedItem();
		SelectionChanged(SelectedItem.hItem);	
	}
	*pResult = 0;
}

void CObjectBrowser::OnCopy2() 
{
	//CString sUnsavedProject = theWorkspace.LoadResourceString(IDS_PROJECT);
	//if (m_pControl->GetKeyPath().Left(sUnsavedProject.GetLength()) == sUnsavedProject)
	if( theWorkspace.GetActiveDocument()->GetPathName().IsEmpty() )
	{
		int nWhatNext = MessageBox( theWorkspace.LoadResourceString(IDS_RENAMEPROJECT),
																theWorkspace.LoadResourceString(IDR_MAINFRAME),
																MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 );
		if( nWhatNext == IDYES )
		{
			if( !theWorkspace.GetActiveDocument()->DoFileSave() )
				return;
		}
		else if( nWhatNext != IDNO )
			return;
	}

	CString source = m_sClipBoardDefun2; 
	//put your text in source
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		TCHAR * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, (source.GetLength()+1) * sizeof(TCHAR));
		buffer = (TCHAR*)GlobalLock(clipbuffer);
		lstrcpyn(buffer, LPCTSTR(source), source.GetLength() + 1);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}

}

void CObjectBrowser::OnCopy1() 
{
	//CString sUnsavedProject = theWorkspace.LoadResourceString(IDS_PROJECT);
	//if (m_pControl->GetKeyPath().Left(sUnsavedProject.GetLength()) == sUnsavedProject)
	if( theWorkspace.GetActiveDocument()->GetPathName().IsEmpty() )
	{
		int nWhatNext = MessageBox( theWorkspace.LoadResourceString(IDS_RENAMEPROJECT),
																theWorkspace.LoadResourceString(IDR_MAINFRAME),
																MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 );
		if( nWhatNext == IDYES )
		{
			if( !theWorkspace.GetActiveDocument()->DoFileSave() )
				return;
		}
		else if( nWhatNext != IDNO )
			return;
	}

	CString source = m_sClipBoardDefun1; 
	//put your text in source
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		TCHAR * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, (source.GetLength()+1) * sizeof(TCHAR));
		buffer = (TCHAR*)GlobalLock(clipbuffer);
		lstrcpyn(buffer, LPCTSTR(source), source.GetLength() + 1);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
	
}

void CObjectBrowser::OnDestroy() 
{
	CResizableDialog::OnDestroy();
	m_OleObjectList.clear();
}

void CObjectBrowser::OnSize(UINT nType, int cx, int cy) 
{
	CResizableDialog::OnSize(nType, cx, cy);
	ResizeControls(cx, cy);
}

void CObjectBrowser::ResizeControls(int cx, int cy) 
{
	int nCopyCount = 0;
	if (IsWindow(m_RichBox.m_hWnd))
	{
		CRect rc(nDeLeft,nDeTop,cx-nDeRight, cy-nDeBottom);
			
		if (cx == nNotSet)
		{
			CRect rcDlg;
			CResizableDialog::GetClientRect(&rcDlg);
			cx = rcDlg.Width();
			cy = rcDlg.Height();
			rc.SetRect(nDeLeft,nDeTop,cx-nDeRight, cy-nDeBottom);	
		}
		m_RichBox.MoveWindow(rc, TRUE);

		CRect rcOK(rc.right + nDeOffset,nDeTop,rc.right + nDeOffset65, nDeButtonHeigth);
		m_OK.MoveWindow(rcOK, TRUE);
		
		CRect rc2(11,nDeTop,rc.left - 3, cy-nDeOffset);
		m_ListBox.MoveWindow(rc2, TRUE);
		
		CRect rc3(nDeLeft,nDeOffset11,cx-nDeRight, nDeTop);
		m_MefDef.MoveWindow(rc3, TRUE);


		if (m_Copy1.IsWindowVisible())
			nCopyCount++;
		if (m_Copy2.IsWindowVisible())
			nCopyCount++;
		if (m_Copy3.IsWindowVisible())
			nCopyCount++;

		if (nCopyCount == 1)
		{
			CRect rc4(rc.left,cy-nDeOffset35,rc.right, cy-nDeOffset);

			if (m_Copy1.IsWindowVisible())
				m_Copy1.MoveWindow(rc4, TRUE);
			if (m_Copy2.IsWindowVisible())
				m_Copy2.MoveWindow(rc4, TRUE);
			if (m_Copy3.IsWindowVisible())
				m_Copy3.MoveWindow(rc4, TRUE);
		}
		
		if (nCopyCount == 2)
		{
			int nOffset = (rc.Width()/2);
			CRect rc4(rc.left,cy-nDeOffset35,rc.left + nOffset - 2, cy-nDeOffset);
			CRect rc5(rc.left + nOffset + 2,cy-nDeOffset35, rc.left + nOffset+ nOffset, cy-nDeOffset);
			
			if (m_Copy1.IsWindowVisible())
				m_Copy1.MoveWindow(rc4, TRUE);
			if (m_Copy2.IsWindowVisible())
				m_Copy2.MoveWindow(rc5, TRUE);
		}

		if (nCopyCount == 3)
		{
			int nOffset = (rc.Width()/3);
			CRect rc4(rc.left,cy-nDeOffset35,rc.left + nOffset -2, cy-nDeOffset);
			CRect rc5(rc.left + nOffset +2,cy-nDeOffset35,rc.left + nOffset+ nOffset-2, cy-nDeOffset);
			CRect rc6(rc.left + nOffset + nOffset +2,cy-nDeOffset35,rc.left + nOffset + nOffset + nOffset, cy-nDeOffset);

			if (m_Copy1.IsWindowVisible())
				m_Copy1.MoveWindow(rc4, TRUE);
			if (m_Copy2.IsWindowVisible())
				m_Copy2.MoveWindow(rc5, TRUE);
			if (m_Copy3.IsWindowVisible())
				m_Copy3.MoveWindow(rc6, TRUE);
		}
	}
}


void CObjectBrowser::OnCopy3() 
{
	//CString sUnsavedProject = theWorkspace.LoadResourceString(IDS_PROJECT);
	//if (m_pControl->GetKeyPath().Left(sUnsavedProject.GetLength()) == sUnsavedProject)
	if( theWorkspace.GetActiveDocument()->GetPathName().IsEmpty() )
	{
		int nWhatNext = MessageBox( theWorkspace.LoadResourceString(IDS_RENAMEPROJECT),
																theWorkspace.LoadResourceString(IDR_MAINFRAME),
																MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 );
		if( nWhatNext == IDYES )
		{
			if( !theWorkspace.GetActiveDocument()->DoFileSave() )
				return;
		}
		else if( nWhatNext != IDNO )
			return;
	}

	CString source = m_sClipBoardDefun3; 
	//put your text in source
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		TCHAR * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, (source.GetLength()+1) * sizeof(TCHAR));
		buffer = (TCHAR*)GlobalLock(clipbuffer);
		lstrcpyn(buffer, LPCTSTR(source), source.GetLength() + 1);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}
}

CString CObjectBrowser::GetTypeName( VARTYPE vt, AxMethodDescriptor *pMethod, AxPropertyDescriptor *pProperty )
{
	CString sType = VARTYPEtoString(vt);
	if(sType == _T("OLE Object") && (pMethod || pProperty))
	{
		if (pProperty != NULL)
		{
			RefCountedPtr< COleControlObject > pOleObject = theWorkspace.GetOleControlFor(pProperty);
			if (pOleObject != NULL)
			{
				LoadOleObjectIntoTree(pOleObject);
				return pOleObject->GetActiveXTypeName();
			}
		}
		else
		{
			RefCountedPtr< COleControlObject > pOleObject = theWorkspace.GetOleControlFor(pMethod);
			if (pOleObject != NULL)
			{
				LoadOleObjectIntoTree(pOleObject);
				return pOleObject->GetActiveXTypeName();
			}			
		}
	}
	return sType;
}
