// PropertyListCtrl.cpp : Implementation of the CPropertyListCtrl ActiveX Control class.

#include "stdafx.h"
#include "PropertyListCtrl.h"
#include "ObjectBrowser.h"
#include "PropertyObject.h"
#include "OpenDCLView.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PictureObject.h"
#include "ListBoxDlg.h"
#include "PropertyIds.h"
#include "ControlHolder.h"
#include "AxContainerCtrl.h"
#include "AxInterfaceDescriptor.h"
#include "ColourPopup.h"
#include "VarUtils.h"
#include "ControlTypes.h"
#include "ChildFrm.h"
#include "PreviewFileDlg.h"
#include "FontPropPage.h"
#include "ArchiveEx.h"
#include "Geometry.h"
#include "Colors.h"
#include "ComboBoxPage.h"
#include "TextBoxFilters.h"
#include "ButtonStyles.h"
#include "ImageListPage.h"
#include "TabsPane.h"
#include "SortTabs.h"
#include "ColumnsPage.h"
#include "ToolTipsPage.h"
#include "ProgressBarPage.h"
#include "ProjectCollection.h"
#include "OpenDCL.h"
#include "EditorWorkspace.h"
#include "SharedRes.h"

#define nPromptForNewPicture  1
#define nPromptForNewMasked  2


static CString LTOA(int nVal)
{
  CString sLong;
	sLong.Format(_T("%d"), nVal);
  return sLong;
}


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPropertyListCtrl, CWnd)
	//{{AFX_MSG_MAP(CPropertyListCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_PAINT()
	ON_EN_CHANGE(IDC_PROPEDIT, OnChangeEdit)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map




/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl::CPropertyListCtrl - Constructor

CPropertyListCtrl::CPropertyListCtrl()
{
	m_pIntelHelp = NULL;
	m_PopUpCreated = FALSE;
	m_ScrollBarCreated = FALSE;
	m_Cursor = NULL;
	m_Cursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_pView = NULL;	
	m_pPropTitle = NULL;
	m_pPropDesc = NULL;
	m_pColorPopup = NULL;
	m_pModeless = NULL;
	m_pParent = NULL;
	m_pControl = NULL;
	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
      m_Cursor,
      ::GetSysColorBrush(COLOR_WINDOW),
      NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl::~CPropertyListCtrl - Destructor

CPropertyListCtrl::~CPropertyListCtrl()
{
	if (m_Cursor != NULL)
	{
		DeleteObject (m_Cursor);
		m_Cursor = NULL;
	}
	if (m_pIntelHelp != NULL)
	{
		delete m_pIntelHelp;
		m_pIntelHelp = NULL;
	}
}

BOOL CPropertyListCtrl::Create(const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	//return CWnd::Create(WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,rect, pParentWnd, nID);
	m_pParent = pParentWnd;
	return CWnd::Create(
		m_ClassName,
		m_ClassName,
		WS_CHILD|WS_VISIBLE,//|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		rect,
		pParentWnd,
		nID,
		NULL);		
	
}



/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl message handlers



BOOL CPropertyListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN ||  pMsg->message == WM_CHAR )
	{		
		switch (pMsg->wParam)
		{
			case VK_RETURN:
				{
				if (m_Edit.IsWindowVisible())
				{										
					if (m_Edit.m_bAllowReturn)
						return CWnd::PreTranslateMessage(pMsg);
					else
						UpdateControls(m_Edit.m_pProp->GetID());
				}
					
				if (m_SelectedIndex < m_PropertyList.GetCount()-1)
				{
					if (m_Edit.IsWindowVisible())
						m_Edit.ShowWindow(FALSE);
	
					if (m_Button.IsWindowVisible())
						m_Button.ShowWindow(FALSE);

					
					if (m_SelectedIndex < m_PropertyList.GetCount()- 1)
					{
						int nNewSelectedIndex = m_SelectedIndex + 1;
						
						ChangeSelectedItem(nNewSelectedIndex);
						DoSetupInputType(nNewSelectedIndex);
						m_SelectedIndex = nNewSelectedIndex;
						pMsg->wParam = NULL;
						pMsg->message = NULL;

						if (m_ScrollBar.IsWindowVisible())
						{
							int nPos = m_ScrollBar.GetScrollPos();
							int nMax, nMin;
							m_ScrollBar.GetScrollRange(&nMin, &nMax);
							if (nPos < nMax)
							{
								nPos++;
								m_ScrollBar.SetScrollPos(nPos, TRUE);
							}
							OnVScroll(SB_LINEDOWN, nPos, &m_ScrollBar);
						}
					}
				}
				
				break;
				}
			case VK_DELETE:
				{
					if (!m_Edit.IsWindowVisible())
						m_pView->DeleteSelectedControls();
				break;
				}
			case VK_ESCAPE:			
				m_Edit.SetWindowText(m_Edit.m_OriginalValue);
				m_Edit.CommitValue();
				m_Edit.SetSel(0, m_Edit.m_OriginalValue.GetLength(), FALSE);

				return 1;
				break;
			default:
				m_pView->PreTranslateMessage(pMsg);
				break;

		}
		
	}	
	
	
	return CWnd::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
// CPropertyListCtrl message handlers

int CPropertyListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// fix up 3D styles
	ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
	
	m_SelectedIndex = 0;
	m_TopIndex = 0;
	
	CRect rcScrollBar(
		lpCreateStruct->x - nScrollBarWidth,
		0, 
		nScrollBarWidth, 
		lpCreateStruct->cy - 4);

	m_ScrollBarCreated = m_ScrollBar.Create(
		WS_VISIBLE | WS_CHILD | SBS_VERT | 
		WS_GROUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		rcScrollBar, 
		this,
		nScrollBarID);
	
	m_ScrollBar.ShowScrollBar();
	m_ScrollBar.ShowWindow(FALSE);
	m_ScrollBarNeeded = FALSE;
	m_TopIndex = 0;
	m_TopIndex = 0;
	
	nItemRowHeight = nDeListRowHeight;
    CRect rc(0,0,2,2);
	m_Button.Create(rc, this, IDC_BUTTON);
	m_Button.ShowWindow(FALSE);
	
	
	m_Edit.Create(WS_CHILD|ES_MULTILINE|ES_AUTOHSCROLL|ES_WANTRETURN|ES_AUTOVSCROLL, rc, this, IDC_PROPEDIT);
	m_Edit.ShowWindow(FALSE);
	
	return 0;
}

void CPropertyListCtrl::SetFont(CFont *pFont)
{
	m_pFont = pFont;
	m_Edit.SetFont(m_pFont);
}

void CPropertyListCtrl::UpdateControls(PropertyId nPropId) 
{
	m_pView->RefreshChildControl(m_pControl, nPropId);
}

void CPropertyListCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	// turn off the edit control because we don't need it now
	if (m_ScrollBarCreated == TRUE)
	{
		m_Button.ShowWindow(FALSE);
		m_Edit.ShowWindow(FALSE);
	}
	SetScrollBar();
}

void CPropertyListCtrl::Refresh() 
{
	RePaint();
}

void CPropertyListCtrl::RefreshGrid(HDC hdc) 
{
	int nCount = m_PropertyList.GetCount();
	if (nCount <= 0)
		return;

	try
	{
		CRect rcPropArea;
		GetClientRect(&rcPropArea);
		
		int nTopIndexIndex = -1;

		if (m_SelectedIndex >= nCount)
			m_SelectedIndex = nCount - 1;
		
		HGDIOBJ pOldFont = NULL;
		// setup the font			
		try{
			// setup the font			
			pOldFont = SelectObject(hdc, m_pFont->m_hObject);
		}
		catch(...)
		{
		}

		// get the text size
		CSize szText;
		CString sText;
		sText = theWorkspace.LoadResourceString(IDS_NONE);
		::GetTextExtentPoint32(hdc, sText, sText.GetLength(), &szText);
	

		if(pOldFont != NULL)
			// restore the old font ** a must
			SelectObject(hdc, pOldFont);
	
		
		nItemRowHeight = szText.cy + 3;

		
		if((nTopIndexIndex + (nCount * nItemRowHeight)) > rcPropArea.Height() - 4)
		{	
			rcPropArea.right = rcPropArea.right - nScrollBarWidth;
			m_ScrollBarNeeded = TRUE;
		}
		else
			m_ScrollBarNeeded = FALSE;

		SetScrollBar();
		
		// test for a need for a scroll bar and draw accordingly
		for(int i = m_TopIndex; i < nCount; i++)
		{	
			// get the text position
			POSITION pos = m_PropertyList.FindIndex(i);
			if (pos != NULL)
			{
				// get the property Name text
				RefCountedPtr< CPropertyObject > pProperty = m_PropertyList.GetAt(pos);

				int nDrawStyle;
				
				if (m_SelectedIndex == i)
					nDrawStyle = nDrawStyle_SelectedRow;
				else
					nDrawStyle = nDrawStyle_NonSelectedRow;
				
				
				DrawRow(
					i,
					nDrawStyle,
					&rcPropArea, 
					hdc);
			}
			nTopIndexIndex = nTopIndexIndex + nItemRowHeight;
		}	
		
		if (nTopIndexIndex < rcPropArea.bottom)
		{
			if(rcPropArea.bottom - (nTopIndexIndex + 1) > 1)
			{			
				//setup the remaining area to be filled
				CRect rcFillArea(0, nTopIndexIndex + 1, rcPropArea.right-1, rcPropArea.bottom);	
				// draw the solid rectangle
				::SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
				::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcFillArea, NULL, 0, NULL);
			}
		}
	}
	catch(...)
	{
	}
}

void CPropertyListCtrl::RePaint() 
{
	// make any child controls not visible
	m_Button.ShowWindow(FALSE);
	m_Edit.ShowWindow(FALSE);

	// repaint the entire control	
	HDC hdc = ::GetDC(m_hWnd);
	if (m_PropertyList.GetCount() == 0)
	{
		ClearArea(hdc);
	}
	else	
	{

		RefreshGrid(hdc);
	}

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

	if (m_ScrollBarNeeded == TRUE)
	{
		//CWnd::ShowScrollBar(nScrollBarID, TRUE);
		m_ScrollBar.ShowScrollBar(TRUE);
		m_ScrollBar.ShowWindow(TRUE);
		m_ScrollBar.RedrawWindow(NULL, NULL, RDW_UPDATENOW|RDW_INVALIDATE);
	}
	else
	{
		m_ScrollBar.ShowScrollBar(FALSE);
		m_ScrollBar.ShowWindow(FALSE);
	}
}


RefCountedPtr< CPropertyObject > CPropertyListCtrl::GetPropertyObject(short PropertyIndex) 
{
	if (m_PropertyList.GetCount() == 0)
		return NULL;
	POSITION pos = m_PropertyList.FindIndex(PropertyIndex);

	if (pos == NULL)
		return NULL;

	RefCountedPtr< CPropertyObject > pPropertyObject = m_PropertyList.GetAt(pos);

	return pPropertyObject;
}

//void CPropertyListCtrl::ClearImageListPropery(short DclFormIndex, short ArxControlIndex, short PropertyIndex) 
//{
//	RefCountedPtr< CPropertyObject > pPropertyObject = GetPropertyObject(PropertyIndex);
//	
//	// check if the indexes were correct
//	if (pPropertyObject != NULL)
//	{
//		if (pPropertyObject->GetShortValue() > -1)
//		{
//			// get requested dcl form
//			CDclFormObject *pDclForm = GetDclFormObject(DclFormIndex);
//		
//			// check if the index were correct
//			if (pDclForm != NULL)
//			{				
//				POSITION pos;
//			
//				// get the position of the image list index
//				pos = pDclForm->m_ImageListCollection.FindIndex(pPropertyObject->GetShortValue());
//					
//				// get the image list object
//				CImageListObject *pImageList = pDclForm->m_ImageListCollection.GetAt(pos);
//	
//				// clear the image list
//				pImageList->m_ImageList.DeleteImageList();
//			}
//		}
//	}
//	
//}




//short CPropertyListCtrl::AddPictureToImageList(short DclFormIndex, short ArxControlIndex, short PropertyIndex, LPPICTUREDISP newPicture) 
//{
//	// create a new property object and point it at the object in the list
//	RefCountedPtr< CPropertyObject > pPropertyObject = GetPropertyObject(PropertyIndex);
//	
//	// check if the indexes were correct
//	if (pPropertyObject == NULL)
//		// return failed indicator
//		return -5;
//	else
//	{
//		CPictureHolder NewPicture;
//
//		// put picture into a picture holder
//		NewPicture.SetPictureDispatch(newPicture);
//		
//		// get requested dcl form
//		CDclFormObject *pDclForm = CPropertyListCtrl::GetDclFormObject(DclFormIndex);
//		
//		// check if the index were correct
//		if (pDclForm == NULL)
//			// return failed indicator
//			return -5;
//		else
//		{				
//			// get the position of the image list index
//			POSITION pos = pDclForm->m_ImageListCollection.FindIndex(pPropertyObject->GetShortValue());
//	
//			// get the image list object
//			CImageListObject *pImageList = pDclForm->m_ImageListCollection.GetAt(pos);
//		
//			// add the picture
//			ImageListAddPicture(&NewPicture, pImageList, &pImageList->m_ImageList, &pImageList->m_ImageSize, TRUE);
//			
//			return 0;				
//		}
//	}
//	
//	return -1;
//}


//void CPropertyListCtrl::InitImageList(short DclFormIndex, short ArxControlIndex, short PropertyIndex, short ImageWidth, short ImageHeight) 
//{
//	// create a new DclFormObject object and point it at the object in the list
//	RefCountedPtr< CPropertyObject > pPropertyObject = GetPropertyObject(PropertyIndex);
//	
//	// check if the indexes were correct
//	if (pPropertyObject != NULL)
//	{	
//		if (pPropertyObject->GetShortValue() == -1)
//		{					
//			// create new image list object
//			CImageListObject *pImageList = new CImageListObject;
//
//			// init the image list
//			pImageList->m_ImageList.m_hImageList = NULL;
//			
//			// get requested dcl form
//			CDclFormObject *pDclForm = CPropertyListCtrl::GetDclFormObject(DclFormIndex);
//			
//			// check if the index were correct
//			if (pDclForm != NULL)
//			{
//				// add the new image list object
//				pDclForm->m_ImageListCollection.AddTail(pImageList);
//					
//				// assign the index to the property object
//				pPropertyObject->SetShortValue(pDclForm->m_ImageListCollection.GetCount() - 1);
//			}
//		}
//	}
//}


short CPropertyListCtrl::CountPictures() 
{	
	return m_pControl->GetOwnerProject()->GetPictureList().GetCount();
}




//LPPICTUREDISP CPropertyListCtrl::GetImageListPicture(short DclFormIndex, short ArxControlIndex, short PropertyIndex, short ImageIndex) 
//{
//	// create a new DclFormObject object and point it at the object in the list
//	RefCountedPtr< CPropertyObject > pPropertyObject = GetPropertyObject(PropertyIndex);
//	
//	// check if the indexes were correct
//	if (pPropertyObject == NULL)
//		// return failed indicator
//		return NULL;
//	
//	// get requested dcl form
//	CDclFormObject *pDclForm = CPropertyListCtrl::GetDclFormObject(DclFormIndex);
//	
//	// if the object is null return a blank picture
//	if (pDclForm == NULL)
//		// return failed indicator
//		return NULL;
//	
//	// get the position of the image list index
//	int nImageIndex = pPropertyObject->GetShortValue();
//	POSITION pos = pDclForm->m_ImageListCollection.FindIndex(pPropertyObject->GetShortValue());
//			
//	// if the object is null return a blank picture
//	if (pos == NULL)
//		return NULL;
//	
//	// get the image list object
//	CImageListObject *pImageList = pDclForm->m_ImageListCollection.GetAt(pos);
//
//	// if the object is null return a blank picture
//	if (pImageList == NULL)
//		return NULL;
//	
//	// extract the icon
//	HICON hExtractedIcon = pImageList->m_ImageList.ExtractIcon(ImageIndex);
//
//	// if the object is null return a blank picture
//	if (hExtractedIcon == NULL)
//		return NULL;
//	
//
//	CPictureHolder ExtractedPicture;
//
//	// place the icon into the picture holder
//	ExtractedPicture.CreateFromIcon(hExtractedIcon, FALSE);
//
//	// return the picture 
//	return ExtractedPicture.GetPictureDispatch();
//	
//}
//
//long CPropertyListCtrl::GetImageListPictureWidth(short DclFormIndex, short ArxControlIndex, short PropertyIndex) 
//{
//	// create a new DclFormObject object and point it at the object in the list
//	RefCountedPtr< CPropertyObject > pPropertyObject = GetPropertyObject(PropertyIndex);
//	
//	// check if the indexes were correct
//	if (pPropertyObject == NULL)
//		// return failed indicator
//		return 0;
//	else
//	{
//		// get requested dcl form
//		CDclFormObject *pDclForm = CPropertyListCtrl::GetDclFormObject(DclFormIndex);
//		
//		// check if the index were correct
//		if (pDclForm == NULL)
//			// return failed indicator
//			return 0;
//		else
//		{		
//			// get the position of the image list index
//			POSITION pos = pDclForm->m_ImageListCollection.FindIndex(pPropertyObject->GetShortValue());
//					
//			// get the image list object
//			CImageListObject *pImageList = pDclForm->m_ImageListCollection.GetAt(pos);
//		
//			// return the picture 
//			return pImageList->m_ImageSize.cx;
//		}
//	}
//	
//	return 0;
//}
//
//long CPropertyListCtrl::GetImageListPictureHeight(short DclFormIndex, short ArxControlIndex, short PropertyIndex) 
//{
//	// create a new DclFormObject object and point it at the object in the list
//	RefCountedPtr< CPropertyObject > pPropertyObject = GetPropertyObject(PropertyIndex);
//	
//	// check if the indexes were correct
//	if (pPropertyObject == NULL)
//		// return failed indicator
//		return 0;
//	else
//	{
//		// get requested dcl form
//		CDclFormObject *pDclForm = CPropertyListCtrl::GetDclFormObject(DclFormIndex);
//		
//		// check if the index were correct
//		if (pDclForm == NULL)
//			// return failed indicator
//			return 0;
//		else
//		{		
//			// get the position of the image list index
//			POSITION pos = pDclForm->m_ImageListCollection.FindIndex(pPropertyObject->GetShortValue());
//					
//			// get the image list object
//			CImageListObject *pImageList = pDclForm->m_ImageListCollection.GetAt(pos);
//		
//			// return the picture 
//			return pImageList->m_ImageSize.cy;
//		}
//	}	
//
//	return 0;
//}




CDclControlObject* CPropertyListCtrl::GetArxControlObject(short DclFormIndex, short ArxControlIndex)
{
	if (ArxControlIndex < 0)
		return NULL;

	CDclControlObject *pRetObject;
	bool bRetVal;
 	if(m_pControl->GetOwnerProject()->GetDclFormList().GetCount() > DclFormIndex)
	{
		// create a position variable to hold the converted DclFormIndex
		POSITION FormPos;
			
		// set the position variable to be equal the index to passing to the GetAt method
		FormPos = m_pControl->GetOwnerProject()->GetDclFormList().FindIndex(DclFormIndex);	
		// create a new DclFormObject object and point it at the object in the list
		CDclFormObject* pNewDclForm = m_pControl->GetOwnerProject()->GetDclFormList().GetAt(FormPos);	
		

		if(pNewDclForm->GetControlList().GetCount() >= ArxControlIndex)
		{
			// create a position variable to hold the converted ArxControlIndex
			POSITION ControlPos;
			
			// set the position variable to be equal the index to passing to the GetAt method
			ControlPos = pNewDclForm->GetControlList().FindIndex(ArxControlIndex);	
			
			// set the pass pointer to point at the object in the list
			pRetObject = pNewDclForm->GetControlList().GetAt(ControlPos);
			
			bRetVal = true;
		}				
		else
			bRetVal = false;
	}
	else
		bRetVal = false;

	if (bRetVal)
		return pRetObject;
	else
		return NULL;
}

CDclFormObject* CPropertyListCtrl::GetDclFormObject(short DclFormIndex)
{
	CDclFormObject *pRetObject;
	bool bRetVal;

	
	if(m_pControl->GetOwnerProject()->GetDclFormList().GetCount() > DclFormIndex)
	{
		// set the position variable to be equal the index to passing to the GetAt method
		POSITION pos = m_pControl->GetOwnerProject()->GetDclFormList().FindIndex(DclFormIndex);	
		
		if (pos == NULL)
			return NULL;
		// set the pass pointer to point at the object in the list
		pRetObject = m_pControl->GetOwnerProject()->GetDclFormList().GetAt(pos);
		
		bRetVal = true;
	}
	else
		bRetVal = false;

	if (bRetVal)
		return pRetObject;
	else
		return NULL;
}


void CPropertyListCtrl::ClearGrid() 
{
	m_PropertyList.RemoveAll();

	if (!IsWindow(m_hWnd))
		return;

	// repaint the entire control
	HDC hdc = ::GetDC(m_hWnd);

	ClearArea(hdc);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

	m_pControl = NULL;

	// make any child controls not visible
	if (m_Button.IsWindowVisible())
		m_Button.ShowWindow(SW_HIDE);

	if (m_Edit.IsWindowVisible())
	{
		m_Edit.ShowWindow(FALSE);
		UpdateControls(m_Edit.m_pProp->GetID());
	}

	if (m_ScrollBarCreated && m_ScrollBar.IsWindowVisible())
		m_ScrollBar.ShowWindow(SW_HIDE);
}

void CPropertyListCtrl::ClearArea(HDC hdc)
{;
			
	CRect rcThis;
	if (IsWindow(m_hWnd))
	{
		GetWindowRect(&rcThis);
	
		CRect rcCell(0,0,rcThis.Width(), rcThis.Height());

		// draw the Window background for the cell				
		::SetBkColor(hdc, ::GetSysColor(COLOR_WINDOW));
		::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcCell, NULL, 0, NULL);
	}
}

void CPropertyListCtrl::DisplayProperties(CDclControlObject *pControl) 
{
	if( pControl == m_pControl )
		return;
	
	if (pControl != NULL)
	{
		m_pControl = pControl;
		m_TopIndex = 0;
		m_ScrollBar.SetScrollPos(m_TopIndex, FALSE);			
	}	
	
	if (pControl == NULL || (pControl->GetType() == CtlForm && pControl->GetOwnerForm()->GetParentForm()) )
	{
		ClearGrid();
		m_Edit.ShowWindow( SW_HIDE );
		return;
	}

	// clear the property list
	m_PropertyList.RemoveAll();
	POSITION posProp = pControl->GetPropertyList().GetHeadPosition();
	while (posProp != NULL)
	{
		RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyList().GetNext(posProp);
		if (!pProp->IsHidden()) // if not a hidden property add it to the property list
			m_PropertyList.AddTail(pProp);
	}

	// if the selected index is greater than the available properties for this control, set it to zero
	if (m_SelectedIndex > m_PropertyList.GetCount() - 1)
		m_SelectedIndex = 0;

	// get the property selected
	RefCountedPtr< CPropertyObject > pPropSelected = GetPropertyObject(m_SelectedIndex);
					
	if (pPropSelected == NULL)
	{
		ClearGrid();
		return;
	}

	RePaint();
	
	// fire the cell changed event to notify parent of the new cell selected,
	// pass it the property ID and the caption text
	UpdatePropHelpCtrls(pPropSelected);
}


void CPropertyListCtrl::OnDestroy() 
{
	m_Button.DestroyWindow();
	m_Edit.DestroyWindow();
	m_ScrollBar.DestroyWindow();
	CWnd::OnDestroy();
}

void CPropertyListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_PropertyList.GetCount() == 0)
		return;

	// get the selected index
	int nNewSelectedIndex = GetSelectedIndex(point.y);
	
	// display the selected index
	if(m_SelectedIndex != -1)
		ChangeSelectedItem(nNewSelectedIndex);
		
	CWnd::OnLButtonDblClk(nFlags, point);
}

void CPropertyListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	SetFocus();

	if (m_PopUpCreated == TRUE && m_pModeless != NULL)
	{
		m_pModeless->DestroyWindow();
		delete m_pModeless;
		m_pModeless = NULL;
		m_PopUpCreated = FALSE;
	}

	if (m_PropertyList.GetCount() == 0)
		return;

	if (m_Button.IsWindowVisible())
	{
		m_Button.ShowWindow(FALSE);		
	}

	if (m_Edit.IsWindowVisible())
	{
		m_Edit.ShowWindow(FALSE);
		UpdateControls(m_Edit.m_pProp->GetID());
	}

	
	// get the selected index
	int nNewSelectedIndex = GetSelectedIndex(point.y);
	
	// display the selected index
	if(nNewSelectedIndex != -1)
		ChangeSelectedItem(nNewSelectedIndex);
		
	CWnd::OnLButtonDown(nFlags, point);
}

void CPropertyListCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// get the selected index
	int nNewSelectedIndex = GetSelectedIndex(point.y);
	
	DoSetupInputType(nNewSelectedIndex);

	CWnd::OnLButtonUp(nFlags, point);
}

void CPropertyListCtrl::DoSetupInputType(int nNewSelectedIndex)
{
	int nType = -1;

	if (m_PropertyList.GetCount() == 0)
		return;

	if (m_Button.IsWindowVisible())
	{
		m_Button.ShowWindow(FALSE);		
	}

	if (m_Edit.IsWindowVisible())
	{
		m_Edit.ShowWindow(FALSE);
		UpdateControls(m_Edit.m_pProp->GetID());
	}

	// display the selected index
	if(nNewSelectedIndex != -1)
	{
		CRect rcCell = ChangeSelectedItem(nNewSelectedIndex);

		if (rcCell.top == nReturnError && rcCell.left == nReturnError)
			return;

		POSITION pos = m_PropertyList.FindIndex(m_SelectedIndex);
		if (pos == NULL)
			return;

		RefCountedPtr< CPropertyObject > pProp = m_PropertyList.GetAt(pos);
		
		// if this is an ActiveX property of type BOOL then
		if (pProp->GetType() == PropActiveXProp)
		{
			VARTYPE varType = pProp->GetAxInterfaceDescriptorPtr()->GetType();
			switch (varType)
			{
				case VT_BOOL:		
				case VT_UI1:
				case VT_I1:
					nType = PropBool;
					break;
				case VT_BSTR:
				case VT_DATE:
				case VT_LPSTR:
				case VT_LPWSTR:
				case VT_VARIANT:			
					nType = PropString;
					break;				
				case VT_I2:
				case VT_I4:
				case VT_R4:
				case VT_R8:
				case VT_DECIMAL:
				case VT_INT:
				case VT_UINT:
				case VT_UI2:
				case VT_CY:
				case VT_UI4:
					nType = PropLong;
					break;					
							
				case VT_DISPATCH:
					{
					nType = -1;
					}

					break;		
				case VT_USERDEFINED:	
					//nType = PropActiveXRunTime;
					//pProp->GetType() = PropActiveXRunTime;
					break;				
			}
			if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == GUID_COLOR)
				nType = PropOLEColor;
			if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IPictureDisp)
				nType = PropPicture;
			if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFont ||
					pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFontDisp)
			{
				if (m_Button.GetButtonStyle() != nDotsImage)
				{
					m_Button.SetButtonStyle(nDotsImage);
					m_Button.Refresh();
				}
				m_Button.MoveWindow(
					rcCell.left + rcCell.Width() - nButtonWidth,
					rcCell.top,
					nButtonWidth,
					rcCell.Height(),
					TRUE);		
				m_Button.ShowWindow(TRUE);		
			
			}
					
			// if an enum is required
			if(pProp->GetAxInterfaceDescriptorPtr()->GetEnumDescriptor())
				nType = PropEnum;
		}
		else
			nType = pProp->GetType();
		
		
		switch(nType)
		{		
			case PropBool:
			case PropEnum:
			case PropPicture:
			case PropStringArray:
			case PropIntArray:
			case PropOLEColor:
			{
				m_Button.ShowWindow(FALSE);		
				if (m_Button.GetButtonStyle() != nDropDownImage)
				{
					m_Button.SetButtonStyle(nDropDownImage);
					m_Button.Refresh();
				}
				
				m_Button.MoveWindow(
					rcCell.left + rcCell.Width() - nButtonWidth-1,
					rcCell.top,
					nButtonWidth,
					rcCell.Height(),
					TRUE);		
				m_Button.ShowWindow(TRUE);		
				break;
			}	
			case PropActiveXMethods:
			case PropActiveXPropPages:
			case PropCustom:	
			case PropEvent:
			case PropImageList:			
			{
				
				if (m_Button.GetButtonStyle() != nDotsImage)
				{
					m_Button.SetButtonStyle(nDotsImage);
					m_Button.Refresh();
				}
				m_Button.MoveWindow(
					rcCell.left + rcCell.Width() - nButtonWidth-1,
					rcCell.top,
					nButtonWidth,
					rcCell.Height(),
					TRUE);		
				m_Button.ShowWindow(TRUE);		
				break;
			}	
			case PropLong:
			case PropString:
			case PropDouble:
			{
				if (pProp->GetID() == nLabelName)
				{
					if (m_Button.GetButtonStyle() != nDotsImage)
					{
						m_Button.SetButtonStyle(nDotsImage);
						m_Button.Refresh();
					}
					m_Button.MoveWindow(
						rcCell.left + rcCell.Width() - nButtonWidth,
						rcCell.top,
						nButtonWidth,
						rcCell.Height(),
						TRUE);		
					m_Button.ShowWindow(TRUE);		
				
				}
				else
				{
					// calc the edit's position
					CRect rcEdit = rcCell;
					int nEditHeight = rcEdit.Height();
					
					//rcEdit.left += nEditCellXOffset + 2;
					//rcEdit.top += nEditCellYOffset + 15 + 2;
					rcEdit.left += nEditCellXOffset;
					rcEdit.top += nEditCellYOffset;
					rcEdit.right--;
					//rcEdit.bottom += nEditHeight;
					
					CString sText;
					CControlHolder *pAxCtrl = NULL;
						
					if (pProp->GetType() == PropActiveXProp)
					{
						pAxCtrl = (CControlHolder*)m_pControl->m_pCtrlHolder;
						pAxCtrl->GetActiveXCtrl()->GetProperty( pProp->GetAxInterfaceDescriptorPtr()->GetGetDescriptor(), sText );
					}
					else
						// get the property text.
						sText = pProp->GetStdProperty();

					// set the string keyboard filter according to the type of property
					switch (pProp->GetType())
					{
						case PropLong:
							m_Edit.m_sFilter = theWorkspace.LoadResourceString(IDS_LONGFILTER);
							break;
						case PropDouble:
							m_Edit.m_sFilter = theWorkspace.LoadResourceString(IDS_DOUBLEFILTER);
							break;
						case PropActiveXPropPages:
						case PropActiveXProp:
						case PropActiveXEnum:
						case PropActiveXEvent:
						case PropActiveXRunTime:
						case PropActiveXMethods:
							switch (pProp->GetAxInterfaceDescriptorPtr()->GetType())
							{
								case VT_DATE:
									m_Edit.m_sFilter = theWorkspace.LoadResourceString(IDS_DATEFILTER);
									break;	
								case VT_I2:
								case VT_I4:
								case VT_INT:
									m_Edit.m_sFilter = theWorkspace.LoadResourceString(IDS_LONGFILTER);
									break;
								case VT_R4:
								case VT_R8:
								case VT_DECIMAL:
								case VT_CY:
									m_Edit.m_sFilter = theWorkspace.LoadResourceString(IDS_DOUBLEFILTER);
									break;						
								case VT_UINT:
								case VT_UI2:
								case VT_UI4:
									m_Edit.m_sFilter = theWorkspace.LoadResourceString(IDS_UINTFILTER);
									break;				
								case VT_UI1:
								case VT_I1:
									m_Edit.m_sFilter = LTOA(10);
									break;				
								
							}
							break;
						default:
							m_Edit.m_sFilter = theWorkspace.LoadResourceString(IDS_ANYFILTER);
							break;						
					}
					// set the string keyboard filter according to the type of ActiveX property
					// set the original value
					m_Edit.m_OriginalValue = sText;
					// set the AxContainer so the edit box can update the property correctly.
					m_Edit.m_pAxContainer = pAxCtrl;
					// allow the edit box to accept returns
					m_Edit.m_bAllowReturn = false;
					// set the property pointer
					m_Edit.m_pProp = pProp;
					// set the edit's text
					m_Edit.SetWindowText(sText);
					m_Edit.m_pControl = m_pControl;
					m_Edit.m_pView = m_pView;
					// move the edit into position
					m_Edit.MoveWindow(rcEdit, TRUE);					
					try
					{
						m_pParent->SetFocus();
					}
					catch(...){}
					// show the edit.
					m_Edit.ShowWindow(TRUE);	
					// force the edit to take focus
					m_Edit.SetFocus();
					// set the edit to select all the text.
					m_Edit.SetSel(0, sText.GetLength(), TRUE);
					
				}
				break;
			}

		} // end of switch
	}
		
	
}

void CPropertyListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_PropertyList.GetCount() == 0)
		return;

	if (nFlags == MK_LBUTTON)
	{
		// get the selected index
		int nNewSelectedIndex = GetSelectedIndex(point.y);
		
		// display the selected index
		if(nNewSelectedIndex != -1)
			ChangeSelectedItem(nNewSelectedIndex);
	}
	
	CWnd::OnMouseMove(nFlags, point);
}

void CPropertyListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int MinPos;
	int MaxPos;
	int ThisPos = m_TopIndex;
	FireScrolling();

	if (m_Button.IsWindowVisible())
	{
		m_Button.ShowWindow(FALSE);
	}

	if (m_Edit.IsWindowVisible())
	{
		m_Edit.ShowWindow(FALSE);
		UpdateControls(m_Edit.m_pProp->GetID());
	}

	m_ScrollBar.GetScrollRange(&MinPos, &MaxPos);
	
	switch (nSBCode)
	{
	case SB_LINEUP:
		{
			m_TopIndex = m_TopIndex - 1;
			break;
		}
	case SB_LINEDOWN:
		{
			m_TopIndex = m_TopIndex + 1;
			break;
		}		
	case SB_PAGEUP:
		{
			m_TopIndex = m_TopIndex - m_RowsPerPage;
			break;
		}
	case SB_PAGEDOWN:
		{
			m_TopIndex = m_TopIndex + m_RowsPerPage;
			break;
		}
	case SB_THUMBPOSITION:
		{
			m_TopIndex = (int)nPos;
			break;
		}
	}
	if (m_TopIndex < MinPos)
		m_TopIndex = MinPos;
	if (m_TopIndex > MaxPos)
		m_TopIndex = MaxPos;
		
	m_ScrollBar.SetScrollPos(m_TopIndex, TRUE);	
	
	// turn off the edit control because we don't need it now
	m_Button.ShowWindow(FALSE);	
	m_Edit.ShowWindow(FALSE);
	
	if (ThisPos != m_TopIndex)
	{
		
		HDC hdc = ::GetDC(m_hWnd);
		RefreshGrid(hdc);
		// then releasing the DC itself
  	    ::ReleaseDC(m_hWnd, hdc);

	}
	
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CPropertyListCtrl::SetScrollBar() 
{
	CRect rcThis;	
	
	int nCount = 0;
	
	GetWindowRect(&rcThis);
	
	nCount = m_PropertyList.GetCount();						
	
	if (nCount <=0)
		return;
	
	int nTopIndexIndex = -1;
	
	
	// check for the need for a scroll bar
	if((nTopIndexIndex + (nCount * nItemRowHeight) ) > (rcThis.Height() - 4) && m_ScrollBarCreated == TRUE)
	{		
		m_ScrollBarNeeded = TRUE;
		// move the scroll bar to the right side of the control
		m_ScrollBar.MoveWindow(
			rcThis.Width() - nScrollBarWidth - 4, 
			0,
			nScrollBarWidth,
			rcThis.Height() - 4,
			TRUE);
		
		int nPos = nCount - ((rcThis.Height() - 4) / nItemRowHeight);
		
		m_RowsPerPage = nCount - nPos;
		m_ScrollBar.SetScrollRange(0, nPos, TRUE);
		
	
		int MinPos;
		int MaxPos;
	
		m_ScrollBar.GetScrollRange(&MinPos, &MaxPos);
	
		if (m_TopIndex > MaxPos)
			m_TopIndex = MaxPos;

		m_ScrollBar.SetScrollPos(m_TopIndex, TRUE);
	
		
		if (m_ScrollBarCreated == TRUE)
		{
			if (m_ScrollBarNeeded == FALSE)
				m_ScrollBar.ShowWindow(SW_HIDE);
			else
			{
				m_ScrollBar.ShowWindow(SW_SHOWNORMAL);
				m_ScrollBar.RedrawWindow(NULL, NULL, RDW_UPDATENOW|RDW_INVALIDATE);
			}
		}
	}
	else
	{
		if (m_ScrollBarCreated == TRUE)
		{
			m_ScrollBarNeeded = FALSE;
			m_ScrollBar.ShowWindow(SW_HIDE);
		}
	}

}

void CPropertyListCtrl::DrawRow(int nRow, int nDrawStyle, CRect *pRcClientArea, HDC hdc) 
{
	switch(nDrawStyle)
	{
	// redraw the old selected cell
	case nDrawStyle_RemoveHighLight:
	// high light the new selected cell
	case nDrawStyle_SetAsHighLight:	
		{
		DrawCell(
			nRow, 
			0, 
			nDrawStyle,
			pRcClientArea,
			hdc);				
		DrawRowGrid(nRow, pRcClientArea, hdc);
		break;
		}
	case nDrawStyle_NonSelectedRow:
	case nDrawStyle_SelectedRow:
		{
		DrawCell(
			nRow, 
			0, 
			nDrawStyle,
			pRcClientArea,
			hdc);
		DrawCell(
			nRow, 
			1, 
			nDrawStyle,
			pRcClientArea,
			hdc);
		DrawRowGrid(nRow, pRcClientArea, hdc);
		
		break;
		}
	}

}
void CPropertyListCtrl::DrawRowGrid(int nRow, CRect *pRcClientArea, HDC hdc)
{
	CPoint point;
	int nTopIndexIndex = ((nRow - m_TopIndex) * nItemRowHeight) -1;
	int nBottom = nTopIndexIndex + nItemRowHeight;
	int nMiddle = pRcClientArea->right / 2;

	if (nTopIndexIndex < pRcClientArea->bottom)
	{
		COLORREF rgb = ::GetSysColor(COLOR_BTNFACE);
		HGDIOBJ pen = ::CreatePen(PS_SOLID, 1, rgb);
		HGDIOBJ OldPen = SelectObject(hdc, pen);
		
		// draw the properties rectangle
		MoveToEx(hdc, 0, nTopIndexIndex + nItemRowHeight, &point);
		LineTo(hdc, pRcClientArea->right-1, nBottom);
		LineTo(hdc, pRcClientArea->right-1, nTopIndexIndex);
		MoveToEx(hdc, nMiddle, nTopIndexIndex, &point);		
		LineTo(hdc, nMiddle, nBottom);
		
		SelectObject(hdc, OldPen);			
		DeleteObject(pen);
	}
	
}
void CPropertyListCtrl::DrawCell(int nRow, int nCell, int nDrawStyle, CRect *pRcClientArea, HDC hdc)
{
	int nTopIndex = ((nRow - m_TopIndex) * nItemRowHeight) -1;
	int nMiddle = (pRcClientArea->right / 2);

	if (nTopIndex < pRcClientArea->bottom)
	{
	
		CRect rcText;
		CRect rcCell;		
		
		if (nCell > 0)
		{
			// setup for the name cell 
			rcCell.left = nMiddle + 1;
			rcCell.top = nTopIndex + 1;
			rcCell.right = pRcClientArea->right;
			rcCell.bottom = nTopIndex + nItemRowHeight;			
			
			// setup the text position
			rcText.left = nMiddle + nValueLeftOffset;
			rcText.right = pRcClientArea->right - 1;
			rcText.top = nTopIndex + nTextTopOffset;
			rcText.bottom = nTopIndex + nItemRowHeight;// - nTextTopOffset;			
		}
		else	
		{
			// setup for the value cell position
			rcCell.left = 0;
			rcCell.top = nTopIndex + 1;
			rcCell.right = nMiddle;
			rcCell.bottom = nTopIndex + nItemRowHeight;

			// setup the Value text position
			rcText.left = nNameLeftOffset;
			rcText.top = nTopIndex + nTextTopOffset;
			rcText.right = nMiddle - nNameLeftOffset;
			rcText.bottom =	nTopIndex + nItemRowHeight;// - nTextTopOffset;	
		}
		
		// set the colors accordingly
		switch (nDrawStyle)
		{
		case nDrawStyle_RemoveHighLight:
		case nDrawStyle_NonSelectedRow:
			{
			// setup the color for the text
			SetTextColor(hdc, ::GetSysColor(COLOR_BTNTEXT));
			SetBkColor(hdc, ::GetSysColor(COLOR_WINDOW));	
			break;
			}
		case nDrawStyle_SetAsHighLight:
			{
			// setup the color for the text
			SetTextColor(hdc, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
			SetBkColor(hdc, ::GetSysColor(COLOR_HIGHLIGHT));			
			break;
			}
		case nDrawStyle_SelectedRow:
			{
			if (nCell == 0)
			{
				// setup the color for the text
				SetTextColor(hdc, ::GetSysColor(COLOR_HIGHLIGHTTEXT));
				SetBkColor(hdc, ::GetSysColor(COLOR_HIGHLIGHT));			
			}
			else
			{
				// setup the color for the text
				SetTextColor(hdc, ::GetSysColor(COLOR_BTNTEXT));
				SetBkColor(hdc, ::GetSysColor(COLOR_WINDOW));	
			}
			break;
			}
		}
		// draw the solid rectangle
		::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcCell, NULL, 0, NULL);

		CString CellText;
		POSITION pos;

		if (nCell == 0)
		{
			// get the text position
			pos = m_PropertyList.FindIndex(nRow);
			
			// get the property Name text
			RefCountedPtr< CPropertyObject > pProperty = m_PropertyList.GetAt(pos);
			
			// get the property desc name.
			CellText = pProperty->GetName();			
		}
		else
		{
			// get the text position
			pos = m_PropertyList.FindIndex(nRow);
			if (pos == NULL)
				return;
			// get the property Name text
			RefCountedPtr< CPropertyObject > pProperty = m_PropertyList.GetAt(pos);

			switch (pProperty->GetType())				
			{
			case PropEnum:
				{
					UINT enumId;
					if (pProperty->GetID() == nAlternateOrient)
						enumId = nOrientation * 100;
					else if (pProperty->GetID() == nSplitterStyle)
						enumId = IDS_SPLITTERStyle_0 - 1;
					else if (pProperty->GetID() == nBorderStyle)
						enumId = IDS_BORDERSTYLE_0 - 1;
					else
						enumId = pProperty->GetID() * 100;
					enumId += pProperty->GetLongValue() + 1;
					CellText = theWorkspace.LoadResourceString(enumId);
					break;
				}
			case PropStringArray:
			case PropIntArray:				
				{
					CellText = theWorkspace.LoadResourceString(IDS_LIST);
					break;
				}			
			case PropImageList:				
				{
					CellText = theWorkspace.LoadResourceString(IDS_IMAGELIST);
					break;
				}			
			case PropEvent:				
				{
					CellText = theWorkspace.LoadResourceString(IDS_EVENT);
					break;
				}	
			case PropPicture:	
				{
					CellText = pProperty->GetStdProperty();
				
					if (CellText == _T("0"))
						CellText = theWorkspace.LoadResourceString(IDS_NONE);
					break;
				}	
			case PropActiveXRunTime:
				{					
					CellText = theWorkspace.LoadResourceString(IDS_RUNTIME);
					break;
				}
			case PropActiveXProp:
				{
					CAxContainerCtrl *pCtrl = ((CControlHolder*)m_pControl->m_pCtrlHolder)->GetActiveXCtrl();
					
					// only properties with one or none parameters may be edited during design time
					if (pProperty->GetAxInterfaceDescriptorPtr()->GetParamQty() > 1)
					{
						// so we need to set and display the property as a run time only property
						//CellText = theWorkspace.LoadResourceString(IDS_RUNTIME);
						//pProperty->GetType() = PropActiveXRunTime;
					}
					else
						pCtrl->GetProperty( pProperty->GetAxInterfaceDescriptorPtr()->GetGetDescriptor(), CellText );

					if (!CellText.IsEmpty() && pProperty->GetAxInterfaceDescriptorPtr()->GetEnumDescriptor())
						CellText = pProperty->GetAxInterfaceDescriptorPtr()->GetEnumDesc(CellText);
					if (pProperty->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IPictureDisp)
					{
						if (CellText != theWorkspace.LoadResourceString(IDS_RUNTIME))
							CellText = theWorkspace.LoadResourceString(IDS_PICTUREDESC);
					}
					// if the property is a color then
					else if (pProperty->GetAxInterfaceDescriptorPtr()->GetGuid() == GUID_COLOR)
					{
						// get the color
						unsigned long ulColor = pCtrl->GetColor(pProperty->GetAxInterfaceDescriptorPtr()->GetGetDispId());
						COLORREF color;
						OleTranslateColor(ulColor, NULL, &color);

						// and lets draw the color in the property value box.
						int nRightSide = rcText.right;
						rcText.top++;
						rcText.left--;
						rcText.bottom -= 2;
						rcText.right = rcText.left + 45;
						if (rcText.right > nRightSide)
							rcText.right = nRightSide;

						// draw the solid rectangle
						::SetBkColor(hdc, color);
						::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcText, NULL, 0, NULL);
						
						HBRUSH hBrush = ::CreateSolidBrush(GetSysColor(BLACK_BRUSH));

						FrameRect(hdc, &rcText, hBrush);

						// delete the brush
						DeleteObject(hBrush);
	
						return;
					}
					if (pProperty->GetAxInterfaceDescriptorPtr()->GetType() == VT_DISPATCH)
					{
						if (pProperty->GetAxInterfaceDescriptorPtr()->GetGuid() != IID_IPictureDisp &&
							pProperty->GetAxInterfaceDescriptorPtr()->GetGuid() != GUID_COLOR &&
							pProperty->GetAxInterfaceDescriptorPtr()->GetGuid() != IID_IFontDisp &&
							pProperty->GetAxInterfaceDescriptorPtr()->GetGuid() != IID_IFont)
						{
							/*if (m_pControl->IsMicrosoftActiveXCtrl() == TRUE &&
								(pProperty->GetName() == "ImageList" || pProperty->GetName() == "HotImageList" || pProperty->GetName() == "DisabledImageList" || pProperty->GetName() == "Icons" || pProperty->GetName() == "SmallIcons"))
								CellText = pProperty->GetStringValue();
							else
							*/
								CellText = CString();
						}
					}
					break;
				}
			default:
				{
					CellText = pProperty->GetStdProperty();
					break;
				}
			}
		}

		
		CFont* pOldFont = NULL;
		// setup the font			
		try
		{
			HGDIOBJ pOldFont = SelectObject(hdc, m_pFont->m_hObject);
		}
		catch(...)
		{
		}
		LPCTSTR lpszText;
		lpszText = CellText;
		
		::DrawText(hdc, lpszText, CellText.GetLength(), rcText, DT_SINGLELINE|DT_VCENTER|DT_NOPREFIX); 

		if (pOldFont != NULL)
			// restore the old font ** a must
			SelectObject(hdc, pOldFont);	
	}
}

CRect CPropertyListCtrl::ChangeSelectedItem(int nNewSelectionIndex) 
{	
	CRect rcAbort(nReturnError,nReturnError, 0, 0);
	
	CRect rcGridArea;		
	CRect rcPropArea;
	GetClientRect(&rcPropArea);
	
	int nCount = m_PropertyList.GetCount();
		
	if((-1 + ((nNewSelectionIndex - m_TopIndex) * nItemRowHeight)) >= rcPropArea.bottom)
	{		
		return rcAbort;
	}
	
	// is a scroll bar is required adjust the size of the drawing area accordingly
	if((-1 + (nCount * nItemRowHeight)) > rcPropArea.Height())
	{		
		rcPropArea.right = rcPropArea.right - nScrollBarWidth;
	}

	// redraw the old selected cell
	int nTopIndex = ((nNewSelectionIndex - m_TopIndex) * nItemRowHeight) -1;	
		
	// setup to return the point calculated for the editable cell area
	CRect rcReturn(
		(rcPropArea.right / 2)-2,
		nTopIndex + 1, 
		rcPropArea.right,
		nTopIndex + nItemRowHeight);
	
	// if the selected index did not change exit here and return the selected cell rect
	if (nNewSelectionIndex == m_SelectedIndex)
		return rcReturn;
	
	// if the new select is greater than or equal to 
	// the count then this means that it is trying
	// to access a hidden property.  So lets return
	// an abort value.
	if (nNewSelectionIndex >= nCount)
		return rcAbort;
	
	
	HDC hdc = ::GetDC(m_hWnd);
	// redraw the old selected cell
	DrawRow(
		m_SelectedIndex, 
		nDrawStyle_RemoveHighLight, 
		&rcPropArea,
		hdc);
	
	// high light the new selected cell
	DrawRow(
		nNewSelectionIndex, 
		nDrawStyle_SetAsHighLight, 
		&rcPropArea,
		hdc);
	
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

	// set the selection index
	m_SelectedIndex = nNewSelectionIndex;

	CString CellText;
	// get the property selected
	RefCountedPtr< CPropertyObject > pProp = GetPropertyObject(m_SelectedIndex);
	// fire the cell changed event to notify parent of the new cell selected,
	// pass it the property ID and the caption text
	UpdatePropHelpCtrls(pProp);
	
	return rcReturn;
}



int CPropertyListCtrl::GetSelectedIndex(long y)
{
	int nTopIndex = -1;
	 
	int nCount = m_PropertyList.GetCount();
	
	// test for a need for a scroll bar and set accordingly
	for(int i = m_TopIndex; i < nCount; i++)
	{	
		if(nTopIndex < y && (nTopIndex + nItemRowHeight) >= y)
		{
			return i;
		}
		// set the top value for the next property to be drawn
		nTopIndex = nTopIndex + nItemRowHeight;

	}
	return -1;
}

void CPropertyListCtrl::OnButtonPressed()
{
	// get the property object
	RefCountedPtr< CPropertyObject > pProp = GetPropertyObject(m_SelectedIndex);
	int nCalcHeight = 100;

	if (m_Button.GetButtonStyle() != nDropDownImage)
	{		
		switch(pProp->GetType())
		{
		case PropCustom:
			ShowPropertyDlg();
			break;
		case PropActiveXMethods:
			{
				EditObjectbrowser();
				break;
			}
		case PropActiveXPropPages:
			{
				CControlHolder *pCtrl = (CControlHolder*)m_pControl->m_pCtrlHolder;
				pCtrl->ShowPropertyPages();
			break;
			}
		case PropImageList:
			FireInvokeImageList(
				m_SelectedIndex,
				pProp->GetID());				
			break;
		case PropOLEColor:
			break;
		case PropActiveXProp:
		{			
			// get the Ole dispatch property type			
			if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFontDisp ||
					pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFont)
			{				
				pProp->GetAxInterfaceDescriptorPtr()->DoActiveXFontPropDlg(((CControlHolder*)m_pControl->m_pCtrlHolder)->GetActiveXCtrl());	
			}
			else if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IPictureDisp)
			{
				DISPID dispid = pProp->GetAxInterfaceDescriptorPtr()->GetPutDispId();
				WORD flag;
				if (pProp->GetAxInterfaceDescriptorPtr()->GetPropPutRef())
					flag = DISPATCH_PROPERTYPUTREF;
				else
					flag = DISPATCH_PROPERTYPUT;
				CString sFileName = GetOnePictureFile();
				CControlHolder *pCtrl = (CControlHolder*)m_pControl->m_pCtrlHolder;
				pCtrl->LoadPictureFile(dispid, sFileName, flag);
			}			
			break;
		}
		case PropString:
			{
				// if the property is font name invoke the font dialog
				if (pProp->GetID() == nLabelName)
				{					
					FireInvokeFontPane();
				}
			break;
			}		
			break;
		}
	}
	if (m_Button.GetButtonStyle() == nDropDownImage)
	{
		switch(pProp->GetType())
		{
		case PropActiveXProp:
			{
			
			if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == GUID_COLOR)
				{
			
				CControlHolder *pCtrl = (CControlHolder*)m_pControl->m_pCtrlHolder;
				unsigned long ulColor = pCtrl->GetColor(pProp->GetAxInterfaceDescriptorPtr()->GetGetDispId());
				COLORREF color;
				OleTranslateColor(ulColor, NULL, &color);

				//m_bActive = TRUE;
				CRect rectButton;
				CRect rect;
				m_Button.GetWindowRect(rectButton);
				GetWindowRect(rect);
				CString sDefaultText;
				sDefaultText = theWorkspace.LoadResourceString(IDS_WINSYSCOLOR);
				CString sCustomText;
				sCustomText = theWorkspace.LoadResourceString(IDS_OTHERCOLORS);
				CPoint pt(rectButton.right, rectButton.bottom);

				m_pColorPopup = new CColourPopup(CPoint(rectButton.right, rectButton.bottom),    // Point to display popup
                     color,                       // Selected colour
                     this,                        // parent
                     sDefaultText, 
                     sCustomText,
					 pProp,
					 pCtrl);                // Custom Text
				return;
			}
			break;
			}
		case PropStringArray:
		case PropIntArray:
			{
				CRect rcCell = ChangeSelectedItem(m_SelectedIndex);
				CRect rcPopUp(
					rcCell.left + 3,
					rcCell.bottom + 2,
					rcCell.left + rcCell.Width()-1,
					rcCell.top + nCalcHeight);
				
				
				CRect rcThis;
				GetClientRect(&rcThis);
				if (rcThis.bottom < rcPopUp.bottom)
					rcPopUp.bottom = rcThis.bottom;
				// set the string keyboard filter according to the type of property
				switch (pProp->GetType())
				{
					case PropIntArray:
						m_Edit.m_sFilter = theWorkspace.LoadResourceString(IDS_LONGFILTER);
						m_Edit.m_bAllowReturn = true;
						break;
					default:
						m_Edit.m_bAllowReturn = true;
						m_Edit.m_sFilter = theWorkspace.LoadResourceString(IDS_ANYFILTER);
						break;						
				}
				CString sText;
				for (size_t i = 0; i < pProp->size(); i++)
					sText += pProp->GetStringItem(i) + _T("\r\n");
				// set the original value
				m_Edit.m_OriginalValue = sText;
				// allow the edit box to accept returns
				m_Edit.m_bAllowReturn = true;
				// set the property pointers
				m_Edit.m_pProp = pProp;
				m_Edit.m_pControl = m_pControl;
				m_Edit.m_pView = m_pView;					
				// set the edit's text
				m_Edit.SetWindowText(sText);
				// move the edit into position
				m_Edit.MoveWindow(rcPopUp, TRUE);
				try{
				m_pParent->SetFocus();
				}catch(...){}
				// show the edit.
				m_Edit.ShowWindow(TRUE);
				// force the edit to take focus
				m_Edit.SetFocus();
				// set the edit to select all the text.
				m_Edit.SetSel(0, 0, TRUE);
				
				return;
				break;
			}
		}
		
		CRect rcThis;
		GetWindowRect(&rcThis);
		CRect rcCell = ChangeSelectedItem(m_SelectedIndex);
		CRect rcPopUp(
			rcThis.left + rcCell.left + 4,
			rcThis.top + rcCell.bottom + 2,
			rcThis.left + rcCell.left + rcCell.Width() + 3,
			rcThis.top + rcCell.bottom + nCalcHeight + 4);
		
		if (m_PopUpCreated == FALSE)
		{
			
			m_pModeless = new CListBoxDlg(this);
			m_PopUpCreated = m_pModeless->Create();
			m_pModeless->m_pControl = m_pControl;
			m_pModeless->m_pView = m_pView;

			if (m_pModeless == NULL) return;
			// move the listbox into position
			m_pModeless->MoveWindow(&rcPopUp);
			
			// call method to populate the list box
			int nTotalHeight = SetListBox();
			
			if (nTotalHeight > nDeTotalHeight)
				nTotalHeight = nDeTotalHeight;
			else
				// add 6 to accound for border safety
				nTotalHeight = nTotalHeight + 6;

			// get the listbox's adjusted size
			CRect rCWnd;
			if (m_pModeless == NULL) return;
			m_pModeless->m_ListBox.GetWindowRect(&rCWnd);

			int bScreenBottom = ::GetSystemMetrics(SM_CYSCREEN);
	
			int nFinalBottom;
			if (rcPopUp.top + nTotalHeight > bScreenBottom)
				nFinalBottom = bScreenBottom;
			else
				nFinalBottom = rcPopUp.top + nTotalHeight;

			CRect rcNewPos(
				rcPopUp.left,
				rcPopUp.top,
				rcPopUp.right,
				nFinalBottom);
			
			if (m_pModeless == NULL) return;
			m_pModeless->MoveWindow(&rcNewPos);
			// show the listbox
			if (m_pModeless == NULL) return;
			m_pModeless->ShowWindow(TRUE);
			if (m_pModeless != NULL)
			{
				if (m_pModeless == NULL) return;
				m_pModeless->m_ListBox.SetFocus();
				if (m_pModeless == NULL) return;
				m_pModeless->SetActiveWindow();
				if (m_pModeless == NULL) return;
				m_pModeless->m_ListBox.SetFocus();
			}
		}		
	}
}

int CPropertyListCtrl::SetListBox()
{
	if (m_pModeless == NULL) return 0;
	// get the property object
	RefCountedPtr< CPropertyObject > pProp = GetPropertyObject(m_SelectedIndex);
	
	// set the list box to point to the property object
	m_pModeless->SetPropertyPointer(pProp);

	CString sEnumDesc;
	int nType = pProp->GetType();

	// check for ActiveX property types
	if (nType == PropActiveXProp) 
	{
		m_pModeless->m_pAxContainer = (CControlHolder*)m_pControl->m_pCtrlHolder;
	
		switch (pProp->GetAxInterfaceDescriptorPtr()->GetType())
		{
		case VT_BOOL:
		case VT_UI1:
		case VT_I1:
			nType = PropBool;
			break;
		default:
			{
				AxPropertyDescriptor *pPropDesc = pProp->GetAxInterfaceDescriptorPtr()->GetEnumDescriptor();
				if (pPropDesc)
				{
					for (size_t i=0; i<pPropDesc->GetEnum().size(); i++)
						m_pModeless->AddString(VariantToString(pPropDesc->GetEnum()[i].Var) + _T('-') + pPropDesc->GetEnum()[i].Name);
				}
			}
			nType = PropEnum;
			break;
		case VT_DISPATCH:
		case VT_UNKNOWN:
			if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IPictureDisp)
			{
				sEnumDesc = theWorkspace.LoadResourceString(IDS_NONE);
				m_pModeless->AddString(sEnumDesc);
					
				sEnumDesc = theWorkspace.LoadResourceString(IDS_ADD);
				m_pModeless->AddString(sEnumDesc);

				if (m_pControl->GetOwnerProject()->GetPictureList().GetCount() > 0)
				{
					// set counter for Pictures
					int nCount = 0;
				
					// do loop to navigate Pictures
					while (nCount < m_pControl->GetOwnerProject()->GetPictureList().GetCount())
					{
						POSITION pos = m_pControl->GetOwnerProject()->GetPictureList().FindIndex(nCount);
						if (pos != NULL)
						{
							CPictureObject *pPic = m_pControl->GetOwnerProject()->GetPictureList().GetAt(pos);
							if (pPic != NULL)
							{
								char Value[80];
								_ltoa(pPic->GetID(), Value, 10);
								m_pModeless->AddString(Value);
							}
						}
						// increment counter
						nCount++;
					}
					nCount = 0;	
				}
			}
				
			/*
			// if a microsoft control and the ImageList property is selected.
			if (m_pControl->IsMicrosoftActiveXCtrl() == TRUE &&
				(pProp->GetName() == "ImageList" || pProp->GetName() == "HotImageList" || pProp->GetName() == "DisabledImageList" || pProp->GetName() == "Icons" || pProp->GetName() == "SmallIcons"))
			{
				CString sNone;
				sNone = theWorkspace.LoadResourceString(IDS_NONE);
				m_pModeless->AddString(sNone);						
				POSITION pos = m_pDclForm->GetControlList().GetHeadPosition();
				while (pos != NULL)
				{
					CDclControlObject *pCtrl = m_pDclForm->GetControlList().GetNext(pos);
					if (pCtrl != NULL)
					{
						if (pCtrl->GetType() == CtlActiveX && pCtrl->IsMicrosoftActiveXCtrl())
						{
							CString sName = pCtrl->GetActiveXTypeName();
							if (sName == "ImageListCtrl")
							{
								sName = pCtrl->GetStrProperty(nName);
								m_pModeless->AddString(sName);
							}
						}
					}
				}				
			}
			*/
			break;
		}
	}
	switch(nType)
	{	
		case PropBool:
		{
			// add it's items			
			m_pModeless->AddString(theWorkspace.LoadResourceString(IDS_TRUE));
			m_pModeless->AddString(theWorkspace.LoadResourceString(IDS_FALSE));					
			break;
		}
		case PropPicture:
		{
			sEnumDesc = theWorkspace.LoadResourceString(IDS_NONE);
			m_pModeless->AddString(sEnumDesc);
				
			sEnumDesc = theWorkspace.LoadResourceString(IDS_ADD);
			m_pModeless->AddString(sEnumDesc);

			if (m_pControl->GetOwnerProject()->GetPictureList().GetCount() > 0)
			{
				// set counter for Pictures
				int nCount = 0;
			
				// do loop to navigate Pictures
				while (nCount < m_pControl->GetOwnerProject()->GetPictureList().GetCount())
				{
					POSITION pos = m_pControl->GetOwnerProject()->GetPictureList().FindIndex(nCount);
					if (pos != NULL)
					{
						CPictureObject *pPic = m_pControl->GetOwnerProject()->GetPictureList().GetAt(pos);
						if (pPic != NULL)
						{
							m_pModeless->AddString(LTOA(pPic->GetID()));
						}
					}
					// increment counter
					nCount++;
				}
				nCount = 0;
	
			}
			break;
		}
		case PropEnum:
		{

			switch(pProp->GetID())
			{	
			case nURLLinkType:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_URL_TYPE_0);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_URL_TYPE_1);
					m_pModeless->AddString(sEnumDesc);
					
					break;
				}
			
			case nInsertOrXref:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_DRAGSTYLE_0);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_DRAGSTYLE_1);
					m_pModeless->AddString(sEnumDesc);
					
					break;
				}
			
			case nBorderStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BORDERSTYLE_0);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_BORDERSTYLE_1);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BORDERSTYLE_2);
					m_pModeless->AddString(sEnumDesc);
					
					break;
				}
				
			case nButtonStyle:
				{
					// add it's items
					m_pModeless->AddString(theWorkspace.LoadResourceString(IDS_BUTTONSTYLE_0));
					m_pModeless->AddString(theWorkspace.LoadResourceString(IDS_BUTTONSTYLE_1));
					m_pModeless->AddString(theWorkspace.LoadResourceString(IDS_BUTTONSTYLE_2));
					m_pModeless->AddString(theWorkspace.LoadResourceString(IDS_BUTTONSTYLE_3));
					m_pModeless->AddString(theWorkspace.LoadResourceString(IDS_BUTTONSTYLE_4));
					m_pModeless->AddString(theWorkspace.LoadResourceString(IDS_BUTTONSTYLE_5));
					m_pModeless->AddString(theWorkspace.LoadResourceString(IDS_BUTTONSTYLE_6));
					break;
				}
			case nComboBoxStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_1);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_2);
					m_pModeless->AddString(sEnumDesc);
					
					if (m_pControl->GetType() == CtlComboBox)
					{
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_3);
						m_pModeless->AddString(sEnumDesc);
					
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_4);
						m_pModeless->AddString(sEnumDesc);
						
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_5);
						m_pModeless->AddString(sEnumDesc);
						
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_6);
						m_pModeless->AddString(sEnumDesc);
						
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_7);
						m_pModeless->AddString(sEnumDesc);
						
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_8);
						m_pModeless->AddString(sEnumDesc);
						
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_9);
						m_pModeless->AddString(sEnumDesc);
							
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_10);
						m_pModeless->AddString(sEnumDesc);					
						
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_11);
						m_pModeless->AddString(sEnumDesc);					

						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_12);
						m_pModeless->AddString(sEnumDesc);
							
						sEnumDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_13);
						m_pModeless->AddString(sEnumDesc);
					}
					break;
				}				
			case nFilterStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILTERSTYLE_0);				
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILTERSTYLE_1);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILTERSTYLE_2);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILTERSTYLE_3);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILTERSTYLE_4);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILTERSTYLE_5);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILTERSTYLE_6);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILTERSTYLE_7);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILTERSTYLE_8);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nEventInvoke:
				{
					sEnumDesc = theWorkspace.LoadResourceString(IDS_EVENTINVOKE_0);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_EVENTINVOKE_1);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nFileDlgStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILEDLGSTYLE_0);				
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_FILEDLGSTYLE_1);
					m_pModeless->AddString(sEnumDesc);									
					break;
				}
			case nListViewStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_LV_STYLE_0);				
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_LV_STYLE_1);
					m_pModeless->AddString(sEnumDesc);
									
					sEnumDesc = theWorkspace.LoadResourceString(IDS_LV_STYLE_2);
					m_pModeless->AddString(sEnumDesc);
									
					sEnumDesc = theWorkspace.LoadResourceString(IDS_LV_STYLE_3);
					m_pModeless->AddString(sEnumDesc);
					
					break;
				}
			case nListViewSort:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_LV_SORT_0);				
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_LV_SORT_1);
					m_pModeless->AddString(sEnumDesc);
									
					sEnumDesc = theWorkspace.LoadResourceString(IDS_LV_SORT_2);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nListViewIconAlign:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_LV_IA_0);				
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_LV_IA_1);
					m_pModeless->AddString(sEnumDesc);
									
					break;
				}
				
			case nBlockListStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BL_STYLE_0);				
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BL_STYLE_1);
					m_pModeless->AddString(sEnumDesc);
									
					break;
				}
				
			case nTabLabelAlign:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_JUSTIFICATION_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_JUSTIFICATION_1);
					m_pModeless->AddString(sEnumDesc);
					
					break;
				}
		
			case nAllowOrbiting:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_UM_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_UM_1);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_UM_2);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_UM_3);
					m_pModeless->AddString(sEnumDesc);					
					break;
				}

			case nTabJustified:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_TAB_JUSTIFY_0);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_TAB_JUSTIFY_1);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nTabStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_TABSTYLE_0);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_TABSTYLE_1);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
				
			case nJustification:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_JUSTIFICATION_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_JUSTIFICATION_1);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_JUSTIFICATION_2);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nSplitterStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_SPLITTERStyle_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_SPLITTERStyle_1);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_SPLITTERStyle_2);
					m_pModeless->AddString(sEnumDesc);
					
					sEnumDesc = theWorkspace.LoadResourceString(IDS_SPLITTERStyle_3);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nAlternateOrient:
			case nOrientation:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_ORIENTATION_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_ORIENTATION_1);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nSelectStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_SELECTSTYLE_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_SELECTSTYLE_1);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_SELECTSTYLE_2);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nRenderMode:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_RM_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_RM_1);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_RM_2);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_RM_3);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_RM_4);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_RM_5);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_RM_6);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_BV_RM_7);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nRectStyle:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_RECTSTYLE_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_RECTSTYLE_1);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_RECTSTYLE_2);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_RECTSTYLE_3);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_RECTSTYLE_4);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nDockableSides:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_SIDES_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_SIDES_1);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_SIDES_2);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_SIDES_3);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_SIDES_4);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_SIDES_5);
					m_pModeless->AddString(sEnumDesc);
					break;
				}
			case nDefaultDockedSide:
				{
					// add it's items
					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_DEFAULT_0);
					m_pModeless->AddString(sEnumDesc);
				
					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_DEFAULT_1);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_DEFAULT_2);
					m_pModeless->AddString(sEnumDesc);

					sEnumDesc = theWorkspace.LoadResourceString(IDS_DOC_DEFAULT_3);
					m_pModeless->AddString(sEnumDesc);

					break;
				}

			case nPicture:
				{
					// to be done
					break;
				}
			}
		}	
	}
	// get the number of list items
	int nCount = m_pModeless->m_ListBox.GetCount();
	// return the total height of all the list items
	return m_pModeless->m_ListBox.GetItemHeight(0) * nCount;	
}

void CPropertyListCtrl::CloseListBox(int nInstructions)
{
	if (theEditorWorkspace.GetActiveDocument() != NULL && nInstructions != -1)
		theEditorWorkspace.GetActiveDocument()->SetModifiedFlag(TRUE);

	CString sTitle;
	sTitle = theWorkspace.LoadResourceString(IDS_DEFFONT);
	
	CRect rcThis;
	GetWindowRect(&rcThis);
	CRect rcCell(0,0,rcThis.Width(), rcThis.Height());
	
	// make adjustments for the scrollbar if visible
	if (m_ScrollBarNeeded == TRUE)
		rcCell.right = rcCell.right - nScrollBarWidth;
		
	HDC hdc = ::GetDC(m_hWnd);
	
	// redraw the cell
	DrawCell(
		m_SelectedIndex,
		1, 
		nDrawStyle_NonSelectedRow,
		&rcCell,
		hdc);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

	// if the popup window has not been close then close it
	if (m_PopUpCreated == TRUE && m_pModeless != NULL)
	{
		// remove the popup list box
		m_PopUpCreated = FALSE;
		CString sText;
		int nNewID = 100;
		
		if (m_pModeless->m_ListBox.GetCurSel() >= 2)
		{
			m_pModeless->m_ListBox.GetText(m_pModeless->m_ListBox.GetCurSel(), sText);
			nNewID = _tstol(sText);
		}
		else if (m_pModeless->m_ListBox.GetCurSel() == 1)
		{
			m_pModeless->m_ListBox.GetText(1, sText);
			CString sADD;
			sADD = theWorkspace.LoadResourceString(IDS_ADD);
			if (sText == sADD)
				nInstructions = nPromptForNewPicture;
		}
		else if (m_pModeless->m_ListBox.GetCurSel() == 0)
		{
			nNewID = -1;
		}
		m_pModeless->DestroyWindow();
		delete m_pModeless;
		m_pModeless = NULL;

		if (nInstructions == nPromptForNewMasked)
		{
			CString sFileName = GetOnePictureFile();
			if (sFileName.GetLength() == 0)
				return;

			// load and add the masked picture
			CPictureObject* pPict = new CPictureObject( nNewID, sFileName, true );
			if (pPict->IsValid())
			{
				m_pControl->GetOwnerProject()->AddPicture(pPict);
				RefCountedPtr< CPropertyObject > pProp = GetPropertyObject(m_SelectedIndex);
				// set the property
				pProp->SetLongValue(nNewID);			

				if (m_pControl->GetID() == -1)
				{
					CChildFrame *pChildFrame = NULL;
					try{
						pChildFrame = (CChildFrame*) m_pView->GetParentFrame();
					}catch(...){}
					if (pChildFrame)
						pChildFrame->SetTitleBarIcon(nNewID);
				}
			}
			else
			{
				MessageBox (theWorkspace.LoadResourceString(IDS_PICNOTVALID), sTitle, MB_ICONEXCLAMATION);
				return;
			}
		}
		if (nInstructions == nPromptForNewPicture)
		{
			CString sFileName = GetOnePictureFile();
			if (sFileName.GetLength() == 0)
				return;

			UINT nLargestPicId = 0;

			// find the largest current picture Id
			for (int i=0; i<m_pControl->GetOwnerProject()->GetPictureList().GetCount(); i++)
			{
				POSITION pos = m_pControl->GetOwnerProject()->GetPictureList().FindIndex(i);
				if (pos != NULL)
				{
					CPictureObject *pPic = m_pControl->GetOwnerProject()->GetPictureList().GetAt(pos);
					if (pPic != NULL)
					{
						if (nLargestPicId <= pPic->GetID())
							nLargestPicId = pPic->GetID();		
					}
				}
			}
			if (nLargestPicId == 0)
				nLargestPicId = nDeLargestPicId;
			nNewID = nLargestPicId + 1;
			// load the picture
			LoadPicture(sFileName, nNewID, TRUE);
			// get the property
			RefCountedPtr< CPropertyObject > pProp = GetPropertyObject(m_SelectedIndex);
			if (pProp != NULL)
			{
				// set the property
				pProp->SetLongValue(nNewID);

				if (m_pControl->GetID() == -1)
				{
					CChildFrame *pChildFrame = NULL;
					try
					{
						pChildFrame = (CChildFrame*) m_pView->GetParentFrame();
					}
					catch(...){}
					if (pChildFrame)
						pChildFrame->SetTitleBarIcon(nNewID);
				}
			}
		}

		// get the property
		RefCountedPtr< CPropertyObject > pProp = GetPropertyObject(m_SelectedIndex);

		if (pProp != NULL)
		{
			// call the method to inform the parent that a value has changed
			PropertyHasChanged(pProp->GetID());

			if (pProp->GetAxInterfaceDescriptorPtr() &&
					pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IPictureDisp)
			{
				CControlHolder *pCtrl = (CControlHolder*)m_pControl->m_pCtrlHolder;
				pCtrl->GetActiveXCtrl()->LoadPicture(pProp->GetAxInterfaceDescriptorPtr()->GetPutDispId(), nNewID);
			}

		}

		if (m_pModeless == NULL)
			// repaint this control so the property is updated to the screen
			Invalidate();		
	}
	
}


void CPropertyListCtrl::PropertyHasChanged(PropertyId nId)
{
	// inform the parrent that a property has changed
	FirePropertyChanged(nId);
}

void CPropertyListCtrl::DeleteFlaggedControls(short DclFormIndex) 
{
	CDclFormObject *pDclForm = CPropertyListCtrl::GetDclFormObject(DclFormIndex);
	if (pDclForm == NULL)
		return;
	pDclForm->PurgeDeletedControls();
	pDclForm->PurgeDeletedImageLists();
}

CString CPropertyListCtrl::GetPictureFile(LPCTSTR Filter)
{
	m_FileList.RemoveAll();

	// create the open dialog box
	CPreviewFileDlg BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		Filter,
		CWnd::GetActiveWindow());

	// proceed to setup the file buffer size
	BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
	TCHAR* pc = new TCHAR[MAX_PATH];
	BrowseWnd.m_ofn.lpstrFile = pc;
    BrowseWnd.m_ofn.lpstrFile[0] = NULL;

	// call method to invoke the file dialog box
	int iReturn = BrowseWnd.DoModal();

	CString strResult;
	if(iReturn == IDOK)
		strResult = BrowseWnd.GetPathName();

	delete[] pc; 
	return strResult;
}


short CPropertyListCtrl::GetPictureID(short Index) 
{
	short nResult = 0;
	
	if(m_pControl->GetOwnerProject()->GetPictureList().GetCount() > Index)
	{		
		POSITION pos = m_pControl->GetOwnerProject()->GetPictureList().FindIndex(Index);
		if (pos != NULL)
		{
			CPictureObject *pPic = m_pControl->GetOwnerProject()->GetPictureList().GetAt(pos);
			if (pPic != NULL)
			{
				// set return value to the name
				nResult = pPic->GetID();		
			}
		}
	}
	else
	{
		// set return variable to -1 to indicate error
		nResult = -1;
	}

	return nResult;
}

CString CPropertyListCtrl::GetOnePictureFile() 
{
	CString strResult;
	CString sFilter;

	sFilter = theWorkspace.LoadResourceString(IDS_FILTER);
	// create the open dialog box
	
	CPreviewFileDlg BrowseWnd(
		TRUE, 
		NULL,
		NULL, 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		sFilter,
		CWnd::GetActiveWindow());

	// proceed to setup the file buffer size
	TCHAR szPath[MAX_PATH];
	BrowseWnd.m_ofn.nMaxFile = MAX_PATH;
	BrowseWnd.m_ofn.lpstrFile = szPath;
	BrowseWnd.m_ofn.lpstrFile[0] = NULL;
	
	if(BrowseWnd.DoModal() == IDOK)
		strResult = BrowseWnd.GetPathName();

	return strResult;

}


BOOL CPropertyListCtrl::SetDclParent(short nIndex, LPCTSTR sParentName, short nTabIndex) 
{
	CDclFormObject* pNewDclForm = GetDclFormObject(nIndex);

	if (pNewDclForm == NULL)
		return FALSE;

	// retreive the string name value required.
	pNewDclForm->SetParentForm(sParentName);
	pNewDclForm->SetTabIndex(nTabIndex);
	return TRUE;
}

short CPropertyListCtrl::GetDclParentsTabIndex(short Index) 
{
	short nReturn;

	CDclFormObject* pNewDclForm = GetDclFormObject(Index);

	if (pNewDclForm != NULL)
	{
		// retreive the string name value required.
		nReturn = pNewDclForm->GetTabIndex();
	}
	else
	{
		// return This quote to indicate error
		nReturn = -1;
	}

	return nReturn;
}

short CPropertyListCtrl::GetArxControlClientHeight(short DclFormIndex, short ArxControlIndex) 
{
	CDclControlObject* pControlObject = GetArxControlObject(DclFormIndex, ArxControlIndex);

	if (pControlObject != NULL)
		return pControlObject->m_ClientHeight; // return the value to equal the control type
	else
		return -1; // return -1 to indicate error
}

BOOL CPropertyListCtrl::IsArxControlDeleted(short DclFormIndex, short ArxControlIndex) 
{
	CDclControlObject *pControl = GetArxControlObject(DclFormIndex, ArxControlIndex);

	if (pControl == NULL)
		return FALSE;
		
	return pControl->m_Delete;
}

void CPropertyListCtrl::DisplayVaries() 
{
	m_PropertyList.RemoveAll();

	if (!IsWindow(m_hWnd))
		return;

	// repaint the entire control
	HDC hdc = ::GetDC(m_hWnd);
	ClearArea(hdc);

	
	// setup the color for the text
	SetTextColor(hdc, ::GetSysColor(COLOR_BTNTEXT));
	SetBkColor(hdc, ::GetSysColor(COLOR_WINDOW));	
			
	CString CellText;
	CellText = theWorkspace.LoadResourceString(IDS_VARIES);
	
	CRect rcThis;
	GetWindowRect(&rcThis);
	CRect rcText(3,0,rcThis.Width() - 3, rcThis.Height() - 3);
	// setup the font			
	HGDIOBJ pOldFont = SelectObject(hdc, m_pFont->m_hObject);

	// draw the property Name text
	TextOut(hdc, rcText.left, rcText.top, CellText, CellText.GetLength());
	// restore the old font ** a must
	SelectObject(hdc, pOldFont);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

	// make any child controls not visible
	if (m_Button.IsWindowVisible())
	{
		m_Button.ShowWindow(FALSE);
	}

	if (m_Edit.IsWindowVisible())
	{
		m_Edit.ShowWindow(FALSE);
		UpdateControls(m_Edit.m_pProp->GetID());
	}

	
	if (m_ScrollBarCreated && m_ScrollBar.IsWindowVisible())
	{
		m_ScrollBar.ShowWindow(FALSE);
	}
}


void CPropertyListCtrl::LoadPicture(LPCTSTR sFileName, short nPictureTag, bool bApplyMask) 
{
	m_pControl->GetOwnerProject()->LoadPictureFile(sFileName, nPictureTag, bApplyMask);
}


void CPropertyListCtrl::SearchPictureRefs(CDclFormObject *pDclObject) 
{
	for (int i=0; i<pDclObject->GetControlList().GetCount(); i++)
	{
		POSITION pos = pDclObject->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pControl = pDclObject->GetControlList().GetAt(pos);
			int nPictureId = pControl->GetLongProperty(nPicture);
			if (nPictureId > -1)
			{
				if (!pDclObject->GetProject()->FindPicture(nPictureId))
					pControl->SetLongProperty(nPicture, 0);
			}
			int nPressedPictureId = pControl->GetLongProperty(nPressedPicture);
			if (nPressedPictureId > -1)
			{
				if (!pDclObject->GetProject()->FindPicture(nPressedPictureId))
					pControl->SetLongProperty(nPressedPicture, 0);
			}
			int nIconId = pControl->GetLongProperty(nIcon);
			if (nIconId > -1)
			{
				if (!pDclObject->GetProject()->FindPicture(nIconId))
					pControl->SetLongProperty(nIcon, 0);
			}
		}
	}
	
}

// search every picture id property and ensure it's id is still valid
void CPropertyListCtrl::CheckPictureRefs() 
{
	// do loops to search every picture id property and ensure it's id is still valid
	for (int i=0; i<m_pControl->GetOwnerProject()->GetDclFormList().GetCount(); i++)
	{
		POSITION pos = m_pControl->GetOwnerProject()->GetDclFormList().FindIndex(i);
		if (pos != NULL)
		{
			CDclFormObject *pDclObject = m_pControl->GetOwnerProject()->GetDclFormList().GetAt(pos);
			if (pDclObject != NULL)
			{
				// call method search this dcl form's controls for invalide picture id's
				SearchPictureRefs(pDclObject);
			}
		}	
	}
	
}

void CPropertyListCtrl::DefaultFontDlg() 
{
	CPropertySheet Dlg;
	CFontPropertyPage *pFontPage = NULL;
	RefCountedPtr< CPropertyObject > pProp = NULL;
	pFontPage = new CFontPropertyPage;
	
	// set the title
	CString sTitle;
	sTitle = theWorkspace.LoadResourceString(IDS_DEFFONT);
	Dlg.SetTitle(sTitle);

	Dlg.DoModal();

	if (pFontPage)
		delete pFontPage;
	
	Invalidate();
}

void CPropertyListCtrl::ShowPropertyDlg(bool bFontActive, bool bImageListActive) 
{
	CPropertySheet Dlg;
	CGeometry *pGeometryPage = NULL;
	CColors *pBackColorsPage = NULL;
	CColors *pForeColorsPage = NULL;
	CColors *pAltColorsPage = NULL;
	CComboBoxPage *pComboBoxPage = NULL;
	CTextBoxFilters *pTextBoxPage = NULL;
	CFontPropertyPage *pFontPage = NULL;
	CButtonStyles *pButtonPage = NULL;
	CImageListPage *pImageListPage = NULL;
	RefCountedPtr< CPropertyObject > pProp = NULL;
	CTabsPane *pTabs = NULL;
	CSortTabs *pSortTabs = NULL;
	CColumnsPage *pColumnsPage = NULL;
	CToolTipsPage *pToolTipsPage = NULL;
	CProgressBarPage *pProgressPage = NULL;


	CDclControlObject *pArxCtrl = m_pControl;

	if (pArxCtrl == NULL)
		return;

	//if (pArxCtrl->GetID() < 0)
	//	return;

	// Show the geometry page if required
	pProp = pArxCtrl->GetPropertyObject(nUseTopFromBottom);
	if (pProp != NULL)
	{
		pGeometryPage = new CGeometry;

		pGeometryPage->m_pUseTopFromBottom = pProp;
		pGeometryPage->m_pUseBottomFromBottom = pArxCtrl->GetPropertyObject(nUseBottomFromBottom);
		pGeometryPage->m_pUseLeftFromRight = pArxCtrl->GetPropertyObject(nUseLeftFromRight);
		pGeometryPage->m_pUseRightFromRight = pArxCtrl->GetPropertyObject(nUseRightFromRight);
		
		pGeometryPage->m_pControl = pArxCtrl;
		pGeometryPage->m_pDclForm = m_pControl->GetOwnerForm();
		
		Dlg.AddPage(pGeometryPage);
	}

	
	// Show the color selection page if required
	pProp = pArxCtrl->GetPropertyObject(nDisplayPercentage);
	if (pProp != NULL)
	{
		pProgressPage = new CProgressBarPage(IDD_PROGRESSBAR);
		pProgressPage->m_sTitle = theWorkspace.LoadResourceString(IDS_CAPTIONOPTIONS);

		pProgressPage->m_pArxCtrl = pArxCtrl;
		//pProgressPage->m_pTime = pArxCtrl->GetPropertyObject(nDisplaySeconds);
		
		Dlg.AddPage(pProgressPage);
	}
	

	// Show the color selection page if required
	pProp = pArxCtrl->GetPropertyObject(nAcadColor);
	if (pProp != NULL)
	{
		pBackColorsPage = new CColors(IDD_BACKCOLORS);
		pBackColorsPage->m_sTitle = theWorkspace.LoadResourceString(IDS_BACKGROUND);
		pBackColorsPage->m_pColor = pProp;
		
		Dlg.AddPage(pBackColorsPage);
	}
	// Show the color selection page if required
	pProp = pArxCtrl->GetPropertyObject(nAlternateColor);
	if (pProp != NULL)
	{
		pAltColorsPage = new CColors(IDD_ALTCOLORS);
		pAltColorsPage->m_sTitle = theWorkspace.LoadResourceString(IDS_ALTCOLORS);
		pAltColorsPage->m_pColor = pProp;
		
		Dlg.AddPage(pAltColorsPage);
	}
	

	// Show the color selection page if required
	pProp = pArxCtrl->GetPropertyObject(nForeColor);
	if (pProp != NULL)
	{
		pForeColorsPage = new CColors(IDD_FORECOLORS);
		pForeColorsPage->m_sTitle = theWorkspace.LoadResourceString(IDS_FOREGROUND);
		pForeColorsPage->m_pColor = pProp;
		
		Dlg.AddPage(pForeColorsPage);
	}
	
	// Show the combo box style selection page if required
	pProp = pArxCtrl->GetPropertyObject(nComboBoxStyle);
	if (pProp != NULL)
	{
		pComboBoxPage = new CComboBoxPage;

		pComboBoxPage->m_pStyle = pProp;
		pComboBoxPage->m_nCtrl = pArxCtrl->GetType();
		
		Dlg.AddPage(pComboBoxPage);
	}

	// Show the text box filter style selection page if required
	pProp = pArxCtrl->GetPropertyObject(nFilterStyle);
	if (pProp != NULL)
	{
		pTextBoxPage = new CTextBoxFilters;

		pTextBoxPage->m_pStyle = pProp;
		
		Dlg.AddPage(pTextBoxPage);
	}

	// Show the font page if required
	pProp = pArxCtrl->GetPropertyObject(nLabelName);
	if (pProp != NULL)
	{
		pFontPage = new CFontPropertyPage;

		pFontPage->m_pFontName = pProp;
		pFontPage->m_pFontSize = pArxCtrl->GetPropertyObject(nLabelSize);
		pFontPage->m_pFontStrikeOut = pArxCtrl->GetPropertyObject(nLabelStrikeOut);
		pFontPage->m_pFontUnderline = pArxCtrl->GetPropertyObject(nLabelUnderline);
		pFontPage->m_pFontBold = pArxCtrl->GetPropertyObject(nLabelBold);
		pFontPage->m_pFontItalic = pArxCtrl->GetPropertyObject(nLabelItalic);
		pFontPage->m_pFontSizeStyle = pArxCtrl->GetPropertyObject(nFontSizeStyle);
		Dlg.AddPage(pFontPage);
	}
	
	// Show the button styles if required
	pProp = pArxCtrl->GetPropertyObject(nButtonStyle);
	if (pProp != NULL)
	{
		pButtonPage = new CButtonStyles( pArxCtrl );

		pButtonPage->m_pStyle = pProp;
		pButtonPage->m_pPicture = pArxCtrl->GetPropertyObject(nPicture);
		Dlg.AddPage(pButtonPage);
	}

	// Show the button styles if required
	if( pArxCtrl->GetPropertyObject( nImageList ) )
	{
		pImageListPage = new CImageListPage( pArxCtrl );
		Dlg.AddPage( pImageListPage );
	}
	
	bool bShowColumns = true;
	pProp = pArxCtrl->GetPropertyObject(nColumnCaptions);
	if (pArxCtrl->GetPropertyObject(nListViewStyle) != NULL)
	{
		if (pArxCtrl->GetPropertyObject(nListViewStyle)->GetLongValue() < 3)
			bShowColumns = false;
	}
	
	if (pProp != NULL && bShowColumns)
	{
		pColumnsPage = new CColumnsPage;

		pColumnsPage->m_pImageListPage = pImageListPage;
		
		pColumnsPage->m_bShowStyles = (pArxCtrl->GetPropertyObject(nListViewStyle) == NULL);

		pColumnsPage->m_pColCaptions = pProp;	
		pColumnsPage->m_pColWidths = pArxCtrl->GetPropertyObject(nColumnWidths);	
		pColumnsPage->m_pColImages = pArxCtrl->GetPropertyObject(nColumnImages);	
		pColumnsPage->m_pColStyles = pArxCtrl->GetPropertyObject(nColumnStyles);
		pColumnsPage->m_pColAlignment = pArxCtrl->GetPropertyObject(nColumnAlignments);
		pColumnsPage->m_pColDefault = pArxCtrl->GetPropertyObject(nColumnDefaultImages);	
		pColumnsPage->m_pColAlternate = pArxCtrl->GetPropertyObject(nColumnAlternateImages);	
		pColumnsPage->m_pColListItems = pArxCtrl->GetPropertyObject(nColumnListItems);	
		pColumnsPage->m_pColImageItems = pArxCtrl->GetPropertyObject(nColumnListImages);	
		
		pColumnsPage->bUsesRowHeader = pArxCtrl->GetBoolProperty(nRowHeader) == TRUE;	

		pColumnsPage->m_pDclForm = m_pControl->GetOwnerForm();
		pColumnsPage->m_pControl = pArxCtrl;
		pColumnsPage->m_pView = m_pView;
		Dlg.AddPage(pColumnsPage);
	}

	pProp = pArxCtrl->GetPropertyObject(nTabsCaption);	
	if (pProp != NULL && m_pControl->GetOwnerForm() != NULL)
	{
		pTabs = new CTabsPane( m_pView, pArxCtrl, pImageListPage->mpImageList );
		Dlg.AddPage(pTabs);

		pSortTabs = new CSortTabs( m_pView, pArxCtrl, pTabs );
		Dlg.AddPage(pSortTabs);
	}
	
	pProp = pArxCtrl->GetPropertyObject(nToolTipTitle);		
	if (pProp != NULL)
	{
		pToolTipsPage = new CToolTipsPage;
		
		pToolTipsPage->m_pToolTipBalloon = pArxCtrl->GetPropertyObject(nToolTipBalloon);	
		pToolTipsPage->m_pToolTipTitle = pProp;	
		pToolTipsPage->m_pToolTipLine = pArxCtrl->GetPropertyObject(nToolTipLine);	
		pToolTipsPage->m_pToolTipBody = pArxCtrl->GetPropertyObject(nToolTipBody);	
		pToolTipsPage->m_pToolTipPicture = pArxCtrl->GetPropertyObject(nToolTipPicture);	
		pToolTipsPage->m_pToolTipAvi = pArxCtrl->GetPropertyObject(nToolTipAviFileName);
		pToolTipsPage->m_pToolTipTitleColor = pArxCtrl->GetPropertyObject(nToolTipTitleColor);
		Dlg.AddPage(pToolTipsPage);
	}
	
	// set the title
	CString sTitle;
	sTitle = theWorkspace.LoadResourceString(IDS_PROPERTYWIZARD);
	Dlg.SetTitle(sTitle + pArxCtrl->GetStrProperty(nName));

	if (pColumnsPage)
		Dlg.SetActivePage(pImageListPage);
	// if the font page is to be default
	else if (bFontActive)	
		Dlg.SetActivePage(pFontPage);
	// if the font page is to be default
	else if (bImageListActive)
		Dlg.SetActivePage(pImageListPage);

	if (Dlg.DoModal() == IDOK)
	{
		if (theEditorWorkspace.GetActiveDocument() != NULL)
			theEditorWorkspace.GetActiveDocument()->SetModifiedFlag(TRUE);
	}
	Invalidate();
	
	delete pTabs;
	delete pToolTipsPage;
	delete pColumnsPage;
	delete pGeometryPage;
	delete pBackColorsPage;
	delete pAltColorsPage;
	delete pForeColorsPage;
	delete pComboBoxPage;
	delete pTextBoxPage;
	delete pFontPage;
	delete pButtonPage;
	delete pImageListPage;
	delete pSortTabs;	
	delete pProgressPage;
	// update the control no matter what, just incase the user pressed the apply button then cancel
	UpdateControls((PropertyId)-2);
	Invalidate();
}

void CPropertyListCtrl::OnEnterPressed()
{
	UpdateControls(m_Edit.m_pProp->GetID());

	if (m_SelectedIndex < m_PropertyList.GetCount()-1)
	{
		int nNewSelectedIndex = m_SelectedIndex +1;
		try
		{
			ChangeSelectedItem(nNewSelectedIndex);
		}
		catch(...)
		{
		}
/*		
		if (m_Edit.IsWindowVisible())
		{
			//m_Edit.ShowWindow(FALSE);
			//UpdateControls(m_Edit.m_pProp->GetID());
		}
*/		
		m_SelectedIndex = nNewSelectedIndex;
	}
}

void CPropertyListCtrl::FirePropertyChanged(PropertyId ChangedPropertyID)
{
	UpdateControls(ChangedPropertyID);
}

void CPropertyListCtrl::FireInvokeImageList(short PropertyIndex, long PropertyID)
{
	ShowPropertyDlg(false, true);
}

void CPropertyListCtrl::UpdatePropHelpCtrls(RefCountedPtr< CPropertyObject > pProp)
{
	// set the property name
	m_pPropTitle->SetWindowText(pProp->GetName());
	m_pPropDesc->SetWindowText(pProp->GetDocumentationDesc());
}

void CPropertyListCtrl::FireScrolling()
{
}

void CPropertyListCtrl::FireInvokeFontPane()
{
	ShowPropertyDlg(true);
}
	
void CPropertyListCtrl::OnPaint() 
{
	try
	{
		PAINTSTRUCT ps; 
		CDC* pdc = BeginPaint(&ps);    
		
		if (m_PropertyList.GetCount() == 0)
			ClearArea(pdc->m_hDC);
		else
			RefreshGrid(pdc->m_hDC);	
		
		EndPaint(&ps);
	}
	catch(...)
	{
	}
}

void CPropertyListCtrl::OnChangeEdit() 
{

}

BOOL CPropertyListCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	SetCursor(m_Cursor	);
	return TRUE;
}

void CPropertyListCtrl::EditObjectbrowser() 
{
	if (m_pControl == NULL)
		return;

	AfxInitRichEdit();

	COpenDCLApp *pApp = (COpenDCLApp*)AfxGetApp();

	if (pApp->m_pCtrlHelp == NULL)
	{
		CObjectBrowser Dlg;
		Dlg.m_pControl = (COleControlObject*)m_pControl;
		Dlg.m_pControl.Lock();
		Dlg.m_pDclForm = m_pControl->GetOwnerForm();
		Dlg.m_sDclFormName = m_pControl->GetOwnerForm()->GetKeyName();
		Dlg.DoModal();
		return;
	}

	if (m_pIntelHelp == NULL)
	{
		m_pIntelHelp = new CObjectBrowser();
		m_pIntelHelp->m_pControl = (COleControlObject*)m_pControl;
		m_pIntelHelp->m_pControl.Lock();
		m_pIntelHelp->m_pDclForm = m_pControl->GetOwnerForm();
		m_pIntelHelp->m_sDclFormName = m_pControl->GetOwnerForm()->GetKeyName();
		m_pIntelHelp->Create(MAKEINTRESOURCE(IDD_OBJECTBROWSER), AfxGetApp()->m_pMainWnd);
	}
	else
	{
		m_pIntelHelp->m_pControl = (COleControlObject*)m_pControl;
		m_pIntelHelp->m_pControl.Lock();
		m_pIntelHelp->m_pDclForm = m_pControl->GetOwnerForm();
		m_pIntelHelp->m_sDclFormName = m_pControl->GetOwnerForm()->GetKeyName();
		m_pIntelHelp->Setup();
	}
	m_pIntelHelp->ShowWindow(TRUE);
}
