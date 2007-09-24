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
	: CResizableDialog(CObjectBrowser::IDD, pParent)
{
	m_pControl = NULL;
	m_sDclFormName = theWorkspace.LoadResourceString(IDS_DclFormName);
}


void CObjectBrowser::DoDataExchange(CDataExchange* pDX)
{
	CResizableDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COPY3, m_Copy3);
	DDX_Control(pDX, IDC_METHDEF, m_MefDef);
	DDX_Control(pDX, IDOK, m_OK);
	DDX_Control(pDX, IDC_COPY2, m_Copy2);
	DDX_Control(pDX, IDC_COPY1, m_Copy1);
	DDX_Control(pDX, IDC_RICHEDIT, m_RichBox);
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
}


BEGIN_MESSAGE_MAP(CObjectBrowser, CResizableDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_LISTBOX, OnSelchangedListbox)
	ON_BN_CLICKED(IDC_COPY2, OnCopy2)
	ON_BN_CLICKED(IDC_COPY1, OnCopy1)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_COPY3, OnCopy3)
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
	EnableSaveRestore(theWorkspace.LoadResourceString(IDS_ObjectBrowser),
										theWorkspace.LoadResourceString(IDS_PROP_FORMEVENTSIZE));
	
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

HTREEITEM CObjectBrowser::LoadOleObjectIntoTree(TDclControlPtr pControl) 
{
	std::vector< TDclControlPtr >::const_iterator pos = m_OleObjectList.begin();
	// lets make sure we do not insert any OleObjects more than once.
	while (pos != m_OleObjectList.end())
	{
		TDclControlPtr pObject = *pos++;
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

void CObjectBrowser::LoadAllAssociatedOleObjects(TDclControlPtr pControl) 
{
	const TPropertyList& Props = pControl->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
	{
		const AxInterfaceDescriptor* pAxDesc = (*iter)->GetAxInterfaceDescriptorPtr();
		if( pAxDesc )
		{
			SearchMethods( (*iter) );
			TOleControlPtr pOleObject;

			// check events
			pOleObject = activeProject->GetOleObject(pAxDesc->GetEvent());
			if (pOleObject != NULL)
				LoadOleObjectIntoTree(TDclControlLockedPtr(pOleObject));
			
			// check each property pointer type
			pOleObject = activeProject->GetOleObject(pAxDesc->GetProp());
			if (pOleObject != NULL)
				LoadOleObjectIntoTree(TDclControlLockedPtr(pOleObject));
			
			pOleObject = activeProject->GetOleObject(pAxDesc->GetPropGet());
			if (pOleObject != NULL)
				LoadOleObjectIntoTree(TDclControlLockedPtr(pOleObject));
			
			pOleObject = activeProject->GetOleObject(pAxDesc->GetPropPut());
			if (pOleObject != NULL)
				LoadOleObjectIntoTree(TDclControlLockedPtr(pOleObject));
			
			pOleObject = activeProject->GetOleObject(pAxDesc->GetPropPutRef());
			if (pOleObject != NULL)
				LoadOleObjectIntoTree(TDclControlLockedPtr(pOleObject));
		}
	}
}


void CObjectBrowser::SearchMethods(TPropertyPtr pProp) 
{
	size_t idx = pProp->size();
	while( idx-- > 0 )
	{
		TOleControlPtr pOleObject = activeProject->GetOleObject(pProp->GetAxInterfaceDescriptorPtr()->GetMethods()->at(idx));
		if (pOleObject != NULL)
			LoadOleObjectIntoTree(TDclControlLockedPtr(pOleObject));			
	}
}

void CObjectBrowser::LoadInfoTree(TDclControlPtr pControl, HTREEITEM hParentItem, int nIndex) 
{
	const TPropertyList& Props = pControl->GetPropertyList();
	size_t idx = 0;
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
	{
		++idx;
		switch ((*iter)->GetType())
		{
		case PropCustom:
		case PropImageList:
		case PropStringArray:
		case PropIntArray:
		case PropActiveXPropPages:
			break;
		case PropActiveXMethods:
			{
				int nCount = (*iter)->size();
				for (int j = 0; j < nCount; j++)
				{
					CString sTitle = (*iter)->GetAxInterfaceDescriptorPtr()->GetMethods()->at(j)->GetName();								
					HTREEITEM hItem = m_ListBox.InsertItem(sTitle, hParentItem, TVI_SORT);
					m_ListBox.SetItemData(hItem, (DWORD_PTR)(-((long)j + 1)));
					m_ListBox.SetItemImage(hItem, 3, 3);
				}
			}
			break;
		default:
			{
				if ((*iter)->GetID() != Prop::Name && (*iter)->GetID() != Prop::GlobalVarName)
				{
					CString sTitle;
					if( (*iter)->GetType() == PropActiveXEvent )
						sTitle = (*iter)->GetAxInterfaceDescriptorPtr()->GetEvent()->GetName();
					else
						sTitle = (*iter)->GetName();
					if (sTitle.GetLength() > 0)
					{
						HTREEITEM hItem = m_ListBox.InsertItem(sTitle, hParentItem, TVI_SORT);
						m_ListBox.SetItemData(hItem, idx);
						switch ((*iter)->GetType())
						{
						case PropActiveXEvent:
						case PropEvent:
							m_ListBox.SetItemImage(hItem, 2, 2);
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
			}
			break;
		}
	}

	if (nIndex == 0)
	{
		if (pControl->GetType() != CtlForm &&
				pControl->GetType() != CtlInvalid &&
				pControl->GetType() != CtlFileDlgCtrl)
			LoadMethods(_T("AllCtrls.mth"), hParentItem);
		
		// here we need to see which control it being displayed to load the correct method info
		switch (pControl->GetType())
		{
		case CtlFileDlgCtrl:
			LoadMethods(_T("FileDlgCtrl.mth"), hParentItem);
			break;
		case CtlTabStrip:
			LoadMethods(_T("Tab.mth"), hParentItem);
			break;
		case CtlListBox:
			LoadMethods(_T("ListBox.mth"), hParentItem);
			break;
		case CtlListView:
			LoadMethods(_T("ListCtrl.mth"), hParentItem);
			break;
		case CtlForm: // a form
			LoadMethods(_T("Forms.mth"), hParentItem);
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
			LoadMethods(_T("DwgList.mth"), hParentItem);
			break;	
		case CtlBlockList:
			LoadMethods(_T("BlockList.mth"), hParentItem);
			break;	
		case CtlPictureBox:
			LoadMethods(_T("PictureBox.mth"), hParentItem);
			break;
		case CtlOptionList:
			LoadMethods(_T("OptionList.mth"), hParentItem);
			break;
		case CtlBlockView:
			LoadMethods(_T("BlockView.mth"), hParentItem);
			break;
		case CtlHatch:
			LoadMethods(_T("Hatch.mth"), hParentItem);
			break;
		case CtlSlideView:
			LoadMethods(_T("SlideView.mth"), hParentItem);
			break;
		case CtlTree:
			LoadMethods(_T("TreeCtrl.mth"), hParentItem);
			break;
		case CtlComboBox:
			LoadMethods(_T("ComboBox.mth"), hParentItem);
			break;
		case CtlTextBox:
			LoadMethods(_T("TextBox.mth"), hParentItem);
			break;	
		case CtlDwgPreview:
			LoadMethods(_T("DwgPreview.mth"), hParentItem);
			break;
		case CtlActiveX:
			LoadMethods(_T("AxCtrls.mth"), hParentItem);
			break;	
		case CtlMonth:
			LoadMethods(_T("Month.mth"), hParentItem);
			break;	
		case CtlGraphicButton:
			LoadMethods(_T("GrphicBtn.mth"), hParentItem);
			break;	
		case CtlHtmlCtrl:
			LoadMethods(_T("Html.mth"), hParentItem);
			break;				
		}	
	}
	else
	{
		LoadMethods(_T("AxObjects.mth"), hParentItem);
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
		CString sMethod2 = _T("[Method]");
		CString sEOF = _T("[End of File]");
		
		// do loop until the end is found.
		while (sLine != sEOF && fMthFile.GetPosition() < fMthFile.GetLength())
		{
			if (sLine == sMethod2)
			{		
				fMthFile.ReadString(sLine);	
				HTREEITEM hItem = m_ListBox.InsertItem(sLine, hParentItem, TVI_SORT);
				// set item data to 0 to indicate the info must be loaded from file.
				m_ListBox.SetItemData(hItem, 0); 
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
	for (int i = 0; i < sMethodName.GetLength(); i++)
	{
		TCHAR ch = sMethodName[i];
		if (ch != _T('('))
			sCompile += ch;
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

		CString sMethod2 = _T("[Method]");
		CString sEOF = _T("[End of File]");
		
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
					while (sLine.Left(10) != _T("[Arguments") &&
								 sLine != _T("[Argument]") &&
								 fMthFile.GetPosition() < fMthFile.GetLength())
					{
						fMthFile.ReadString(sLine);	
						if (sLine.Left(10) != _T("[Arguments") && sLine != _T("[Argument]"))
							sDesc += sLine;			
					}
					// clear the clipboard string holder
					m_sClipBoardDefun2.Empty();					

					sDefun1 = _T("\\par ");
					bool bHasReturn = false;
					// setup the return type
					if (sLine.Left(18) == _T("[Arguments Return]"))
					{
						// here we must setup the sTitle (first line) so it will show the Method + function short name + as something for return.
						sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString(IDS_METHOD) + _T(" \\b ") + sMethodName + _T(" \\b0 \\cf1 ") + sLine.Mid(19) + _T("\\cf0");
						sDefun1 += _T("(\\cf2 Setq \\cf1 rValue \\cf0 (\\cf2") + sFuncName + _T("\\cf0  ");
						m_sClipBoardDefun2 = _T("(Setq rValue (") + sFuncName + _T(" ");
						bHasReturn = true;
					}
					else
					{
						sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString(IDS_METHOD) + _T(" \\b ") + sMethodName + _T(" \\b0 \\cf0");
						sDefun1 += _T("(\\cf2") + sFuncName + _T("\\cf0  ");
						m_sClipBoardDefun2 = _T("(") + sFuncName + _T(" ");
					}
					

					fMthFile.ReadString(sLine);	// now we can get past the [Arguments] 
					// here we loop through all the remaining arguments and add to them to the tail of the autolisp syntax structure
					while (sLine != sMethod2 && !sLine.IsEmpty() && sLine != sEOF)
					{
						if (sLine == theWorkspace.LoadResourceString(IDS_PROJECTFILENAME))
						{
							CString sControlName = m_pControl->GetVarName();
							sDefun1 += _T("\\cf3") + sControlName + _T("\\cf0");
							m_sClipBoardDefun2 += sControlName;
						}						
						else if (sLine == theWorkspace.LoadResourceString(IDS_PROJECTNAME))
						{
							sDefun1 += _T("\\cf3") + theEditorWorkspace.GetActiveProjectName() + _T("\\cf0");
							m_sClipBoardDefun2 += theEditorWorkspace.GetActiveProjectName();
						}						
						else if (sLine.Left(1) == _T("[") && sLine.Mid(1, 2) == theWorkspace.LoadResourceString(IDS_AS))
						{
							sDefun1 += _T("\\cf5\\i  ") + sLine + _T("\\i0\\cf0 ");
							m_sClipBoardDefun2 += sLine;
						}
						else if (sLine == _T("[Begin List]"))
						{
							sDefun1 += _T("\\cf0 (list ");
							m_sClipBoardDefun2 += _T(" (list ");
						}
						else if (sLine == _T("[End List]"))
						{
							sDefun1 += _T("\\cf0)");
							m_sClipBoardDefun2 += _T(")");
						}
						else if (!sLine.IsEmpty() && sLine != _T(" ") && sLine != _T("  ") && sLine != _T("   ")) 
						{
							sDefun1 += _T("\\cf1  ") + sLine ;
							// if the last char was not a ( add a space before we add the argument
							if (m_sClipBoardDefun2.Right(1) != _T("("))
								m_sClipBoardDefun2 += _T(' ');
							m_sClipBoardDefun2 += sLine + _T(' ');
						}
						if( fMthFile.GetPosition() >= fMthFile.GetLength() )
							break;
						fMthFile.ReadString(sLine);	// now we can get past the [Arguments] 				
					}

					if (bHasReturn)
					{
						sDefun1 += _T("\\cf0))");
						m_sClipBoardDefun2 += _T("))");
					}
					else
					{
						sDefun1 += _T("\\cf0)");
						m_sClipBoardDefun2 += _T(")");
					}

					sDefun1 += _T("\\par \\par ");
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
	CString sExtraText;
	CString sArgType;
	CString sGlobalVarName;

	m_Copy1.ShowWindow(FALSE);
	m_Copy2.ShowWindow(FALSE);
	m_Copy3.ShowWindow(FALSE);

	hItem = m_ListBox.GetSelectedItem();
	if (hItem == NULL)
	{
		m_RichBox.SetRTF(CString());			
		return;
	}
		
	HTREEITEM hParent = m_ListBox.GetParentItem(hItem);
	if (hParent == NULL)
	{
		m_RichBox.SetRTF(CString());			
		ResizeControls();
		return;
	}

	TDclControlPtr pControl = m_OleObjectList.at(m_ListBox.GetItemData(hParent));
	if (pControl == NULL)
		return;

	sGlobalVarName = pControl->GetVarName();
	
	if (pControl != m_pControl)
		sGlobalVarName = theWorkspace.LoadResourceString(IDS_OLEOBJECT);
	
	int nThisItemData = m_ListBox.GetItemData(hItem);

	TPropertyPtr pProp;
	if ((long)nThisItemData < 0)
	{
		pProp = pControl->GetPropertyObject( Prop::ObjectBrowser );
		nThisItemData = -(long)nThisItemData;
		nThisItemData--;
	}
	else if( nThisItemData > 0 )
	{
		const TPropertyList& Props = pControl->GetPropertyList();
		TPropertyList::const_iterator iter = Props.begin();
		while( nThisItemData-- > 1 ) ++iter;
		pProp = *iter;
	}

	CString sTitle;
	CString sDesc;
	CString sVarType;
	CString sDefun1;
	CString sDefun2;
	CString sItemText = m_ListBox.GetItemText(hItem);

	if (!pProp)
	{
		
		if (pControl->GetType() == CtlActiveX)
		{
			LoadFullMethod(_T("AxCtrls.mth"), sItemText, sTitle, sDesc, sDefun1);
			// call the method to load info from the appropriate method info files.
			LoadFullMethod(_T("AllCtrls.mth"), sItemText, sTitle, sDesc, sDefun1);					
		}
		else if (pControl->GetType() == CtlForm || pControl->GetID() == -1)
			LoadFullMethod(_T("Forms.mth"), sItemText, sTitle, sDesc, sDefun1);	
		else
		{
			if (pControl->GetType() != CtlFileDlgCtrl)
				LoadFullMethod(_T("AllCtrls.mth"), sItemText, sTitle, sDesc, sDefun1);					

			// here we need to see which control it being displayed to load the correct method info
			switch (pControl->GetType())
			{
			
			case CtlFileDlgCtrl:
				LoadFullMethod(_T("FileDlgCtrl.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlTabStrip:
				LoadFullMethod(_T("Tab.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlListBox:
				LoadFullMethod(_T("ListBox.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlListView:
				LoadFullMethod(_T("ListCtrl.mth"), sItemText, sTitle, sDesc, sDefun1);
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

			case CtlImageComboBox:
				LoadFullMethod(_T("ImageComboBox.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlAnimate:
				LoadFullMethod(_T("AnimationCtrl.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlDwgList:
				LoadFullMethod(_T("DwgList.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlBlockList:
				LoadFullMethod(_T("BlockList.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlPictureBox:
				LoadFullMethod(_T("PictureBox.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlOptionList:
				LoadFullMethod(_T("OptionList.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlBlockView:
				LoadFullMethod(_T("BlockView.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;
			
			case CtlHatch:
				LoadFullMethod(_T("Hatch.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;
			
			case CtlSlideView:
				LoadFullMethod(_T("SlideView.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;
			
			case CtlTree:
				LoadFullMethod(_T("TreeCtrl.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;
			
			case CtlComboBox:
				LoadFullMethod(_T("ComboBox.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlTextBox:
				LoadFullMethod(_T("TextBox.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlDwgPreview:
				LoadFullMethod(_T("DwgPreview.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlActiveX:
				LoadFullMethod(_T("AxCtrls.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;

			case CtlMonth:
				LoadFullMethod(_T("Month.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;	

			case CtlGraphicButton:
				LoadFullMethod(_T("GrphicBtn.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;	

			case CtlHtmlCtrl:
				LoadFullMethod(_T("Html.mth"), sItemText, sTitle, sDesc, sDefun1);
				break;	
			}
		}
	}
	else
	{
		switch (pProp->GetType())
		{
		case PropActiveXMethods:
			{
				sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString(IDS_METHOD) + _T(" \\b ") + sItemText + _T(" \\b0 \\cf0");
				if (pProp->GetAxInterfaceDescriptorPtr() != NULL)
				{
					sDesc = pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodDesc(nThisItemData);
					sVarType = GetTypeName(pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodReturnType(nThisItemData),
																 pProp->GetAxInterfaceDescriptorPtr()->GetAxMethod(nThisItemData));
					
					// here we need to put in OleObject closing instructions if required.
					if (pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodReturnType(nThisItemData) == VT_DISPATCH ||
						pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodReturnType(nThisItemData) == VT_UNKNOWN)
					{
						if (!sDesc.IsEmpty())
							sDesc += _T(" \\par \\par ");						
						if (sVarType.IsEmpty() || sVarType == theWorkspace.LoadResourceString(IDS_OLEOBJECT))
							sDesc += theWorkspace.LoadResourceString(IDS_OLENOTE1);
						else					
							sDesc += theWorkspace.LoadResourceString(IDS_OLENOTE2) + sVarType + theWorkspace.LoadResourceString(IDS_OLENOTE3);
					}
				}
				sDefun1 = _T("\\par ");
				
				// clear the clipboard string holder
				m_sClipBoardDefun2.Empty();
				// setup the defun is it is to return a value
				if (!sVarType.IsEmpty())
				{
					sDefun1 += _T("(\\cf2 Setq \\cf1 rValue \\cf0 ");
					m_sClipBoardDefun2 = _T("(Setq rValue ( ");						
				}
				// add the DoMethod
				sDefun1 += CString(_T("(\\cf2 dcl_AxControl_DoMethod \\cf0 \\cf3 ")) + sGlobalVarName + _T(" \\cf0 \\cf3\\b \"");
				m_sClipBoardDefun2 += _T("(dcl_AxControl_DoMethod ") + sGlobalVarName + _T(" \"");
				// add the method name
				sDefun1 += m_ListBox.GetItemText(hItem) + _T("\"\\cf0\\b0 ");
				m_sClipBoardDefun2 += m_ListBox.GetItemText(hItem) + _T("\"");
				
				if (pProp->GetAxInterfaceDescriptorPtr() != NULL)
				{
					size_t nCount = pProp->GetAxInterfaceDescriptorPtr()->CountAxMethodParams(nThisItemData);
					for (size_t i = 0; i < nCount; ++i)
					{
						// add the argument name
						sDefun1 += _T(" \\cf1 ") + pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodParamName(nThisItemData, i);
						m_sClipBoardDefun2 += CString(_T(" ")) + pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodParamName(nThisItemData, i); 
						// add the [as ...]
						sArgType = pProp->GetAxInterfaceDescriptorPtr()->GetAxMethodParamVarType(nThisItemData, i);
				
						if (sArgType == CString())
						{
							sArgType = theWorkspace.LoadResourceString(IDS_OPTIONALNIL);
							sDefun1 += _T(" \\cf5\\i ") + sArgType + _T("\\i0 ");
							m_sClipBoardDefun2 += sArgType;
						}
						else if (!sArgType.IsEmpty())
						{
							sDefun1 += _T(" \\cf5\\i [") + theWorkspace.LoadResourceString(IDS_AS) + _T(" ") + sArgType + _T("]\\i0 ");
							m_sClipBoardDefun2 += _T("[") + theWorkspace.LoadResourceString(IDS_AS) + _T(" ") + sArgType + _T("]");
						}
						else
							sDefun1 += _T(" \\cf5 ");
						// if it is the second to last argument add close the color.
						if (i < nCount - 1)
						{
							sDefun1 += _T("\\cf0 ");
							m_sClipBoardDefun2 += _T(" ");
						}
					}
				}

				// close the brackets
				if (!sVarType.IsEmpty())
				{
					sDefun1 += _T("\\cf0 ))\\par ");
					m_sClipBoardDefun2 += _T("))");
				}
				else
				{
					sDefun1 += _T("\\cf0 )\\par ");
					m_sClipBoardDefun2 += _T(")");
				}

				// add a more info disclaimer.
				sDefun1 += _T("\\par ") + theWorkspace.LoadResourceString(IDS_FORMOREINFO);
				// add a microsoft disclaimer.
				if (pControl->IsMicrosoftActiveXCtrl() == TRUE)
					sDefun1 += theWorkspace.LoadResourceString(IDS_TOFINDIT);
						
				// set the second copy button
				CString sCopy2Text = theWorkspace.LoadResourceString(IDS_COPYTOCLIP);			
				m_Copy2.SetWindowText(sCopy2Text);
				m_Copy2.ShowWindow(TRUE);
			}
			break;
		case PropActiveXEvent:
			{
				CString sEventName = pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->GetName();
				if (!sEventName.IsEmpty())
					sDefun1.Format( _T(" \\par c:%s_On%s () \\par "), (LPCTSTR)sGlobalVarName, (LPCTSTR)sEventName );
				sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString(IDS_EVENTTITLE) + _T(" \\b ") + (sEventName.IsEmpty()? sItemText : sEventName) + _T(" \\b0 \\cf0");
				sDesc = pProp->GetAxInterfaceDescriptorPtr()->GetEvent()->GetDesc();
			}
			break;
		case PropEvent:
			{
				CString sEventName = GetPropertyName(pProp->GetID());
				sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString(IDS_EVENTTITLE) + _T(" \\b ") + (sEventName.IsEmpty()? sItemText : sEventName) + _T(" \\b0 \\cf0");

				// get the args and desc
				CString sEventArgs;
				LoadArgsNDesc(pProp->GetID(), pControl, sEventArgs, sDesc);
				
				sDefun1 = _T(" \\par ");
				if (!pProp->GetStringValue().IsEmpty())
				{
					if (!sEventArgs.IsEmpty())
						sDefun1 += pProp->GetStringValue() + _T(" (") + sEventArgs + _T(" /)");						
					else
						sDefun1 += pProp->GetStringValue() + _T(" ()");						

					sDefun1 += _T(" \\par ");
				}
				else
				{
					sDefun1 += _T("c:");
					sDefun1 += sGlobalVarName + _T("_On") + sEventName;
					if (!sEventArgs.IsEmpty())
						sDefun1 += _T(" (") + sEventArgs + _T(" /)");
					else
						sDefun1 += _T(" ()");
					sDefun1 += _T(" \\par ");
				}
			}
			break;
		case PropActiveXProp:
		case PropActiveXRunTime:
			{
				m_sClipBoardDefun1.Empty();
				m_sClipBoardDefun2.Empty();
				AxPropertyDescriptor* pAxPropGet = pProp->GetAxInterfaceDescriptorPtr()->GetGetDescriptor();
				AxPropertyDescriptor* pAxPropPut = pProp->GetAxInterfaceDescriptorPtr()->GetPutDescriptor();
				sDesc = pProp->GetAxInterfaceDescriptorPtr()->GetDesc();
				sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString(IDS_PROPERTYTITLE) + _T(" \\b ") + sItemText + _T(" \\b0 \\cf0");
				
				// do a special override for the color set properties
				if (pAxPropPut)
				{
					if (pAxPropPut->GetGuid() == GUID_COLOR)
					{
						sDefun1 = _T("\\par(\\cf2 dcl_AxControl_SetColor \\cf0 \\cf3 ") + sGlobalVarName + _T(" \\cf0 \\cf3  \\b1 \"") + m_ListBox.GetItemText(hItem) + _T("\" \\b0");
						m_sClipBoardDefun1 = _T("(dcl_AxControl_SetColor ") + sGlobalVarName + _T(" \"") + m_ListBox.GetItemText(hItem) + _T("\" ");
			
						sVarType = GetTypeName(pProp->GetAxInterfaceDescriptorPtr()->GetType(), NULL, pAxPropPut);
			
						sDefun1 += theWorkspace.LoadResourceString(IDS_REDCOLORDESC2);
						m_sClipBoardDefun1 += theWorkspace.LoadResourceString(IDS_REDCOLORDESC);
						sDefun1 += _T(") \\par ");
						m_sClipBoardDefun1 += _T(")");

						if (sDesc.IsEmpty())
							sDesc = theWorkspace.LoadResourceString(IDS_DESCNOTSET);

						sDesc += theWorkspace.LoadResourceString(IDS_SETCOLORDESC);
							
						// show the first copy button
						m_Copy1.ShowWindow(TRUE);
					}
					else
					{
						// lets set the put property
						sDefun1 = _T("\\par(\\cf2 dcl_AxControl_SetProperty \\cf0 \\cf3 ") + sGlobalVarName + _T(" \\cf0 \\cf3\\b \"");
						m_sClipBoardDefun1 = _T("(dcl_AxControl_SetProperty ") + sGlobalVarName + _T(" \"");

						sVarType = GetTypeName(pProp->GetAxInterfaceDescriptorPtr()->GetType(), NULL, pAxPropPut);
			
						if (sVarType == theWorkspace.LoadResourceString(IDS_PROP_PICTURE))
						{
							if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IPictureDisp)
							{
								CString sLoad;
								sExtraText = theWorkspace.LoadResourceString(IDS_PICTURESHORTCUT);
								sExtraText += _T("dcl_AxControl_LoadPicture \\cf0 \\cf3") + sGlobalVarName;
								sLoad = theWorkspace.LoadResourceString(IDS_PICTURESHORTCUT2);
								m_sClipBoardDefun3 = _T("(dcl_AxControl_LoadPicture ") + sGlobalVarName + theWorkspace.LoadResourceString(IDS_PICDESC);
								sExtraText += sLoad;
								m_Copy3.ShowWindow(TRUE);
							}
						}
						
						// here we need to put in OleObject closing instructions if required.
						if (pProp->GetAxInterfaceDescriptorPtr()->GetType() == VT_DISPATCH ||
								pProp->GetAxInterfaceDescriptorPtr()->GetType() == VT_UNKNOWN)
						{
							if (!sDesc.IsEmpty())
								sDesc += _T(" \\par \\par ");
							if (sVarType == theWorkspace.LoadResourceString(IDS_OLEOBJECT) || sVarType == CString())
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
							sDefun1 += m_ListBox.GetItemText(hItem) + _T("\"\\cf0\\b0 ");
							m_sClipBoardDefun1 += m_ListBox.GetItemText(hItem) + _T("\"");

							size_t nCount = pAxPropPut->GetArgs().size();
							for (size_t i=0; i<nCount; i++)
							{
								CString sArg = pAxPropPut->GetArgs()[i].name;
								if (sArg.IsEmpty())
									sArg = theWorkspace.LoadResourceString(IDS_NEWVAL3);
								sDefun1 += _T(" \\cf1 ") + sArg;
								m_sClipBoardDefun1 += _T(" ") + sArg;
								sArgType = GetTypeName(pAxPropPut->GetArgs()[i].vt, NULL, pAxPropPut);
								if (sArgType.IsEmpty())
								{
									sArgType = theWorkspace.LoadResourceString(IDS_OPTIONALNILASB);
									sDefun1 += _T(" \\cf5\\i [") + sArgType + _T("]\\i0 ");
									m_sClipBoardDefun1 += _T(" [") + sArgType + _T("]");
								}
								else if (!sArgType.IsEmpty())
								{						
									sDefun1 += _T(" \\cf5\\i [") + theWorkspace.LoadResourceString(IDS_AS) + _T(" ") + sArgType + _T("]\\i0 ");
									m_sClipBoardDefun1 += _T(" [") + theWorkspace.LoadResourceString(IDS_AS) + _T(" ") + sArgType + _T("]");
								}
								else
									sDefun1 += _T(" \\cf5 ");
								if (i < nCount-1)
								{
									sDefun1 += _T("\\cf0  ");
									m_sClipBoardDefun1 += _T(" ");
								}
							}							
						}
						sDefun1 += _T(") \\par \\cf0");
						m_sClipBoardDefun1 += _T(")");
						// show the first copy button
						m_Copy1.ShowWindow(TRUE);
					}
				}
				else
					m_Copy1.ShowWindow(TRUE);

				if (pAxPropGet)
				{
					sVarType = GetTypeName(pProp->GetAxInterfaceDescriptorPtr()->GetType(), NULL, pAxPropGet);

					// lets set the get property
					sDefun2 = _T("\\par(\\cf2 Setq \\cf1 Value \\cf0 (\\cf2 dcl_AxControl_") + theWorkspace.LoadResourceString(IDS_GETPROP) + _T("\\cf0 \\cf3 ") + sGlobalVarName + _T(" \\cf0 \\cf3\\b \"");
					m_sClipBoardDefun2 = _T("(Setq Value (dcl_AxControl_") + theWorkspace.LoadResourceString(IDS_GETPROP) + sGlobalVarName + _T(" \"");
					sDefun2 += m_ListBox.GetItemText(hItem) + _T("\"\\cf0\\b0 ");
					m_sClipBoardDefun2 += m_ListBox.GetItemText(hItem) + _T("\"");

					// lets add the arguments
					if (pAxPropGet != NULL && !pAxPropGet->GetArgs().empty())
					{					
						size_t nCount = pAxPropGet->GetArgs().size();
						for (size_t i=0; i<nCount; i++)
						{
							CString sArg = pAxPropGet->GetArgs()[i].name;
							if (sArg.IsEmpty())
								sArg = theWorkspace.LoadResourceString(IDS_NEWVAL3);
							sDefun2 += _T(" \\cf1 ") + sArg;
							m_sClipBoardDefun2 += _T(" ") + sArg;
							sArgType = GetTypeName(pAxPropGet->GetArgs()[i].vt, NULL, pAxPropGet);
							if (sArgType == CString())
							{
								sArgType = theWorkspace.LoadResourceString(IDS_OPTIONALNILASB);
								sDefun2 += _T(" \\cf5\\i [") + sArgType + _T("]\\i0 ");
								m_sClipBoardDefun2 += _T(" [") + sArgType + _T("]");
							}
							else if (!sArgType.IsEmpty())
							{														
								sDefun2 += _T(" \\cf5\\i [") + theWorkspace.LoadResourceString(IDS_AS) + _T(" ") + sArgType + _T("]\\i0 ");
								m_sClipBoardDefun2 += _T(" [") + theWorkspace.LoadResourceString(IDS_AS) + _T(" ") + sArgType + _T("]");
							}
							if (i < nCount - 1)
							{
								sDefun2 += _T("\\cf0  ");
								m_sClipBoardDefun2 += _T(" ");
							}
						}
					}
					sDefun2 += _T("))\\par");
					m_sClipBoardDefun2 += _T("))");	

					// set the second copy button
					CString sCopy2Text;
					sCopy2Text = theWorkspace.LoadResourceString(IDS_COPYGETTOCLIP);			
					m_Copy2.SetWindowText(sCopy2Text);
					m_Copy2.ShowWindow(TRUE);
				}
				else
					m_Copy2.ShowWindow(FALSE);
			}
			break;
		default:
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
				sTitle = _T("\\b0 ") + theWorkspace.LoadResourceString(IDS_PROPERTY) + _T(" \\b ") + sItemText + _T(" \\b0 \\cf0");
				sDesc = pProp->GetDocumentationDesc();
				bool bHidden = pProp->IsHidden();
				switch( pProp->GetID() )
				{
				case Prop::FontSize:
				case Prop::FontBold:
				case Prop::FontItalic:
				case Prop::FontStrikeout:
				case Prop::FontUnderline:
					bHidden = false; //these hidden properties should be treated like normal properties
					break;
				}
				if (bHidden)
				{
					sDesc += _T(" \\par \\par ");
					sDesc += theWorkspace.LoadResourceString(IDS_HIDDENPROP);
				}
				else if(m_pControl->GetType() == CtlFileDlgCtrl || m_pControl->GetType() <= CtlInvalid)
				{
					sDesc += _T(" \\par \\par ");
					sDesc += theWorkspace.LoadResourceString(IDS_DESIGNTIMEONLY);
				}
				else 
				{	
					switch (pProp->GetID())
					{
					case Prop::ComboBoxStyle:
					case Prop::ButtonStyle:
					case Prop::FilterStyle:
					case Prop::MultiRow:
					case Prop::Orientation:
					case Prop::RectStyle:
					case Prop::Justification:
					case Prop::AutoHScroll:
					case Prop::AutoVScroll:
					case Prop::DropDownHeight:
					case Prop::MultiColumn:
					case Prop::SelectStyle:
					case Prop::NoIntegralHeight:
					case Prop::Sorted:
					case Prop::UseTabStops:
					case Prop::DisableNoScroll:
					case Prop::SmoothProgress:
					case Prop::AutoWrap:
					case Prop::MinTabWidth:
					case Prop::Resizable:
					case Prop::ShowSelectAlways:
					case Prop::HasLines:
					case Prop::LinesAtRoot:
					case Prop::HasButtons:
					case Prop::EditLabels:
					case Prop::CheckBoxes:
					case Prop::Icon:
					case Prop::ShowEditBox:
					case Prop::InputFilter:
					case Prop::IsTabStop:
					case Prop::BeginGroup:
					case Prop::ListViewStyle:
					case Prop::ListViewSort:
					case Prop::ListViewIconAlign:
					case Prop::BlockListStyle:
					case Prop::AutoArrange:
					case Prop::ColHeader:
					case Prop::LabelWrap:
					case Prop::IconXSpacing:
					case Prop::IconYSpacing:
					case Prop::GridLines:
					case Prop::FullRowSelect:
					case Prop::ReturnAsTab:
					case Prop::SingleExpanded:
					case Prop::DefSelIndex:
					case Prop::DockableSides:
					case Prop::DefaultDockedSide:
					case Prop::TitleBarText:
					case Prop::MinDialogWidth:
					case Prop::MaxDialogWidth:
					case Prop::MinDialogHeight:
					case Prop::MaxDialogHeight:
						{
							CString sMsg;
							if (m_pControl->GetType() < CtlLabel || m_pControl->GetType() > CtlFileDlgCtrl)
								sMsg = theWorkspace.LoadResourceString(IDS_RUNTIMENOTALLOWEDDLG);
							else
								sMsg = theWorkspace.LoadResourceString(IDS_RUNTIMENOTALLOWEDCTRL);
							sDesc += CString(_T(" \\par \\par ")) + sMsg;
							break;
						}
					default:
						{
							if (pControl->GetType() == CtlForm)
								sDesc += CString(_T(" \\par \\par ")) + theWorkspace.LoadResourceString(IDS_DESIGNTIMEONLY);
							else
							{
								// lets set the Put property
								sDefun1 = CString("\\par (\\cf2 dcl_Control_Set") + m_ListBox.GetItemText(hItem) + " \\cf0 \\cf3" + sGlobalVarName + " ";
								m_sClipBoardDefun1 = CString("(dcl_Control_Set") + m_ListBox.GetItemText(hItem) + _T(" ");
								sDefun1 += "\\par \\cf1 newValue [as " + sVarType + "]\\cf0 ) " + _T(" \\par ");
								m_sClipBoardDefun1 += sGlobalVarName + "\r\n\t newValue [as " + sVarType + _T("])");
								
								// lets set the get property
								sDefun2 = CString("\\par (\\cf2 Setq \\cf1 Value \\cf0 (\\cf2 dcl_Control_Get") + m_ListBox.GetItemText(hItem) + " \\cf0 \\cf3";
								m_sClipBoardDefun2 = "(Setq Value (dcl_Control_Get" + m_ListBox.GetItemText(hItem) + _T(" ");
								sDefun2 += sGlobalVarName + "\\cf0))";
								m_sClipBoardDefun2 += sGlobalVarName + _T("))");
								
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
			break;
		}
	}

	if (sDesc.IsEmpty())
		sDesc = theWorkspace.LoadResourceString(IDS_DESCNOTSET);

	CHARFORMAT cf;

	// set the first default char settings
	cf.dwMask = CFM_STRIKEOUT|CFM_BOLD;
	cf.dwEffects = NULL;
	m_RichBox.SetDefaultCharFormat(cf);

	// add the headers to the RTF string
	CString sRtf =
		_T("{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang4105{\\fonttbl{\\f0\\fnil\\fcharset0 Times New Roman;}}\r\n")
		_T("{\\colortbl ;\\red255\\gree0\\blue0;\red0\\gree0\\blue255;\red0\\green128\\blue0;}\r\n")
		_T("\\viewkind4\\uc1\\pard\\f0\\fs20 ");
	
	// set the title then start the bold statment
	sRtf += sTitle;
		
	//if the var type has been set
	if (!sVarType.IsEmpty())
	{
		// add it to the RTF text as the color red.
		sRtf += _T(" \\cf1 ") + theWorkspace.LoadResourceString(IDS_ISA) + sVarType + _T("\\cf0  ");
	}
	sRtf += _T(" \\par \\par ");
	// add the description
	sRtf += sDesc + _T(" \\par ");		
	
	// close the RTF statement
	if (!sDefun1.IsEmpty() && sDefun2.IsEmpty())
	{
		sRtf += _T(" \\par \\b ") + theWorkspace.LoadResourceString(IDS_ALISPSYN) + _T("\\b0 ");
		// add it to the RTF text as the color red.
		sRtf += sDefun1;
	}
	else if (!sDefun1.IsEmpty() && !sDefun2.IsEmpty())
	{
		sRtf += _T(" \\par \\b ") + theWorkspace.LoadResourceString(IDS_ALSPS) + _T("\\b0 ");
		// add it to the RTF text as the color red.
		sRtf += sDefun1;
		sRtf += _T(" \\par \\b ") + theWorkspace.LoadResourceString(IDS_ALGPS) + _T("\\b0 ");
		// add it to the RTF text as the color red.
		sRtf += sDefun2;
	}			
	else if (!sDefun2.IsEmpty())
	{
		m_Copy1.ShowWindow(FALSE);
		m_Copy2.ShowWindow(TRUE);
		sRtf += _T(" \\par \\b ") + theWorkspace.LoadResourceString(IDS_ALGPS) + _T("\\b0 ");
		// add it to the RTF text as the color red.
		sRtf += sDefun2;
	}
	
	sRtf += sExtraText;
	sRtf += _T(" \\par \r\n}");
	// add any extra text if any extists.
	
	m_RichBox.SetRTF(sRtf);			
	ResizeControls();
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
	if( theWorkspace.GetActiveDocument()->GetPathName().IsEmpty() )
	{
		int nWhatNext = MessageBox( theWorkspace.LoadResourceString(IDS_RENAMEPROJECT),
																theWorkspace.LoadResourceString(IDR_MAINFRAME),
																MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 );
		if( nWhatNext == IDYES )
		{
			if( !theWorkspace.GetActiveDocument()->DoFileSave() )
				return;
			SelectionChanged( m_ListBox.GetSelectedItem() );
		}
		else if( nWhatNext != IDNO )
			return;
	}

	CStringA source( m_sClipBoardDefun2 ); 
	//put your text in source
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		CHAR * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength() + 1);
		buffer = (CHAR*)GlobalLock(clipbuffer);
		lstrcpynA(buffer, (LPCSTR)source, source.GetLength() + 1);
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT,clipbuffer);
		CloseClipboard();
	}

}

void CObjectBrowser::OnCopy1() 
{
	if( theWorkspace.GetActiveDocument()->GetPathName().IsEmpty() )
	{
		int nWhatNext = MessageBox( theWorkspace.LoadResourceString(IDS_RENAMEPROJECT),
																theWorkspace.LoadResourceString(IDR_MAINFRAME),
																MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 );
		if( nWhatNext == IDYES )
		{
			if( !theWorkspace.GetActiveDocument()->DoFileSave() )
				return;
			SelectionChanged( m_ListBox.GetSelectedItem() );
		}
		else if( nWhatNext != IDNO )
			return;
	}

	CStringA source( m_sClipBoardDefun1 ); 
	//put your text in source
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		CHAR * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength() + 1);
		buffer = (CHAR*)GlobalLock(clipbuffer);
		lstrcpynA(buffer, (LPCSTR)source, source.GetLength() + 1);
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
			
		if (cx == -1)
		{
			CRect rcDlg;
			CResizableDialog::GetClientRect(&rcDlg);
			cx = rcDlg.Width();
			cy = rcDlg.Height();
			rc.SetRect(nDeLeft,nDeTop,cx-nDeRight, cy-nDeBottom);	
		}
		m_RichBox.MoveWindow(rc, TRUE);

		CRect rcOK(rc.right + 10, nDeTop, rc.right + 65, nDeButtonHeigth);
		m_OK.MoveWindow(rcOK, TRUE);
		
		CRect rc2(11, nDeTop, rc.left - 3, cy - 10);
		m_ListBox.MoveWindow(rc2, TRUE);
		
		CRect rc3(nDeLeft, 11, cx - nDeRight, nDeTop);
		m_MefDef.MoveWindow(rc3, TRUE);


		if (m_Copy1.IsWindowVisible())
			nCopyCount++;
		if (m_Copy2.IsWindowVisible())
			nCopyCount++;
		if (m_Copy3.IsWindowVisible())
			nCopyCount++;

		if (nCopyCount == 1)
		{
			CRect rc4(rc.left, cy - 35, rc.right, cy - 10);

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
			CRect rc4(rc.left, cy - 35, rc.left + nOffset - 2, cy - 10);
			CRect rc5(rc.left + nOffset + 2, cy - 35, rc.left + nOffset + nOffset, cy - 10);
			
			if (m_Copy1.IsWindowVisible())
				m_Copy1.MoveWindow(rc4, TRUE);
			if (m_Copy2.IsWindowVisible())
				m_Copy2.MoveWindow(rc5, TRUE);
		}

		if (nCopyCount == 3)
		{
			int nOffset = (rc.Width()/3);
			CRect rc4(rc.left, cy - 35, rc.left + nOffset - 2, cy - 10);
			CRect rc5(rc.left + nOffset + 2, cy - 35, rc.left + nOffset+ nOffset - 2, cy - 10);
			CRect rc6(rc.left + nOffset + nOffset + 2, cy - 35, rc.left + nOffset + nOffset + nOffset, cy - 10);

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
	if( theWorkspace.GetActiveDocument()->GetPathName().IsEmpty() )
	{
		int nWhatNext = MessageBox( theWorkspace.LoadResourceString(IDS_RENAMEPROJECT),
																theWorkspace.LoadResourceString(IDR_MAINFRAME),
																MB_YESNOCANCEL | MB_ICONEXCLAMATION | MB_DEFBUTTON1 );
		if( nWhatNext == IDYES )
		{
			if( !theWorkspace.GetActiveDocument()->DoFileSave() )
				return;
			SelectionChanged( m_ListBox.GetSelectedItem() );
		}
		else if( nWhatNext != IDNO )
			return;
	}

	CStringA source( m_sClipBoardDefun3 );
	//put your text in source
	if(OpenClipboard())
	{
		HGLOBAL clipbuffer;
		CHAR * buffer;
		EmptyClipboard();
		clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength() + 1);
		buffer = (CHAR*)GlobalLock(clipbuffer);
		lstrcpynA(buffer, (LPCSTR)source, source.GetLength() + 1);
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
			TOleControlPtr pOleObject = theWorkspace.GetOleControlFor(pProperty);
			if (pOleObject != NULL)
			{
				LoadOleObjectIntoTree(TDclControlLockedPtr(pOleObject));
				return pOleObject->GetActiveXTypeName();
			}
		}
		else
		{
			TOleControlPtr pOleObject = theWorkspace.GetOleControlFor(pMethod);
			if (pOleObject != NULL)
			{
				LoadOleObjectIntoTree(TDclControlLockedPtr(pOleObject));
				return pOleObject->GetActiveXTypeName();
			}			
		}
	}
	return sType;
}
