// ObjectDCLView.cpp : implementation of the CObjectDCLView class
//

#include "stdafx.h"
#include "ObjectDCLView.h"
#include "DclFormObject.h"
#include "ChildFrm.h"
#include "PropertyIds.h"
#include "ControlHolder.h"
#include "ZOrderListCtrl.h"
#include "Project.h"
#include "PropertyObject.h"
#include "ControlTypes.h"
#include "MainFrm.h"
#include "ControlName.h"
#include "DclControlObject.h"
#include "FileDlgCtrls.h"
#include "AxContainer.h"
#include "OptionListBox.h"
#include "StaticLink.h"
#include "3DRect.h"
#include "Splitter.h"
#include "PictureBox.h"
#include "RoundSliderCtrl.h"
#include "AcadColorListBox.h"
#include "ColorButton.h"
#include "ColorEdit.h"
#include "VdclGroupBox.h"
#include "GraphicButton.h"
#include "VdclStatic.h"
#include "ListCtrlEx.h"
#include "PurchaseMode.h"
#include "OleFont.h"
#include "AxPropertyDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "ObjectDCL.h"
#include "EditorWorkspace.h"
#include "SharedRes.h"


LPCTSTR gpszCliboardFormatName = _T("ODCL.Clipboard.Controls");


static CString LTOA(int nVal)
{
  CString sLong;
	sLong.Format(_T("%d"), nVal);
  return sLong;
}


static bool IsVersionFree()
{
	return activeProject->m_bFreeVersion;
}


static CDclControlObject* FindArxControlObject(CDclFormObject *pDclForm, CString sControlName, CDclControlObject *pCtrl = NULL )
{
	CDclControlObject *pRetObject = NULL;
		
	if (pDclForm == NULL)
		return NULL;

	// create a position variable to hold the converted ArxControlIndex
	POSITION pos = pDclForm->GetControlList().GetHeadPosition();
	while (pos)
	{
		CDclControlObject *pControl = pDclForm->GetControlList().GetNext(pos);
		if (pControl->GetStrProperty(nName) == sControlName && pCtrl != pControl)
			return pControl;
	}
	return NULL;
}


static CDclControlObject* GetArxControlObject(CDclFormObject *pDclForm, short ArxControlIndex)
{
	CDclControlObject *pRetObject;

	if(pDclForm->GetControlList().GetCount() > ArxControlIndex)
	{
		// create a position variable to hold the converted ArxControlIndex
		POSITION ControlPos;
		
		// set the position variable to be equal the index to passing to the GetAt method
		ControlPos = pDclForm->GetControlList().FindIndex(ArxControlIndex);	
		
		// set the pass pointer to point at the object in the list
		pRetObject = pDclForm->GetControlList().GetAt(ControlPos);
	}				
	else
		return NULL;
		
	return pRetObject;
}

static bool AddControlPropertyListItem( RefCountedPtr< CPropertyObject > pProp, LPCTSTR pszNewValue ) 
{
	if( !pProp )
		return false;
	assert( pProp->GetStringArrayPtr() != NULL );
	pProp->GetStringArrayPtr()->push_back( pszNewValue );
	return true;
}

static bool AddControlPropertyListItem( RefCountedPtr< CPropertyObject > pProp, int nNewValue ) 
{
	if( !pProp )
		return false;
	assert( pProp->GetIntArrayPtr() != NULL );
	pProp->GetIntArrayPtr()->push_back( nNewValue );
	return true;
}

static RefCountedPtr< CPropertyObject > AddControlHiddenProperty( CDclControlObject* pDclControl,
																																	PropertyId nID,
																																	LPCTSTR pszValue,
																																	PropertyType type )
{
	RefCountedPtr< CPropertyObject > pProp = pDclControl->AddStringProperty( nID, type, pszValue );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}


static CString FindTabCaption2(CDclFormObject *pDclTab, int nTabIndex)
{
	// set counter for ArxControls
	int nCount = pDclTab->GetControlList().GetCount();
	
	// set start position for navigating ArxControls
	POSITION pos = pDclTab->GetControlList().GetHeadPosition();
	
	// do loop to navigate Arx Controls
	while (pos != NULL)
	{
		// get current ArxControlObject
		CDclControlObject* pArxControl = pDclTab->GetControlList().GetNext(pos);

		if (pArxControl->GetType() == CtlTabStrip)
		{
			return pArxControl->GetPropertyListItem(nTabsCaption, nTabIndex);
		}

		// increment counter
		nCount--;
	}
	
	return CString();
}


static CString FindTabCaption(CDclFormObject *pDclTab)
{
	// do loop to add all the tree items
	for (int i=0; i<activeProject->GetDclFormList().GetCount(); i++)
	{
		POSITION pos = activeProject->GetDclFormList().FindIndex(i);
		if (pos != NULL)
		{
			CDclFormObject *pDcl = activeProject->GetDclFormList().GetAt(pos);
			if (pDcl != NULL)
			{
				if (pDclTab->GetParentName() == pDcl->GetUniqueName())
					return FindTabCaption2(pDcl, pDclTab->GetTabIndex());
			}			
		}
	}
	return CString();
}


class CClipboardObject : public CObject
{
	DECLARE_SERIAL(CClipboardObject);
public:
	CClipboardObject() {}
	virtual ~CClipboardObject()
	{
		mControls.RemoveAll();
		mImageListCollection.RemoveAll();
	}

	CTypedPtrList< CObList, CDclControlObject* > mControls;
	CTypedPtrList< CObList, CImageListObject* > mImageListCollection;	

	virtual void Serialize(CArchive& ar)
	{
		CObject::Serialize( ar );
		mControls.Serialize(ar);
		mImageListCollection.Serialize(ar);
	}
};
IMPLEMENT_SERIAL(CClipboardObject, CObject, 1);

/////////////////////////////////////////////////////////////////////////////
// CObjectDCLView

IMPLEMENT_DYNCREATE(CObjectDCLView, CView)

BEGIN_MESSAGE_MAP(CObjectDCLView, CView)
	//{{AFX_MSG_MAP(CObjectDCLView)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()	
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_BRINGTOFRONT, OnBringtofront)
	ON_COMMAND(ID_SENDTOBACK, OnSendtoback)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_EDIT_OBJECTBROWSER, OnEditObjectbrowser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectDCLView construction/destruction

CObjectDCLView::CObjectDCLView():CView()//(CObjectDCLView::IDD)
{
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);
    
	CString sText;
	sText = theWorkspace.LoadResourceString(IDS_nGridSpacing);
	
	// get the grid spacing
    m_gridSpacing = pApp->GetProfileInt(sProfileName,sText /*_T("nGridSpacing")*/, nDeGridSpacing);
    
	NullDrawRect();
	m_ControlId = nControlStartId;
	m_bShowGrip = true;
	m_bGripsCreate = false;
	m_MouseDown = false;
	m_bMoving = false;
	m_SelectedControl.m_nIndex = nNotSet;
	m_SelectedList.RemoveAll();
	m_UndoList.RemoveAll();
	m_MouseDown = false;
	m_bDrawing = false;
	m_nResizeQuadrant = nNotSet;
	m_StartupSize.cx = nDeStartupSize;
	m_StartupSize.cy = nDeStartupSize;
	m_StandardCursorID = false;
	m_rcDrawLast.SetRect(0,0,0,0);
	m_szGripSize.cx = GetSystemMetrics(SM_CXVSCROLL);
	m_szGripSize.cy = GetSystemMetrics(SM_CYHSCROLL);
	m_pThisDclForm = NULL;
	m_clsid.Data1 = 0;
	m_clsid.Data2 = 0;
	m_clsid.Data3 = 0;
}

CObjectDCLView::~CObjectDCLView()
{
	if (m_pThisDclForm)
	{
		m_pThisDclForm->m_pMdiChildWnd = NULL;
		m_pThisDclForm = NULL;
	}
		
	ClearSelection();
	ClearUndoList();
	
}

BOOL CObjectDCLView::PreCreateWindow(CREATESTRUCT& cs)
{
	
	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_CLIPCHILDREN;
	
	if( !CView::PreCreateWindow(cs) )
		return FALSE;
	
	return TRUE;
}

void CObjectDCLView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	ModifyStyleEx(WS_EX_CLIENTEDGE, NULL, 0);
	if (m_pThisDclForm == NULL)
	{
		ResizeParentToFit(false);	
	}	
}


void CObjectDCLView::ResizeParentToFit(BOOL bShrinkOnly)
{
	// determine current size of the client area as if no scrollbars present
	CRect rectClient;
	GetWindowRect(rectClient);
 	CChildFrame *pParentFrame = (CChildFrame*)GetParentFrame();

	rectClient.right =  pParentFrame->m_StartupSize.cx;
	rectClient.bottom =  pParentFrame->m_StartupSize.cy;
	CRect rect = rectClient;

	CalcWindowRect(rect);
	rectClient.left += rectClient.left - rect.left;
	rectClient.top += rectClient.top - rect.top;
	rectClient.right -= rect.right - rectClient.right;
	rectClient.bottom -= rect.bottom - rectClient.bottom;
	rectClient.OffsetRect(-rectClient.left, -rectClient.top);
	ASSERT(rectClient.left == 0 && rectClient.top == 0);

	CSize m_totalDev(300,300);
	// determine desired size of the view
	CRect rectView(0, 0,m_totalDev.cx, m_totalDev.cy);
	if (bShrinkOnly)
	{
		if (rectClient.right <= m_totalDev.cx)
			rectView.right = rectClient.right;
		if (rectClient.bottom <= m_totalDev.cy)
			rectView.bottom = rectClient.bottom;
	}
	CalcWindowRect(rectView, CWnd::adjustOutside);
	rectView.OffsetRect(-rectView.left, -rectView.top);
	ASSERT(rectView.left == 0 && rectView.top == 0);
	if (bShrinkOnly)
	{
		if (rectClient.right <= m_totalDev.cx)
			rectView.right = rectClient.right;
		if (rectClient.bottom <= m_totalDev.cy)
			rectView.bottom = rectClient.bottom;
	}

	// dermine and set size of frame based on desired size of view
	CRect rectFrame;
	CFrameWnd* pFrame = GetParentFrame();
	ASSERT_VALID(pFrame);
	if (IsWindow(pFrame->m_hWnd))
		pFrame->GetWindowRect(rectFrame);
	CSize size = rectFrame.Size();
	size.cx += rectView.right - rectClient.right;
	size.cy += rectView.bottom - rectClient.bottom;
	pFrame->SetWindowPos(NULL, 0, 0, pParentFrame->m_StartupSize.cx, pParentFrame->m_StartupSize.cy,
		SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE);
}

/////////////////////////////////////////////////////////////////////////////
// CObjectDCLView diagnostics

#ifdef _DEBUG
void CObjectDCLView::AssertValid() const
{
	CView::AssertValid();
}

void CObjectDCLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

//CObjectDCLDoc* CObjectDCLView::GetDocument() const // non-debug version is inline
//{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CObjectDCLDoc)));
//	return (CObjectDCLDoc*)m_pDocument;
//}

/////////////////////////////////////////////////////////////////////////////
// CObjectDCLView message handlers

void CObjectDCLView::DrawActiveXCtrl(CDclControlObject *pCtrl, CDC* pDC) 
{
	/*
	if (pCtrl->m_Delete == TRUE)
		return;

	CControlHolder *pCtrlHolder = (CControlHolder*)pCtrl->m_pCtrlHolder;
	CRect rcClient;
	pCtrlHolder->GetWindowRect(&rcClient);
	ScreenToClient(rcClient);

	IViewObject2 *m_lpViewObject = NULL;
	IDispatch *pDispatch = NULL;
	pDispatch = pCtrlHolder->GetOleIDispatch();
	if (pDispatch == NULL)
	{
		return;
	}
	HRESULT hr = pDispatch->QueryInterface(IID_IViewObject2, (void **) &m_lpViewObject);
	if (!SUCCEEDED(hr))
		return; // not supported

	
	IOleObject	*pOleObject = NULL;
	hr = pDispatch->QueryInterface(IID_IOleObject, (void **) &pOleObject);
	
	if (!SUCCEEDED(hr))
		return; // not supported
	
	if (m_lpViewObject == NULL)
	{
		return;
	}

	RECTL rc;
	rc.left = rcClient.left;
	rc.right = rcClient.right;
	rc.top = rcClient.top;
	rc.bottom = rcClient.bottom;
//	GetItemState();
	
	//m_lpViewObject->Draw( DVASPECT_CONTENT, nNotSet, NULL, NULL, NULL, pDC->m_hDC, &rc, NULL, NULL, 0 );
	
	//pCtrlHolder->MoveWindow(rcClient, TRUE);
		
	//pCtrl->m_pCtrlHolder->RedrawWindow();
	//pCtrl->m_pCtrlHolder->Invalidate();
	*/
}



/////////////////////////////////////////////////////////////////////////////
// CObjectDCLView message handlers
void CObjectDCLView::ClearUndoList()
{
	for (int i=m_UndoList.GetCount()-1; i>=0; i--)
	{
		POSITION pos = m_UndoList.FindIndex(i);
		if (pos != NULL)
		{
			CUndoActions *pUndoAction = m_UndoList.GetAt(pos);
			if (pUndoAction != NULL)
			{
				m_UndoList.RemoveAt(pos);
				pUndoAction->m_pControl = NULL;
				delete pUndoAction;
			}
		}
	}
	m_UndoList.RemoveAll();
}

void CObjectDCLView::NullDrawRect()
{
	m_bDrawing = false;
	m_rcDraw.top = 0;
	m_rcDraw.left = 0;
	m_rcDraw.right = 0;
	m_rcDraw.bottom = 0;
}
	

void CObjectDCLView::PaintBackGround(HDC hdc)
{
	try
	{
		CRect rcThis;
		GetWindowRect(&rcThis);
		
		CRect rcCell(0,0,rcThis.Width(), rcThis.Height());
		
		if (m_pThisDclForm == NULL)
			::SetBkColor(hdc, GetSysColor(COLOR_APPWORKSPACE));
		else			
			::SetBkColor(hdc, GetSysColor(COLOR_BTNFACE));
		// draw the solid rectangle
		::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rcCell, NULL, 0, NULL);

		if (m_gridSpacing < nMinGridSpacing)
			return;

		if (m_pThisDclForm == NULL)
			return;

		COLORREF clrGrid = GetSysColor(COLOR_BTNTEXT);
		for (int pX=0; pX<rcThis.Width(); pX += m_gridSpacing)
		{
			for (int pY=0; pY<rcThis.Height(); pY += m_gridSpacing)
			{
				SetPixel(hdc, pX, pY, clrGrid);
			}
		}
	}
	catch(...)
	{
	}
}

void CObjectDCLView::OnGridSpacingChanged() 
{
	try
	{
	GetDocument()->SetModifiedFlag(TRUE);
	Invalidate();
	}
	catch(...)
	{
	}
}

void CObjectDCLView::OnControlToInsertChanged() 
{
	m_SelectedControl.m_pArxObject = NULL;
	m_SelectedControl.m_pControl = NULL;
	m_SelectedControl.m_nIndex = nNotSet;
	GetDocument()->SetModifiedFlag(TRUE);
}
void CObjectDCLView::HideGrips()
{
	m_GripRect1.ShowWindow(FALSE);
	m_GripRect2.ShowWindow(FALSE);
	m_GripRect3.ShowWindow(FALSE);
	m_GripRect4.ShowWindow(FALSE);
	m_GripRect5.ShowWindow(FALSE);
	m_GripRect6.ShowWindow(FALSE);
	m_GripRect7.ShowWindow(FALSE);
	m_GripRect8.ShowWindow(FALSE);

}
void CObjectDCLView::MoveGripRectsForward()
{
	m_GripRect1.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect2.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect3.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect4.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect5.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect6.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect7.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect8.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);

}
void CObjectDCLView::ShowGripRects(BOOL bShow, CRect rcCtrl)
{
	if (m_bGripsCreate == FALSE)
		return;
	

	// set the predetermined pos's
	int LeftX = rcCtrl.left - (nGripSize / 2);
	int MidX = rcCtrl.left + ((rcCtrl.Width() - nGripSize) / 2);
	int RightX = rcCtrl.left + rcCtrl.Width() - (nGripSize / 2);
	int TopY = rcCtrl.top - (nGripSize / 2);
	int MidY = rcCtrl.top + ((rcCtrl.Height() - nGripSize) / 2);
	int BottomY = rcCtrl.top + rcCtrl.Height() - (nGripSize / 2);
	
	try
	{

		// set the positions of all the grip rect's
		m_GripRect1.MoveWindow( 		
			LeftX, TopY, nGripSize, nGripSize, 
			TRUE);
		
		m_GripRect2.MoveWindow( 
			MidX, TopY, nGripSize, nGripSize,
			TRUE);
			

		m_GripRect3.MoveWindow( 
			RightX,
			TopY, nGripSize, nGripSize,
			TRUE);
			

		m_GripRect4.MoveWindow( 
			LeftX,
			MidY,
			nGripSize, nGripSize, 
			TRUE);
			
		m_GripRect5.MoveWindow( 
			RightX, 
			MidY,
			nGripSize, nGripSize,
			TRUE);

		// set the positions of all the grip rect's
		m_GripRect6.MoveWindow( 
			LeftX,
			BottomY,
			nGripSize, nGripSize, 
			TRUE);
		
		m_GripRect7.MoveWindow( 
			MidX, 
			BottomY,
			nGripSize, nGripSize,
			TRUE);

		m_GripRect8.MoveWindow( 
			RightX,
			BottomY,
			nGripSize, nGripSize,
			TRUE);

		if (m_GripRect1.IsWindowVisible() == FALSE)
		{
			// now show all the grip rect's
			m_GripRect1.ShowWindow(bShow);
			m_GripRect2.ShowWindow(bShow);
			m_GripRect3.ShowWindow(bShow);
			m_GripRect4.ShowWindow(bShow);
			m_GripRect5.ShowWindow(bShow);
			m_GripRect6.ShowWindow(bShow);
			m_GripRect7.ShowWindow(bShow);
			m_GripRect8.ShowWindow(bShow);
		}
	}
	catch(...)
	{
	}
}

void CObjectDCLView::SelectControl(CString sName) 
{
	CDclControlObject* pArxObject = FindArxControlObject(m_pThisDclForm, sName);
	if (pArxObject == NULL)
		return;

	CControlHolder *pControl = (CControlHolder*)pArxObject->m_pCtrlHolder;

	if (m_SelectedControl.m_pArxObject == NULL &&
		!IsInSelection(pArxObject))
	{
		m_SelectedControl.m_pArxObject = pArxObject;	
		m_SelectedControl.m_pControl = pControl;
		m_SelectedControl.m_nIndex = pArxObject->m_Index;
		ClearSelection();
		CRect rcCtrl;
		pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		ShowGripRects(TRUE, rcCtrl);
		FireControlSelected(pArxObject);
	}
	else // add the selected control to the multi selection
	{
		if (!IsInSelection(pArxObject) && pArxObject != m_SelectedControl.m_pArxObject)
		{
			CSelectedControl *pSelection = new CSelectedControl;
			pSelection->m_pArxObject = pArxObject;
			pSelection->m_pControl = pControl;
			pSelection->m_nIndex = pArxObject->m_Index;
			m_SelectedList.AddTail(pSelection);
			pControl->SetSelected(TRUE);			
			FireControlSelected(NULL);
		}
	}

}
bool CObjectDCLView::CheckControlsForSelection(CRect rcSelArea, bool bLookForOne) 
{
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();

	bool bReturn = false;
	// we are going to do a loop to querry each control if it's to be selected
	for (int i=m_pThisDclForm->GetControlList().GetCount()- 1; i> 0; i--)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			// get the control
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			if (pArxObject != NULL && pArxObject->m_Delete == FALSE)
			{
				CControlHolder *pControl = (CControlHolder*)pArxObject->m_pCtrlHolder;
				// get the control's position
				CRect rcCtrl;
				pControl->GetWindowRect(&rcCtrl);
				ScreenToClient(rcCtrl);

				bool bCrosses = false;
				// check if the left selection rect crosses this control
				if (rcSelArea.left > rcCtrl.left &&
					rcSelArea.left < rcCtrl.right &&
					rcSelArea.top < rcCtrl.bottom &&
					rcSelArea.bottom > rcCtrl.top)
					bCrosses = true;
				
				// check if the right selection rect crosses this control
				if (rcSelArea.right < rcCtrl.right &&
					rcSelArea.right > rcCtrl.left &&
					rcSelArea.top < rcCtrl.bottom &&
					rcSelArea.bottom > rcCtrl.top)
					bCrosses = true;
				
				// check if the top selection rect crosses this control
				if (rcSelArea.right > rcCtrl.right &&
					rcSelArea.left < rcCtrl.right &&
					rcSelArea.top < rcCtrl.bottom &&
					rcSelArea.top > rcCtrl.top)
					bCrosses = true;

				// check if the bottom selection rect crosses this control
				if (rcSelArea.right > rcCtrl.right &&
					rcSelArea.left < rcCtrl.right &&
					rcSelArea.bottom < rcCtrl.bottom &&
					rcSelArea.bottom > rcCtrl.top)
					bCrosses = true;

				// check if this control is inside the selection rect
				if (rcSelArea.right > rcCtrl.right &&
					rcSelArea.left < rcCtrl.left &&
					rcSelArea.bottom > rcCtrl.bottom &&
					rcSelArea.top < rcCtrl.top)
					bCrosses = true;

				// is the point inside the control
				if (bCrosses)
				{
					if (m_SelectedControl.m_pArxObject == NULL &&
						!IsInSelection(pArxObject))
					{
						pZOrderList->ClearSelection();
						pZOrderList->SelectItem(pArxObject->GetStrProperty(nName), true);
						// call the method to setup the fonts in the font tool bar.
						theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar(pArxObject);
	
						m_SelectedControl.m_pArxObject = pArxObject;
						m_SelectedControl.m_pControl = pControl;
						m_SelectedControl.m_nIndex = i;
						ClearSelection();
						ShowGripRects(TRUE, rcCtrl);
						if(bLookForOne)
							return true;
					}
					else // add the selected control to the multi selection
					{
						if (!IsInSelection(pArxObject) && pArxObject != m_SelectedControl.m_pArxObject)
						{
							CSelectedControl *pSelection = new CSelectedControl;
							pZOrderList->SelectItem(pArxObject->GetStrProperty(nName), false);
							// call the method to setup the fonts in the font tool bar.
	 						theEditorWorkspace.GetMainFrame()->m_wndDlgBar.AddFontToToolBar(pArxObject);
	
							pSelection->m_pArxObject = pArxObject;
							pSelection->m_pControl = pControl;
							pSelection->m_nIndex = i;
							m_SelectedList.AddTail(pSelection);
							pControl->SetSelected(TRUE);
							
						}
					}
					bReturn = true;
				}			
			}
		} 
	}
	
	return bReturn;
}

bool CObjectDCLView::CheckControlsForSelection(CPoint point, UINT nFlags, bool bMouseDown) 
{
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();	
	if (m_SelectedList.GetCount() == 0 &&
		nFlags > 1 &&
		m_SelectedControl.m_nIndex == nNotSet)
	{
		nFlags = 1;
	}
	// we are going to do a loop to querry each control if it's to be selected
	for (int i=m_pThisDclForm->GetControlList().GetCount()- 1; i>0; i--)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			// get the control
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			if (pArxObject != NULL && pArxObject->m_Delete == FALSE)
			{
				CControlHolder *pControl = (CControlHolder*)pArxObject->m_pCtrlHolder;
				if (!pControl)
					continue;
				// get the control's position
				CRect rcCtrl;
				pControl->GetWindowRect(&rcCtrl);
				ScreenToClient(rcCtrl);								

				// is the point inside the control
				if (rcCtrl.PtInRect(point))
				{
					if (nFlags == 1 && 
						!IsInSelection(pArxObject) &&
						pArxObject != m_SelectedControl.m_pArxObject)
					{
						HideGrips();
						pZOrderList->ClearSelection();
						pZOrderList->SelectItem(pArxObject->GetStrProperty(nName), true);
						// call the method to setup the fonts in the font tool bar.
						theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar(pArxObject);
	
						m_SelectedControl.m_pArxObject = pArxObject;
						m_SelectedControl.m_pControl = pControl;
						m_SelectedControl.m_nIndex = i;
						ClearSelection();
						ShowGripRects(TRUE, rcCtrl);
						FireControlSelected(pArxObject);
					}
					else // add the selected control to the multi selection
					{
						if (!IsInSelection(pArxObject) && pArxObject != m_SelectedControl.m_pArxObject)
						{
							CSelectedControl *pSelection = new CSelectedControl;
							pZOrderList->SelectItem(pArxObject->GetStrProperty(nName), false);
							// call the method to setup the fonts in the font tool bar.
							theEditorWorkspace.GetMainFrame()->m_wndDlgBar.AddFontToToolBar(pArxObject);
	
							pSelection->m_pArxObject = pArxObject;
							pSelection->m_pControl = pControl;
							pSelection->m_nIndex = i;
							m_SelectedList.AddTail(pSelection);
							pControl->SetSelected(TRUE);
							if (!bMouseDown)
								FireControlSelected(NULL);
						}
					}
					return true;
				}			
			} 
		}
	}
	
	return false;
}

void CObjectDCLView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_MouseDown = false;
	FirePopUpMenuPlease(point.x, point.y);
	
	
	CView::OnRButtonUp(nFlags, point);
}


void CObjectDCLView::MoveControl(CSelectedControl *pSelectedControl, CPoint point)
{
	CRect rcCtrl;
	CRect rcNewPos;

	if (pSelectedControl->m_pControl == NULL)
		return;

	int nXDiff = m_DrawStartPoint.x - point.x;
	int nYDiff = m_DrawStartPoint.y - point.y;

	
	rcNewPos.left = RoundToGridPattern(((CControlHolder*)pSelectedControl->m_pControl)->m_pLeftProp->GetLongValue() - nXDiff);
	rcNewPos.right = rcNewPos.left + ((CControlHolder*)pSelectedControl->m_pControl)->m_pWidthProp->GetLongValue();
	rcNewPos.top = RoundToGridPattern(((CControlHolder*)pSelectedControl->m_pControl)->m_pTopProp->GetLongValue() - nYDiff);
	rcNewPos.bottom = rcNewPos.top + ((CControlHolder*)pSelectedControl->m_pControl)->m_pHeightProp->GetLongValue();

	CRect rcThis;
	GetWindowRect(&rcThis);
	
	if (pSelectedControl->m_rcLastDrawn != rcNewPos)
	{
		HDC hdc = ::GetDC(NULL);
		CRect rc;
		// undraw the preview rectangle
		if (pSelectedControl->m_rcLastDrawn.Width() != 0)
		{
			rc.left = pSelectedControl->m_rcLastDrawn.left + rcThis.left+3;
			rc.top = pSelectedControl->m_rcLastDrawn.top + rcThis.top+3;
			rc.right = pSelectedControl->m_rcLastDrawn.right + rcThis.left+3;
			rc.bottom = pSelectedControl->m_rcLastDrawn.bottom + rcThis.top+3;
			::DrawFocusRect(hdc, rc);			
		}
		rc.left = rcNewPos.left + rcThis.left + 3;
		rc.top = rcNewPos.top + rcThis.top + 3;
		rc.right = rcNewPos.right + rcThis.left+3;
		rc.bottom = rcNewPos.bottom + rcThis.top+3;			
		::DrawFocusRect(hdc, rc);
		pSelectedControl->m_rcLastDrawn = rcNewPos;
		::ReleaseDC(NULL, hdc);
	}
	
	theEditorWorkspace.GetToolBox()->ResetToPointer();
	m_bMoving = true;
}

void CObjectDCLView::RemoveDragRect(CSelectedControl *pSelectedControl)
{
	//if (pSelectedControl->m_pControl == NULL)
	//	return;

	HDC hdc = ::GetDC(NULL);

		
	// undraw the preview rectangle
	if (pSelectedControl->m_rcLastDrawn.Width() != 0)
	{
		CRect rcThis;
		GetWindowRect(&rcThis);

		CRect rc;
		rc.left = pSelectedControl->m_rcLastDrawn.left + rcThis.left+3;
		rc.top = pSelectedControl->m_rcLastDrawn.top + rcThis.top+3;
		rc.right = pSelectedControl->m_rcLastDrawn.right + rcThis.left+3;
		rc.bottom = pSelectedControl->m_rcLastDrawn.bottom + rcThis.top+3;
		::DrawFocusRect(hdc, rc);			
	}
	::ReleaseDC(NULL, hdc);
}

void CObjectDCLView::AllRemoveDragRects()
{
	RemoveDragRect(&m_SelectedControl);
	
	for (int i=0; i<m_SelectedList.GetCount(); i++)
	{
		POSITION pos = m_SelectedList.FindIndex(i);
		if (pos != NULL)
		{
			CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
			if (pSelControl != NULL)
				RemoveDragRect(pSelControl);
		}
	}
}



bool CObjectDCLView::StoreControlsPosition(CSelectedControl *pSelectedControl)
{
	CRect rcCtrl;

	if (pSelectedControl->m_pControl == NULL)
		return false;

	rcCtrl = pSelectedControl->m_rcLastDrawn;

    if (rcCtrl.Width() == 0)
		return false;

	pSelectedControl->m_pControl->MoveWindow(rcCtrl, TRUE);
	CRect rc;
	pSelectedControl->m_pControl->GetWindowRect(&rc);
	ScreenToClient(rc);
	if (rcCtrl != rc)
	{
		ShowGripRects(TRUE, rc);
		// invalidate the last draw rect info Note, this does not mean redraw
		m_SelectedControl.Reset();
		return false;
	}

	// set the position properties
	pSelectedControl->m_pArxObject->SetLongProperty(nLeft, rcCtrl.left);
	pSelectedControl->m_pArxObject->SetLongProperty(nTop, rcCtrl.top);
	
	pSelectedControl->m_pArxObject->SetLongProperty(nWidth, rcCtrl.Width());
	pSelectedControl->m_pArxObject->SetLongProperty(nHeight, rcCtrl.Height());

	CalcControlOffsetDistances(pSelectedControl->m_pArxObject, rcCtrl);

	return true;
}

void CObjectDCLView::CalcControlOffsetDistances(CDclControlObject *pArxObject, CRect &rcCtrl)
{
	CRect rcThis;
	GetClientRect(&rcThis);

	if (pArxObject->m_Delete == TRUE)
		return;

	int nTheLeft = pArxObject->GetLngProperty(nLeft);
	int nTheTop = pArxObject->GetLngProperty(nTop);
	
	int nTheWidth = pArxObject->GetLngProperty(nWidth);
	int nTheHeight = pArxObject->GetLngProperty(nHeight);

	// get the offset boolean flags
	int lLeftFromRight = 0;
	if (pArxObject->m_pUseLeftOffset)
	{
		if (pArxObject->m_pUseLeftOffset->GetType() == PropBool)
			lLeftFromRight = pArxObject->m_pUseLeftOffset->GetBooleanValue();
		else
			lLeftFromRight = pArxObject->m_pUseLeftOffset->GetLongValue();
	}

	int lRightFromRight = 0;
	if (pArxObject->m_pUseLeftOffset)
	{
		if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
			lRightFromRight = pArxObject->m_pUseRightOffset->GetBooleanValue();
		else
			lRightFromRight = pArxObject->m_pUseRightOffset->GetLongValue();
	}

	int lTopFromBottom = 0;
	if (pArxObject->m_pUseLeftOffset)
	{
		if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
			lTopFromBottom = pArxObject->m_pUseTopOffset->GetBooleanValue();
		else
			lTopFromBottom = pArxObject->m_pUseTopOffset->GetLongValue();
	}

	int lBottomFromBottom = 0;
	if (pArxObject->m_pUseLeftOffset)
		lBottomFromBottom = pArxObject->m_pUseBottomOffset->GetLongValue();

	// set the offset position properties
	if (lLeftFromRight == 0 || lLeftFromRight == 1)
		pArxObject->SetLongProperty(nLeftFromRight, rcThis.Width() - nTheLeft);
	else if (nLeftFromRight == 2)
		pArxObject->SetLongProperty(nLeftFromRight, nTheLeft - (rcThis.Width()/2));
	else
	{
		CRect rc = GetSplitterRect(lLeftFromRight);
		pArxObject->SetLongProperty(nLeftFromRight, nTheLeft - rc.left);
	}
	

	if (lRightFromRight == 0 || lRightFromRight == 1)
		pArxObject->SetLongProperty(nTopFromBottom, rcThis.Height() - nTheTop);
	else
	{
		CRect rc = GetSplitterRect(lRightFromRight);
		pArxObject->SetLongProperty(nTopFromBottom, nTheTop - rc.top);
	}


	if (lTopFromBottom == 0 || lTopFromBottom == 1)
		pArxObject->SetLongProperty(nRightFromRight, rcThis.Width() - nTheLeft - nTheWidth);
	else
	{
		CRect rc = GetSplitterRect(lTopFromBottom);
		pArxObject->SetLongProperty(nRightFromRight, nTheLeft + nTheWidth - rc.left);	
	}
	

	if (lBottomFromBottom == 0 || lBottomFromBottom == 1)
		pArxObject->SetLongProperty(nBottomFromBottom, rcThis.Height() - nTheTop - nTheHeight);
	else
	{
		CRect rc = GetSplitterRect(lBottomFromBottom);
		pArxObject->SetLongProperty(nBottomFromBottom, (nTheTop + nTheHeight)-rc.top);
	}
}

CRect CObjectDCLView::GetSplitterRect(int nId)
{
	CRect rc;
	// just in case get the client rect
	GetClientRect(&rc);

	// now find and retrieve the rectangle for the splitter.
	POSITION pos = m_pThisDclForm->GetControlList().GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject *pCtrl = m_pThisDclForm->GetControlList().GetNext(pos);

		if (pCtrl->GetType() == CtlSplitter &&
			pCtrl->m_Id == nId)
		{
			rc.SetRect(
				pCtrl->GetLngProperty(nLeft),
				pCtrl->GetLngProperty(nTop),
				pCtrl->GetLngProperty(nWidth),
				pCtrl->GetLngProperty(nHeight));

			return rc;
		}
	}
	return rc;
}
int CObjectDCLView::CheckGripsMouseMoveOver(CPoint point) 
{
	CRect rc1;
	CRect rc2;
	CRect rc3;
	CRect rc4;
	CRect rc5;
	CRect rc6;
	CRect rc7;
	CRect rc8;
	m_GripRect1.GetWindowRect(&rc1);
	m_GripRect2.GetWindowRect(&rc2);
	m_GripRect3.GetWindowRect(&rc3);
	m_GripRect4.GetWindowRect(&rc4);
	m_GripRect5.GetWindowRect(&rc5);
	m_GripRect6.GetWindowRect(&rc6);
	m_GripRect7.GetWindowRect(&rc7);
	m_GripRect8.GetWindowRect(&rc8);
	ScreenToClient(rc1);
	ScreenToClient(rc2);
	ScreenToClient(rc3);
	ScreenToClient(rc4);
	ScreenToClient(rc5);
	ScreenToClient(rc6);
	ScreenToClient(rc7);
	ScreenToClient(rc8);
	
	// here we need to check each grip rectangle to see if the point to process is inside one of these
	// grips, so we can changed the cursor and send back the correct quadrant.
	if (rc1.PtInRect(point))
	{
		SetCursor(m_GripRect1.m_hCursor);		
		return 1;
	}
	else if (rc2.PtInRect(point))
	{
		SetCursor(m_GripRect2.m_hCursor);
		return 2;
	}
	else if (rc3.PtInRect(point))
	{
		SetCursor(m_GripRect3.m_hCursor);
		return 3;
	}
	else if (rc4.PtInRect(point))
	{
		SetCursor(m_GripRect4.m_hCursor);
		return 4;
	}
	else if (rc5.PtInRect(point))
	{
		SetCursor(m_GripRect5.m_hCursor);
		return 5;
	}
	else if (rc6.PtInRect(point))
	{
		SetCursor(m_GripRect6.m_hCursor);
		return 6;
	}
	else if (rc7.PtInRect(point))
	{
		SetCursor(m_GripRect7.m_hCursor);
		return 7;
	}
	else if (rc8.PtInRect(point))
	{
		SetCursor(m_GripRect8.m_hCursor);
		return 8;
	}
	else
	{
		CChildFrame *pFrame = (CChildFrame*)GetParentFrame();
		if (m_StandardCursorID)
		{
			SetCursor(pFrame->m_CrossCursor);		
		}
		else
		{
			SetCursor(pFrame->m_ArrowCursor);
		}
	}
	// return a value indicating not over a grip control
	return nNotSet;
}
void CObjectDCLView::OnMouseMove(UINT nFlags, CPoint point) 
{
	
	CheckGripsMouseMoveOver(point);

	// here we are going to resize the draw rect if 
	// the user is resizing a control
	if (m_MouseDown && !m_bDrawing && m_nResizeQuadrant > nNotSet)
	{
		DragResized(m_nResizeQuadrant, point);
		CView::OnMouseMove(nFlags, point);
		return;
	}

	if (m_MouseDown && !m_bDrawing &&
		m_SelectedControl.m_nIndex > nNotSet)
	{		
		MoveControl(&m_SelectedControl, point);						
	}
	

	if (m_MouseDown && !m_bDrawing &&
		m_SelectedList.GetCount() > 0)
	{
		
		for (int i=0; i<m_SelectedList.GetCount(); i++)
		{
			POSITION pos = m_SelectedList.FindIndex(i);
			if (pos != NULL)
			{
				CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
				if (pSelControl != NULL)
				{
					MoveControl(pSelControl, point);						
				}
			}
		}
		
	}

	
	// setup the CRect for FillRect
	if (m_bDrawing)
	{
		CRect rcNew;
		
		// set the x coord's
		if (point.x < m_DrawStartPoint.x)
		{
			rcNew.left = RoundToGridPattern(point.x);
			rcNew.right = m_DrawStartPoint.x;
		}
		else
		{			
			rcNew.right = RoundToGridPattern(point.x);
			rcNew.left = m_DrawStartPoint.x;
		}
		
		// set the y coord's
		if (point.y < m_DrawStartPoint.y)
		{
			rcNew.top = RoundToGridPattern(point.y);
			rcNew.bottom = m_DrawStartPoint.y;
		}
		else
		{			
			rcNew.bottom = RoundToGridPattern(point.y);
			rcNew.top = m_DrawStartPoint.y;
		}
		
		CRect rc;
		GetWindowRect(&rc);

		m_rcDraw = rcNew;

		rcNew.left += rc.left+3;
		rcNew.top += rc.top+3;
		rcNew.bottom += rc.top+3;
		rcNew.right += rc.left+3;

		// draw the focus rect
		HDC hdc = ::GetDC(NULL);
		::DrawFocusRect(hdc, m_rcDrawLast);	
		m_rcDrawLast = rcNew;
		::DrawFocusRect(hdc, m_rcDrawLast);	
		::ReleaseDC(NULL, hdc);
		
	}
			
	CView::OnMouseMove(nFlags, point);
}
void CObjectDCLView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnLButtonUp(nFlags, point);
	
	HideSizingRect();
	
	if (m_pThisDclForm == NULL)
		return;

	m_MouseDown = false;
	
	// here we are going to resize the draw rect if 
	// the user is resizing a control
	if (!m_bDrawing && m_nResizeQuadrant > nNotSet)
	{
		CompletedDragResize(m_nResizeQuadrant, point);		
		CalcAllOffsets();
		theEditorWorkspace.GetToolBox()->ResetToPointer();
		return;
	}


	if (m_bMoving && m_SelectedControl.m_pControl != NULL)
	{
		m_bMoving = false;
		AllRemoveDragRects();

		CRect rcCtrl;
		m_SelectedControl.m_pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		
		int nXdiff = m_SelectedControl.m_rcLastDrawn.left - rcCtrl.left;
		int nYdiff = m_SelectedControl.m_rcLastDrawn.top - rcCtrl.top;
		
		if (nXdiff < 0)
			nXdiff = nXdiff * nNotSet;
		if (nYdiff < 0)
			nYdiff = nYdiff * nNotSet;
		
		// the user can only move the control the length of a grid space as a minumum
		if (nXdiff < m_gridSpacing && nYdiff < m_gridSpacing)
		{
			ShowGripRects(TRUE, m_SelectedControl.m_rcLastDrawn);	
			
			// invalidate the last draw rect
			m_SelectedControl.Reset();
			
			for (int i=0; i<m_SelectedList.GetCount(); i++)
			{
				POSITION pos = m_SelectedList.FindIndex(i);
				if (pos != NULL)
				{
					CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
					if (pSelControl != NULL)
					{
						if (pSelControl->m_pControl != NULL)
						{
							pSelControl->Reset();
						}
					}
				}
			}
		}
		
		// the user can only move the control the length of a grid space as a minumum
		else 
		{
			// add the event to the undo list
			CUndoActions *pUndo = new CUndoActions(
				uaMoved,
				m_SelectedControl.m_pArxObject,
				m_SelectedControl.m_pControl);		
			
			m_SelectedControl.m_pArxObject->m_rcOldPosition = rcCtrl;
			pUndo->rcPos = rcCtrl;
			m_UndoList.AddTail(pUndo);
			
			if (!StoreControlsPosition(&m_SelectedControl))
				return;
			
			ShowGripRects(TRUE, m_SelectedControl.m_rcLastDrawn);	
			
			// invalidate the last draw rect
			m_SelectedControl.Reset();
			for (int i=0; i<m_SelectedList.GetCount(); i++)
			{
				POSITION pos = m_SelectedList.FindIndex(i);
				if (pos != NULL)
				{
					CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
					if (pSelControl != NULL)
						if (pSelControl->m_pControl != NULL)
						{
							// add the event to the undo list
							CUndoActions *pUndo = new CUndoActions(
								uaMoved,
								pSelControl->m_pArxObject,
								pSelControl->m_pControl);		
							pUndo->rcPos.left = pSelControl->m_pArxObject->GetLngProperty(nLeft);
							pUndo->rcPos.top = pSelControl->m_pArxObject->GetLngProperty(nTop);
							pUndo->rcPos.right = pUndo->rcPos.left + pSelControl->m_pArxObject->GetLngProperty(nWidth);
							pUndo->rcPos.bottom = pUndo->rcPos.top + pSelControl->m_pArxObject->GetLngProperty(nHeight);
							pSelControl->m_pArxObject->m_rcOldPosition = pUndo->rcPos;
			
							m_UndoList.AddTail(pUndo);
							
							StoreControlsPosition(pSelControl);
							// invalidate the last draw rect
							pSelControl->Reset();
			
						}
				}
				// inform the parent that the undo is to be enabled
				FireSetUndo();
			}
		}
		CalcAllOffsets();
	}
	
	if (!m_bDrawing && m_SelectedControl.m_pArxObject != NULL)
	{
		if (m_SelectedList.GetCount() != 0)
			FireControlSelected(NULL);
		else
		{
			FireControlSelected(m_SelectedControl.m_pArxObject);
		}	
	}
	
	
	if (m_bDrawing)
	{
		m_bDrawing = false;
		
		if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl < CtlLabel)
		{
			if (CheckControlsForSelection(m_rcDraw, false))
			{				
				// call method to ensure the selection set is highlighted
				ShowSelection();		
				
				if (m_SelectedList.GetCount() > 0)
					FireControlSelected(NULL);
				else
					FireControlSelected(m_SelectedControl.m_pArxObject);
			}
			else
			{
			
				HideGrips();
				ClearSelection();
			}
		}
		else
		{
		
			HideGrips();
			ClearSelection();
		}
		HideSizingRect();
		// draw the focus rect
		m_bDrawing = false;
		CalcAllOffsets();
	}

	
	if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl < CtlLabel &&
		(m_SelectedControl.m_nIndex == nNotSet ||
		 m_SelectedControl.m_pArxObject == NULL ||
		 m_SelectedControl.m_pControl == NULL)
		)
	{
		FireShowFormGrips(TRUE);		
	}
	else
		FireShowFormGrips(FALSE);
	
	// check to see if we are going to insert a control
	if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl >= CtlLabel)
	{
		if (m_rcDraw.Width() != 0 && m_rcDraw.Height() != 0)
		{		
			// call method to insert the new control
			InsertControl(m_rcDraw);
		}
		else
		{
			FireShowFormGrips(FALSE);
		}
		m_StandardCursorID = false;
		theEditorWorkspace.GetToolBox()->ResetToPointer();
	}
	
	// clear the draw rect info
	NullDrawRect();
					
	CalcAllOffsets();
}

void CObjectDCLView::ClearSelection(bool bResetZOrder)
{
	if (m_SelectedList.GetCount() == 0)
		return;

	HideGrips();		
	for (int i=m_SelectedList.GetCount()- 1; i>=0; i--)
	{
		POSITION pos = m_SelectedList.FindIndex(i);
		if (pos != NULL)
		{
			CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
			if (pSelControl != NULL)
			{
				((CControlHolder*)pSelControl->m_pControl)->ReleaseSelection();
				m_SelectedList.RemoveAt(pos);
				pSelControl->Clear();
				delete pSelControl;
			}
		}
	}
	m_SelectedList.RemoveAll();

	if (bResetZOrder)
	{
		CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
		pZOrderList->ClearSelection();
	}
}

void CObjectDCLView::ShowSelection()
{
	if (m_SelectedList.GetCount() == 0)
		return;

	for (int i=0; i<m_SelectedList.GetCount(); i++)
	{
		POSITION pos = m_SelectedList.FindIndex(i);
		if (pos != NULL)
		{
			CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
			((CControlHolder*)pSelControl->m_pControl)->SetSelected(TRUE);
		}
	}
}
bool CObjectDCLView::IsInSelection(CDclControlObject *pArxObject)
{
	if (m_SelectedList.GetCount() == 0)
		return false;

	for (int i=0; i<m_SelectedList.GetCount(); i++)
	{
		POSITION pos = m_SelectedList.FindIndex(i);
		if (pos != NULL)
		{
			CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);			
			if (pSelControl->m_pArxObject->m_Id == pArxObject->m_Id)
			{
				return true;
			}
		}
	}
	
	return false;
}
void CObjectDCLView::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	if (m_pThisDclForm == NULL)
		return;

	if (m_MouseDown && !m_bDrawing &&
		m_SelectedControl.m_nIndex > nNotSet)
		return;		
	
	int nQuad = CheckGripsMouseMoveOver(point);
	if (nQuad > nNotSet)
		SetupDragResize(nQuad);
	else
		OnMouseButtonDown(nFlags, point);

	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		FireMouseDownEvent();
	}
	
	//
	CView::OnLButtonDown(nFlags, point);
		
}
void CObjectDCLView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	OnMouseButtonDown(1, point);
	
	if (m_pThisDclForm == NULL)
		return;

	// check to see if we are going to try and select controls
	bool bSkipDragRect = CheckControlsForSelection(point, nFlags, true);
	if (bSkipDragRect)
	{
		ShowSelection();	
		m_bDrawing = false;
	}
	else
	{
		m_SelectedControl.m_pArxObject = NULL;
		m_SelectedControl.m_pControl = NULL;
		m_SelectedControl.m_nIndex = nNotSet;
		HideGrips();
		ClearSelection();
		FireShowFormGrips(TRUE);
	}	
	
	if (m_SelectedControl.m_nIndex != nNotSet &&
		m_SelectedControl.m_pArxObject != NULL &&
		m_SelectedControl.m_pControl != NULL)
	{
		m_MouseDown = false;
		CRect rcCtrl;
		m_SelectedControl.m_pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		ShowGripRects(TRUE, rcCtrl);						
		if (m_SelectedList.GetCount() > 0)
			FireControlSelected(NULL);
		else
			FireControlSelected(m_SelectedControl.m_pArxObject);
	}
	else
	{
		HideSizingRect();
		m_bDrawing = false;	
		HideGrips();
		ClearSelection();
		FireMouseDownEvent();
	
	}

	CView::OnRButtonDown(nFlags, point);
}

void CObjectDCLView::OnMouseButtonDown(UINT nFlags, CPoint point) 
{
	FireShowFormGrips(FALSE);
	m_MouseDown = true;	
	bool bSkipDragRect = false;
	
	if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl >= CtlLabel)
	{
		HideGrips();
		ClearSelection();
	}

	// check to see if we are going to try and select controls
	if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl < CtlLabel)
	{
		bSkipDragRect = CheckControlsForSelection(point, nFlags, true);
		if (bSkipDragRect)
		{
			m_DrawStartPoint.x = point.x;
			m_DrawStartPoint.y = point.y;
			ShowSelection();	
			m_bDrawing = false;
		}
		else
		{
			m_SelectedControl.m_pArxObject = NULL;
			m_SelectedControl.m_pControl = NULL;
			m_SelectedControl.m_nIndex = nNotSet;
			HideGrips();
			ClearSelection();
			m_bDrawing = true;
		}
	}	
	
	
	if (!bSkipDragRect)
	{
		m_DrawStartPoint.x = RoundToGridPattern(point.x);
		m_DrawStartPoint.y = RoundToGridPattern(point.y);
		m_bDrawing = true;
	}
	
}
int CObjectDCLView::RoundToGridPattern(int Value)
{
	// return value if the grid spacing is too small
	if (m_gridSpacing < nMinGridSpacing)
		return Value;
	
	// return value if we are not inserting a control
	if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl < CtlLabel && !m_bMoving && m_nResizeQuadrant == nNotSet)
		return Value;
			
	// rounds Value up or down to nearest ten
	int NewValue;
  
	NewValue = ((int)(Value / m_gridSpacing)) * m_gridSpacing;
	
	return NewValue;

}

void CObjectDCLView::InsertControl(CRect rcPos)
{
	// if the control to insert in not a valid control, exit here
	if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl < CtlLabel)
		return;

	InsertControl(rcPos, (ControlType)theEditorWorkspace.GetToolBox()->m_nSelectedCtrl);
}


void CObjectDCLView::InsertControl(CRect rcPos, ControlType nCtrlToInsert)
{

	// if the dcl form is not valid, exit here
	if (m_pThisDclForm == NULL)
		return;
	
	GetDocument()->SetModifiedFlag(TRUE);
	
	// get the next available name for the control
	CString sControlName;
	if (nCtrlToInsert == CtlFileDlgCtrl)
		sControlName = GetControlName(nCtrlToInsert);
	else
		sControlName = FindNextControlName(GetControlName(nCtrlToInsert));
	
	CDclControlObject *pDclControl = m_pThisDclForm->AddControl( nCtrlToInsert, sControlName );
	assert( pDclControl != NULL );

	// call the method to add the new control
	CWnd *pControl = AddCWndControl( pDclControl, rcPos, true );
	if (pControl == NULL)
	{
		m_pThisDclForm->DeleteControl( pDclControl ); //pDclControl is invalid after this call!
		UpdateZOrderList();
		return;
	}
	
	// add the event to the undo list
	CUndoActions *pUndo = new CUndoActions( uaInsert, pDclControl, pControl );
	m_UndoList.AddTail(pUndo);

	m_SelectedControl.m_pArxObject = pDclControl;
	m_SelectedControl.m_pControl = pControl;
	m_SelectedControl.m_nIndex = pDclControl->m_Index;
	pControl->GetWindowRect(&rcPos);
	ScreenToClient(rcPos);
	ShowGripRects(TRUE, rcPos);
	ClearSelection();

	// Fire the control was inserted event to let the parent know
	FireControlInserted(pDclControl, nCtrlToInsert);

	// call this method to hide the sizing controls
	// and move them to the top of the z-order
	HideSizingRect();
	MoveGripRectsForward();
	
	// add the new control to the Zorder list
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
	pZOrderList->m_pView = this;
	pZOrderList->ClearSelection();
	pZOrderList->AddControlToList(pDclControl->GetStrProperty(nName), pDclControl->GetType());
	pZOrderList->SelectItem(pDclControl->GetStrProperty(nName), true);
	// call the method to setup the fonts in the font tool bar.
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar(pDclControl);
}

int CObjectDCLView::GetNextControlId()
{
	int nHighest = nControlStartId;
	POSITION pos = m_pThisDclForm->GetControlList().GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject *pCtrl = m_pThisDclForm->GetControlList().GetNext(pos);
		if (pCtrl->m_Id > nHighest)
			nHighest = pCtrl->m_Id;
	}
	return nHighest + 1;
}

CWnd* CObjectDCLView::AddCWndControl( CDclControlObject* pDclControl, CRect rcPos, bool bForceUpdate )
{
	// add the properties to the project list class for the requested control
	if (pDclControl->m_Id >= 0)
		AddProperties( pDclControl );
	else
		return NULL;

	if (rcPos.Width() >= 0 &&
		rcPos.Height() > 0)
	{
		// set the position properties
		pDclControl->SetLongProperty(nLeft, rcPos.left);
		pDclControl->SetLongProperty(nTop, rcPos.top);
		pDclControl->SetLongProperty(nWidth, rcPos.Width());
		pDclControl->SetLongProperty(nHeight, rcPos.Height());
	}
	else
	{
		// set the position properties
		rcPos.left = pDclControl->GetLngProperty(nLeft);
		rcPos.top = pDclControl->GetLngProperty(nTop);
		rcPos.right = rcPos.left + pDclControl->GetLngProperty(nWidth);
		rcPos.bottom = rcPos.top + pDclControl->GetLngProperty(nHeight);
	}

	CRect rcThis;
	GetClientRect(&rcThis);
	
	// set the offset position properties
	CalcControlOffsetDistances(pDclControl, rcPos);
	
	// create a new control holder
	CControlHolder *pControl = new CControlHolder;
	
	if (pDclControl->m_Id == -1)
		m_ControlId = GetNextControlId();
	
	// create an ordinary control
	pControl->Create(WS_CHILD|WS_CLIPSIBLINGS, rcPos, this, m_ControlId);
	
	
	if (pDclControl->GetType() != CtlActiveX)
		pControl->SetFont(GetFont());
	pControl->ShowWindow(FALSE);

	pDclControl->m_pCtrlHolder = pControl;

	// set the member var's
	pControl->m_pLeftProp = pDclControl->GetPropertyObject(nLeft);
	pControl->m_pTopProp = pDclControl->GetPropertyObject(nTop);
	pControl->m_pWidthProp = pDclControl->GetPropertyObject(nWidth);
	pControl->m_pHeightProp = pDclControl->GetPropertyObject(nHeight);
	pControl->m_ControlType = pDclControl->GetType();
	
	if (IsWindow(pControl->m_hWnd))
	{
		// disable the control so the control won't react to mouse events.
		pControl->EnableWindow(FALSE);	
	}
	
	
	// call method to add the control
	if (!CreateChildControl(pControl, pDclControl, bForceUpdate))
	{
		delete pControl;
		return NULL;
	}

	// if the activeX control's name has not been set yet, we must do it here
	if (pDclControl->GetType() == CtlActiveX)
	{
		CString sName = pDclControl->GetStrProperty(nName);
		if (sName.IsEmpty())
			// lets get the correct name set here.
			pDclControl->SetStringProperty(nName,  FindNextControlName(pDclControl->GetActiveXTypeName()));		
	}
	
	if (IsWindow(pControl->m_hWnd))
		// set the new control to the top of the Z-order
		pControl->SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	
	if (pDclControl->m_Delete == TRUE)
	{
		// to delete first we are going to move it off the visible area
		CRect rc(nm100,nm100,nm99,nm99);
		pControl->MoveWindow(rc, TRUE);

		// now we have to ensure it was moved (it's an ActiveX problem to work around because some controls won't be moved)
		CRect rcNew;
		pControl->GetWindowRect(&rcNew);
		ScreenToClient(rcNew);

		// if the control was not moved then we hide it. Other wise we can't because some controls destroy themselves if they are hidden.
		if (rc != rcNew)
			pControl->ShowWindow(FALSE);
	}
	else
	{
		//if (nControlToInsert != CtlActiveX)
			pControl->ShowWindow(TRUE);
	}	

	return pControl;
}

int CObjectDCLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == nNotSet)
		return nNotSet;
	
	// this frame will recieve all mouse events and pass them directly to this CView for processing.
	CChildFrame *pFrame = (CChildFrame*)GetParentFrame();
	pFrame->m_pChildView = this;
	
	// get the font collection
	m_pFontCollection = &theWorkspace.GetFontCollection();

	ModifyStyle(NULL, WS_CLIPCHILDREN, NULL);
	
	CRect rcGrip(0,0,nGripSize,nGripSize);
	int nGridIDCounter = nDeGridIDCounter;
	
	// create all the grip rect and hide them for later selection use
	m_bGripsCreate = m_GripRect1.Create(rcGrip, this, nGridIDCounter);
	m_GripRect1.SetGripCursor(IDC_SIZENWSE);
	m_GripRect1.m_nQuadrant = 1;
	m_GripRect1.ShowWindow(FALSE);		
	nGridIDCounter++;
	
	m_GripRect2.Create(rcGrip, this, nGridIDCounter);
	m_GripRect2.SetGripCursor(IDC_SIZENS);
	m_GripRect2.m_nQuadrant = 2;
	m_GripRect2.ShowWindow(FALSE);
	nGridIDCounter++;
	
	m_GripRect3.Create(rcGrip, this, nGridIDCounter);
	m_GripRect3.SetGripCursor(IDC_SIZENESW);
	m_GripRect3.m_nQuadrant = 3;
	m_GripRect3.ShowWindow(FALSE);
	nGridIDCounter++;
	
	m_GripRect4.Create(rcGrip, this, nGridIDCounter);
	m_GripRect4.SetGripCursor(IDC_SIZEWE);
	m_GripRect4.m_nQuadrant = 4;
	m_GripRect4.ShowWindow(FALSE);
	nGridIDCounter++;
	
	m_GripRect5.Create(rcGrip, this, nGridIDCounter);
	m_GripRect5.SetGripCursor(IDC_SIZEWE);
	m_GripRect5.m_nQuadrant = 5;
	m_GripRect5.ShowWindow(FALSE);
	nGridIDCounter++;
	
	m_GripRect6.Create(rcGrip, this, nGridIDCounter);
	m_GripRect6.ShowWindow(FALSE);
	m_GripRect6.m_nQuadrant = 6;
    m_GripRect6.SetGripCursor(IDC_SIZENESW);
	nGridIDCounter++;
	
	m_GripRect7.Create(rcGrip, this, nGridIDCounter);
	m_GripRect7.SetGripCursor(IDC_SIZENS);
	m_GripRect7.m_nQuadrant = 7;
	m_GripRect7.ShowWindow(FALSE);
	nGridIDCounter++;
	
	m_GripRect8.Create(rcGrip, this, nGridIDCounter);
	m_GripRect8.SetGripCursor(IDC_SIZENWSE);
	m_GripRect8.m_nQuadrant = 8;
	m_GripRect8.ShowWindow(FALSE);

	
	return 0;
}

int CObjectDCLView::GetDclFormIndex() 
{
	POSITION pos;
	int nCount = 0;

	while (nCount < activeProject->GetDclFormList().GetCount())
	{
		pos = activeProject->GetDclFormList().FindIndex(nCount);
		if (pos != NULL)
		{
			CDclFormObject *m_pThisDclForm = activeProject->GetDclFormList().GetAt(pos);
			if (m_pThisDclForm->GetUniqueName() == m_DclFormName)
			{
				return nCount;
			}
		}
		nCount++;
	}
	return nNotSet;
}

void CObjectDCLView::OnDclFormNameChange() 
{
	GetDclFormIndex();

	GetDocument()->SetModifiedFlag(TRUE);
}

CString CObjectDCLView::FindNextControlName(CString sControlName) 
{
	
	int nNameCounter = 1;
	
	// we are going to do a loop to find the next available control number 'TextBox#'
	for (int i=1; i<m_pThisDclForm->GetControlList().GetCount(); i++)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			CString sName = pArxObject->GetStrProperty(nName);
			
			// if this name is already take, reset loop search for next available control name
			if (sName == (sControlName + LTOA(nNameCounter)))
			{
				// reset the counter to the begining
				i = 1;
				// increment the control number counter
				nNameCounter++;
			}
		}
	}
	// set the return name to the free one found
	sControlName = sControlName + LTOA(nNameCounter);
	
	// to do later.
	return sControlName;
}


void CObjectDCLView::OnDestroy() 
{
	if (m_pThisDclForm != NULL)
		m_pThisDclForm->m_pChildWnd = NULL;
	try
	{
		PreDestroy();
		CView::OnDestroy();
	}
	catch(...)
	{
	}
}

void CObjectDCLView::PreDestroy() 
{
	ClearControls();
	
	CProjectTreeCtrl *pProjectTree = theEditorWorkspace.GetProjectTreeCtrl();
	pProjectTree->RemoveViewPointer(this);
	pProjectTree->SetDocument(NULL);

	// when the view is closed we need to ensure the property list and zorder panes are updated correctly
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
	
	// check if the zorder tab it listing this CView
	if (pZOrderList->m_pView == this)
	{
		// clear the list
		pZOrderList->ClearList(NULL);
		// reset the properties tab pan
		theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
	}
	m_pThisDclForm = NULL;
}

void CObjectDCLView::ClearControls() 
{
	ClearSelection();
	if (m_pThisDclForm == NULL)
		return;

	if (m_pThisDclForm->GetType() < nNotSet || m_pThisDclForm->GetType() > 5)
		return;

	POSITION pos = m_pThisDclForm->GetControlList().GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject *pCtrl = m_pThisDclForm->GetControlList().GetNext(pos);
		CControlHolder *pCtrlHolder = (CControlHolder*)pCtrl->m_pCtrlHolder;
		if (pCtrlHolder != NULL)
		{
			ClearChildControls(pCtrlHolder);
			try{
				pCtrlHolder->DestroyWindow();
				delete pCtrlHolder;			
			}
			catch(...)
			{}
			pCtrl->m_pCtrlHolder = NULL;
		}
	}
	m_pThisDclForm = NULL;
}

void CObjectDCLView::ClearChildControls(CControlHolder *pParentControl) 
{
	for (int i=nControlStartId; i<=pParentControl->m_ControlId; i++)
	{
		CWnd *pControl = pParentControl->GetDlgItem(i);
		if( pControl )
		{
			if (pParentControl->m_ControlType == CtlActiveX)
				pParentControl->m_pArxObject->SaveToStream(pParentControl->GetActiveXCtrl());			
			pControl->DestroyWindow();			
			delete pControl;
		}
	}
	pParentControl->m_ControlId = nNotSet;
}

bool CObjectDCLView::CreateChildControl(CControlHolder *pParent, CDclControlObject *pDclControl, bool bForceUpdate)
{
	pParent->m_pArxObject = pDclControl;
	// create the appropriate control to display
	switch(pDclControl->GetType())
	{		
		case CtlLabel:
			CreateLabel(pParent, pDclControl);
			break;
		case CtlStdButton:
			CreateButton(pParent, pDclControl);
			break;
		case CtlGraphicButton:
			CreateGraphicButton(pParent, pDclControl);
			break;
		case CtlFrame:
			CreateFrame(pParent, pDclControl);
			break;
		case CtlTextBox:
			CreateTextBox(pParent, pDclControl);
			break;
		case CtlCheckBox:
			CreateCheckBox(pParent, pDclControl);
			break;
		case CtlOptionButton:
			CreateOptionButton(pParent, pDclControl);
			break;
		case CtlComboBox:
			CreateComboBox(pParent, pDclControl);
			break;
		case CtlListBox:
			CreateListBox(pParent, pDclControl);
			break;
		case CtlScrollBar:
			CreateScrollBar(pParent, pDclControl);
			break;
		case CtlSlider:
			CreateSlider(pParent, pDclControl);
			break;
		case CtlPictureBox:
			CreatePictureBox(pParent, pDclControl);
			break;
		case CtlTabStrip:
			CreateTabStrip(pParent, pDclControl);
			break;
		case CtlMonth:
			CreateMonthCal(pParent, pDclControl);
			break;
		case CtlImageComboBox:
			CreateImageComboBox(pParent, pDclControl);
			break;
		case CtlAnimate:
			CreateAnimate(pParent, pDclControl);			
			break;		
		case Ctl3DRect:
			Create3DRect(pParent, pDclControl);
			break;
		case CtlProgress:
			CreateProgress(pParent, pDclControl);
			break;
		case CtlSpinButton:
			CreateSpinButton(pParent, pDclControl);
			break;
		case CtlStaticURL:
			CreateStaticURL(pParent, pDclControl);
			break;
		case CtlRoundSlider:
			CreateRoundSlider(pParent, pDclControl);
			break;			
		case CtlBlockView:
			CreateBlockView(pParent, pDclControl);
			break;			
		case CtlSlideView:
			CreateSlideView(pParent, pDclControl);
			break;			
		case CtlTree:
			CreateTree(pParent, pDclControl);
			break;
		case CtlHtmlCtrl:
			CreateHtmlCtrl(pParent, pDclControl);
			break;
		case CtlDwgPreview:
			CreateDwgPreview(pParent, pDclControl);
			break;	
		case CtlGrid:
			CreateGrid(pParent, pDclControl);
			break;	
		case CtlListView:
			CreateListView(pParent, pDclControl);
			break;	
		case CtlBlockList:
			CreateBlockList(pParent, pDclControl);
			break;	
		case CtlOptionList:
			CreateOptionList(pParent, pDclControl);			
			break;
		case CtlActiveX:
			{
				bool bCreated;
				bCreated = CreateActiveX(pParent, pDclControl, bForceUpdate);			
				if (!bCreated)
					return bCreated;
				break;
			}
		case CtlDwgList:
			CreateDwgDirList(pParent, pDclControl);			
			break;
		case CtlSplitter:
			CreateSplitter(pParent, pDclControl);
			break;
		case CtlHatch:
			CreateHatch(pParent, pDclControl);
			break;			
		case CtlFileDlgCtrl:
			CreateFileDlgCtrls(pParent, pDclControl);			
			break;
	}
	
 	UpdateChildControl(pDclControl, pParent);
	// check to see if autosize should resize the control
	CheckAutoSizeProp(pDclControl, pParent);
	
	return true;
	
}


void CObjectDCLView::CreateFileDlgCtrls(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height()+5);
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
	
	CFileDlgCtrls *pControl = new CFileDlgCtrls;

	m_ControlCreated = pControl->Create(
		CString(),
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());
}


bool CObjectDCLView::CreateActiveX(CControlHolder *pParent, CDclControlObject *pArxObject, bool bForceUpdate)
{
	CString sMsg;
	CString sTitle;
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	
	CAxContainer *pControl = new CAxContainer(pArxObject->GetOwnerForm());

	// if the activeX control is being inserted by the user
	if (pArxObject->m_clsid.Data1 == 0 && 
		pArxObject->m_clsid.Data2 == 0 && 
		pArxObject->m_clsid.Data3 == 0)		
	{	
		if (m_clsid.Data1 == 0 && 
			m_clsid.Data2 == 0 && 
			m_clsid.Data3 == 0)	
		{
			pArxObject->m_Delete = true;
			return false;
		}
		pArxObject->m_sLicenseKey = m_sLicenseKey;
		// create this way if new
		if (!pControl->CreateCtrl(m_clsid, pArxObject, rc, pParent->GetId(), pParent, true))
		{
			sMsg = theWorkspace.LoadResourceString(IDS_BADACTIVEX);
			sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
			delete pControl;
			MessageBox(sMsg, sTitle, MB_ICONEXCLAMATION);
			return false;		
		}
		
		if (!IsWindow(pControl->m_hWnd) && pArxObject->m_sLicenseKey.IsEmpty())
		{
			sMsg = theWorkspace.LoadResourceString(IDS_BADACTIVEX);
			sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
			delete pControl;
			MessageBox(sMsg, sTitle, MB_ICONEXCLAMATION);
			return false;		
		}
	}
	else // if the activeX control is being inserted from memory
	{
		// create this way if not new
		if (!pControl->CreateCtrl(pArxObject->m_clsid, pArxObject, rc, pParent->GetId(), pParent, false))
		{
			sMsg = theWorkspace.LoadResourceString(IDS_BADACTIVEX);
			sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
			delete pControl;
			MessageBox(sMsg, sTitle, MB_ICONEXCLAMATION);
			return false;		
		}
		if (!IsWindow(pControl->m_hWnd))
		{
			sMsg = theWorkspace.LoadResourceString(IDS_BADACTIVEX);
			sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
			delete pControl;
			MessageBox(sMsg, sTitle, MB_ICONEXCLAMATION);
			return false;		
		}
	}

	// lets check to see if the control is the right size
	CRect rcAx;
	pControl->GetWindowRect(&rcAx);
	ScreenToClient(rcAx);
	if (rc.Width() != rcAx.Width() && 
		rc.Height() != rcAx.Height())
	{
		pParent->GetWindowRect(&rc);
		ScreenToClient(rc);
		rc.right = rc.left + rcAx.Width();
		rc.bottom = rc.top + rcAx.Height();
		pParent->MoveWindow(rc);
	}

	// lets get the correct name set here.
	if (bForceUpdate)
	{
		pArxObject->SetStringProperty(nName, FindNextControlName(pArxObject->GetActiveXTypeName()));		
		pArxObject->ForceUpdateGlobalVariable(m_pThisDclForm->GetKeyName());
	}
	return true;
}


void CObjectDCLView::CreateOptionList(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | LBS_OWNERDRAWVARIABLE
		| LBS_HASSTRINGS | LBS_NOTIFY |WS_CLIPSIBLINGS;
	
	if (pArxObject->GetBoolProperty(nNoIntegralHeight) == TRUE)
		dwStyle = dwStyle | LBS_NOINTEGRALHEIGHT;
	
	if (pArxObject->GetBoolProperty(nVScrollBar) == TRUE)
		dwStyle = dwStyle | WS_VSCROLL;	
	

	COptionListBox *pControl = new COptionListBox;

	pControl->m_RowHeight = (short)pArxObject->GetLngProperty(nRowHeight);

	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());
}




void CObjectDCLView::CreateSpinButton(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS |WS_CLIPSIBLINGS ;
	
	if (rcThis.Width() >= rcThis.Height())
	{
		pArxObject->SetLongProperty(nOrientation, 0);
		dwStyle = dwStyle | UDS_HORZ;
	}
	else
		pArxObject->SetLongProperty(nOrientation, 1);

	CSpinButtonCtrl *pControl = new CSpinButtonCtrl;
	m_ControlCreated = pControl->Create( dwStyle, rc, pParent, pParent->GetId() );
	
	pControl->MoveWindow(rc, TRUE);
	pControl->ShowWindow(TRUE);
}


void CObjectDCLView::CreateStaticURL(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	CStaticLink *pControl = new CStaticLink;
	pControl->m_pControl = pArxObject;
	m_ControlCreated = pControl->Create( CString(), rc, pParent, pParent->GetId());
}

void CObjectDCLView::CreateProgress(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS;

	if (pArxObject->GetBoolProperty(nSmoothProgress) == TRUE)
		dwStyle = dwStyle | PBS_SMOOTH;

	if (pArxObject->GetLngProperty(nOrientation) == 1)
		dwStyle = dwStyle | PBS_VERTICAL ;

	

	CProgressCtrl *pControl = new CProgressCtrl;
	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());		

	
}

void CObjectDCLView::Create3DRect(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	C3DRect *pControl = new C3DRect;
	m_ControlCreated = pControl->Create(
		0, 
		rc, 
		pParent,
		pParent->GetId());
}


void CObjectDCLView::CreateSplitter(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	if (rcThis.Width() >= rcThis.Height())
	{
		rc.bottom = rc.top + 6;
	}
	else
	{
		rc.right = rc.left + 6;
	}
	
	CSplitter *pControl = new CSplitter;

	pControl->m_nStyle = Splitter_DoubleRaised;
	
	m_ControlCreated = pControl->Create(
		0, 
		rc, 
		pParent,
		pParent->GetId());
}
void CObjectDCLView::CreateImageComboBox(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	CComboBoxEx *pControl = new CComboBoxEx;
	
	DWORD dwStyle = WS_CHILD|WS_VISIBLE;

	switch (pArxObject->GetLngProperty(nComboBoxStyle))
	{
		case 0:
		{
			m_ControlCreated = pControl->Create(
				dwStyle | CBS_DROPDOWN,
				rc, 
				pParent,
				pParent->GetId());
			pControl->SetWindowText(pArxObject->GetStrProperty(nText));
			
			break;
		}
		case 1:
		{
			m_ControlCreated = pControl->Create(
				dwStyle | CBS_SIMPLE,
				rc, 
				pParent,
				pParent->GetId());
			pControl->SetWindowText(pArxObject->GetStrProperty(nText));
			break;
		}
		case 2:
		{
			m_ControlCreated = pControl->Create(
				dwStyle | CBS_DROPDOWNLIST,
				rc, 
				pParent,
				pParent->GetId());
			
			CString sText = pArxObject->GetStrProperty(nText);
			CString sName = pArxObject->GetStrProperty(nText);
			if (sName != sText)
			{
				pControl->SetWindowText(sName);
				pArxObject->SetStringProperty(nText, sName);
			}
			break;
		}	
	}	
	
}


void CObjectDCLView::CreateAnimate(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	CPictureBox *pControl = new CPictureBox;
	m_ControlCreated = pControl->Create(
		WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());

	pControl->SetIcon(IDI_ANIMATE);	
	pControl->SetAcadColor(nButtonFace);
}

void CObjectDCLView::CreateTree(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | 
		TVS_DISABLEDRAGDROP | TVS_INFOTIP | WS_TABSTOP  |WS_CLIPSIBLINGS;

	if (pArxObject->GetBoolProperty(nShowSelectAlways))
		dwStyle = dwStyle | TVS_SHOWSELALWAYS;
	if (pArxObject->GetBoolProperty(nHasLines))
		dwStyle = dwStyle | TVS_HASLINES;
	if (pArxObject->GetBoolProperty(nLinesAtRoot))
		dwStyle = dwStyle | TVS_LINESATROOT;
	if (pArxObject->GetBoolProperty(nHasButtons))
		dwStyle = dwStyle | TVS_HASBUTTONS;
	if (pArxObject->GetBoolProperty(nEditLabels))
		dwStyle = dwStyle | TVS_EDITLABELS;
	if (pArxObject->GetBoolProperty(nCheckBoxes))
		dwStyle = dwStyle | TVS_CHECKBOXES;

	
	CTreeCtrl *pControl = new CTreeCtrl;
	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());
	
	pControl->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);	
		
	SetupTreeControl(pControl);
}

void CObjectDCLView::SetupTreeControl(CTreeCtrl *pControl)
{
	pControl->DeleteAllItems();

	HTREEITEM   m_rghItem[4];

	// load the sample node text
	CString strNodeText;	
	strNodeText = theWorkspace.LoadResourceString(IDS_SAMPLENODE);


	// create first parrent sample node
	m_rghItem[0] = pControl->InsertItem(
		TVIF_TEXT, 
		strNodeText.GetBuffer(strNodeText.GetLength()),
		0,
		0,
		TVIS_EXPANDEDONCE,
		TVIF_TEXT,
		0,
		NULL,
		0);

	// create first child node
	m_rghItem[1] = pControl->InsertItem(
		TVIF_TEXT, 
		strNodeText.GetBuffer(strNodeText.GetLength()),
		0,
		0,
		TVIS_EXPANDEDONCE ,
		TVIF_TEXT,
		0,
		m_rghItem[0],
		0);

	// create second child node
	m_rghItem[2] = pControl->InsertItem(
		TVIF_TEXT, 
		strNodeText.GetBuffer(strNodeText.GetLength()),
		0,
		0,
		0,
		TVIF_TEXT,
		0,
		m_rghItem[0],
		0);
	
	// create second parrent sample node
	m_rghItem[3] = pControl->InsertItem(
		TVIF_TEXT, 
		strNodeText.GetBuffer(strNodeText.GetLength()),
		0,
		0,
		0,
		TVIF_TEXT,
		0,
		NULL,
		0);

	pControl->Expand(m_rghItem[0], TVE_EXPAND);
}

void CObjectDCLView::CreateTabStrip(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | TCS_FOCUSNEVER |WS_GROUP |WS_CLIPSIBLINGS;
	
	if (pArxObject->GetBoolProperty(nMultiRow))
		dwStyle = dwStyle | TCS_MULTILINE;
	else
		dwStyle = dwStyle | TCS_SINGLELINE;
	
	CTabCtrl *pControl = new CTabCtrl;
	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());
}

void CObjectDCLView::CreatePictureBox(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	CPictureBox *pControl = new CPictureBox;
	m_ControlCreated = pControl->Create(
		WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());

	pControl->m_AutoSize = FALSE;
		
}


void CObjectDCLView::CreateRoundSlider(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS;
	
	//if (pArxObject->GetBoolProperty(nShowTicks) == TRUE)
		//dwStyle = dwStyle;


	CRoundSliderCtrl *pControl = new CRoundSliderCtrl;
	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());

	pControl->SetRange(nDeRoundRangeMin, nDeRoundRangeMax, FALSE);
	pControl->SetZero(nDeZeroAngle);
	pControl->SetInverted();
		
}

void CObjectDCLView::CreateSlider(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |  TBS_BOTTOM |WS_CLIPSIBLINGS;
	
	if (rcThis.Width() >= rcThis.Height())
	{
		pArxObject->SetLongProperty(nOrientation, 0);
		dwStyle = dwStyle | TBS_HORZ;
	}
	else
	{
		pArxObject->SetLongProperty(nOrientation, 1);
		dwStyle = dwStyle | TBS_VERT;
	}
	

	if (pArxObject->GetBoolProperty(nShowTicks) == TRUE)
		dwStyle = dwStyle | TBS_AUTOTICKS;


	CSliderCtrl *pControl = new CSliderCtrl;
	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());

	pControl->SetRange(1, 10, TRUE);
	pControl->SetPos(5);
		
}


void CObjectDCLView::CreateScrollBar(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS;
	
	if (rcThis.Width() >= rcThis.Height())
	{
		pArxObject->SetLongProperty(nOrientation, 0);
		dwStyle = dwStyle | SBS_HORZ;
	}
	else
	{
		pArxObject->SetLongProperty(nOrientation, 1);
		dwStyle = dwStyle | SBS_VERT;
	}
	
	CScrollBar *pControl = new CScrollBar;
	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());

	pControl->ShowScrollBar(TRUE);
		
}



void CObjectDCLView::CreateListBox(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE
		| LBS_HASSTRINGS | LBS_NOTIFY |WS_CLIPSIBLINGS;
	
	if (pArxObject->GetBoolProperty(nNoIntegralHeight) == TRUE)
		dwStyle = dwStyle | LBS_NOINTEGRALHEIGHT;
	
	if (pArxObject->GetBoolProperty(nMultiColumn) == TRUE)
		dwStyle = dwStyle | LBS_MULTICOLUMN;
	
	if (pArxObject->GetBoolProperty(nSorted) == TRUE)
		dwStyle = dwStyle | LBS_SORT;		

	
	if (pArxObject->GetBoolProperty(nVScrollBar) == TRUE)
		dwStyle = dwStyle | WS_VSCROLL;	
	
	CAcadColorListBox *pControl = new CAcadColorListBox;
	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());
	
	
}


void CObjectDCLView::CreateDwgDirList(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE
		| LBS_HASSTRINGS | LBS_NOTIFY |WS_CLIPSIBLINGS;
	
	dwStyle = dwStyle | LBS_NOINTEGRALHEIGHT;
	
	if (pArxObject->GetBoolProperty(nMultiColumn) == TRUE)
		dwStyle = dwStyle | LBS_MULTICOLUMN;
	
	if (pArxObject->GetBoolProperty(nSorted) == TRUE)
		dwStyle = dwStyle | LBS_SORT;		

	dwStyle = dwStyle | WS_VSCROLL;	
	
	CAcadColorListBox *pControl = new CAcadColorListBox;
	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());
	
	
}
void CObjectDCLView::CreateComboBox(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	CRect rcAfter;			
	pParent->GetWindowRect(&rcThis);
	ScreenToClient(rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL 
		| CBS_HASSTRINGS | WS_GROUP | ES_AUTOHSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	
	//if (pArxObject->GetBoolProperty(nNoIntegralHeight) == TRUE)
		//dwStyle = dwStyle | CBS_NOINTEGRALHEIGHT;
	dwStyle = dwStyle | CBS_NOINTEGRALHEIGHT;

	if (pArxObject->GetBoolProperty(nSorted) == TRUE)
		dwStyle = dwStyle | CBS_SORT;

	
	if (pArxObject->GetLngProperty(nComboBoxStyle) == nComboStyle12)
		pArxObject->SetLongProperty(nDropDownHeight, nComboDropHeight);
		
	switch (pArxObject->GetLngProperty(nComboBoxStyle))
	{
		case 0:
		{
			CComboBox *pControl = new CComboBox;
			m_ControlCreated = pControl->Create(
				dwStyle | CBS_DROPDOWN,
				rc, 
				pParent,
				pParent->GetId());
			pControl->SetWindowText(pArxObject->GetStrProperty(nText));
			
			// get the size after insertion
			pControl->GetWindowRect(&rcAfter);			
			rcThis.bottom = rcThis.top + rcAfter.Height();
			break;
		}
		case 1:
		{
			CComboBox *pControl = new CComboBox;
			m_ControlCreated = pControl->Create(
				dwStyle | CBS_SIMPLE,
				rc, 
				pParent,
				pParent->GetId());
			pControl->SetWindowText(pArxObject->GetStrProperty(nText));
			
			// get the size after insertion
			pControl->GetWindowRect(&rcAfter);			
			rcThis.bottom = rcThis.top + rcAfter.Height();
			
			break;
		}
		case 2:
		{
			CComboBox *pControl = new CComboBox;
			m_ControlCreated = pControl->Create(
				dwStyle | CBS_DROPDOWNLIST,
				rc, 
				pParent,
				pParent->GetId());
			
			CString sText = pArxObject->GetStrProperty(nText);
			CString sName = pArxObject->GetStrProperty(nText);
			if (sName != sText)
			{
				pControl->SetWindowText(sName);
				pArxObject->SetStringProperty(nText, sName);
			}
			
			
			// get the size after insertion
			pControl->GetWindowRect(&rcAfter);			
			rcThis.bottom = rcThis.top + rcAfter.Height();
			break;
		}
		case 9:
		{
			CComboBox *pControl = new CComboBox;
			m_ControlCreated = pControl->Create(
				dwStyle | CBS_SIMPLE,
				rc, 
				pParent,
				pParent->GetId());
			
			CString sDesc;
			sDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_0 + 9);
			sDesc = sDesc.Mid(4);

			pArxObject->SetStringProperty(nText, sDesc);
			pControl->SetWindowText(sDesc);			
			
			// get the size after insertion
			pControl->GetWindowRect(&rcAfter);			
			rcThis.bottom = rcThis.top + rcAfter.Height();
			
			break;
		}
		default:
		{
			CComboBoxEx *pControl = new CComboBoxEx;
			m_ControlCreated = pControl->Create(
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | CBS_DROPDOWN,
				rc, 
				pParent,
				pParent->GetId());
			
			CString sDesc;
			int nThisValue = pArxObject->GetLngProperty(nComboBoxStyle);
			sDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_0 + nThisValue);
			if (nThisValue > 9)
				sDesc = sDesc.Mid(5);
			else
				sDesc = sDesc.Mid(4);

			pArxObject->SetStringProperty(nText, sDesc);
			pControl->SetWindowText(sDesc);
			
			// get the size after insertion
			pControl->GetWindowRect(&rcAfter);			
			rcThis.bottom = rcThis.top + rcAfter.Height();					
			break;
		}
	}

	pArxObject->SetLongProperty(nHeight, rcAfter.Height());
	//pParent->MoveWindow(rcThis, TRUE);
}

void CObjectDCLView::CreateOptionButton(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	CColorButton *pControl = new CColorButton;
	m_ControlCreated = pControl->Create(
		pArxObject->GetStrProperty(nCaption),
		WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());
	
	
}

void CObjectDCLView::CreateCheckBox(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());

	DWORD dwStyle = WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX|WS_CLIPSIBLINGS ;
	
	CColorButton *pControl = new CColorButton;
	m_ControlCreated = pControl->Create(
		pArxObject->GetStrProperty(nCaption),
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());
}

void CObjectDCLView::CreateTextBox(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|ES_WANTRETURN |WS_CLIPSIBLINGS; 

	if (pArxObject->GetBoolProperty(nHScrollBar) == TRUE)
		dwStyle = dwStyle | WS_HSCROLL;
	if (pArxObject->GetBoolProperty(nVScrollBar) == TRUE)
		dwStyle = dwStyle | WS_VSCROLL;	
	if (pArxObject->GetBoolProperty(nReadOnly) == TRUE)
		dwStyle = dwStyle | ES_READONLY;
	
	// adjust the dwStyle for filter settings
	switch (pArxObject->GetLngProperty(nFilterStyle))
	{			
	case 2: /* Integer */
		dwStyle = dwStyle | ES_NUMBER|ES_AUTOHSCROLL;
		break;	
	case 5:/*Upper case*/
		dwStyle = dwStyle | ES_UPPERCASE|ES_AUTOHSCROLL;
		break;
	case 6:/*lower case*/
		dwStyle = dwStyle | ES_LOWERCASE|ES_AUTOHSCROLL;
		break;
	case 7:/*password*/
		dwStyle = dwStyle | ES_PASSWORD|ES_AUTOHSCROLL;
		break;
	case 8:/*MultiLine*/			
		dwStyle = dwStyle | ES_MULTILINE;
		break;
	default:
		dwStyle = dwStyle | ES_AUTOHSCROLL;
		break;
	}

	

	// adjust the justification style				
	switch (pArxObject->GetLngProperty(nJustification))
	{
	case 0:/*Left*/
		dwStyle = dwStyle | ES_LEFT;
		break;
	case 1:/*Center*/
		dwStyle = dwStyle | ES_CENTER;
		break;
	case 2:/*Right*/
		dwStyle = dwStyle | ES_RIGHT;
		break;
	}



	CColorEdit *pControl = new CColorEdit;
	m_ControlCreated = pControl->Create(
		dwStyle,
		rc, 
		pParent,
		pParent->GetId());

	if (pArxObject->GetLngProperty(nFilterStyle) == 7)
		pControl->SetPasswordChar('*');
	else
		pControl->SetPasswordChar(0);
	
	pControl->SetWindowText(pArxObject->GetStrProperty(nText));

	// fix up 3D styles
	pControl->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
	
}
void CObjectDCLView::CreateFrame(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	VdclGroupBox *pControl = new VdclGroupBox;
	m_ControlCreated = pControl->Create(
		pArxObject->GetStrProperty(nCaption),
		rc, 
		pParent,
		pParent->GetId());

	
}

void CObjectDCLView::CreateGraphicButton(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	switch (pArxObject->GetLngProperty(nButtonStyle))
	{
	case 2:
	case 3:
	case 4:
		{
			CString sText;
			sText = theWorkspace.LoadResourceString(IDS_25);
	
			RefCountedPtr< CPropertyObject > pPropHeight = pArxObject->GetPropertyObject(nHeight);
			pPropHeight->SetStringValue(sText);

			sText = theWorkspace.LoadResourceString(IDS_26);
			RefCountedPtr< CPropertyObject > pPropWidth = pArxObject->GetPropertyObject(nWidth);
			pPropWidth->SetStringValue(sText);
			rc.bottom = rc.top + nBtnHeight;
			rc.right = rc.left + nBtnWidth;
			//pPropHeight = NULL;
			//pPropWidth = NULL;
		}
		break;
	}


	
	
	CGraphicButton *pControl = new CGraphicButton;
	
	m_ControlCreated = pControl->Create(
		pArxObject->GetLngProperty(nButtonStyle),
		WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());
	
	pControl->SetWindowText(pArxObject->GetStrProperty(nCaption));
	if (pArxObject->GetLngProperty(nButtonStyle) < 2)
	{
		int nPictureID = pArxObject->GetLngProperty(nPicture);
		if (nPictureID > 0)
			pControl->SetPictureID(nPictureID);
		else
			pControl->SetPictureBlank();
	}
}
			
void CObjectDCLView::CreateButton(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	CButton *pControl = new CButton;
	int nTextId = pParent->GetId();
	m_ControlCreated = pControl->Create(
		pArxObject->GetStrProperty(nCaption),
		WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|BS_MULTILINE,
		rc, 
		pParent,
		pParent->GetId());
	
}


void CObjectDCLView::CreateMonthCal(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	ScreenToClient(rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	
	CMonthCalCtrl *pControl = new CMonthCalCtrl;
	m_ControlCreated = pControl->Create(
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());
	
	pControl->SizeMinReq(TRUE);
	
}

void CObjectDCLView::CreateLabel(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	
	VdclStatic *pControl = new VdclStatic;
	m_ControlCreated = pControl->Create(
		pArxObject->GetStrProperty(nCaption),
		WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());
	
	
}

void CObjectDCLView::CreateHtmlCtrl(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	
	CPictureBox *pControl = new CPictureBox;
	m_ControlCreated = pControl->Create(
		WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());

	pControl->SetIcon(IDI_HTML);	
}


void CObjectDCLView::CreateListView(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	
	CPictureBox *pControl = new CPictureBox;
	m_ControlCreated = pControl->Create(
		WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());

	pControl->SetIcon(IDI_LISTVIEW);	

	
}

void CObjectDCLView::CreateGrid(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	

	CListCtrlEx *pControl = new CListCtrlEx;
	m_ControlCreated = pControl->Create(
		WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());
	
	pControl->SetIcon(IDI_GRID);
	
	
	
	
}
void CObjectDCLView::CreateBlockList(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	
	CPictureBox *pControl = new CPictureBox;
	m_ControlCreated = pControl->Create(
		WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());
	
	pControl->SetIcon(IDI_BLOCKLIST);	
	
	
}
void CObjectDCLView::CreateDwgPreview(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	
	CPictureBox *pControl = new CPictureBox;
	m_ControlCreated = pControl->Create(
		WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());

	pControl->SetIcon(IDI_DWGPREVIEW);
}

void CObjectDCLView::CreateHatch(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	
	CPictureBox *pControl = new CPictureBox;	
	m_ControlCreated = pControl->Create(
		WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());

	pControl->SetIcon(IDI_HATCH);

	
}

void CObjectDCLView::CreateBlockView(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	
	CPictureBox *pControl = new CPictureBox;	
	m_ControlCreated = pControl->Create(
		WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());

	pControl->SetIcon(IDI_BLOCKVIEW);

	
}


void CObjectDCLView::CreateSlideView(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	
	CPictureBox *pControl = new CPictureBox;
	m_ControlCreated = pControl->Create(
		WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());

	pControl->SetIcon(IDI_SLIDEVW);	
	
}
void CObjectDCLView::CreateDwgThumbNail(CControlHolder *pParent, CDclControlObject *pArxObject)
{
	CRect rcThis;
	pParent->GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	CStatic *pControl = new CStatic;
	m_ControlCreated = pControl->Create(
		CString(),
		WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS,
		rc, 
		pParent,
		pParent->GetId());
		
}

void CObjectDCLView::AddProperties( CDclControlObject *pDclControl )
{
	CString sTrue;
	CString sFalse;
	CString sList;
	CString sCustom;		

	sCustom = theWorkspace.LoadResourceString(IDS_CUSTOM);			
	sList = theWorkspace.LoadResourceString(IDS_LIST);			
	sTrue = theWorkspace.LoadResourceString(IDS_TRUE);
	sFalse = theWorkspace.LoadResourceString(IDS_FALSE);
		
	ControlType nType = pDclControl->GetType();

	CRect rcThis;
	GetWindowRect(&rcThis);
	
	// add the name property
	pDclControl->AddStringProperty( nName, PropString, pDclControl->GetKeyName() );

	// do not add the object browser to the list here if it's an ActiveX control
	// because the CControlHolder will add it for us there instead.
	if (nType != CtlActiveX)
		// add the nObjectBrowser property
		pDclControl->AddStringProperty( nObjectBrowser, PropActiveXMethods );

	CString sUnderscore;
	sUnderscore = theWorkspace.LoadResourceString(IDS_UNDERSCORE);

	// add the GlobalVarName property
	pDclControl->AddStringProperty( nGlobalVarName, PropString, pDclControl->GetKeyPath() );
	
	if (nType == CtlActiveX)
		// add the ActiveX property pages property (to display the button)
		pDclControl->AddStringProperty( nActiveXPropPages, PropActiveXPropPages );

	// add the Custom property (to display the button)
	pDclControl->AddStringProperty( nCustom, PropCustom );

	// add the nAsReadOnly property
	if (nType == CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( nAsReadOnly, PropBool, false );

	// add the nAutoHScroll property
	if (nType == CtlTextBox)
		pDclControl->AddBooleanProperty( nAutoHScroll, PropBool, true );

	// add the nRowHeader property
	if (nType == CtlGrid)
		pDclControl->AddBooleanProperty( nRowHeader, PropBool, true );

	// add the AutoSize property
	if (nType == CtlPictureBox)
		pDclControl->AddBooleanProperty( nAutoSize, PropBool, false );

	// add the nAutoVScroll property
	if (nType == CtlTextBox)
		pDclControl->AddBooleanProperty( nAutoVScroll, PropBool, true );

	// add the nAutoWrap property
	if (nType == CtlSpinButton)
		pDclControl->AddBooleanProperty( nAutoWrap, PropBool, false );

	if (nType == CtlHatch)
		pDclControl->AddStringProperty( nHatchScale, PropDouble, _T("5") );
		
	// add the nAcadColor property
	switch (nType)
	{
		case CtlHatch:
			pDclControl->AddLongProperty( nHatchScale, PropDouble, 5 );
			pDclControl->AddLongProperty( nAcadColor, PropLong, nModalSpace );
			pDclControl->AddLongProperty( nForeColor, PropLong, 7 );
			break;
		case CtlBlockView:
		case CtlDwgPreview:
			pDclControl->AddLongProperty( nAcadColor, PropLong, nModalSpace );
			break;
		case CtlPictureBox:
		case CtlListView:
		case CtlBlockList:
			pDclControl->AddLongProperty( nAcadColor, PropLong, nWindowColor );
			break;
		case CtlGrid:
			pDclControl->AddLongProperty( nAcadColor, PropLong, nWindowColor );
			pDclControl->AddLongProperty( nAlternateColor, PropLong, nWindowColor );
			pDclControl->AddLongProperty( nAlternateOrient, PropEnum, 0 );
			break;
			
		case CtlSlideView:
			pDclControl->AddLongProperty( nAcadColor, PropLong, nModalSpace );
			break;
		
		case CtlOptionList:
		case CtlLabel:
		case CtlGraphicButton:
		case CtlCheckBox:
		case CtlOptionButton:
			if (!IsVersionFree())
			{
				pDclControl->AddLongProperty( nForeColor, PropLong, nButtonText );
				pDclControl->AddLongProperty( nAcadColor, PropLong, nButtonFace );
			}
			break;

		case CtlTextBox:
		case CtlListBox:
		case CtlDwgList:
			if (!IsVersionFree())
			{
				pDclControl->AddLongProperty( nForeColor, PropLong, nButtonText );
				pDclControl->AddLongProperty( nAcadColor, PropLong, nWindowColor );
			}
			break;

		case CtlStaticURL:
			pDclControl->AddLongProperty( nForeColor, PropLong, nBlueColor );
			pDclControl->AddLongProperty( nAcadColor, PropLong, nButtonFace );
			break;		
	}
	
	
	// add the nBeginGroup property
	if (nType == CtlProgress)
	{
		pDclControl->AddStringProperty( nSecondText, PropString, _T("second remaining") );
		pDclControl->AddStringProperty( nSecondsText, PropString, _T("seconds remaining") );
		pDclControl->AddStringProperty( nMinuteText, PropString, _T("minute") );
		pDclControl->AddStringProperty( nMinutesText, PropString, _T("minutes") );
		pDclControl->AddBooleanProperty( nDisplaySeconds, PropString, false );
		pDclControl->AddBooleanProperty( nDisplayPercentage, PropString, true );
	}

	// add the nBeginGroup property
	if (nType == CtlOptionButton)
		pDclControl->AddBooleanProperty( nBeginGroup, PropBool, false );

	// add the nBlockName property
	if (nType == CtlBlockView)
		pDclControl->AddStringProperty( nBlockName, PropString );

	// add the nBorderStyle property
	switch (nType)
	{
		case CtlBlockView:
		case CtlHatch:
		case CtlSlideView:
		case Ctl3DRect:
		case CtlDwgPreview:
		case CtlProgress:
			pDclControl->AddLongProperty( nBorderStyle, PropEnum, 2 );
			break;		
		case CtlLabel:
		case CtlAnimate:
		case CtlOptionList:
			pDclControl->AddLongProperty( nBorderStyle, PropEnum, 0 );
			break;
		case CtlListBox:
		case CtlDwgList:
		case CtlPictureBox:
		case CtlTextBox:
		case CtlTree:		
		case CtlHtmlCtrl:	
		case CtlListView:
		case CtlGrid:
		case CtlBlockList:
			pDclControl->AddLongProperty( nBorderStyle, PropEnum, 1 );
			break;
		default:
			break;
	}
	if (nType != CtlFileDlgCtrl)
		// add top from bottom geometry management
		pDclControl->AddLongProperty( nBottomFromBottom, PropLong, 0 );
	
	// add the nToolBarButtonsCaption property
	if (nType == CtlOptionList)
	{
		RefCountedPtr< CPropertyObject > pProp = pDclControl->AddStringProperty( nBtnCaption, PropStringArray, sList );

		// this control must have at least two options, but here we are adding three default option values
		// so the user can see an example of what we are talking about.
		if (pProp->GetStringArrayPtr()->empty())
		{
			pProp->GetStringArrayPtr()->push_back(theWorkspace.LoadResourceString(IDS_OPTION1));
			pProp->GetStringArrayPtr()->push_back(theWorkspace.LoadResourceString(IDS_OPTION2));
			pProp->GetStringArrayPtr()->push_back(theWorkspace.LoadResourceString(IDS_OPTION3));
		}
		pDclControl->AddStringProperty( nBtnTTText, PropStringArray, sList );
	}
	

	// add the nCaption property
	switch (nType)
	{
		case CtlLabel:
		case CtlFrame:
		case CtlStdButton:
		case CtlCheckBox:
		case CtlOptionButton:
		case CtlStaticURL:
			pDclControl->AddStringProperty( nCaption, PropString, pDclControl->GetKeyName() );
			break;
		case CtlGraphicButton:
			pDclControl->AddStringProperty( nCaption, PropString );
			break;
		default:
			break;
	}

	// add the nColHeader property
	switch (nType)
	{
		case CtlListView:		
		case CtlGrid:
			pDclControl->AddBooleanProperty( nColHeader, PropBool, true );
			break;
	}	

	// add the nColumnPropWidth property
	if (nType == CtlListBox)
		pDclControl->AddLongProperty( nColumnWidth, PropLong, nDeColWidth );

	// add the nCreationPrompt property
	if (nType == CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( nCreationPrompt, PropBool, true );

	// add the nDefSelIndex property
	if (nType == CtlOptionList)
		pDclControl->AddLongProperty( nDefSelIndex, PropLong, 0 );
	
	// add the nDisableNoScroll property
	if (nType == CtlListBox || nType == CtlDwgList)
		pDclControl->AddBooleanProperty( nDisableNoScroll, PropBool, false );
	
	if (!IsVersionFree())
	{
		// add the DragnDrop properties
		switch (nType)		
		{	
		case CtlBlockList:
			pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, true );
			break;
		case CtlDwgList:
			if (nCurrentPurchaseMode != nPurchasedR14Pro)
			{
				pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, true );
				pDclControl->AddLongProperty( nInsertOrXref, PropEnum, 0 );
			}
			break;
			
		case CtlListView:	
		case CtlLabel:	
		case CtlStdButton:	
		case CtlGraphicButton:	
		case CtlPictureBox:	
		case CtlTree:	
		case CtlBlockView:	
		case CtlHatch:
		case CtlSlideView:	
		case CtlDwgPreview:	
		case CtlListBox:
			pDclControl->AddBooleanProperty( nDragnDropAllowBegin, PropBool, false );
			pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
			break;				
		case CtlTextBox:	
			pDclControl->AddBooleanProperty( nDragnDropAllowDrop, PropBool, true );
			break;
		}
	}
	
		
	// add the nDropDownPropHeight property
	if (nType == CtlComboBox || nType == CtlImageComboBox)	
		pDclControl->AddLongProperty( nDropDownHeight, PropLong, nDeDropHeight );

	// add the nEditLabels property
	switch (nType)		
	{	
	case CtlTree:
		pDclControl->AddBooleanProperty( nEditLabels, PropBool, false );
	case CtlListView:
		pDclControl->AddBooleanProperty( nEditLabels, PropBool, true );
		break;	
	}

	if (nType != CtlActiveX && nType != CtlFileDlgCtrl)
		// add the Enabled property
		pDclControl->AddBooleanProperty( nEnabled, PropBool, true );

	// add the nEventInvoke property
	if (nCurrentPurchaseMode != nPurchasedR14Pro && m_pThisDclForm != NULL)
	{
		CDclFormObject* pDclForm = m_pThisDclForm;
		
		if (m_pThisDclForm->GetType() == VdclModal)
		{} // do not add the property.
		else if (nType == CtlTextBox)
			pDclControl->AddLongProperty( nEventInvoke, PropEnum, 0 );
		else if (m_pThisDclForm->GetType() == VdclDockable || m_pThisDclForm->GetType() == VdclModeless)
			pDclControl->AddLongProperty( nEventInvoke, PropEnum, 1 );
		else
			pDclControl->AddLongProperty( nEventInvoke, PropEnum, 0 );
	}
	
	// add the nExtCanBeDiff property
	if (nType == CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( nExtCanBeDiff, PropBool, false );
	
	// add the nFileMustExist property
	if (nType == CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( nFileMustExist, PropBool, true );
	
	// add the nFilter property
	if (nType == CtlFileDlgCtrl)
		pDclControl->AddStringProperty( nFilter, PropString, theWorkspace.LoadResourceString(IDS_FILEDLGFILTERDEF) );
	
	// add the nFilterStyle property
	if (nType == CtlTextBox)
		pDclControl->AddLongProperty( nFilterStyle, PropEnum, 0 );

	// add the Font properties
	switch (nType)		
	{	
	case CtlListView:
	case CtlGrid:
	case CtlBlockList:
	case CtlLabel:
	case CtlStdButton:
	case CtlGraphicButton:
	case CtlFrame:
	case CtlTextBox:
	case CtlCheckBox:
	case CtlOptionButton:
	case CtlComboBox:
	case CtlImageComboBox:
	case CtlListBox:
	case CtlDwgList:
	case CtlPictureBox:
	case CtlTabStrip:
	case CtlMonth:
	case CtlTree:
	case CtlStaticURL:
	case CtlRoundSlider:		
	case CtlOptionList:
		pDclControl->AddStringProperty( nLabelName, PropString, pDclControl->GetOwnerProject()->m_sDefaultFontName );
		break;
	}

	// add the nGridLines & nFullRowSelect properties
	if (nType == CtlListView)
	{
		pDclControl->AddBooleanProperty( nFullRowSelect, PropBool, false );
		pDclControl->AddBooleanProperty( nGridLines, PropBool, false );
	}

	if (nType == CtlGrid)
		pDclControl->AddBooleanProperty( nGridLines, PropBool, true );

	// add the nHasButtons property
	if (nType == CtlTree)
		pDclControl->AddBooleanProperty( nHasButtons, PropBool, true );

	// add the nHasLines property
	if (nType == CtlTree)
		pDclControl->AddBooleanProperty( nHasLines, PropBool, true );

	// add the nPropHeight property
	pDclControl->AddLongProperty( nHeight, PropLong, rcThis.Height() );

	// add the nHScrollBar property
	switch (nType)
	{
	case CtlTextBox:	
		pDclControl->AddBooleanProperty( nHScrollBar, PropBool, false );
		break;
	}

	// add the nIconXSpacing property
	switch (nType)
	{
	case CtlListView:	
	case CtlBlockList:	
		pDclControl->AddLongProperty( nIconXSpacing, PropLong, nDeIconSpacing );
		pDclControl->AddLongProperty( nIconYSpacing, PropLong, nDeIconHeight );
		break;
	}

	
	// add the nLabelWrap property
	switch (nType)
	{
	case CtlListView:	
	case CtlGrid:	
	case CtlBlockList:	
		pDclControl->AddBooleanProperty( nLabelWrap, PropBool, true );
		break;
	}
	// add the nListViewIconAlign property
	switch (nType)
	{
	case CtlListView:	
	case CtlBlockList:	
		pDclControl->AddLongProperty( nListViewIconAlign, PropEnum, 0 );
		break;
	}
	// add the nAutoArrange property
	switch (nType)
	{
	case CtlListView:	
		pDclControl->AddBooleanProperty( nAutoArrange, PropBool, false );
		break;
	case CtlBlockList:	
		pDclControl->AddBooleanProperty( nAutoArrange, PropBool, true );
		break;
	}
	
	
	// add the nImageList property
	switch (nType)
	{
	case CtlTree:
	case CtlListView:
	case CtlGrid:
	case CtlImageComboBox:
		pDclControl->AddStringProperty( nImageList, PropImageList, theWorkspace.LoadResourceString(IDS_IMAGELIST) );
		break;
	}

	// add the nIndent property
	if (nType == CtlTree)
		pDclControl->AddLongProperty( nIndent, PropLong, nDeTreeIndent );
	
	// add the nInterfaceMode property
	if (nType == CtlBlockView)
		pDclControl->AddLongProperty( nAllowOrbiting, PropEnum, 1 );

	// add the nIsTabStop property
	switch (nType)
	{
	case CtlStdButton:
	case CtlGraphicButton:
	case CtlTextBox:
	case CtlCheckBox:
	case CtlOptionButton:
	case CtlComboBox:
	case CtlListBox:
	case CtlDwgList:
	case CtlListView:
	case CtlGrid:
	case CtlBlockList:
	case CtlScrollBar:
	case CtlSlider:
	case CtlPictureBox:
	case CtlMonth:
	case CtlTree:
	case CtlSpinButton:
	case CtlStaticURL:
	case CtlRoundSlider:
	case CtlHtmlCtrl:
	case CtlDwgPreview:
	case CtlImageComboBox:
	case CtlSlideView:
	case CtlBlockView:			
	case CtlHatch:
	case CtlOptionList:		
	case CtlActiveX:
		pDclControl->AddBooleanProperty( nIsTabStop, PropBool, true );
		break;
	}
	

	// add the nItemData property
	switch (nType)
	{
	case CtlListBox:
	case CtlComboBox:
		pDclControl->AddStringProperty( nItemData, PropIntArray, sList );
		break;			
	}
	
	// add the nJustification property
	switch (nType)
	{
	case CtlTextBox:
	case CtlLabel:	
		pDclControl->AddLongProperty( nJustification, PropEnum, 0 );
		break;
	}

	// add the nTabLabelAlign property
	//if (nType == CtlTabStrip)
	//	pDclControl->AddLongProperty( nTabLabelAlign, PropEnum, 1 );

	// add the nLargeChange property
	switch (nType)
	{
	case CtlSlider:
	case CtlScrollBar:
		pDclControl->AddLongProperty( nLargeChange, PropLong, nDeLargeChange );
		break;
	}

	// add the nLeft property
	pDclControl->AddLongProperty( nLeft, PropLong, 0 );
	
	// add top from bottom geometry management
	if (nType != CtlFileDlgCtrl)
		pDclControl->AddLongProperty( nLeftFromRight, PropLong, 0 );

	// add the nLimitText property
	switch (nType)
	{
	case CtlTextBox:
		pDclControl->AddLongProperty( nLimitText, PropLong, nDeTextLimit );
		break;
	case CtlComboBox:
	case CtlImageComboBox:
		pDclControl->AddLongProperty( nLimitText, PropLong, nDeTextLimitCB );
		break;
	}

	// add the nLinesAtRoot property
	if (nType == CtlTree)
		pDclControl->AddBooleanProperty( nLinesAtRoot, PropBool, true );

	// add the nURLLinkType property
	if (nType == CtlStaticURL)
		pDclControl->AddLongProperty( nURLLinkType, PropEnum, 0 );

	// add the List property
	switch (nType)
	{
	case CtlListBox:
	case CtlComboBox:
		pDclControl->AddStringProperty( nList, PropStringArray, sList );
		break;			
	}
	
	// add the nMarginLeft & nMarginRight property
	if (nType == CtlTextBox)
	{
		pDclControl->AddLongProperty( nMarginLeft, PropLong, 0 );
		pDclControl->AddLongProperty( nMarginRight, PropLong, 0 );
	}
	
	// add the MaxValue property
	switch (nType)
	{
	case CtlSlider:
	case CtlScrollBar:
	case CtlProgress:
	case CtlSpinButton:
		pDclControl->AddLongProperty( nMaxValue, PropLong, nDeMaxVal );
		break;
	}
	
	// add the nMinTabWidth property
	if (nType == CtlTabStrip)
		pDclControl->AddLongProperty( nMinTabWidth, PropLong, nDeMinTabWidth );

	// add the MinValue property
	switch (nType)
	{
	case CtlSlider:
	case CtlScrollBar:
	case CtlProgress:
	case CtlSpinButton:
		pDclControl->AddLongProperty( nMinValue, PropLong, 1 );
		break;
	}
	
	// add the nMultiColumn property
	if (nType == CtlListBox)
		pDclControl->AddBooleanProperty( nMultiColumn, PropBool, false );
	
	// add the MultiRow property
	if (nType == CtlTabStrip)
		pDclControl->AddBooleanProperty( nMultiRow, PropBool, false );

	// add the nMultiSelection property
	switch (nType)
	{
	case CtlMonth:
		pDclControl->AddLongProperty( nMultiSelection, PropLong, 1 );
		break;
	case CtlListView:
	case CtlBlockList:
		pDclControl->AddBooleanProperty( nMultiSelect, PropBool, false );
		break;
	case CtlFileDlgCtrl:
		pDclControl->AddBooleanProperty( nMultiSelect, PropBool, false );
		break;
	}
	
	// add the nFileMustExist property
	if (nType == CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( nFileMustExist, PropBool, true );

	// add the nNoIntegralHeight property
	switch (nType)
	{
	case CtlListBox:
	case CtlOptionList:
		pDclControl->AddBooleanProperty( nNoIntegralHeight, PropBool, true );
		break;
	}

	// add the nOrientation property
	switch (nType)
	{
	case CtlSpinButton:			
	case CtlProgress:	
	case CtlSlider:
	case CtlScrollBar:	
		pDclControl->AddLongProperty( nOrientation, PropEnum, (rcThis.Width() >= rcThis.Height())? 0 : 1 );
		break;
	}

	// add the nOverWritePrompt property
	if (nType == CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( nOverWritePrompt, PropBool, true );

	// add the nPathMustExist property
	if (nType == CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( nPathMustExist, PropBool, true );

	// add the nPicture property
	switch (nType)
	{
	case CtlGraphicButton:
	case CtlPictureBox:
		pDclControl->AddStringProperty( nPicture, PropPicture, theWorkspace.LoadResourceString(IDS_NONE) );
		break;
	}
	// add the nPressedPicture property
	switch (nType)
	{
	case CtlGraphicButton:
		pDclControl->AddStringProperty( nPressedPicture, PropPicture, theWorkspace.LoadResourceString(IDS_NONE) );
		break;
	}
	
	// add the nReadOnly property
	if (nType == CtlTextBox)
		pDclControl->AddBooleanProperty( nReadOnly, PropBool, false );
	
	// add the nRenderMode property
	if (nType == CtlBlockView)
		pDclControl->AddLongProperty( nRenderMode, PropEnum, nDeRenderMode );
	
	// add the nRetrunAsTab property
	if (nType == CtlTextBox || nType == CtlComboBox || nType == CtlImageComboBox)
		pDclControl->AddBooleanProperty( nReturnAsTab, PropBool, false );
	
	// add top from bottom geometry management
	if (nType != CtlFileDlgCtrl)
		pDclControl->AddLongProperty( nRightFromRight, PropLong, 0 );

	// add the nSelectStyle property
	if (nType == CtlListBox || nType == CtlDwgList)
		pDclControl->AddLongProperty( nSelectStyle, PropEnum, 0 );

	// add the nShow... properties
	if (nType == CtlFileDlgCtrl)
	{
		pDclControl->AddBooleanProperty( nShowCancel, PropBool, true );
		pDclControl->AddBooleanProperty( nShowHelp, PropBool, true );
		pDclControl->AddBooleanProperty( nShowNameLabel, PropBool, true );
		pDclControl->AddBooleanProperty( nShowNameTextBox, PropBool, true );
		pDclControl->AddBooleanProperty( nShowOK, PropBool, true );
		pDclControl->AddBooleanProperty( nShowReadOnlyCheckBox, PropBool, true );
		pDclControl->AddBooleanProperty( nShowTypeComboBox, PropBool, true );
		pDclControl->AddBooleanProperty( nShowTypeLabel, PropBool, true );
	}

	// add the nShowOrbitCirlces property
	if (nType == CtlBlockView)
		pDclControl->AddBooleanProperty( nShowOrbitCirlces, PropBool, false );

	// add the nShowSelectAlways property
	switch (nType)
	{
	case CtlTree:
	case CtlListView:
	case CtlBlockList:
		pDclControl->AddBooleanProperty( nShowSelectAlways, PropBool, false );
		break;
	}

	// add the nShowTicks property
	if (nType == CtlSlider)
		pDclControl->AddBooleanProperty( nShowTicks, PropBool, true );
	
	// add the nSingleExpanded property
	if (nType == CtlTree)
		pDclControl->AddBooleanProperty( nSingleExpanded, PropBool, false );

	// add the nSmallChange property
	switch (nType)
	{
	case CtlSlider:
	case CtlScrollBar:
		pDclControl->AddLongProperty( nSmallChange, PropLong, 1 );
		break;
	}
	
	// add the nSmoothProgress property
	if (nType == CtlProgress)
		pDclControl->AddBooleanProperty( nSmoothProgress, PropBool, false );
	
	// add the Sorted property
	switch (nType)
	{
	case CtlListBox:
	case CtlComboBox:
	case CtlImageComboBox:
		pDclControl->AddBooleanProperty( nSorted, PropBool, false );
		break;
	case CtlListView:
	case CtlGrid:
		pDclControl->AddLongProperty( nListViewSort, PropEnum, 0 );
		break;
	case CtlBlockList:
		pDclControl->AddLongProperty( nListViewSort, PropEnum, 2 );
		break;
	}

	// add the ButtonStyle property
	if (nType == CtlGraphicButton)
		pDclControl->AddLongProperty( nButtonStyle, PropEnum, 0 );
	
	// add the nComboBoxStyle property
	if (nType == CtlComboBox || nType == CtlImageComboBox)
		pDclControl->AddLongProperty( nComboBoxStyle, PropEnum, 0 );

	// add the nListViewStyle property
	if (nType == CtlListView)
		pDclControl->AddLongProperty( nListViewStyle, PropEnum, 3 );

	// add the nBlockListStyle property
	if (nType == CtlBlockList)
		pDclControl->AddLongProperty( nBlockListStyle, PropEnum, 0 );

	// add the nFileDlgStyle property
	if (nType == CtlFileDlgCtrl)
		pDclControl->AddLongProperty( nFileDlgStyle, PropEnum, 1 );

	// add the nRowHeight property
	if (nType == CtlOptionList)
		pDclControl->AddLongProperty( nRowHeight, PropLong, nDeRowHeight );

	if (nType == CtlGrid)
	{
		pDclControl->AddLongProperty( nRowHeight, PropLong, 18 );

		RefCountedPtr< CPropertyObject > pProp = AddControlHiddenProperty(pDclControl, nColumnCaptions, sList, PropStringArray);		
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_COL1));
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_COL2));

		pProp = AddControlHiddenProperty(pDclControl, nColumnAlignments, sList, PropIntArray);		
		//AddControlPropertyListItem(pProp, 0);
		//AddControlPropertyListItem(pProp, 0);

		pProp = AddControlHiddenProperty(pDclControl, nColumnImages, sList, PropIntArray);
		//AddControlPropertyListItem(pProp, -1);
		//AddControlPropertyListItem(pProp, -1);

		pProp = AddControlHiddenProperty(pDclControl, nColumnWidths, sList, PropIntArray);
		//AddControlPropertyListItem(pProp, 100);
		//AddControlPropertyListItem(pProp, 100);

		pProp = AddControlHiddenProperty(pDclControl, nColumnStyles, sList, PropIntArray);
		//AddControlPropertyListItem(pProp, 0);
		//AddControlPropertyListItem(pProp, 0);

		pProp = AddControlHiddenProperty(pDclControl, nColumnDefaultImages, sList, PropIntArray);
		//AddControlPropertyListItem(pProp, 0);
		//AddControlPropertyListItem(pProp, 0);

		pProp = AddControlHiddenProperty(pDclControl, nColumnAlternateImages, sList, PropIntArray);
		//AddControlPropertyListItem(pProp, 1);
		//AddControlPropertyListItem(pProp, 1);

		if (pDclControl->GetPropertyObject(nColumnListItems) == NULL)
		{
			pProp = AddControlHiddenProperty(pDclControl, nColumnListItems, sList, PropStringArrayList);
			//pProp->GetStringArrayListPtr()->push_back(PropVal::TCStringArray());
			//pProp->GetStringArrayListPtr()->push_back(PropVal::TCStringArray());
		}

		if (pDclControl->GetPropertyObject(nColumnListImages) == NULL)
		{		
			pProp = AddControlHiddenProperty(pDclControl, nColumnListImages, sList, PropIntArrayList);
			//pProp->GetIntArrayListPtr()->push_back(PropVal::TIntArray());
			//pProp->GetIntArrayListPtr()->push_back(PropVal::TIntArray());
		}
	}

	// add the nRowHeight property
	if (nType == CtlDwgList)
		pDclControl->AddLongProperty( nRowHeight, PropLong, nDeRowHeightDwg );

	// add the nTabFixedWidth property
	if (nType == CtlTabStrip)
		pDclControl->AddBooleanProperty( nTabFixedWidth, PropBool, false );

	// add the nTabStyle property
	if (nType == CtlTabStrip)
		pDclControl->AddLongProperty( nTabStyle, PropEnum, 0 );

	// add the nText property
	switch (nType)
	{
	case CtlTextBox:
	case CtlComboBox:
	case CtlImageComboBox:
		pDclControl->AddStringProperty( nText, PropString, pDclControl->GetKeyName() );
		break;
	}
	
	// add the nTickFrequency property
	if (nType == CtlSlider)
		pDclControl->AddLongProperty( nTickFrequency, PropLong, nDeTickFrequency );

	// add the nSplitterStyle property
	if (nType == CtlSplitter)
	{
		pDclControl->AddLongProperty( nSplitterStyle, PropEnum, 0 );
		pDclControl->AddLongProperty( nSplitterMin, PropLong, 30 );
		pDclControl->AddLongProperty( nSplitterMax, PropLong, 30 );
	}

	if (!IsVersionFree())
	{
		// add the nToolTipText property
		switch (nType)
		{
		// in not these controls 
		case Ctl3DRect:
		case CtlSplitter:
		case CtlFrame:
		case CtlLabel:
		case CtlHtmlCtrl:
		case CtlScrollBar:
		case CtlSpinButton:
		case CtlTabStrip:
		case CtlOptionList:
		case CtlFileDlgCtrl:
		case CtlAnimate:
			break;
		default: // then add the tool tip text property
			pDclControl->AddStringProperty( nToolTipText, PropString );
			pDclControl->AddStringProperty( nToolTipBody, PropString );
			pDclControl->AddStringProperty( nToolTipPicture, PropPicture, theWorkspace.LoadResourceString(IDS_NONE) );
			pDclControl->AddStringProperty( nToolTipAviFileName, PropString );
			pDclControl->AddBooleanProperty( nToolTipLine, PropBool, false );
			pDclControl->AddLongProperty( nToolTipTitleColor, PropLong, 0 );
			break;
		}
	}
	
	// add the nTop property
	pDclControl->AddLongProperty( nTop, PropLong, 0 );

	// add top from bottom geometry management
	if (nType != CtlFileDlgCtrl)
		pDclControl->AddLongProperty( nTopFromBottom, PropLong, 0 );

	// add the nURLAddress property
	if (nType == CtlStaticURL)
		pDclControl->AddStringProperty( nURLAddress, PropString );
	
	if (nType != CtlFileDlgCtrl)
	{
		// add the geometry management booleans
		pDclControl->AddLongProperty( nUseBottomFromBottom, PropBool, 0 );
		pDclControl->AddLongProperty( nUseLeftFromRight, PropBool, 0 );
		pDclControl->AddLongProperty( nUseRightFromRight, PropBool, 0 );
		pDclControl->AddLongProperty( nUseTopFromBottom, PropBool, 0 );
	}

	// add the nUseTabStops property
	if (nType == CtlListBox)
		pDclControl->AddBooleanProperty( nUseTabStops, PropBool, false );

	// add the nValue property
	switch (nType)
	{
	case CtlCheckBox:
	case CtlOptionButton:
		pDclControl->AddBooleanProperty( nValue, PropBool, false );
		break;
	case CtlRoundSlider:
		pDclControl->AddLongProperty( nValue, PropLong, 0 );
		break;
	case CtlSlider:
	case CtlScrollBar:
	case CtlSpinButton:
		pDclControl->AddLongProperty( nValue, PropLong, 1 );
		break;
	case CtlProgress:
		pDclControl->AddLongProperty( nValue, PropLong, 0 );
		break;
	}

	// add the nVisible property
	if (nType != CtlFileDlgCtrl)
		pDclControl->AddBooleanProperty( nVisible, PropBool, true );

	// add the nVScrollBar property
	switch (nType)
	{
	case CtlListBox:
	case CtlTextBox:
		pDclControl->AddBooleanProperty( nVScrollBar, PropBool, false );
		break;
	case CtlDwgList:
	case CtlOptionList:
		pDclControl->AddBooleanProperty( nVScrollBar, PropBool, true );
		break;
	}

	// add the nPropWidth property
	pDclControl->AddLongProperty( nWidth, PropLong, rcThis.Width() );

	// call method to add all the hidden properties
	AddHiddenProperties(pDclControl, nType, pDclControl->GetKeyName());

}


void CObjectDCLView::AddHiddenProperties(CDclControlObject *pArxObject, short nType, CString Name)
{
	CString sTrue;
	CString sFalse;
	CString sEvent;
	CString sList;
	CString sCustom;		

	sCustom = theWorkspace.LoadResourceString(IDS_CUSTOM);			
	sList = theWorkspace.LoadResourceString(IDS_LIST);			
	sEvent = CString();
	sTrue = theWorkspace.LoadResourceString(IDS_TRUE);
	sFalse = theWorkspace.LoadResourceString(IDS_FALSE);
		

	CRect rcThis;
	
	GetWindowRect(&rcThis);

	// add the nEventFolderChanged property
	if (nType == CtlDwgList)
	{
		AddControlHiddenProperty(pArxObject, nEventFolderChanged, sEvent, PropEvent);
	}	

	// add the nEventSplitterMoved property
	if (nType == CtlSplitter)
	{
		AddControlHiddenProperty(pArxObject, nEventSplitterMoved, sEvent, PropEvent);
	}	
	
	// add the nEventFolderChanged property
	if (nType == CtlFileDlgCtrl)
	{
		AddControlHiddenProperty(pArxObject, nEventFolderChanged, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventOnHelp, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventOnTypeChange, sEvent, PropEvent);
	}

	// add the nTabsCaption property
	if (nType == CtlTabStrip)
	{
		RefCountedPtr< CPropertyObject > pProp = AddControlHiddenProperty(pArxObject, nTabsCaption, sList, PropStringArray);
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_TAB1));
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_TAB2));
		//AddControlPropertyListItem(pProp, theWorkspace.LoadResourceString(IDS_TAB3));

		AddControlHiddenProperty(pArxObject, nTabsImageList, sList, PropIntArray);
		AddControlHiddenProperty(pArxObject, nTabsTTT, sList, PropStringArray);
	}

	// add the nEventNavigateComplete property
	if (nType == CtlHtmlCtrl)
	{
		AddControlHiddenProperty(pArxObject, nEventNavigateComplete, sEvent, PropEvent);
	}

	// add the grid specific properties
	switch (nType)
	{
		case CtlGrid:
			AddControlHiddenProperty(pArxObject, nEventBtnClicked, sEvent, PropEvent);
			break;
		default:
			break;
	}
	
	// add the nEventClicked property
	switch (nType)
	{		
		case CtlSlideView:
		case CtlStdButton:
		case CtlGraphicButton:
		case CtlCheckBox:
		case CtlTree:
		case CtlOptionButton:
		case CtlBlockView:
		case CtlHatch:
		case CtlDwgPreview:
		case CtlPictureBox:
		case CtlListView:
		case CtlBlockList:
			AddControlHiddenProperty(pArxObject, nEventClicked, sEvent, PropEvent);
			break;
		default:
			break;
	}

	if (!IsVersionFree())
	{
		// add the Drag and DropEvents properties 
		switch (nType)
		{		
			case CtlDwgList:
			case CtlListView:
			case CtlBlockList:
			case CtlLabel:	
			case CtlListBox:	
			case CtlStdButton:	
			case CtlGraphicButton:	
			case CtlPictureBox:	
			case CtlTree:	
			case CtlBlockView:	
			case CtlHatch:
			case CtlSlideView:	
			case CtlDwgPreview:		
				AddControlHiddenProperty(pArxObject, nDragnDropToAutoCAD, sEvent, PropEvent);
				AddControlHiddenProperty(pArxObject, nDragnDropFromControl, sEvent, PropEvent);
				if (nCurrentPurchaseMode != nPurchasedR14Pro)
				{
					AddControlHiddenProperty(pArxObject, nDragnDropFromAutoCAD, sEvent, PropEvent);
					AddControlHiddenProperty(pArxObject, nDragnDropBegin, sEvent, PropEvent);
				}
				break;
			case CtlTextBox:	
				AddControlHiddenProperty(pArxObject, nDragnDropFromControl, sEvent, PropEvent);
				if (nCurrentPurchaseMode != nPurchasedR14Pro)
					AddControlHiddenProperty(pArxObject, nDragnDropFromAutoCAD, sEvent, PropEvent);
				break;
			default:
				break;
		}
	}
	// add the nEventEditChanged property
	switch (nType)
	{
		case CtlTextBox:	
		case CtlComboBox:
		case CtlImageComboBox:
			if (nCurrentPurchaseMode != nPurchasedR14Pro)
				AddControlHiddenProperty(pArxObject, nEventEditChanged, sEvent, PropEvent);
			break;
		default:
			break;
	}

	
	// add the nEventDblClicked property
	switch (nType)
	{
		case CtlSlideView:
		case CtlCheckBox:
		case CtlOptionButton:
		case CtlTree:
		case CtlListBox:
		case CtlGraphicButton:
		case CtlDwgList:
		case CtlOptionList:
		case CtlBlockView:
		case CtlHatch:
		case CtlDwgPreview:
		case CtlPictureBox:
		case CtlListView:
		case CtlBlockList:
			AddControlHiddenProperty(pArxObject, nEventDblClicked, sEvent, PropEvent);
			break;
		default:
			break;
	}
	
	// add the nEventKillFocus property
	switch (nType)
	{
		case CtlComboBox:
		case CtlTree:
		case CtlListBox:
		case CtlDwgList:
		case CtlOptionList:
		case CtlPictureBox:
		case CtlTextBox:
		case CtlBlockView:
		case CtlHatch:
		case CtlSlideView:
		case CtlDwgPreview:			
		case CtlListView:
		case CtlGrid:
		case CtlImageComboBox:
		case CtlBlockList:
		//case CtlTabStrip:
			AddControlHiddenProperty(pArxObject, nEventKillFocus, sEvent, PropEvent);
			break;
		default:
			break;
	}
	
	
	// add the nEventSetFocus property
	switch (nType)
	{
		case CtlTextBox:
			if (nCurrentPurchaseMode != nPurchasedR14Pro)
				AddControlHiddenProperty(pArxObject, nEventSetFocus, sEvent, PropEvent);
			break;
		case CtlComboBox:
		case CtlListBox:
		case CtlDwgList:
		case CtlOptionList:
		case CtlPictureBox:
		case CtlTree:
		case CtlImageComboBox:
		case CtlBlockView:
		case CtlHatch:
		case CtlSlideView:
		case CtlDwgPreview:
		case CtlListView:
		case CtlGrid:
		case CtlBlockList:
			AddControlHiddenProperty(pArxObject, nEventSetFocus, sEvent, PropEvent);
			break;
		default:
			break;
	}

	// add the nEventSelChanged property
	switch (nType)
	{
		case CtlComboBox:
		case CtlListBox:
		case CtlDwgList:
		case CtlImageComboBox:
		case CtlOptionList:
		case CtlFileDlgCtrl:
		case CtlTree:
		case CtlMonth:
		case CtlGrid:
			AddControlHiddenProperty(pArxObject, nEventSelChanged, sEvent, PropEvent);
			break;
		default:
			break;
	}

	// add the nEventGetDayState and nEventSelect properties
	if (nType == CtlMonth)
	{
		AddControlHiddenProperty(pArxObject, nEventGetDayState, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventSelect, sEvent, PropEvent);
	}

	
	// add the nEventReleasedCapture property
	switch (nType)
	{
		case CtlSlider:
			AddControlHiddenProperty(pArxObject, nEventReleasedCapture, sEvent, PropEvent);
			break;
		default:
			break;
	}
	
	
	// add the nEventUpdate property
	switch (nType)
	{
		case CtlComboBox:
		case CtlImageComboBox:
		case CtlTextBox:
			if (nCurrentPurchaseMode != nPurchasedR14Pro)
				AddControlHiddenProperty(pArxObject, nEventUpdate, sEvent, PropEvent);
			break;
		default:
			break;
	}

	// add the combo box related only event properties
	if (nType == CtlComboBox ||
		nType == CtlImageComboBox)
	{
		AddControlHiddenProperty(pArxObject, nEventDropDown, sEvent, PropEvent);
	}
	
	// add the text box related only events properties
	if (nType == CtlTextBox)
	{
		AddControlHiddenProperty(pArxObject, nEventMaxText, sEvent, PropEvent);		
		AddControlHiddenProperty(pArxObject, nEventReturnPressed, sEvent, PropEvent);

		if (nCurrentPurchaseMode != nPurchasedR14Pro)
		{
			AddControlHiddenProperty(pArxObject, nEventKeyDown, sEvent, PropEvent);
			AddControlHiddenProperty(pArxObject, nEventKeyUp, sEvent, PropEvent);
		}
	}
	
	// add the List View Controls
	switch (nType) 
	{
		case CtlBlockList:
		{
			AddControlHiddenProperty(pArxObject, nEventReturnPressed, sEvent, PropEvent);
			AddControlHiddenProperty(pArxObject, nEventKeyDown, sEvent, PropEvent);			
			break;
		}
		case CtlGrid:
			{
			AddControlHiddenProperty(pArxObject, nEventBeginLabelEdit, sEvent, PropEvent);
			AddControlHiddenProperty(pArxObject, nEventEndLabelEdit, sEvent, PropEvent);
			AddControlHiddenProperty(pArxObject, nEventColumnClick, sEvent, PropEvent);			
			break;
		}	
		case CtlListView:
		{
			AddControlHiddenProperty(pArxObject, nEventReturnPressed, sEvent, PropEvent);
			AddControlHiddenProperty(pArxObject, nEventKeyDown, sEvent, PropEvent);			
			AddControlHiddenProperty(pArxObject, nEventBeginLabelEdit, sEvent, PropEvent);
			AddControlHiddenProperty(pArxObject, nEventEndLabelEdit, sEvent, PropEvent);
			AddControlHiddenProperty(pArxObject, nEventColumnClick, sEvent, PropEvent);			
			break;
		}		
	}

	// add the Picture box related only events properties
	switch (nType) 
	{
		case CtlSlideView:
		case CtlPictureBox:
		{
		AddControlHiddenProperty(pArxObject, nEventKeyDown, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventKeyUp, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseDown, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseUp, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseMove, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseMovedOff, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseDblClick, sEvent, PropEvent);		
		AddControlHiddenProperty(pArxObject, nEventMouseWheel, sEvent, PropEvent);				
		AddControlHiddenProperty(pArxObject, nEventPaint, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseEntered, sEvent, PropEvent);		
		AddControlHiddenProperty(pArxObject, nEventRClick, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventRDblClick, sEvent, PropEvent);				
		break;
		}	
		case CtlListView:
		{
		AddControlHiddenProperty(pArxObject, nEventKeyDown, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventKeyUp, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseDown, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseUp, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseMove, sEvent, PropEvent);
		break;
		}
		case CtlGrid:
		{
		AddControlHiddenProperty(pArxObject, nEventMouseDown, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseUp, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseMove, sEvent, PropEvent);
		break;
		}
		
		case CtlListBox:
		{
		AddControlHiddenProperty(pArxObject, nEventRClick, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseMove, sEvent, PropEvent);
		break;
		}

		case CtlStdButton:
		case CtlGraphicButton:
		case CtlTextBox:
		case CtlCheckBox:
		case CtlOptionButton:
		case CtlComboBox:
		case CtlTree:
		case CtlRoundSlider:
		case CtlBlockView:
		case CtlHatch:
		case CtlSlider:
		case CtlSpinButton:
		case CtlHtmlCtrl:
		case CtlLabel:
		case CtlImageComboBox:
		case CtlDwgPreview:
		case CtlBlockList:
		case CtlOptionList:
		case CtlDwgList:
		{
		AddControlHiddenProperty(pArxObject, nEventMouseMove, sEvent, PropEvent);
		break;
		}
	}
	

	// add the nEventScroll property
	switch (nType)
	{
		case CtlScrollBar:
			{				
				AddControlHiddenProperty(pArxObject, nEventScroll, sEvent, PropEvent);
				AddControlHiddenProperty(pArxObject, nEventScrolled, sEvent, PropEvent);
				break;
			}
			
		case CtlRoundSlider:
			{				
				AddControlHiddenProperty(pArxObject, nEventScroll, sEvent, PropEvent);
				break;
			}
		default:
			break;
	}

	// add the right click and double click property
	switch (nType)
	{
		case CtlTree:
		case CtlBlockView:
		case CtlHatch:
			{
				AddControlHiddenProperty(pArxObject, nEventRClick, sEvent, PropEvent);
				AddControlHiddenProperty(pArxObject, nEventRDblClick, sEvent, PropEvent);
				break;
			}
		default:
			break;
	}
	
	if (nType == CtlBlockView || nType == CtlHatch)
	{
		AddControlHiddenProperty(pArxObject, nEventMouseDown, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseUp, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventMouseDblClick, sEvent, PropEvent);
	}
		
	// add the tree related only event properties
	if (nType == CtlTree)
	{
		AddControlHiddenProperty(pArxObject, nEventBeginLabelEdit, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventDeleteItem, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventEndLabelEdit, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventItemExpanded, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventItemExpanding, sEvent, PropEvent);
	}

	// add the nImageList property
	switch (nType)
	{
		case CtlTabStrip:
			{
				CString sImageList;
				sImageList = theWorkspace.LoadResourceString(IDS_IMAGELIST);
				AddControlHiddenProperty(pArxObject, nImageList, sImageList, PropImageList);
				break;
			}
		default:
			break;
	}

	if (nType == CtlTabStrip)
	{
		AddControlHiddenProperty(pArxObject, nEventChanged, sEvent, PropEvent);
		AddControlHiddenProperty(pArxObject, nEventSelChanging, sEvent, PropEvent);
	}

	if (nType == CtlSpinButton)
	{
		AddControlHiddenProperty(pArxObject, nEventChanged, sEvent, PropEvent);
	}

	// add the Font properties
	switch (nType)		
	{	
	case CtlListView:
	case CtlGrid:
	case CtlBlockList:	
	case CtlLabel:
	case CtlStdButton:
	case CtlGraphicButton:
	case CtlFrame:
	case CtlTextBox:
	case CtlCheckBox:
	case CtlOptionButton:
	case CtlComboBox:
	case CtlListBox:
	case CtlDwgList:
	case CtlImageComboBox:
	case CtlPictureBox:
	case CtlTabStrip:
	case CtlMonth:
	case CtlTree:
	case CtlRoundSlider:
	case CtlOptionList:
		{
		if (activeProject->m_bDefaultFontBold)
			AddControlHiddenProperty(pArxObject, nLabelBold, sTrue, PropBool);
		else
			AddControlHiddenProperty(pArxObject, nLabelBold, sFalse, PropBool);
		
		
		if (activeProject->m_bDefaultFontItalic)
			AddControlHiddenProperty(pArxObject, nLabelItalic, sTrue, PropBool);
		else
			AddControlHiddenProperty(pArxObject, nLabelItalic, sFalse, PropBool);
		
		AddControlHiddenProperty(pArxObject, nLabelSize, LTOA(activeProject->m_nDefaultFontSize), PropLong);
		
		AddControlHiddenProperty(pArxObject, nLabelStrikeOut, sFalse, PropBool);
		
		if (activeProject->m_bDefaultFontUnderLine)
			AddControlHiddenProperty(pArxObject, nLabelUnderline, sTrue, PropBool);		
		else
			AddControlHiddenProperty(pArxObject, nLabelUnderline, sFalse, PropBool);		

		AddControlHiddenProperty(pArxObject, nFontSizeStyle, sTrue, PropBool);
		
		break;
		}
	case CtlStaticURL:
		{
		AddControlHiddenProperty(pArxObject, nLabelBold, sFalse, PropBool);
		AddControlHiddenProperty(pArxObject, nLabelItalic, sFalse, PropBool);
		AddControlHiddenProperty(pArxObject, nLabelSize, LTOA(nDefontSize), PropLong);
		AddControlHiddenProperty(pArxObject, nLabelStrikeOut, sFalse, PropBool);
		AddControlHiddenProperty(pArxObject, nLabelUnderline, sTrue, PropBool);		
		AddControlHiddenProperty(pArxObject, nFontSizeStyle, sTrue, PropBool);
		break;
		}
	}	
}
void CObjectDCLView::RefreshChildControl(CDclControlObject *pArxObject, PropertyId ChangedPropertyID)
{
	if (pArxObject == NULL)
		return;

	if (pArxObject->m_pCtrlHolder == NULL)
		return;

	if (!IsWindow(pArxObject->m_pCtrlHolder->m_hWnd))
		return;

	CControlHolder *pParent = (CControlHolder*)pArxObject->m_pCtrlHolder;

	// some controls don't need to be recreated when a nNotSet is passed back by the properties wizard dialog box
	if (ChangedPropertyID == nNotSet)
	{
		switch (pArxObject->GetType())
		{
			case CtlTabStrip:
			case CtlStdButton:
			case CtlGraphicButton:
			{
				// refresh all properties without creating a new child control
				ChangedPropertyID = (PropertyId)nRefreshAllProperties;
				break;
			}
		}
	}
	// here we are going to check to see if sertain properties have been requested to be changed
	// if these properties have then we create a new control and destroy the old control.
	// otherwise we just call a method to request the update of that control property
	switch(ChangedPropertyID)
	{
	case nNotSet:
	case nTabJustified:
	case nSmoothProgress:
	case nSorted:
	case nImageList:
	case nMultiColumn:
	case nMultiRow:
	case nOrientation:
	case nFilterStyle:
	case nJustification:
	case nComboBoxStyle:
	case nNoIntegralHeight:
	case nHScrollBar:
	case nVScrollBar:
	case nShowTicks:		
	case nHasLines:
	case nLinesAtRoot:
	case nHasButtons:
	case nCheckBoxes:
	case nButtonStyle:
	case nTabLabelAlign:
	case nMinTabWidth:
		{

			if (pArxObject->GetType() != CtlActiveX)
			{
				CWnd *pOldControl = pParent->GetChildControl();
				//nOldId = pParent->GetId();
				CreateChildControl(pParent, pArxObject, false);
				// call the method to update the arx control's property that was requested to be changed
				ResizeChildControl(pArxObject);
				// if the program made it this far, destroy the previous control
				if (pOldControl != NULL)
					pOldControl->ShowWindow(FALSE);
			}
			else // if an ActiveX control
				ResizeChildControl(pArxObject);				
			break;
		}
	case nRefreshAllProperties:
		{
			// call the method to update all the properties
			UpdateChildControl(pArxObject, pParent);
			break;
		}
	case nLeft:
	case nTop:
	case nWidth:
	case nHeight:
		{
			// call the method to update the arx control's property that was requested to be changed
			UpdateProperty(ChangedPropertyID, pArxObject, pParent);
			ResizeChildControl(pArxObject);
			break;
		}
	case nAutoSize:
	case nPicture:
		{		
			// call the method to update the arx control's property that was requested to be changed
			UpdateProperty(ChangedPropertyID, pArxObject, pParent);
			ResizeChildControl(pArxObject);
			// check to see if autosize should resize the control
			CheckAutoSizeProp(pArxObject, pParent);

			break;
		}
	case nLabelName:
	case nLabelBold:
	case nLabelSize:
	case nLabelItalic:
	case nLabelUnderline:
	case nLabelStrikeOut:
		{		
			// call the method to update the arx control's property that was requested to be changed
			UpdateProperty(ChangedPropertyID, pArxObject, pParent);
			ResizeChildControl(pArxObject);
		
			break;
		}
	default:
		{		
			// call the method to update the arx control's property that was requested to be changed
			UpdateProperty(ChangedPropertyID, pArxObject, pParent);			
			break;
		}
	
	}

	
}
void CObjectDCLView::ResizeChildControl(CDclControlObject *pArxObject)
{
	if (pArxObject == NULL)
		return;

	// if the program made it this far, destroy the previous control
	CControlHolder *pParent = (CControlHolder*)pArxObject->m_pCtrlHolder;
	if (pParent == NULL)
		return;
	CWnd *pControl = pParent->GetChildControl();
	
	if (pControl == NULL)
		return;
	
	// prepare to resize the child control
	CRect rcControl;

	rcControl.left = pArxObject->GetLngProperty(nLeft),
	rcControl.top = pArxObject->GetLngProperty(nTop);
	int nCalcWidth = pArxObject->GetLngProperty(nWidth);
	rcControl.right = rcControl.left + pArxObject->GetLngProperty(nWidth);
	rcControl.bottom = rcControl.top + pArxObject->GetLngProperty(nHeight);
	
	CRect rcThis;
	GetClientRect(&rcThis);
	
	CalcControlOffsetDistances(pArxObject, rcControl);

	// resize this control no mater what property was asked for
	pParent->MoveWindow(rcControl, TRUE);	
}

void CObjectDCLView::CheckAutoSizeProp(CDclControlObject *pArxObject, CControlHolder *pParent)
{
	if (pParent == NULL)
		return;
	
	CWnd *pControl = pParent->GetChildControl();
	if (pControl == NULL)
		return;
	
	switch(pArxObject->GetType())
	{
		/*
	case CtlComboBox:
		{
		if (pArxObject->GetLngProperty(nComboBoxStyle) == 1)
		{
			CRect rcDropDown;
			CListBox   pListBox;
			//((CComboBox*)pControl)->GetDroppedControlRect(&rcDropDown);
		}
		break;
		}
		*/
	case CtlGraphicButton:
		{
		if (pArxObject->GetBoolProperty(nAutoSize) == TRUE)
		{
			int nBorderAddition;

			switch(pArxObject->GetLngProperty(nButtonStyle))
			{
			case 0:
				nBorderAddition = 4;
				break;
			case 1:
				nBorderAddition = 0;
				break;
			default:
				return;
				break;
			}
			
			pArxObject->SetLongProperty(
				nWidth,
				((CGraphicButton*)pControl)->m_cxIcon + nBorderAddition
				);
			pArxObject->SetLongProperty(
				nHeight,
				((CGraphicButton*)pControl)->m_cyIcon + nBorderAddition
				);
			CRect rcCtrl;
			pParent->GetWindowRect(&rcCtrl);
			ScreenToClient(rcCtrl);
			rcCtrl.right = rcCtrl.left + ((CGraphicButton*)pControl)->m_cxIcon + nBorderAddition;
			rcCtrl.bottom = rcCtrl.top + ((CGraphicButton*)pControl)->m_cyIcon + nBorderAddition;
			pParent->MoveWindow(rcCtrl, TRUE);
			
		}
		break;
		}
	case CtlPictureBox:
		{
		if (pArxObject->GetBoolProperty(nAutoSize) == TRUE)
		{
			int nBorderAddition;

			switch(pArxObject->GetLngProperty(nBorderStyle))
			{
			case 0:
				nBorderAddition = 0;
				break;
			case 1:
				nBorderAddition = 4;
				break;
			case 2:
				nBorderAddition = 2;
				break;
			}
			pArxObject->SetLongProperty(
				nWidth,
				((CPictureBox*)pControl)->m_cxIcon + nBorderAddition
				);
			pArxObject->SetLongProperty(
				nHeight,
				((CPictureBox*)pControl)->m_cyIcon + nBorderAddition
				);
			
			CRect rcCtrl;
			pParent->GetWindowRect(&rcCtrl);
			ScreenToClient(rcCtrl);
			rcCtrl.right = rcCtrl.left + ((CPictureBox*)pControl)->m_cxIcon + nBorderAddition;
			rcCtrl.bottom = rcCtrl.top + ((CPictureBox*)pControl)->m_cyIcon + nBorderAddition;
			pParent->MoveWindow(rcCtrl, TRUE);
			
		}
		break;
		}
	case CtlActiveX:
		{
			return;
			break;
		}
	default:
		{
			// get the child control
			CSize pCtrlSize = GetControlSize(pControl, pArxObject->GetType());
	
			if (pCtrlSize.cx != pArxObject->GetLngProperty(nWidth) ||
				pCtrlSize.cy != pArxObject->GetLngProperty(nHeight))
			{
				pArxObject->SetLongProperty(
					nWidth,
					pCtrlSize.cx);
				pArxObject->SetLongProperty(
					nHeight,
					pCtrlSize.cy
					);
				CRect rcCtrl;
				pParent->GetWindowRect(&rcCtrl);
				ScreenToClient(rcCtrl);
				rcCtrl.right = rcCtrl.left + pCtrlSize.cx;
				rcCtrl.bottom = rcCtrl.top + pCtrlSize.cy;
				pParent->MoveWindow(rcCtrl, TRUE);
			}
			break;
		}

	}

}



void CObjectDCLView::UpdateProperty(PropertyId nID, CDclControlObject *pArxObject, CControlHolder *pParent)
{
	if (pParent == NULL)
		return;
	
	CWnd *pControl = pParent->GetChildControl();
	if (pControl == NULL)
		return;
		
	RefCountedPtr< CPropertyObject > pProp = pArxObject->GetPropertyObject(nID);
	// set the appropriate property
	switch(nID)
	{
		case nAutoSize:
			{
			switch (pArxObject->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControl)->m_AutoSize = pArxObject->GetBoolProperty(nAutoSize);
				break;
			case CtlGraphicButton:
				((CGraphicButton*)pControl)->m_AutoSize = pArxObject->GetBoolProperty(nAutoSize);
				break;
			}			
			break;
			}
		case nAcadColor:
		{				
			switch (pArxObject->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
			
			case CtlStdButton:
			case CtlOptionButton:
			case CtlCheckBox:
				((CColorButton*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;

			case CtlAnimate:
			case CtlListView:
			case CtlGrid:
			case CtlHatch:
			case CtlBlockList:
			case CtlBlockView:
			case CtlDwgPreview:
			case CtlPictureBox:
			case CtlSlideView:		
				((CPictureBox*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;

			case CtlTextBox:
				((CColorEdit*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
			
			case CtlListBox:
			case CtlDwgList:
				((CAcadColorListBox*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
			case CtlOptionList:
				((COptionListBox*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;		
				
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;		

			case CtlGraphicButton:
				((CGraphicButton*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
			}
			pControl->Invalidate();
			break;
		}	
		case nForeColor:
		{				
			switch (pArxObject->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				break;		

			case CtlStdButton:
			case CtlOptionButton:
			case CtlCheckBox:
				((CColorButton*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				break;
			case CtlGraphicButton:
				((CGraphicButton*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				break;
			case CtlTextBox:
				//((CColorEdit*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				switch (pArxObject->GetLngProperty(nFilterStyle))
				{
				case 8: //EditFilter_Multiline
					((CColorEdit*)pControl)->m_UseBackColor = false;
				case 4: //EditFilter_Symbol
					break;
				default:
					((CColorEdit*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
					break;
				}
				break;			
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				break;
			case CtlListBox:
			case CtlDwgList:
			case CtlOptionList:
				((CAcadColorListBox*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				break;
			}			
			pControl->Invalidate();
			break;
		}	
		
		case nBorderStyle:
		{
			switch(pArxObject->GetLngProperty(nBorderStyle))
			{
			case 0:
				pControl->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				break;
					
			case 1:
				pControl->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
				break;

			case 2:
				pControl->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_FRAMECHANGED);
				break;
			}
			switch (pArxObject->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControl)->m_BorderStyle = pArxObject->GetLngProperty(nBorderStyle);
				break;
			}
			break;
		}	
		case nButtonStyle:
		{				
			((CGraphicButton*)pControl)->SetDefaultPicture(pArxObject->GetLngProperty(nButtonStyle));
			break;
		}	
		case nCaption:
		{				
			switch (pArxObject->GetType())
			{
				
			case CtlFrame:
				((VdclGroupBox*)pControl)->SetCaption(pArxObject->GetStrProperty(nCaption));
				break;
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetLinkText(pArxObject->GetStrProperty(nCaption));
				break;
			case CtlGraphicButton:
				{
				CString sCaptionText = pArxObject->GetStrProperty(nCaption);
				((CGraphicButton*)pControl)->SetWindowText(sCaptionText);
				((CGraphicButton*)pControl)->Invalidate();
				break;
				}			
			default:
				{
				CString sCaptionText = pArxObject->GetStrProperty(nCaption);
				pControl->SetWindowText(sCaptionText);
				break;
				}
			}
			break;
		}	
		case nColumnWidth:
		{				
			int nNewColWidth = pArxObject->GetLngProperty(nColumnWidth);
			if (nNewColWidth > 0)
				((CAcadColorListBox*)pControl)->SetColumnWidth(nNewColWidth);
			break;
		}	
		
		case nDefSelIndex:
		{
			((COptionListBox*)pControl)->ResetContent();					
			CString sListItem;
			RefCountedPtr< CPropertyObject > pPropList = pArxObject->GetPropertyObject(nBtnCaption);
			int nDefSelection = pArxObject->GetLngProperty(nDefSelIndex) ;
			for (size_t i = 0; i < pPropList->size(); i++)
			{				
				sListItem = pPropList->GetStringItem(i);
				if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
				{
					((COptionListBox *)pControl)->AddString(sListItem);
				}
				if (nDefSelection == i)
					((COptionListBox *)pControl)->SetItemData(i, 1);
				else
					((COptionListBox *)pControl)->SetItemData(i, 0);
			}
			
			break;
		}
		
		case nDisableNoScroll:
		{
			if (pArxObject->GetBoolProperty(nDisableNoScroll) == FALSE)
				((CAcadColorListBox*)pControl)->ModifyStyle(LBS_DISABLENOSCROLL, 0, SWP_FRAMECHANGED);
			else
				((CAcadColorListBox*)pControl)->ModifyStyle(0, LBS_DISABLENOSCROLL, SWP_FRAMECHANGED);
			break;
		}
		
		case nEnabled:
		{
			if (pArxObject->GetType() == CtlSlider)
			{
				((CSliderCtrl *)pControl)->EnableWindow(pArxObject->GetBoolProperty(nEnabled));
			}
			else if (pArxObject->GetType() == CtlOptionList)
			{
				int nData=0;
				if (pArxObject->GetBoolProperty(nEnabled) == FALSE)
					nData = 2;
				for (int i=0; i<((COptionListBox*) pControl)->GetCount(); i++)
				{
					((COptionListBox*) pControl)->SetItemData(i, nData);
				}
				pControl->Invalidate();				
			}			
			else
			{
				pControl->EnableWindow(pArxObject->GetBoolProperty(nEnabled));
				pControl->Invalidate();
			}
			break;				
		}	
		case nFilterStyle:
			{
			switch (pArxObject->GetLngProperty(nFilterStyle))
			{
			case 5:/*Upper case*/
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->ModifyStyle(ES_LOWERCASE, ES_UPPERCASE, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			case 6:/*lower case*/
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->ModifyStyle(ES_UPPERCASE, ES_LOWERCASE, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			case 7:/*password*/
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE, ES_PASSWORD, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar('*');
				break;
				}
			case 8:/*MultiLine*/
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE|ES_PASSWORD, ES_MULTILINE, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			default:
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE|ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			}
			break;
		}

		case nIndent:
		{
			((CTreeCtrl*)pControl)->SetIndent(pArxObject->GetLngProperty(nIndent));
			break;
		}

		case nImageList:
		{
			ResetImageList(pControl, nID, pArxObject);
			break;
		}

		case nJustification:
		{
			switch (pArxObject->GetType())
			{
			case CtlTextBox:
				{
					/*
					((CColorEdit*)pControl)->ModifyStyle(ES_RIGHT, 0, SWP_FRAMECHANGED);
					((CColorEdit*)pControl)->ModifyStyle(ES_CENTER, 0, SWP_FRAMECHANGED);
					((CColorEdit*)pControl)->ModifyStyle(ES_LEFT, 0, SWP_FRAMECHANGED);
					
					switch (pArxObject->GetLngProperty(nJustification))
					{
					case 0:// Left
						((CColorEdit*)pControl)->ModifyStyle(0, ES_LEFT, SWP_FRAMECHANGED);
						break;
					case 1:// Center
						((CColorEdit*)pControl)->ModifyStyle(0, ES_CENTER, SWP_FRAMECHANGED);
						break;
					case 2:// Right
						((CColorEdit*)pControl)->ModifyStyle(0, ES_RIGHT, SWP_FRAMECHANGED);
						break;
					}
					*/
					break;
				}
			case CtlLabel:
				{
					pControl->ModifyStyle(SS_RIGHT, 0, SWP_FRAMECHANGED);
					pControl->ModifyStyle(SS_CENTER, 0, SWP_FRAMECHANGED);
					pControl->ModifyStyle(SS_LEFT, 0, SWP_FRAMECHANGED);
			
					switch (pArxObject->GetLngProperty(nJustification))
					{
					case 0:/*Left*/
						pControl->ModifyStyle(0, SS_LEFT, SWP_FRAMECHANGED);
						break;
					case 1:/*Center*/
						pControl->ModifyStyle(0, SS_CENTER, SWP_FRAMECHANGED);
						break;
					case 2:/*Right*/
						pControl->ModifyStyle(0, SS_RIGHT, SWP_FRAMECHANGED);
						break;
					}
					break;
				}
			}
			pControl->Invalidate();
			break;
			
		}

		case nLabelName:
		{			
			CFont *pFont = const_cast<CFont*>(m_pFontCollection->GetFont(pArxObject, pControl));
			
			if (pArxObject->GetType() == CtlRoundSlider)
			{
				((CRoundSliderCtrl*)pControl)->m_pFont = pFont;
				pControl->RedrawWindow();
			}
			
			else if (pArxObject->GetType() == CtlStaticURL)
			{
				((CStaticLink*)pControl)->SetFont(pFont);
				pControl->Invalidate();				
			}
			
			else if (pArxObject->GetType() == CtlFrame)
			{
				((VdclGroupBox*)pControl)->m_Frame.SetFont(pFont);
				((VdclGroupBox*)pControl)->m_Frame.Invalidate();
			}
			else if (pArxObject->GetType() == CtlGraphicButton)
			{
				((CGraphicButton*)pControl)->SetFont(pFont);	
				pControl->Invalidate();				
			}
			else if (pArxObject->GetType() == CtlOptionList)
			{
				((COptionListBox*)pControl)->SetFont(pFont);	
				pControl->Invalidate();				
			}
			else if (pArxObject->GetType() == CtlGrid)
			{
				((CListCtrlEx*)pControl)->SetFont(pFont);	
				((CListCtrlEx*)pControl)->m_Child.SetFont(pFont);	
				pControl->Invalidate();				
			}
			else
			{
				pControl->SetFont(pFont);	
				pControl->Invalidate();				
			}	
			break;
		}

		case nLimitText:
		{
			((CColorEdit*)pControl)->SetLimitText(pArxObject->GetLngProperty(nLimitText));
			break;
		}
		
		case nBtnCaption:
		{
			CString sListItem;
			((COptionListBox*)pControl)->ResetContent();					
			int nDefSelection = pArxObject->GetLngProperty(nDefSelIndex) ;
			for (size_t i = 0; i < pProp->size(); i++)
			{				
				sListItem = pProp->GetStringItem(i);
				if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
				{
					((COptionListBox *)pControl)->AddString(sListItem);
					if (nDefSelection == i)
						((COptionListBox *)pControl)->SetItemData(i, 1);
					else
						((COptionListBox *)pControl)->SetItemData(i, 0);
				}
			}
			
			break;
		}
	
		case nColumnCaptions:
		{
			((CListCtrlEx *)pControl)->SetupColumns(pArxObject);

			break;
		}
		case nList:
		{
			switch (pArxObject->GetType())
			{
			case CtlListBox:
				{
					CString sListItem;
					((CAcadColorListBox *)pControl)->ResetContent();					
					for (size_t i = 0; i < pProp->size(); i++)
					{				
						sListItem = pProp->GetStringItem(i);
						if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
							((CAcadColorListBox *)pControl)->AddString(sListItem);
					}
					break;
				}
			case CtlComboBox:
				{
					CString sListItem;
					((CComboBox *)pControl)->ResetContent();
					for (size_t i = 0; i < pProp->size(); i++)
					{			
						sListItem = pProp->GetStringItem(i);
						if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
							((CComboBox *)pControl)->AddString(pProp->GetStringItem(i));
					}
					pControl->SetWindowText(pArxObject->GetStrProperty(nText));
					break;
				}
			}
			break;				
		}
		case nMarginLeft:
		{
			((CColorEdit*)pControl)->SetMargins(
				pArxObject->GetLngProperty(nMarginLeft),
				pArxObject->GetLngProperty(nMarginRight));
			break;
		}
		
		case nMinTabWidth:
		{
			try 
			{
				((CTabCtrl*)pControl)->SetMinTabWidth(pArxObject->GetLngProperty(nMinTabWidth));
				((CTabCtrl*)pControl)->RedrawWindow(NULL, NULL, RDW_UPDATENOW);
			}
			catch(...)
			{
			}
			break;
		}
		
		case nMaxValue:
		case nMinValue:
		{
			switch (pArxObject->GetType())
			{
			case CtlProgress:
				((CProgressCtrl*)pControl)->SetRange(
					(short)pArxObject->GetLngProperty(nMinValue),
					(short)pArxObject->GetLngProperty(nMaxValue));
				break;				
			case CtlScrollBar:
				((CScrollBar*)pControl)->SetScrollRange(
					pArxObject->GetLngProperty(nMinValue),
					pArxObject->GetLngProperty(nMaxValue),
					TRUE);
				break;				
			case CtlSlider:
				((CSliderCtrl*)pControl)->SetRange(
					pArxObject->GetLngProperty(nMinValue),
					pArxObject->GetLngProperty(nMaxValue));
				((CSliderCtrl *)pControl)->SetPos(pArxObject->GetLngProperty(nMinValue));
				((CSliderCtrl *)pControl)->SetPos(pArxObject->GetLngProperty(nValue));
				break;				
			}
			break;
		}	
		
		case nSplitterStyle:
		{
			((CSplitter *)pControl)->m_nStyle = pArxObject->GetLngProperty(nSplitterStyle);
			int n = ((CSplitter *)pControl)->m_nStyle;
			if (((CSplitter *)pControl)->m_nStyle <= 0)
			{
				((CSplitter *)pControl)->m_nStyle = 0;
				pArxObject->SetLongProperty(nSplitterStyle, 0);
			}
			pControl->Invalidate();
			break;
		}
			
		case nOrientation:
		{
			if (pArxObject->GetType() == CtlSlider)
			{			
				switch(pArxObject->GetLngProperty(nOrientation))
				{
				case 0:				
					((CSliderCtrl *)pControl)->ModifyStyle(TBS_VERT, 0, SWP_FRAMECHANGED);
					((CSliderCtrl *)pControl)->ModifyStyle(0, TBS_HORZ, SWP_FRAMECHANGED);
					break;
				case 1:
					((CSliderCtrl *)pControl)->ModifyStyle(TBS_HORZ, 0, SWP_FRAMECHANGED);
					((CSliderCtrl *)pControl)->ModifyStyle(0, TBS_VERT, SWP_FRAMECHANGED);
					break;
				}
			}
			break;
		}
		case nPicture:
		{
			switch (pArxObject->GetType())
			{
			case CtlGraphicButton:		
				((CGraphicButton *)pControl)->SetPictureID(pArxObject->GetLngProperty(nPicture));
				break;
			case CtlPictureBox:		
				((CPictureBox *)pControl)->SetPictureID(pArxObject->GetLngProperty(nPicture));
				break;
			}
			break;
		}

			
		case nShowTicks:
		{
			if (pArxObject->GetBoolProperty(nShowTicks) == FALSE)
				((CSliderCtrl *)pControl)->ModifyStyle(TBS_AUTOTICKS, 0, SWP_FRAMECHANGED);
			else
				((CSliderCtrl *)pControl)->ModifyStyle(0, TBS_AUTOTICKS, SWP_FRAMECHANGED);
				
			break;
		}

		case nReadOnly:
		{
			((CColorEdit*)pControl)->SetReadOnly(pArxObject->GetBoolProperty(nReadOnly));
			break;
		}

		case nRowHeight:
		{
			if (pArxObject->GetType() == CtlOptionList)
			{
				((COptionListBox*)pControl)->m_RowHeight = (short)pArxObject->GetLngProperty(nRowHeight);
				((COptionListBox*)pControl)->ResetContent();					
				CString sListItem;
				RefCountedPtr< CPropertyObject > pPropList = pArxObject->GetPropertyObject(nBtnCaption);
				int nDefSelection = pArxObject->GetLngProperty(nDefSelIndex) ;
				for (size_t i = 0; i < pPropList->size(); i++)
				{				
					sListItem = pPropList->GetStringItem(i);
					if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
					{
						((COptionListBox *)pControl)->AddString(sListItem);
						if (nDefSelection == i)
							((COptionListBox *)pControl)->SetItemData(i, 1);
						else
							((COptionListBox *)pControl)->SetItemData(i, 0);
					}
				}
			}
			
			break;
		}

		case nTabsCaption:
		//case nTabsImageList:
		{
			try
			{
				int nImageListIndex = pArxObject->GetImageListIndex();

				TC_ITEM TabCtrlItem;
				CString sTTT;
				if (nImageListIndex == nNotSet)
					TabCtrlItem.mask = TCIF_TEXT;
				else				
					TabCtrlItem.mask = TCIF_TEXT|TCIF_IMAGE;			
				
				// delete all previos tabs
				((CTabCtrl*)pControl)->DeleteAllItems();
				
				int nCount = pArxObject->CountPropertyListItems(nTabsCaption) - 1;
				int nBottom = 0;
				while (nCount >= 0)
				{
					CString Tab = pArxObject->GetPropertyListItem(nTabsCaption, nCount);
					// get the tag caption
					TabCtrlItem.pszText = Tab.GetBuffer(nDeTextLimitCB);		
					
					// set the image list item number is required
					if (nImageListIndex > nNotSet)
						TabCtrlItem.iImage = _tstol(pArxObject->GetPropertyListItem(nTabsImageList, nCount));
					
					// add the new tab
					((CTabCtrl*)pControl)->InsertItem( 0, &TabCtrlItem );
					
					// get the new items' rectangle
					CRect rcTab;
					((CTabCtrl*)pControl)->GetItemRect( 
						((CTabCtrl*)pControl)->GetItemCount() - 1,
						&rcTab);

					// if the bottom of this tab is greater than the last setting
					// update the bottom value
					if (rcTab.bottom > nBottom)
						nBottom = rcTab.bottom;
				
					nCount--;
				}
				
				// get the height of the tab control
				CRect rcTabCtrl;
				((CTabCtrl*)pControl)->GetWindowRect(&rcTabCtrl);
				((CTabCtrl*)pControl)->SetCurSel(0);
				// subtract the lowest tab bottom to get the control area height
				pArxObject->m_ClientHeight = rcTabCtrl.Height() - nBottom;
			}
			catch(...)
			{
			}
			break;
		}
		case nTabSelected:
		{
			((CAcadColorListBox*)pControl)->SetCurSel(pArxObject->GetLngProperty(nTabSelected));
			break;
		}
		case nTabStyle:
		{
			if (pArxObject->GetLngProperty(nTabStyle) == 0)
				((CTabCtrl*)pControl)->ModifyStyle(TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED);
			else
				((CTabCtrl*)pControl)->ModifyStyle(TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED);
			break;
		}
		
		case nTabFixedWidth:
		{
			try
			{
				if (pArxObject->GetBoolProperty(nTabFixedWidth) == TRUE)
				{
					((CTabCtrl*)pControl)->ModifyStyle(0, TCS_FIXEDWIDTH, SWP_FRAMECHANGED);
					CRect rc;
					((CTabCtrl*)pControl)->GetItemRect(0, &rc);
					CSize szTabs;
					szTabs.cx = pArxObject->GetLngProperty(nMinTabWidth);
					szTabs.cy = rc.Height();
					((CTabCtrl*)pControl)->SetItemSize(szTabs);
				}
				else
				{	
					((CTabCtrl*)pControl)->ModifyStyle(TCS_FIXEDWIDTH, 0, SWP_FRAMECHANGED);
				}
			}
			catch(...)
			{
			}
			break;
		}
		case nText:
		{
			int nTextLimit = pArxObject->GetLngProperty(nLimitText);

			CString sNewText = pArxObject->GetStrProperty(nText);

			if (nTextLimit > nNotSet) 
			{
				sNewText = sNewText.Left(nTextLimit);
				pArxObject->SetStringProperty(nText, sNewText);
			}

			pControl->SetWindowText(sNewText);
			break;
		}
		case nTickFrequency:
		{
			((CSliderCtrl *)pControl)->SetTicFreq(pArxObject->GetLngProperty(nTickFrequency));
			
			break;
		}
		
		case nUseTabStops:
		{
			if (pArxObject->GetBoolProperty(nUseTabStops) == FALSE)
				((CAcadColorListBox*)pControl)->ModifyStyle(LBS_USETABSTOPS, 0, SWP_FRAMECHANGED);
			else
				((CAcadColorListBox*)pControl)->ModifyStyle(0, LBS_USETABSTOPS, SWP_FRAMECHANGED);
			break;
		}
		
		case nValue:
		{
			switch (pArxObject->GetType())
			{
			case CtlCheckBox:
			case CtlOptionButton:
				((CColorButton *)pControl)->SetCheck(pArxObject->GetBoolProperty(nValue));
				break;									
			case CtlRoundSlider:				
				//((CRoundSliderCtrl *)pControl)->SetText(pArxObject->GetLngProperty(nValue));
				((CRoundSliderCtrl *)pControl)->SetPos(pArxObject->GetLngProperty(nValue));				
				((CRoundSliderCtrl *)pControl)->RedrawWindow();

				break;
			case CtlSlider:
				((CSliderCtrl *)pControl)->SetPos(pArxObject->GetLngProperty(nValue));
				break;
			case CtlScrollBar:
				((CScrollBar *)pControl)->SetScrollPos(pArxObject->GetLngProperty(nValue), TRUE);
				break;
			case CtlProgress:
				((CProgressCtrl*)pControl)->SetPos(pArxObject->GetLngProperty(nValue));
				break;
			
			}
			break;
		}
		case nVScrollBar:
		{
			if (pArxObject->GetType() != CtlDwgList)
			{
				if (pArxObject->GetBoolProperty(nVScrollBar) == FALSE)
					((CAcadColorListBox*)pControl)->ModifyStyle(WS_VSCROLL, 0, SWP_FRAMECHANGED);
				else
				{
					((CAcadColorListBox*)pControl)->ModifyStyle(0, WS_VSCROLL, SWP_FRAMECHANGED);
					pControl->Invalidate();
				}
			}
			break;
		}
		
	}	
}

void CObjectDCLView::ResetImageList(CWnd *pControl, int nID, CDclControlObject *pArxObject)
{
	CImageListObject *pImageListObject = NULL;
	int nImageListIndex = pArxObject->GetImageListIndex();
	
	if (pArxObject->GetType() == CtlGrid)
	{
		if (pArxObject->m_pImageList == NULL)
		{				
			POSITION pos = m_pThisDclForm->m_ImageListCollection.FindIndex(pArxObject->GetLngProperty(nImageList));
			if (pos != NULL)
			{
				CImageListObject *pImageListObj = m_pThisDclForm->m_ImageListCollection.GetAt(pos);
				((CListCtrlEx*)pControl)->m_Child.SetImageList(&pImageListObj->m_ImageList);
			}				
		}
		else
		{
			((CListCtrlEx*)pControl)->m_Child.SetImageList(&pArxObject->m_pImageList->m_ImageList);
		}
	}

	// exit method if there is no imagelists assigned to this control
	if (nImageListIndex == nNotSet)
		return;

	
	if (m_pThisDclForm->m_ImageListCollection.IsEmpty() == TRUE)
		return;
	
	// exit method if no image lists are available
	if (m_pThisDclForm->m_ImageListCollection.GetCount() == 0)
		return;

	if (pArxObject->m_pImageList == NULL)
	{
		POSITION pos = m_pThisDclForm->m_ImageListCollection.FindIndex(nImageListIndex);
		if (pos != NULL)
			CImageListObject *pImageListObject = m_pThisDclForm->m_ImageListCollection.GetAt(pos);			
		else
			return;
	}	
	else
	{
		pImageListObject = pArxObject->m_pImageList;
	}
	switch (pArxObject->GetType())
	{
		case CtlTabStrip:
		{
			try
			{
				((CTabCtrl*)pControl)->SetImageList(&pImageListObject->m_ImageList);
			}
			catch(...)
			{
			}
			break;
		}
		case CtlTree:
		{
			//((CTreeCtrl*)pControl)->SetImageList(&pImageListObject->m_ImageList);
			break;
		}
	}
			
}
void CObjectDCLView::UpdateChildControl(CDclControlObject *pArxObject, CControlHolder *pParent)
{
	POSITION pos;
	int nCount = 0;
	pArxObject->m_pCtrlHolder->ShowWindow(FALSE);
	while(nCount < pArxObject->GetPropertyList().GetCount())
	{
		pos = pArxObject->GetPropertyList().FindIndex(nCount);
	
		if (pos != NULL)
		{
			RefCountedPtr< CPropertyObject > pProp = pArxObject->GetPropertyList().GetAt(pos);
			if (pProp != NULL)
				UpdateProperty(pProp->GetID(), pArxObject, pParent);
		}
		nCount++;
	}
	pArxObject->m_pCtrlHolder->ShowWindow(TRUE);
	
}

void CObjectDCLView::HideSizingRect()
{
	if (m_rcDrawLast.Width() == 0 && m_rcDrawLast.Height() == 0)
		return;

	HDC hdc = ::GetDC(NULL);
	::DrawFocusRect(hdc, m_rcDrawLast);	
	::ReleaseDC(NULL, hdc);
	
	m_rcDrawLast.SetRect(0,0,0,0);
	
}	



void CObjectDCLView::CopyControlToClipBoard() 
{
	CClipboardObject ClipBoardObject;

	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL)
		return;

	// if the dcl form is not valid, exit here
	if (m_pThisDclForm == NULL)
		return;
	
	
	activeProject->sDclFormCopiedFrom = m_pThisDclForm->GetKeyName();

	ClipBoardObject.mControls.AddTail(m_SelectedControl.m_pArxObject);
	int nImageListIndex = m_SelectedControl.m_pArxObject->GetImageListIndex();
	
	POSITION pos = m_pThisDclForm->m_ImageListCollection.FindIndex(nImageListIndex);
	if (pos != NULL)
	{
		CImageListObject *pImageListObj = m_pThisDclForm->m_ImageListCollection.GetAt(pos);
		ClipBoardObject.mImageListCollection.AddTail(pImageListObj);
	}

	for (int i=0; i<m_SelectedList.GetCount(); i++)
	{
		POSITION pos = m_SelectedList.FindIndex(i);
		if (pos != NULL)
		{
			CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
			if (pSelControl != NULL)
			{
				if (pSelControl->m_nIndex > nNotSet)	
				{
					ClipBoardObject.mControls.AddTail(pSelControl->m_pArxObject);

					nImageListIndex = pSelControl->m_pArxObject->GetImageListIndex();
	
					POSITION pos = m_pThisDclForm->m_ImageListCollection.FindIndex(nImageListIndex);
					if (pos != NULL)
					{
						CImageListObject *pImageListObj = m_pThisDclForm->m_ImageListCollection.GetAt(pos);
						ClipBoardObject.mImageListCollection.AddTail(pImageListObj);
					}
				}
			}
		}
	}

   // CG: This block was added by the Clipboard Assistant component
	CSharedFile memFile;
	CArchive ar(&memFile, CArchive::store|CArchive::bNoFlushOnDelete);
	UINT m_nClipboardFormat = RegisterClipboardFormat(gpszCliboardFormatName);

	// serialize data to archive object       
	ClipBoardObject.Serialize(ar);

	ar.Flush();
	HGLOBAL hData = memFile.Detach();

	// _MFC_VER might need to be updated.
	#if _MFC_VER <= 0x0420
		::GlobalUnlock(hData);
	#endif

	if (OpenClipboard())
	{
		
		::SetClipboardData(m_nClipboardFormat, hData);
		CloseClipboard();
	}


}



void CObjectDCLView::DeleteSelectedControls() 
{
	bool FireTabDeleted = false;
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
		return;

	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();

	if (m_SelectedControl.m_pArxObject->GetType() != CtlFileDlgCtrl)
	{

		GetDocument()->SetModifiedFlag(TRUE);

		// to delete first we are going to move it off the visible area
		CRect rc(nm100,nm100,nm99,nm99);
		m_SelectedControl.m_pControl->MoveWindow(rc, TRUE);

		// now we have to ensure it was moved (it's an ActiveX problem to work around because some controls won't be moved)
		CRect rcNew;
		m_SelectedControl.m_pControl->GetWindowRect(&rcNew);
		ScreenToClient(rcNew);

		// if the control was not moved then we hide it. Other wise we can't because some controls destroy themselves if they are hidden.
		if (rc != rcNew)
			m_SelectedControl.m_pControl->ShowWindow(FALSE);
		
		m_SelectedControl.m_pArxObject->m_Delete = TRUE;

		// add the new control to the Zorder list
		pZOrderList->RemoveControlFromList(m_SelectedControl.m_pArxObject->GetStrProperty(nName));
		

		if (m_SelectedControl.m_pArxObject->GetType() == CtlTabStrip)
			FireTabDeleted = true;
			

		HideGrips();

		// add the event to the undo list
		CUndoActions *pUndo = new CUndoActions(
			uaDeleted,
			m_SelectedControl.m_pArxObject,
			m_SelectedControl.m_pControl);
		m_UndoList.AddTail(pUndo);
	}
	for (int i=0; i<m_SelectedList.GetCount(); i++)
	{
		POSITION pos = m_SelectedList.FindIndex(i);
		if (pos != NULL)
		{
			CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
			if (pSelControl != NULL)
			{
				if (pSelControl->m_nIndex > nNotSet && pSelControl->m_pArxObject->GetType() != CtlFileDlgCtrl)	
				{
					pSelControl->m_pControl->ShowWindow(FALSE);
					((CControlHolder*)pSelControl->m_pControl)->SetSelected(FALSE);
					pSelControl->m_pArxObject->m_Delete = TRUE;	
					pZOrderList->RemoveControlFromList(pSelControl->m_pArxObject->GetStrProperty(nName));
					if (m_SelectedControl.m_pArxObject->GetType() == CtlTabStrip)
						FireTabDeleted = true;
					// add the event to the undo list
					CUndoActions *pUndo = new CUndoActions(
						uaDeleted,
						pSelControl->m_pArxObject,
						pSelControl->m_pControl);
					m_UndoList.AddTail(pUndo);
				}
			}
		}
	}
	
	if (FireTabDeleted)	
		FireTabControlDeleted();
}

void CObjectDCLView::UpdateFont(CString sName) 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
		return;

	GetDocument()->SetModifiedFlag(TRUE);

	
	if (m_SelectedControl.m_pArxObject->GetType() == CtlActiveX)
	{	
		UpdateAxFont(&m_SelectedControl, nLabelName, false, nNotSet, sName);		
		// add the event to the undo list
		CUndoActions *pUndo = new CUndoActions(
			uaFontName,
			m_SelectedControl.m_pArxObject,
			m_SelectedControl.m_pControl);
		
	}
	else
	{	
		// get the  property object
		RefCountedPtr< CPropertyObject > pProp = m_SelectedControl.m_pArxObject->GetPropertyObject(nLabelName);
		
		if (pProp != NULL)
		{
			// add the event to the undo list
			CUndoActions *pUndo = new CUndoActions(
				uaFontName,
				m_SelectedControl.m_pArxObject,
				m_SelectedControl.m_pControl);
			
			// and store it's current value
			pUndo->sString = pProp->GetStringValue();
			m_UndoList.AddTail(pUndo);
			
			// update the property 
			pProp->SetStringValue(sName);

			// call the method to update the control
			UpdateProperty(nLabelName, m_SelectedControl.m_pArxObject, (CControlHolder*)m_SelectedControl.m_pArxObject->m_pCtrlHolder);
		}
	}

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos != NULL)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->m_nIndex > nNotSet)	
			{
				if (pSelControl->m_pArxObject->GetType() == CtlActiveX)
				{	
					UpdateAxFont(pSelControl, nLabelName, false, nNotSet, sName);		

					// add the event to the undo list
					CUndoActions *pUndo = new CUndoActions(
						uaFontName,
						pSelControl->m_pArxObject,
						pSelControl->m_pControl);
	
				}
				else
				{		
					// get the  property object
					RefCountedPtr< CPropertyObject > pProp = pSelControl->m_pArxObject->GetPropertyObject(nLabelName);
					
					if (pProp != NULL)
					{
						// add the event to the undo list
						CUndoActions *pUndo = new CUndoActions(
							uaFontName,
							pSelControl->m_pArxObject,
							pSelControl->m_pControl);
		
						// and store it's current value
						pUndo->sString = pProp->GetStringValue();
						m_UndoList.AddTail(pUndo);
						
						// update the property 
						pProp->SetStringValue(sName);
						
						// call the method to update the control
						UpdateProperty(nLabelName, pSelControl->m_pArxObject, (CControlHolder*)pSelControl->m_pArxObject->m_pCtrlHolder);
					}
				}
			}
		}		
		
	}	
}

void CObjectDCLView::UpdateAxFont(CSelectedControl *pSelControl, int nId, bool bBool, int nSize, CString sName)
{
	CString	sFontName;
	try
	{
		RefCountedPtr< CPropertyObject > pProp;
		RefCountedPtr< CPropertyObject > pFontProp = NULL;
		CDclControlObject *pCtrl = pSelControl->m_pArxObject;
		POSITION pos = pCtrl->GetPropertyList().GetHeadPosition();
	
		while (pos != NULL)
		{
			pProp = pCtrl->GetPropertyList().GetNext(pos);
			if (pProp->GetType() == PropActiveXProp)
			{
				if (pProp->GetAxInterfaceDescriptorPtr()->GetActiveXProperyGuid() == IID_IFontDisp ||
						pProp->GetAxInterfaceDescriptorPtr()->GetActiveXProperyGuid() == IID_IFont)
				{
					pFontProp = pProp;
					break;
				}
			}
		}
		if (pFontProp == NULL) 
			return;
		
		CAxContainer *axContainer = ((CControlHolder*)pCtrl->m_pCtrlHolder)->GetActiveXCtrl();
		COleFont font;
		if (pFontProp->GetAxInterfaceDescriptorPtr()->GetPropGet())
			font = axContainer->GetFont(pFontProp->GetAxInterfaceDescriptorPtr()->GetPropGet()->Id);
		else if (pFontProp->GetAxInterfaceDescriptorPtr()->GetProp())
			font = axContainer->GetFont(pFontProp->GetAxInterfaceDescriptorPtr()->GetProp()->Id);
		else if (pFontProp->GetAxInterfaceDescriptorPtr()->GetPropPut())
			font = axContainer->GetFont(pFontProp->GetAxInterfaceDescriptorPtr()->GetPropPut()->Id);
		else
			return;

		CString	sFontName = font.GetName();
		CY cyFontSize = font.GetSize();
		BOOL m_bBold = font.GetBold();
		BOOL m_bUnderline = font.GetUnderline();
		BOOL m_bStrike = font.GetStrikethrough();
		BOOL m_bItalic = font.GetItalic();
		int m_nFontWeight = font.GetWeight();
		int m_nFontCharset = font.GetCharset();


		switch (nId)
		{
			case nLabelName:
				{
				sFontName = sName;
				break;
				}
			case nLabelSize:
				{
				cyFontSize.Lo = nSize * 10000;
				cyFontSize.Hi = 0;
				break;
				}
			case nLabelBold:
				{
				m_bBold = bBool;
				break;
				}
			case nLabelItalic:
				{
				m_bItalic = bBool;
				break;
				}
			case nLabelUnderline:
				{
				m_bUnderline = bBool;
				break;
				}
		}


		font.SetName(sFontName);
		font.SetSize( cyFontSize );
		font.SetWeight(m_nFontCharset);	
		font.SetBold(m_bBold);	
		font.SetUnderline(m_bUnderline);
		font.SetItalic(m_bItalic);
		font.SetStrikethrough(m_bStrike);		

		if (pFontProp->GetAxInterfaceDescriptorPtr()->GetPropPut())
			axContainer->SetFont(pFontProp->GetAxInterfaceDescriptorPtr()->GetPropPut()->Id, font);
		else if (pFontProp->GetAxInterfaceDescriptorPtr()->GetProp())
			axContainer->SetFont(pFontProp->GetAxInterfaceDescriptorPtr()->GetProp()->Id, font);
		else if (pFontProp->GetAxInterfaceDescriptorPtr()->GetPropGet())
			axContainer->SetFont(pFontProp->GetAxInterfaceDescriptorPtr()->GetPropGet()->Id, font);
		
	}
	catch (...)
	{
		return;
	}

}
void CObjectDCLView::UpdateFontBool(bool bBool, PropertyId nId)
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
		return;

	RefCountedPtr< CPropertyObject > pProp = NULL;
	
	GetDocument()->SetModifiedFlag(TRUE);

	if (m_SelectedControl.m_pArxObject->GetType() == CtlActiveX)
	{	
		UpdateAxFont(&m_SelectedControl, nId, bBool);		
		// add the event to the undo list
		CUndoActions *pUndo = new CUndoActions(
			uaFontBold,
			m_SelectedControl.m_pArxObject,
			m_SelectedControl.m_pControl);
	}
	else
	{
		// get the  property object
		pProp = m_SelectedControl.m_pArxObject->GetPropertyObject(nId);
		
		if (pProp != NULL)
		{
			// add the event to the undo list
			CUndoActions *pUndo = new CUndoActions(
				uaFontBold,
				m_SelectedControl.m_pArxObject,
				m_SelectedControl.m_pControl);

			// and store it's current value
			pUndo->bBool = pProp->GetBooleanValue();
			m_UndoList.AddTail(pUndo);
			
			// update the property 
			pProp->SetBooleanValue(bBool);

			// call the method to update the control
			UpdateProperty(nLabelName, m_SelectedControl.m_pArxObject, (CControlHolder*)m_SelectedControl.m_pArxObject->m_pCtrlHolder);
		}
	}

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos != NULL)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->m_nIndex > nNotSet)	
			{
				if (pSelControl->m_pArxObject->GetType() == CtlActiveX)
				{
					UpdateAxFont(pSelControl, nId, bBool);
					// add the event to the undo list
					CUndoActions *pUndo = new CUndoActions(
						uaFontBold,
						pSelControl->m_pArxObject,
						pSelControl->m_pControl);
		
				}
				else
				{				
					// get the  property object
					pProp = pSelControl->m_pArxObject->GetPropertyObject(nId);
					
					if (pProp != NULL)
					{						
						// add the event to the undo list
						CUndoActions *pUndo = new CUndoActions(
							uaFontBold,
							pSelControl->m_pArxObject,
							pSelControl->m_pControl);

						// and store it's current value
						pUndo->bBool = pProp->GetBooleanValue();
						m_UndoList.AddTail(pUndo);
						
						// update the property 
						pProp->SetBooleanValue(bBool);
						
						// call the method to update the control
						UpdateProperty(nLabelName, pSelControl->m_pArxObject, (CControlHolder*)pSelControl->m_pArxObject->m_pCtrlHolder);
					}
				}
			}
		}		
	}	
}

void CObjectDCLView::UpdateFontSize(int nSize) 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
		return;

	GetDocument()->SetModifiedFlag(TRUE);

	
	if (m_SelectedControl.m_pArxObject->GetType() == CtlActiveX)
	{	
		UpdateAxFont(&m_SelectedControl, nLabelSize, false, nSize);		

		// add the event to the undo list
		CUndoActions *pUndo = new CUndoActions(
			uaFontSize,
			m_SelectedControl.m_pArxObject,
			m_SelectedControl.m_pControl);
	
	}
	else
	{
		// get the  property object
		RefCountedPtr< CPropertyObject > pProp = m_SelectedControl.m_pArxObject->GetPropertyObject(nLabelSize);

		if (pProp != NULL)
		{
			// add the event to the undo list
			CUndoActions *pUndo = new CUndoActions(
				uaFontSize,
				m_SelectedControl.m_pArxObject,
				m_SelectedControl.m_pControl);
			
			// and store it's current value
			pUndo->lLong= pProp->GetLongValue();
			m_UndoList.AddTail(pUndo);
			
			// update the property 
			pProp->SetLongValue(nSize);

			// call the method to update the control
			UpdateProperty(nLabelName, m_SelectedControl.m_pArxObject, (CControlHolder*)m_SelectedControl.m_pArxObject->m_pCtrlHolder);
		}
	}

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos != NULL)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->m_nIndex > nNotSet)	
			{
				if (pSelControl->m_pArxObject->GetType() == CtlActiveX)
				{	
					UpdateAxFont(pSelControl, nLabelSize, false, nSize);		

					// add the event to the undo list
					CUndoActions *pUndo = new CUndoActions(
						uaFontSize,
						pSelControl->m_pArxObject,
						pSelControl->m_pControl);
				}
				else
				{				
					// get the  property object
					RefCountedPtr< CPropertyObject > pProp = pSelControl->m_pArxObject->GetPropertyObject(nLabelSize);
					
					if (pProp != NULL)
					{
						// add the event to the undo list
						CUndoActions *pUndo = new CUndoActions(
							uaFontSize,
							pSelControl->m_pArxObject,
							pSelControl->m_pControl);
			
						// and store it's current value
						pUndo->lLong = pProp->GetLongValue();
						m_UndoList.AddTail(pUndo);
						
						// update the property 
						pProp->SetLongValue(nSize);
						
						// call the method to update the control
						UpdateProperty(nLabelName, pSelControl->m_pArxObject, (CControlHolder*)pSelControl->m_pArxObject->m_pCtrlHolder);
					}
				}
			}
		}		
	}	
}


void CObjectDCLView::PasteFromClipBoard()
{
	CClipboardObject ClipBoardObject; 

	// CG: This block was added by the Clipboard Assistant component
	if (OpenClipboard())
	{
		UINT m_nClipboardFormat = RegisterClipboardFormat(gpszCliboardFormatName);
		HANDLE hData = ::GetClipboardData(m_nClipboardFormat);
		if (hData != NULL)
		{
			CSharedFile memFile;
			memFile.SetHandle(hData,FALSE);
			CArchive ar(&memFile, CArchive::load);
			
			// Serialize data to document
			ClipBoardObject.Serialize(ar);
			ar.Close();
			
			#if _MFC_VER <= 0x0420
				::GlobalUnlock(memFile.Detach());
			#else
				memFile.Detach();
			#endif
		}

		CloseClipboard();
	}
	else
		return;
	

	int nCount = ClipBoardObject.mControls.GetCount();
	int nCopied = 0;
	
	// if the dcl form is not valid, exit here
	if (m_pThisDclForm == NULL)
		return;
	
	
	if (ClipBoardObject.mControls.GetCount() > 0)
	{		
		CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
		pZOrderList->m_pView = this;
		pZOrderList->ClearSelection();
	
		HideGrips();
		// clear the previous selection
		m_SelectedControl.m_nIndex = nNotSet;
		m_SelectedControl.m_pArxObject = NULL;
		m_SelectedControl.m_pControl = NULL;
		ClearSelection();
		
		// create a position variable to hold the counter increment
		POSITION pos;	
		
		// set start position for navigating clipboard controls
		pos = ClipBoardObject.mControls.GetHeadPosition();

		// do loop to navigate clip board controls
		while (pos != NULL)
		{
			// get current clipboard control
			CDclControlObject* pCopyOfArxControlObject = ClipBoardObject.mControls.GetNext(pos);
			pCopyOfArxControlObject->SetOwner(m_pThisDclForm);
					
			ClearEventProperties(pCopyOfArxControlObject);
			if (pCopyOfArxControlObject->GetType() == CtlFileDlgCtrl)
				continue;

			bool bContinue = false;
			if (nCurrentPurchaseMode != nPurchasedEnt)
			{
				switch(pCopyOfArxControlObject->GetType())
				{
				case CtlGrid:
					bContinue = true;
					break;
				}
			}
			if (nCurrentPurchaseMode != nPurchasedPro &&
				nCurrentPurchaseMode != nPurchasedEnt)
			{
				switch(pCopyOfArxControlObject->GetType())
				{
				case CtlDwgList:
				case CtlBlockView:
				case CtlImageComboBox:
					bContinue = true;
					break;
				}
			}
			if (nCurrentPurchaseMode == nPurchasedLT)
			{
				switch(pCopyOfArxControlObject->GetType())
				{
				case CtlPictureBox:
				case CtlTabStrip:
				case CtlMonth:
				case CtlSpinButton:
				case CtlStaticURL:
				case CtlHtmlCtrl:
				case CtlDwgPreview:
				case CtlListView:
				case CtlGrid:
				case CtlActiveX:
				case CtlDwgList:
				case CtlFileDlgCtrl:
				case CtlBlockView:
				case CtlHatch:
				case CtlBlockList:
					bContinue = true;
					break;
				}
			}
			
			if (bContinue)
				continue;

			//if the copy destination is the same as the source
			if (activeProject->sDclFormCopiedFrom == m_pThisDclForm->GetKeyName())
			{
				// make the control be offset by nControlOffset
				pCopyOfArxControlObject->SetLongProperty(nLeft, pCopyOfArxControlObject->GetLngProperty(nLeft) + nControlOffset);
				pCopyOfArxControlObject->SetLongProperty(nTop, pCopyOfArxControlObject->GetLngProperty(nTop) + nControlOffset);
		
				// get the next available name for the control
				CString sControlName = FindNextControlName(GetControlName(pCopyOfArxControlObject->GetType()));	
				pCopyOfArxControlObject->SetStringProperty(nName, sControlName);			
				pCopyOfArxControlObject->ForceUpdateGlobalVariable(sControlName);
			}
			else
				pCopyOfArxControlObject->ForceUpdateGlobalVariable(m_pThisDclForm->GetKeyName());
			
			CRect rcThis;
			GetClientRect(&rcThis);
			
			// check to make sure the control in inside the form
			int nCtrlLeft = pCopyOfArxControlObject->GetLngProperty(nLeft);
			if (nCtrlLeft > rcThis.Width())
			{
				int nCtrlWidth = pCopyOfArxControlObject->GetLngProperty(nWidth);
				nCtrlLeft = rcThis.Width() - nCtrlWidth;
				pCopyOfArxControlObject->SetLongProperty(nLeft, nCtrlLeft);
			}
			
			// check to make sure the control in inside the form
			int nCtrlTop = pCopyOfArxControlObject->GetLngProperty(nTop);
			if (nCtrlTop > rcThis.Height())
			{
				int nCtrlWidth = pCopyOfArxControlObject->GetLngProperty(nHeight);
				nCtrlTop = rcThis.Height() - nCtrlWidth;
				pCopyOfArxControlObject->SetLongProperty(nTop, nCtrlTop);
			}

			// add current clipboard control to DclFormObject
			m_pThisDclForm->AddControl(pCopyOfArxControlObject);
			
			CRect rcPos(0,0,0,0);
			// call the method to add the CWnd control 
			CWnd *pControl = AddCWndControl(pCopyOfArxControlObject, rcPos, true);
			if (pControl == NULL)
				return;
			
			// add the new control to the Zorder list
			pZOrderList->AddControlToList(pCopyOfArxControlObject->GetStrProperty(nName), pCopyOfArxControlObject->GetType());
			pZOrderList->SelectItem(pCopyOfArxControlObject->GetStrProperty(nName), false);			
			// call the method to setup the fonts in the font tool bar.
			theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar(pCopyOfArxControlObject);
	
			// increment counter
			nCount--;
			nCopied++;
			if (nCopied == 1)
			{
				m_SelectedControl.m_pArxObject = pCopyOfArxControlObject;
				m_SelectedControl.m_pControl = pControl;
				m_SelectedControl.m_nIndex = pCopyOfArxControlObject->m_Index;				
			}
			else
			{
				CSelectedControl *pSelCtrl = new CSelectedControl;
				pSelCtrl->m_pArxObject = pCopyOfArxControlObject;
				pSelCtrl->m_pControl = pControl;				
				pSelCtrl->m_nIndex = pCopyOfArxControlObject->m_Index;
				((CControlHolder*)pControl)->SetSelected(TRUE);
				m_SelectedList.AddTail(pSelCtrl);
			}			
		}
		// call method to ensure the selection set is highlighted
		CRect rcCtrl;
		if (m_SelectedControl.m_pControl == NULL)
		{
			CalcAllOffsets();
			return;
		}
		m_SelectedControl.m_pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		ShowGripRects(TRUE, rcCtrl);
		MoveGripRectsForward();
		ShowSelection();		
	}	
		
	CalcAllOffsets();
}
void CObjectDCLView::SetupDragResize(int nQuadrant) 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return;
	}

	m_bDrawing = false;
	m_MouseDown = true;
	m_nResizeQuadrant = nQuadrant;	
	CRect rcPos;
	m_SelectedControl.m_pControl->GetWindowRect(&rcPos);
	
	// set the x & y coord's
	m_DrawStartPoint.x = rcPos.left;
	m_DrawStartPoint.y = rcPos.top;
	m_rcDraw = rcPos;
	HideSizingRect();

}

void CObjectDCLView::ClearEventProperties(CDclControlObject *pCtrl)
{
	pCtrl->ResetProperty(nEventBtnClicked);
	pCtrl->ResetProperty(nEventClicked);
	pCtrl->ResetProperty(nEventFolderChanged);
	pCtrl->ResetProperty(nEventOnHelp);
	pCtrl->ResetProperty(nEventOnTypeChange);
	pCtrl->ResetProperty(nEventEditChanged);
	pCtrl->ResetProperty(nEventDblClicked);
	pCtrl->ResetProperty(nEventSelChanged);      
	pCtrl->ResetProperty(nEventGetDayState);
	pCtrl->ResetProperty(nEventSelect);
	pCtrl->ResetProperty(nEventKillFocus);
	pCtrl->ResetProperty(nEventOutOfMemory);
	pCtrl->ResetProperty(nEventRClick);
	pCtrl->ResetProperty(nEventRDblClick);
	pCtrl->ResetProperty(nEventReturn);
	pCtrl->ResetProperty(nEventSetFocus);
	pCtrl->ResetProperty(nEventBeginLabelEdit);
	pCtrl->ResetProperty(nEventDeleteItem);
	pCtrl->ResetProperty(nEventEndLabelEdit);
	pCtrl->ResetProperty(nEventItemExpanded);
	pCtrl->ResetProperty(nEventItemExpanding);
	pCtrl->ResetProperty(nEventKeyDown);
	pCtrl->ResetProperty(nEventKeyUp);
	pCtrl->ResetProperty(nEventMouseDown);
	pCtrl->ResetProperty(nEventMouseUp);
	pCtrl->ResetProperty(nEventMouseMove);
	pCtrl->ResetProperty(nEventMouseMovedOff);
	pCtrl->ResetProperty(nEventMouseEntered);
	pCtrl->ResetProperty(nEventColumnClick);
	pCtrl->ResetProperty(nEventMouseDblClick);
	pCtrl->ResetProperty(nEventMouseWheel);
	pCtrl->ResetProperty(nEventPaint);
	pCtrl->ResetProperty(nEventNavigateComplete);
  pCtrl->ResetProperty(nEventSelChanging);
	pCtrl->ResetProperty(nEventChanged);
	pCtrl->ResetProperty(nEventMaxText);
  pCtrl->ResetProperty(nEventUpdate);
  pCtrl->ResetProperty(nEventDropDown);
  pCtrl->ResetProperty(nEventScroll);
	pCtrl->ResetProperty(nEventScrolled);
	pCtrl->ResetProperty(nEventReleasedCapture);
	pCtrl->ResetProperty(nFormEventClose);
	pCtrl->ResetProperty(nFormEventInitialize);
	pCtrl->ResetProperty(nFormEventSize);
	pCtrl->ResetProperty(nFormEventShow);
	pCtrl->ResetProperty(nDocEventActivated);
	pCtrl->ResetProperty(nCfgEventCancel);
	pCtrl->ResetProperty(nCfgEventOK);
	pCtrl->ResetProperty(nCfgEventHelp);
	pCtrl->ResetProperty(nCfgEventApply);
	pCtrl->ResetProperty(nEventReturnPressed);
	pCtrl->ResetProperty(nFormEventCancelClose);
  pCtrl->ResetProperty(nFormEventOnOk);
	pCtrl->ResetProperty(nOnLMouseEvent);
	pCtrl->ResetProperty(nOnMMouseEvent);
	pCtrl->ResetProperty(nOnRMouseEvent);
	pCtrl->ResetProperty(nDragnDropToAutoCAD);
	pCtrl->ResetProperty(nDragnDropFromControl);        
	pCtrl->ResetProperty(nDragnDropFromAutoCAD);
	pCtrl->ResetProperty(nDragnDropBegin);


}

void CObjectDCLView::DragResized(int nQuadrant, CPoint point) 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return;
	}

	CRect rcPos = CalcResizeRect(nQuadrant, point);
	
	HDC hdc = ::GetDC(NULL);
	if (m_rcDrawLast.Width() > 0 && m_rcDrawLast.Height() > 0)
		::DrawFocusRect(hdc, m_rcDrawLast);	
	m_rcDrawLast = rcPos;
	::DrawFocusRect(hdc, m_rcDrawLast);	
	::ReleaseDC(NULL, hdc);
	
	m_rcDraw = m_rcDrawLast;
}
CRect CObjectDCLView::CalcResizeRect(int nQuadrant, CPoint point)
{
	CRect rcForm;
	GetWindowRect(&rcForm);
	CRect rcPos = m_rcDraw;
	// set the rect acording to the quadrant dragged
	switch (nQuadrant)
	{
	case 1:
		{
			rcPos.left = rcForm.left + RoundToGridPattern(point.x) + 3;
			rcPos.top =  rcForm.top + RoundToGridPattern(point.y) + 3;
	
			// ensure the user cannot make the control have negitive values
			if (rcPos.left >= rcPos.right)
				rcPos.left = rcPos.right - m_gridSpacing;
			if (rcPos.top >= rcPos.bottom)
				rcPos.top = rcPos.bottom - m_gridSpacing;
			
			break;
		}
	case 2:
		{
			rcPos.top =  rcForm.top + RoundToGridPattern(point.y) + 3;
	
			// ensure the user cannot make the control have negitive values
			if (rcPos.top >= rcPos.bottom)
				rcPos.top = rcPos.bottom - m_gridSpacing;
			
			break;
		}
	case 3:
		{
			// get the coord	
			rcPos.right = rcForm.left + RoundToGridPattern(point.x) + 3;
			rcPos.top =  rcForm.top + RoundToGridPattern(point.y) + 3;
	
			// ensure the user cannot make the control have negitive values
			if (rcPos.top >= rcPos.bottom)
				rcPos.top = rcPos.bottom - m_gridSpacing;
			if (rcPos.right <= rcPos.left)
				rcPos.right = rcPos.left + m_gridSpacing;
			
			break;
		}
	case 4:
		{
			rcPos.left = rcForm.left + RoundToGridPattern(point.x) + 3;
			
			// ensure the user cannot make the control have negitive values
			if (rcPos.left >= rcPos.right)
				rcPos.left = rcPos.right - m_gridSpacing;
			
			break;
		}
	case 5:
		{
			// get the coord	
			rcPos.right = rcForm.left + RoundToGridPattern(point.x) + 3;
			
			// ensure the user cannot make the control have negitive values
			if (rcPos.right <= rcPos.left)
				rcPos.right = rcPos.left + m_gridSpacing;
			break;
		}
	case 6:
		{
			// get the coord	
			rcPos.bottom = rcForm.top + RoundToGridPattern(point.y) + 3;			
			rcPos.left = rcForm.left + RoundToGridPattern(point.x) + 3;
			
			// ensure the user cannot make the control have negitive values
			if (rcPos.left >= rcPos.right)
				rcPos.left = rcPos.right - m_gridSpacing;
			if (rcPos.bottom <= rcPos.top)
				rcPos.bottom = rcPos.top + m_gridSpacing;
			break;
		}
	case 7:
		{
			// get the coord	
			rcPos.bottom = rcForm.top + RoundToGridPattern(point.y) + 3;			
			
			// ensure the user cannot make the control have negitive values
			if (rcPos.bottom <= rcPos.top)
				rcPos.bottom = rcPos.top + m_gridSpacing;
			break;
		}
	case 8:
		{
			// get the coord	
			rcPos.right = rcForm.left + RoundToGridPattern(point.x) + 3;
			rcPos.bottom = rcForm.top + RoundToGridPattern(point.y) + 3;			
			
			// ensure the user cannot make the control have negitive values
			if (rcPos.right <= rcPos.left)
				rcPos.right = rcPos.left + m_gridSpacing;
			if (rcPos.bottom <= rcPos.top)
				rcPos.bottom = rcPos.top + m_gridSpacing;
			break;
		}
	}
	return rcPos;
}

void CObjectDCLView::CompletedDragResize(int nQuadrant, CPoint point) 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideSizingRect();
		m_bDrawing = false;	
		HideGrips();
		return;
	}
	m_MouseDown = false;
	
	m_GripRect1.m_bResizing = false;
	m_GripRect2.m_bResizing = false;
	m_GripRect3.m_bResizing = false;
	m_GripRect4.m_bResizing = false;
	m_GripRect5.m_bResizing = false;
	m_GripRect6.m_bResizing = false;
	m_GripRect7.m_bResizing = false;
	m_GripRect8.m_bResizing = false;
	m_nResizeQuadrant = nNotSet;	
	
	CRect rcPos;
	
	// hide the sizing rect.
	HideSizingRect();
	m_bDrawing = false;	
		
	if (m_rcDraw.Width() <= 0 ||
		m_rcDraw.Height() <= 0 ||
		m_rcDraw.right <= 0 ||
		m_rcDraw.bottom <= 0)
		return;

	// get the new size of the control
	rcPos = m_rcDraw;

	// we have to move the control up by -3,-3 because of a positioning bug that I can't seem to trace down to.
	rcPos.left -= 3;
	rcPos.right -= 3;
	rcPos.top -= 3;
	rcPos.bottom -= 3;
		
	CWnd *pControl = ((CControlHolder*)m_SelectedControl.m_pControl)->GetChildControl();
	

	CRect rcOldPos;
	m_SelectedControl.m_pControl->GetWindowRect(rcOldPos);
	ScreenToClient(rcOldPos);
	CRect rcForm;
	GetWindowRect(&rcForm);
	CRect rcNewPos;
	rcNewPos.left =	rcPos.left - rcForm.left,
	rcNewPos.top = rcPos.top - rcForm.top,
	rcNewPos.right = rcNewPos.left + rcPos.Width();
	rcNewPos.bottom = rcNewPos.top + rcPos.Height();
	
	// move the control
	m_SelectedControl.m_pControl->MoveWindow(rcNewPos, TRUE);
	
	if (m_SelectedControl.m_pArxObject->GetType() == CtlTabStrip)
	{
		//short nTabHeight = GetSelectedTabClientHeight();
		//short nTabWidth = GetSelectedTabClientWidth();
		ResizeChildTabPanes();
	}
	CRect rc;
	m_SelectedControl.m_pControl->GetWindowRect(&rc);
	ScreenToClient(rc);
/*	if (rc != rcNewPos)
	{
		m_SelectedControl.m_pControl->ShowWindow(TRUE);
		ShowGripRects(TRUE, rc);
		return;
	}
*/
	// add the event to the undo list
	CUndoActions *pUndo = new CUndoActions(
		uaMoved,
		m_SelectedControl.m_pArxObject,
		m_SelectedControl.m_pControl);		
	
	pUndo->rcPos = rcOldPos;
	m_UndoList.AddTail(pUndo);
	

	CRect rcChild(0,0, rcPos.Width(), rcPos.Height());
	// resize the child control
	pControl->MoveWindow(rcChild, TRUE);
	
	rcPos = rcNewPos;

	pControl->GetWindowRect(&rcChild);

	rcPos.right = rcPos.left + rcChild.Width();
	rcPos.bottom = rcPos.top + rcChild.Height();

	// set the position properties
	m_SelectedControl.m_pArxObject->SetLongProperty(nLeft, rcPos.left);
	m_SelectedControl.m_pArxObject->SetLongProperty(nTop, rcPos.top);
	m_SelectedControl.m_pArxObject->SetLongProperty(nWidth, rcPos.Width());
	m_SelectedControl.m_pArxObject->SetLongProperty(nHeight, rcPos.Height());
		
	CRect rcThis;
	GetClientRect(&rcThis);
	

	// set the offset position properties
	CalcControlOffsetDistances(m_SelectedControl.m_pArxObject, rcPos);
	/*
	m_SelectedControl.m_pArxObject->SetLongProperty(nLeftFromRight, rcThis.Width() - rcPos.left);
	m_SelectedControl.m_pArxObject->SetLongProperty(nTopFromBottom, rcThis.Height() - rcPos.top);
	m_SelectedControl.m_pArxObject->SetLongProperty(nRightFromRight, rcThis.Width() - rcPos.right);
	m_SelectedControl.m_pArxObject->SetLongProperty(nBottomFromBottom, rcThis.Height() - rcPos.bottom);
	*/


	// call method to update the position of the grip rects
	ShowGripRects(TRUE, rcPos);
	CheckAutoSizeProp(m_SelectedControl.m_pArxObject, (CControlHolder*)m_SelectedControl.m_pControl);
	pControl->ShowWindow(TRUE);
	m_SelectedControl.m_pControl->ShowWindow(TRUE);
	UpdateClientHeight(m_SelectedControl.m_pArxObject, pControl);

	if (m_SelectedControl.m_pArxObject->GetType() == CtlTabStrip)
	{
		ResizeChildTabPanes();
	}

	FireControlSelected(m_SelectedControl.m_pArxObject);
	FireSetUndo();
}

void CObjectDCLView::UpdateClientHeight(CDclControlObject* pArxObject, CWnd *pControl) 
{
	size_t nCount = 0;
	int nBottom = 0;

	if (m_SelectedControl.m_pArxObject == NULL)
		return;

	if (m_SelectedControl.m_pArxObject->GetType() != CtlTabStrip)
		return;

	while (nCount < pArxObject->CountPropertyListItems(nTabsCaption))
	{
		// get the new items' rectangle
		CRect rcTab;
		((CTabCtrl*)pControl)->GetItemRect( 
			nCount - 1,
			&rcTab);

		// if the bottom of this tab is greater than the last setting
		// update the bottom value
		if (rcTab.bottom > nBottom)
			nBottom = rcTab.bottom;
	
		nCount++;
	}
	
	// get the height of the tab control
	CRect rcTabCtrl;
	if (IsWindow(pControl->m_hWnd))
		((CTabCtrl*)pControl)->GetWindowRect(&rcTabCtrl);
	
	// subtract the lowest tab bottom to get the control area height
	pArxObject->m_ClientHeight = rcTabCtrl.Height() - nBottom;
	
	// inform the parent a tab control has been resized
    FireTabResized(pArxObject->m_ClientHeight);
}

void CObjectDCLView::OnSize(UINT nType, int cx, int cy) 
{
	InvalidateRect(&m_rcGripRect);
	CView::OnSize(nType, cx, cy);
	if (m_pThisDclForm != NULL)
	{
		CDclControlObject *pDclProps = m_pThisDclForm->GetControlProperties();
	
		CRect rc;
		GetParentFrame()->GetWindowRect(&rc);

		GetClientRect(&rc);
		 
		pDclProps->SetLongProperty(nWidth, rc.Width());
		pDclProps->SetLongProperty(nHeight, rc.Height());
		m_pThisDclForm->m_bUsesClientRect = TRUE;

		GetParentFrame()->GetWindowRect(&rc);

		AdjustOffsets(rc.Width(), rc.Height());

		CPropertyTabPane* pPropTabs = theEditorWorkspace.GetPropertyTabs();
		if (pPropTabs->m_PropertiesTabPane.GetPropertiesCtrl().m_pView == this)
		{
			pPropTabs->m_PropertiesTabPane.GetPropertiesCtrl().Invalidate();
		}
	}
	InvalidateRect(&m_rcGripRect);
}	

void CObjectDCLView::UpdateGripPos()
{
	// size-grip goes bottom right in the client area

	GetParentFrame()->GetClientRect(&m_rcGripRect);

	m_rcGripRect.left = m_rcGripRect.right - m_szGripSize.cx;
	m_rcGripRect.top = m_rcGripRect.bottom - m_szGripSize.cy;
}
void CObjectDCLView::AdjustOffsets(int cx, int cy) 
{
	
	if (m_pThisDclForm == NULL)
		return;

	CRect rcThis;
	GetClientRect(&rcThis);

	// do loop to reset the offset distance properties
	for (int i=1; i<m_pThisDclForm->GetControlList().GetCount(); i++)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			if (pArxObject != NULL)
			{
				// get the position of the control
				CRect rcPos(
					pArxObject->GetLngProperty(nLeft),
					pArxObject->GetLngProperty(nTop),
					pArxObject->GetLngProperty(nWidth),
					pArxObject->GetLngProperty(nHeight));

				// set the offset position properties
				CalcControlOffsetDistances(pArxObject, rcPos);
				/*
				pArxObject->SetLongProperty(nLeftFromRight, rcThis.Width() - rcPos.left);
				pArxObject->SetLongProperty(nTopFromBottom, rcThis.Height() - rcPos.top);
				pArxObject->SetLongProperty(nRightFromRight, rcThis.Width() - rcPos.right);
				pArxObject->SetLongProperty(nBottomFromBottom, rcThis.Height() - rcPos.bottom);
				*/
			}
		}
	}
	
}


// refresh the selected control
void CObjectDCLView::RefreshSelectControl() 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return;
	}
	
	// if the program made it this far, destroy the previous control
	CControlHolder *pParent = (CControlHolder*)m_SelectedControl.m_pArxObject->m_pCtrlHolder;
	if (pParent == NULL)
		return;
	CWnd *pControl = pParent->GetChildControl();
	if (pControl == NULL)
		return;
	
	CRect rcControl;
	pParent->GetWindowRect(&rcControl);
	pControl->MoveWindow(0,0,rcControl.Width(),rcControl.Height(), TRUE);
	pParent->ShowWindow(TRUE);
	pControl->ShowWindow(TRUE);
	pControl->RedrawWindow();
	
}


void CObjectDCLView::ZOrdedSelectedControls(short direction) 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return;
	}
	
	if (direction == 0)
		m_SelectedControl.m_pControl->SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);		
	else
		m_SelectedControl.m_pControl->SetWindowPos(&CWnd::wndBottom, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	
	// call method to adjust the control's placement
	m_pThisDclForm->ReorderControl( m_SelectedControl.m_pArxObject, (direction != 0) );
	
	for (int i=0; i<m_SelectedList.GetCount(); i++)
	{
		POSITION pos = m_SelectedList.FindIndex(i);
		if (pos != NULL)
		{
			CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
			if (pSelControl != NULL)
			{
				if (pSelControl->m_nIndex > nNotSet)	
				{
					if (direction == 0)
						pSelControl->m_pControl->SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
					else
						pSelControl->m_pControl->SetWindowPos(&CWnd::wndBottom, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
					// call method to adjust the control's placement
					m_pThisDclForm->ReorderControl( pSelControl->m_pArxObject, (direction != 0) );
				}
			}
		}
	}
	MoveGripRectsForward();
	UpdateZOrderList();
}

CSize CObjectDCLView::GetControlSize(CWnd *pControl, int nControlType)
{
	CSize retSize;
	
	if (pControl != NULL)
	{
		CRect rcControl;
		
		if (nControlType == CtlMonth)
		{
			((CMonthCalCtrl*)pControl)->GetMinReqRect(&rcControl);
			retSize.cx = rcControl.Width();
			retSize.cy = rcControl.Height();	
		}
		else
		{
			pControl->GetWindowRect(&rcControl);
			retSize.cx = rcControl.Width();
			retSize.cy = rcControl.Height();		
		}
	}
	return retSize;
}

short CObjectDCLView::GetSelectedTabClientWidth() 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return nNotSet;
	}
	
	return (short)m_SelectedControl.m_pArxObject->GetLngProperty(nWidth);
}

short CObjectDCLView::GetSelectedTabClientHeight() 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return nNotSet;
	}

	int nCount = 0;
	int nBottom = 0;

	CTabCtrl *pControl = (CTabCtrl*)((CControlHolder*)m_SelectedControl.m_pControl)->GetChildControl();
	
	
	if (pControl == NULL)
		return nNotSet;

	int nTotal = m_SelectedControl.m_pArxObject->CountPropertyListItems(nTabsCaption);
	while (nCount < nTotal)
	{
		// get the new items' rectangle
		CRect rcTab;
		pControl->GetItemRect( 
			nCount - 1,
			&rcTab);

		// if the bottom of this tab is greater than the last setting
		// update the bottom value
		if (rcTab.bottom > nBottom)
			nBottom = rcTab.bottom;
	
		nCount++;
	}
	
	// get the height of the tab control
	CRect rcTabCtrl;
	if (IsWindow(pControl->m_hWnd))
		pControl->GetWindowRect(&rcTabCtrl);
	
	// subtract the lowest tab bottom to get the control area height
	m_SelectedControl.m_pArxObject->m_ClientHeight = rcTabCtrl.Height() - nBottom;
    int nClientHeight = m_SelectedControl.m_pArxObject->m_ClientHeight;

	CRect rcWin;
	CRect rcClient;
	
	GetParentFrame()->GetWindowRect(&rcWin);
	GetClientRect(&rcClient);

	return rcWin.Height() - rcClient.Height();
	return nClientHeight + rcWin.Height() - rcClient.Height();
}


void CObjectDCLView::DisplayControls(CDclFormObject *pDclForm) 
{
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
	m_pThisDclForm = pDclForm;
	if (m_pThisDclForm == NULL)
		return;

	pZOrderList->ClearList(this);
	
	CRect rcThis;
	GetClientRect(&rcThis);
    ScreenToClient(rcThis);

	// clear the zorder list box
	pZOrderList->ClearList(this);
	pZOrderList->SetRedraw(FALSE); // turn drawing off regardless of list mode

	// do loop to reset the offset distance properties
	for (int i=1; i<m_pThisDclForm->GetControlList().GetCount(); i++)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			if (pArxObject != NULL)
			{
				bool bMoveControl = false;
				// get the position of the control
				CRect rcPos;
				rcPos.left = pArxObject->GetLngProperty(nLeft);
				rcPos.top = pArxObject->GetLngProperty(nTop);
				rcPos.right = rcPos.left + pArxObject->GetLngProperty(nWidth);
				rcPos.bottom = rcPos.top + pArxObject->GetLngProperty(nHeight);
				
				if (rcPos.Width() <= 0)
				{
					rcPos.right = rcPos.left + nDeMoveBy50;
					bMoveControl = true;
				}
				if (rcPos.Height() <= 0)
				{
					rcPos.bottom = rcPos.top + nDeMoveBy50;
					bMoveControl = true;
				}
				// if the control was just moved
				if (bMoveControl == true)
				{
					// update the postion properties
					pArxObject->SetLongProperty(nLeft, rcPos.left);
					pArxObject->SetLongProperty(nTop, rcPos.top);
					pArxObject->SetLongProperty(nWidth, rcPos.Width());
					pArxObject->SetLongProperty(nHeight, rcPos.Height());
					// set the offset position properties
					CalcControlOffsetDistances(pArxObject, rcPos);
					/*
					pArxObject->SetLongProperty(nLeftFromRight, rcThis.Width() - rcPos.left);
					pArxObject->SetLongProperty(nTopFromBottom, rcThis.Height() - rcPos.top);
					pArxObject->SetLongProperty(nRightFromRight, rcThis.Width() - rcPos.right);
					pArxObject->SetLongProperty(nBottomFromBottom, rcThis.Height() - rcPos.bottom);
					*/
				}
				
				
				// call the method to add the new control
				CWnd *pControl = AddCWndControl( pArxObject, rcPos, false);
				if (pControl == NULL)
				{
					HideGrips();
					HideSizingRect();
					//return;
				}
				
				// if the control has not been deleted.
				if (!pArxObject->m_Delete)
				{
					// here were are going to add the newly created control to the zorder list				
					pZOrderList->AddControlToList(pArxObject->GetStrProperty(nName), pArxObject->GetType());
					pZOrderList->m_pView = this;
				}

			}
		}
	}

	pZOrderList->SetRedraw(TRUE); // turn drawing back on and update the window
	pZOrderList->Invalidate();
	pZOrderList->UpdateWindow(); 
	
	HideGrips();
	HideSizingRect();
	// move the grips forward so they don't appear behind the controls
	MoveGripRectsForward();

	CalcAllOffsets();

	CRect rc;
	GetClientRect(&rc);
		 
	CDclControlObject *pDclProps = m_pThisDclForm->GetControlProperties();
	pDclProps->SetLongProperty(nWidth, rc.Width());
	pDclProps->SetLongProperty(nHeight, rc.Height());
	m_pThisDclForm->m_bUsesClientRect = TRUE;

//	MoveThisInPosition();
	
}
void CObjectDCLView::MoveThisInPosition() 
{
	CRect rcStartup = ((CChildFrame*)GetParentFrame())->m_rcStartup;
	if (rcStartup.left < 0)
		rcStartup.OffsetRect(rcStartup.left * nNotSet, 0);
	
	if (rcStartup.top < 0)
		rcStartup.OffsetRect(0, rcStartup.top * nNotSet);

	GetParentFrame()->MoveWindow(rcStartup,TRUE);
}

short CObjectDCLView::GetSelectedArxIndex() 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return nNotSet;
	}
	
	if (m_SelectedList.GetCount() > 0)
		return nRefreshAllProperties;
	else
		return m_SelectedControl.m_nIndex;
}

long CObjectDCLView::GetSelectedType() 
{
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return nNotSet;
	}
	

	return m_SelectedControl.m_pArxObject->GetType();
}


void CObjectDCLView::OnLostFocus() 
{
	HideGrips();
	HideSizingRect();
	ClearSelection();
	m_StandardCursorID = false;

}

BOOL CObjectDCLView::CanUndo() 
{
	if (m_UndoList.GetCount() > 0)
		return TRUE;
	else
		return FALSE;
}

void CObjectDCLView::UndoAction() 
{
	POSITION pos = m_UndoList.GetTailPosition();
	
	if (pos != NULL)
	{
		CUndoActions *pUndoAction = m_UndoList.GetAt(pos);
		if (pUndoAction != NULL)
		{
			switch (pUndoAction->m_nAction)
			{
			case uaFontName:
				{
				pUndoAction->m_pArxObject->SetStringProperty(nLabelName, pUndoAction->sString);
				
				// call the method to update the control
				UpdateProperty(nLabelName, pUndoAction->m_pArxObject, (CControlHolder*)pUndoAction->m_pArxObject->m_pCtrlHolder);

				break;
				}
			case uaFontSize:
				{
				pUndoAction->m_pArxObject->SetLongProperty(nLabelSize, pUndoAction->lLong);
				
				// call the method to update the control
				UpdateProperty(nLabelName, pUndoAction->m_pArxObject, (CControlHolder*)pUndoAction->m_pArxObject->m_pCtrlHolder);

				break;
				}
			case uaMoved:
				{
				pUndoAction->m_pControl->MoveWindow(pUndoAction->rcPos, TRUE);
				pUndoAction->m_pArxObject->SetLongProperty(nLeft, pUndoAction->rcPos.left);
				pUndoAction->m_pArxObject->SetLongProperty(nTop, pUndoAction->rcPos.top);
				pUndoAction->m_pArxObject->SetLongProperty(nWidth, pUndoAction->rcPos.Width());
				pUndoAction->m_pArxObject->SetLongProperty(nHeight, pUndoAction->rcPos.Height());
				if (m_SelectedControl.m_nIndex == pUndoAction->m_pArxObject->m_Index)
					ShowGripRects(TRUE, pUndoAction->rcPos);
				break;
				}
			case uaDeleted:
				{
				if (IsWindow(pUndoAction->m_pControl->m_hWnd))
				{
					// if the window is visible we need to move it back to it's origanal place.
					if (pUndoAction->m_pControl->IsWindowVisible())
					{
						CRect rc;
						rc.left = pUndoAction->m_pArxObject->GetLngProperty(nLeft);
						rc.top = pUndoAction->m_pArxObject->GetLngProperty(nTop);
						rc.right = rc.left + pUndoAction->m_pArxObject->GetLngProperty(nWidth);
						rc.bottom = rc.top + pUndoAction->m_pArxObject->GetLngProperty(nHeight);
						pUndoAction->m_pControl->MoveWindow(rc, TRUE);
					}
					else
						pUndoAction->m_pControl->ShowWindow(TRUE);

					
				
					pUndoAction->m_pArxObject->m_Delete = FALSE;
					FireControlInserted(
						pUndoAction->m_pArxObject, 
						pUndoAction->m_pArxObject->GetType());

					
					((CControlHolder*)pUndoAction->m_pControl)->HideGrips();
					UpdateZOrderList();
				}
				break;
				}
			case uaInsert:
				{
					// to delete first we are going to move it off the visible area
					CRect rc(nm100,nm100,nm99,nm99);
					pUndoAction->m_pControl->MoveWindow(rc, TRUE);

					// now we have to ensure it was moved (it's an ActiveX problem to work around because some controls won't be moved)
					CRect rcNew;
					pUndoAction->m_pControl->GetWindowRect(&rcNew);
					ScreenToClient(rcNew);

					// if the control was not moved then we hide it. Other wise we can't because some controls destroy themselves if they are hidden.
					if (rc != rcNew)
						pUndoAction->m_pControl->ShowWindow(FALSE);

					//pUndoAction->m_pControl->ShowWindow(FALSE);
					pUndoAction->m_pArxObject->m_Delete = TRUE;
					if (pUndoAction->m_pArxObject->GetType() == CtlTabStrip)
						FireTabControlDeleted();
					HideGrips();
					break;
				}
			}				
		}
		delete pUndoAction;
		m_UndoList.RemoveAt(pos);
	}
	
	
}

void CObjectDCLView::UpdateControl(CDclControlObject *pArxObject, PropertyId ChangedPropertyID) 
{
	
	if (m_pThisDclForm == NULL)
		return;

	// call the method to refresh the control
	RefreshChildControl(pArxObject, ChangedPropertyID);

	if (m_SelectedControl.m_nIndex != nNotSet &&
		m_SelectedControl.m_pArxObject != NULL &&
		m_SelectedControl.m_pControl != NULL)
	{
		CRect rcCtrl;
		try
		{
			if (IsWindow(m_SelectedControl.m_pControl->m_hWnd))
				m_SelectedControl.m_pControl->GetWindowRect(&rcCtrl);
			ScreenToClient(rcCtrl);
			ShowGripRects(TRUE, rcCtrl);
		}
		catch(...)
		{
		}		
	}

}

short CObjectDCLView::ResetDclFormIndex() 
{
	return GetDclFormIndex();
}

void CObjectDCLView::CheckPictureRefs() 
{
	
	for (int i=1; i<m_pThisDclForm->GetControlList().GetCount(); i++)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			int nPictureId = pArxObject->GetLngProperty(nPicture);
			if (nPictureId >= 0)
			{
				CWnd *pCtrl = pArxObject->m_pCtrlHolder;				
				UpdateProperty(
					nPicture,
					pArxObject,
					(CControlHolder*)pCtrl);
			}
		}
	}

}

void CObjectDCLView::SelectControl(short nArxControlIndex) 
{
	
	CDclControlObject* pArxObject = GetArxControlObject(m_pThisDclForm, nArxControlIndex);

	if (pArxObject == NULL)
		return;


	CWnd *pControl = pArxObject->m_pCtrlHolder;
	
	m_SelectedControl.m_pArxObject = pArxObject;
	m_SelectedControl.m_pControl = pControl;
	m_SelectedControl.m_nIndex = pArxObject->m_Index;
	ClearSelection();

	CRect rcCtrl;
	if (pControl != NULL)
	{
		pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		ShowGripRects(TRUE, rcCtrl);
	}

}



void CObjectDCLView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	if (m_pThisDclForm == NULL)
		return;

	// check to see if we are going to try and select controls
	CheckControlsForSelection(point, nFlags, true);

	
	int nArxIndex = m_SelectedControl.m_nIndex;

	if (nArxIndex == nNotSet)
		FireShowPropertyWizard(m_pThisDclForm->GetControlProperties());
	else
		FireShowPropertyWizard(m_SelectedControl.m_pArxObject);

	CView::OnLButtonDblClk(nFlags, point);
}

void CObjectDCLView::CalcAllOffsets() 
{
	
	CRect rcThis;
	GetClientRect(&rcThis);
	
	if (m_pThisDclForm == NULL)
		return;

	// we are going to do a loop to ensure each control's offset properties are calculated correctly
	for (int i=m_pThisDclForm->GetControlList().GetCount()-1; i> 0; i--)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			// get the control
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			if (pArxObject != NULL && pArxObject->m_Delete == FALSE)
			{
				int nW = pArxObject->GetLngProperty(nWidth);
				int nL = pArxObject->GetLngProperty(nLeft);

				CRect rcPos(pArxObject->GetLngProperty(nLeft),
							pArxObject->GetLngProperty(nTop),
							pArxObject->GetLngProperty(nWidth) - pArxObject->GetLngProperty(nLeft),
							pArxObject->GetLngProperty(nHeight) - pArxObject->GetLngProperty(nTop));
				// set the offset position properties
				CalcControlOffsetDistances(pArxObject, rcPos);
			}
		}
	}
}

void CObjectDCLView::RefreshControlsWithPictures() 
{
	CRect rcThis;
	GetClientRect(&rcThis);
	
	
	// we are going to do a loop to force all controls with a picture property to refresh
	for (int i=m_pThisDclForm->GetControlList().GetCount()-1; i> 0; i--)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			// get the control
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			if (pArxObject != NULL)
			{
				if (pArxObject->GetPropertyObject(nPicture) != NULL)
					RefreshChildControl(pArxObject, nInvalidPropertyId);
			}
		}
	}

}

void CObjectDCLView::ZOrderUpdateOfSelCtrl(short ZIndex) 
{
	
	if (m_SelectedControl.m_nIndex == nNotSet ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return;
	}
			
	// call method to adjust the control's placement
	m_pThisDclForm->ReorderControl( m_SelectedControl.m_pArxObject, size_t(ZIndex) );

	MoveGripRectsForward();
	UpdateZOrderList();
}

bool CObjectDCLView::IsTabsEnabled()
{
	if (m_pThisDclForm == NULL)
		return true;

	// do loop to find out if this form has a tab on it.
	int nCount = 1;
	while (nCount < m_pThisDclForm->GetControlList().GetCount())
	{
		// get the current position
		POSITION CtrlPos = m_pThisDclForm->GetControlList().FindIndex(nCount);
		CDclControlObject* pArxObject = m_pThisDclForm->GetControlList().GetAt(CtrlPos);
		// if it does, we must return a false to indicate the tab toolbox button should be disabled.
		if (pArxObject->GetType() == CtlTabStrip && pArxObject->m_Delete == FALSE)
			return false;
		nCount++;
	}
	// if it doesn't, we must return a true to indicate the tab toolbox button should be enabled.
	return true;
}


void CObjectDCLView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CToolBox *pToolBox = theEditorWorkspace.GetToolBox();

	if (pDeactiveView == pActivateView)
	{
		if (theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().m_pControl != m_SelectedControl.m_pArxObject)
		{
			CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
			pZOrderList->ClearList(this);
		}		
	}
	else
		((CObjectDCLView*)pDeactiveView)->m_pThisDclForm = NULL;

	if (pActivateView != NULL)
	{
		pToolBox->SetActiveView(pActivateView);
		theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetActiveView(pActivateView);

		//if (((CObjectDCLView*)pActivateView)->m_pThisDclForm != NULL)
		//{
		//	try
		//	{
		//		pDeactiveView->GetParentFrame()->SetWindowText(((CObjectDCLView*)pActivateView)->m_pThisDclForm->GetDclFormTitle());			
		//		if (m_pThisDclForm->GetType() != VdclTabForm)
		//			pDeactiveView->GetParentFrame()->SetWindowText(m_pThisDclForm->GetDclFormTitle());// set the title bar text
		//		else
		//			pDeactiveView->GetParentFrame()->SetWindowText(FindTabCaption(m_pThisDclForm));// set the title bar text
		//	}
		//	catch(...)
		//	{
		//	}
		//}		
	}
	else
	{
		if (pDeactiveView == pActivateView)
		{
			pToolBox->SetActiveView(NULL);
			theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetActiveView(NULL);
		}
		else
		{
			pToolBox->SetActiveView(pDeactiveView);
			theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetActiveView(pDeactiveView);
		}
	}

	if(pDeactiveView)
		((CObjectDCLView*)pDeactiveView)->m_StandardCursorID = false;
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	//if (pActivateView != NULL)
	//{
	//	if (((CObjectDCLView*)pDeactiveView)->m_pThisDclForm != NULL)
	//	{
	//		pDeactiveView->GetParentFrame()->SetWindowText(((CObjectDCLView*)pDeactiveView)->m_pThisDclForm->GetDclFormTitle());			
	//		if (m_pThisDclForm->GetType() != VdclTabForm)
	//			pDeactiveView->GetParentFrame()->SetWindowText(m_pThisDclForm->GetDclFormTitle()); // set the title bar text
	//		else
	//			pDeactiveView->GetParentFrame()->SetWindowText(FindTabCaption(m_pThisDclForm));// set the title bar text
	//	}		
	//}
	if (pActivateView == this)
		UpdateZOrderList();
}

void CObjectDCLView::UpdateZOrderList() 
{
	
	if (m_pThisDclForm == NULL)
		return;

	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();

	// clear the list
	pZOrderList->ClearList(this);
	pZOrderList->SetRedraw(FALSE); // turn drawing off regardless of list mode

	for (int i = 1; i<m_pThisDclForm->GetControlList().GetCount(); i++)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			// if this control is not deleted add it to the list
			if (pArxObject != NULL && pArxObject->m_Delete == FALSE)
			{				
				pZOrderList->AddControlToList(pArxObject->GetStrProperty(nName), pArxObject->GetType());
				pZOrderList->m_pView = this;
			}
		}
	}
	pZOrderList->SetRedraw(TRUE); // turn drawing back on and update the window
	pZOrderList->Invalidate();
	pZOrderList->UpdateWindow(); 
	
	if (m_pThisDclForm->m_htiTreeItem != NULL)
	{
		CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
		pProjTree->SelectItem(m_pThisDclForm->m_htiTreeItem);
	}
}
void CObjectDCLView::OnBringtofront() 
{
	ZOrdedSelectedControls(0);
	
}

void CObjectDCLView::OnSendtoback() 
{
	ZOrdedSelectedControls(1);
	
}

void CObjectDCLView::OnEditCut() 
{
	CopyControlToClipBoard();
	DeleteSelectedControls();
	theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
}

void CObjectDCLView::OnEditDelete() 
{
	DeleteSelectedControls();
	theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
}

void CObjectDCLView::OnEditPaste() 
{
	PasteFromClipBoard();
}

void CObjectDCLView::OnEditUndo() 
{
	UndoAction();
}

void CObjectDCLView::OnEditCopy() 
{
	CopyControlToClipBoard();
	
}

void CObjectDCLView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (m_pThisDclForm == NULL)
		return;

	if (m_SelectedControl.m_nIndex != nNotSet &&
		m_SelectedControl.m_pArxObject != NULL &&
		m_SelectedControl.m_pControl != NULL)
	{
		CRect rcCtrl;
		if (IsWindow(m_SelectedControl.m_pControl->m_hWnd))
			m_SelectedControl.m_pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		ShowGripRects(TRUE, rcCtrl);						
		if (point.x == nNotSet && point.y == nNotSet)
		point = (CPoint) GetMessagePos();

		ScreenToClient(&point);

		CMenu menu;
		CMenu* pPopup;

		// the font popup is stored in a resource
		switch (nCurrentPurchaseMode)
		{
		case nPurchasedLT:
			menu.LoadMenu(IDR_POPUPMENUS_LT);
			break;
		case nPurchasedR14Pro:
			menu.LoadMenu(IDR_POPUPMENUS_R14);
			break;
		case nPurchasedStd:
			menu.LoadMenu(IDR_POPUPMENUS_STD);
			break;
		default:
			menu.LoadMenu(IDR_POPUPMENUS);
			break;
		}
		
		pPopup = menu.GetSubMenu(1);
		ClientToScreen(&point);
		pPopup->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this );			
		return;
	}
	
	if (point.x == nNotSet && point.y == nNotSet)
		point = (CPoint) GetMessagePos();

	ScreenToClient(&point);

	CMenu menu;
	CMenu* pPopup;

	// the font popup is stored in a resource
	switch (nCurrentPurchaseMode)
		{
		case nPurchasedLT:
			menu.LoadMenu(IDR_POPUPMENUS_LT);
			break;
		case nPurchasedR14Pro:
			menu.LoadMenu(IDR_POPUPMENUS_R14);
			break;
		case nPurchasedStd:
			menu.LoadMenu(IDR_POPUPMENUS_STD);
			break;
		default:
			menu.LoadMenu(IDR_POPUPMENUS);
			break;
		}
		
	pPopup = menu.GetSubMenu(2);
	ClientToScreen(&point);
	CObjectDCLApp* pApp = (CObjectDCLApp*)AfxGetApp();
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, pApp->m_pMainWnd );
}

BOOL CObjectDCLView::PreTranslateMessage(MSG* pMsg) 
{

	return CView::PreTranslateMessage(pMsg);
}

void CObjectDCLView::FireControlSelected(CDclControlObject *pArxControl)
{
	if (pArxControl != NULL)
	{
		// force the property list to be repainted.
		theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties(pArxControl, this);
		if (m_SelectedControl.m_pArxObject != pArxControl)
		{
			m_SelectedControl.m_pArxObject = pArxControl;
			m_SelectedControl.m_pControl = pArxControl->m_pCtrlHolder;
			m_SelectedControl.m_nIndex = pArxControl->m_Index;
		}
		CRect rc;
		m_SelectedControl.m_pControl->GetWindowRect(&rc);
		ScreenToClient(rc);
		ShowGripRects(TRUE, rc);	
	}
	else
		theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().DisplayVaries();

	m_StandardCursorID = false;
	theEditorWorkspace.GetToolBox()->m_nSelectedCtrl = 1;

}
void CObjectDCLView::FireShowFormGrips(BOOL bVisible)
{
	if (m_pThisDclForm != NULL && bVisible)
	{
		// reset the font toolbar
		theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar(NULL);

		if (m_pThisDclForm->GetType() != VdclTabForm)
			// if this form is NOT a tab form display it's properties
			theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties(m_pThisDclForm->GetControlProperties(), this);			
		else
			// if this form is a tab form clear the property list box display
			theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
	}
}

void CObjectDCLView::FirePopUpMenuPlease(long cx, long cy)
{
}

void CObjectDCLView::FireTabResized(short nClientHeight)
{
}

void CObjectDCLView::FireMouseDownEvent()
{
}

void CObjectDCLView::FireTabControlDeleted()
{
	theEditorWorkspace.GetProjectTreeCtrl()->RemoveChildren( m_pThisDclForm->m_htiTreeItem );
	CProject* pProject = m_pThisDclForm->GetProject();
	POSITION pos = pProject->GetDclFormList().GetHeadPosition();
	while( pos )
	{
		CDclFormObject* pDclForm = pProject->GetDclFormList().GetNext( pos );
		assert( pDclForm != NULL );
		if( pDclForm && pDclForm->GetParentForm() == m_pThisDclForm )
			pDclForm->m_bDeleted = true;
	}
}

void CObjectDCLView::FireSetUndo()
{
}

void CObjectDCLView::FireControlInserted(CDclControlObject *pArxControl, long ControlType)
{
	// have the property list show the new control
	if (pArxControl != NULL)
		theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties(pArxControl, this);
	// reset the toolbar to the pointer
	m_StandardCursorID = false;
	theEditorWorkspace.GetToolBox()->m_nSelectedCtrl = 1;

	//This adds three unnamed tab panes when a new tab strip is inserted. I commented this because the 
	//unnamed panes caused problems with the tab strip not recognizing the panes, and I think it is more 
	//intuitive to add them manually as desired anyway. 2007-02-13 [ORW]
	// if a tab was inserted
	//if (ControlType == CtlTabStrip)
	//	AddTabPanes();
}

//void CObjectDCLView::AddTabPanes()
//{
//	CObjectDCLApp* pApp = (CObjectDCLApp*)AfxGetApp();
//	// create a pointer to pass to the list to insert
//	CProject *pProject = activeProject;
//	
//	for (int i=0; i<3; i++)
//	{
//		CDclFormObject* pNewDclForm = new CDclFormObject( pProject, VdclTabForm );
//		// assign the unique name and dcl form type to the dcl form object
//		pNewDclForm->SetUniqueName(pApp->CreateUniqueName());
//		pNewDclForm->SetParentForm(m_pThisDclForm);
//		pNewDclForm->SetTabIndex(i);
//		
//		// add the new Dcl form object
//		pProject->GetDclFormList().AddTail(pNewDclForm);
//		
//		// make the call to add the properties to the new dcl form object.
//		pApp->AddDclFormProperties(pNewDclForm, VdclTabForm);
//		
//		CDclControlObject *pDclProperties = pNewDclForm->GetControlProperties();
//		if (pDclProperties != NULL)
//		{
//			pDclProperties->SetLongProperty(nHeight, GetSelectedTabClientHeight());
//			pDclProperties->SetLongProperty(nWidth, GetSelectedTabClientWidth());
//		}
//
//		// add the new dcl form to the project tree so it's shown there
//		CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
//		pProjTree->AddFormToTree(pNewDclForm, true);
//	}	
//}


CDclFormObject * CObjectDCLView::AddSingleTabPane(int nIndex)
{
	CDclFormObject* pNewDclForm = NULL;
	CObjectDCLApp* pApp = (CObjectDCLApp*)AfxGetApp();
	// create a pointer to pass to the list to insert
	CProject *pProject = activeProject;
	
	try
	{
		//pNewDclForm = new CDclFormObject( pProject, VdclTabForm );
		//// assign the unique name and dcl form type to the dcl form object
		//pNewDclForm->SetUniqueName(pApp->CreateUniqueName());
		//pNewDclForm->SetParentForm(m_pThisDclForm);
		//pNewDclForm->SetTabIndex(nIndex);
		//
		//POSITION pos = pProject->GetDclFormList().GetHeadPosition();
		//while (pos)
		//{
		//	CDclFormObject* pForm = pProject->GetDclFormList().GetNext(pos);
		//	if (pForm->GetParentName() == m_pThisDclForm->GetUniqueName() && pForm->GetTabIndex() == nIndex)
		//		break;
		//}
		//if (pos)
		//	pProject->GetDclFormList().InsertBefore(pos, pNewDclForm);
		//else
		//	pProject->GetDclFormList().AddTail(pNewDclForm);

		pNewDclForm = pProject->AddForm( VdclTabForm, m_pThisDclForm );
		pNewDclForm->SetTabIndex( nIndex );
		
		CDclControlObject *pDclProperties = pNewDclForm->GetControlProperties();
		assert( pDclProperties != NULL );
		if (pDclProperties != NULL)
		{
			pDclProperties->SetLongProperty(nHeight, GetSelectedTabClientHeight());
			pDclProperties->SetLongProperty(nWidth, GetSelectedTabClientWidth());
		}

		// add the new dcl form to the project tree so it's shown there
		CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
		pProjTree->AddFormToTree(pNewDclForm, true);
	}
	catch(...)
	{
	}
	return pNewDclForm;
}


bool CObjectDCLView::CanRemoveChildTabPane(int nIndex)
{
	if (nIndex == nNotSet)
		return true;

	CObjectDCLApp* pApp = (CObjectDCLApp*)AfxGetApp();
	// create a pointer to pass to the list to insert
	CProject *pProject = activeProject;
	
	RefCountedPtr< CPropertyObject > pProp = m_SelectedControl.m_pArxObject->GetPropertyObject(nTabsCaption);

	POSITION pos = pProject->GetDclFormList().FindIndex(nIndex);
	if (pos != NULL)
	{
		CDclFormObject *pTabForm = pProject->GetDclTabChildForm(m_pThisDclForm->GetUniqueName(), nIndex);
		assert( pTabForm != NULL );
		if( !pTabForm )
			return true;
		if (pTabForm->GetControlList().GetCount() > 1)		
		{
			CString sMsg;
			CString sTitle;
			sMsg = theWorkspace.LoadResourceString(IDS_QDELETETAB);
			sTitle = theWorkspace.LoadResourceString(IDR_MAINFRAME);
			if (MessageBox(sMsg, sTitle, MB_ICONEXCLAMATION|MB_YESNO) == 6)
				return true;
			else
				return false;			
		}
	}	
	return true;
}

void CObjectDCLView::RemoveChildTabPane(CDclFormObject *pDclForm)
{
	if (pDclForm == NULL)
		return;
	if (pDclForm->m_htiTreeItem != NULL)
		theEditorWorkspace.GetProjectTreeCtrl()->DeleteItem(pDclForm->m_htiTreeItem);
	pDclForm->GetProject()->DeleteForm( pDclForm );
}

void CObjectDCLView::ResizeChildTabPanes()
{
	CObjectDCLApp* pApp = (CObjectDCLApp*)AfxGetApp();
	// create a pointer to pass to the list to insert
	CProject *pProject = activeProject;
	
	RefCountedPtr< CPropertyObject > pProp = m_SelectedControl.m_pArxObject->GetPropertyObject(nTabsCaption);

	for (size_t i=0; i<pProp->GetStringArrayPtr()->size(); i++)
	{
		CDclFormObject *pTabForm = pProject->GetDclTabChildForm(m_pThisDclForm->GetUniqueName(), i);
		if (!pTabForm)
			continue;
		CDclControlObject *pDclProperties = pTabForm->GetControlProperties();
		int nNewHeight = GetSelectedTabClientHeight();
		int nNewWidth = GetSelectedTabClientWidth();
		pDclProperties->SetLongProperty(nHeight, nNewHeight);
		pDclProperties->SetLongProperty(nWidth, nNewWidth);
		if (pTabForm->m_pMdiChildWnd != NULL)
			pTabForm->m_pMdiChildWnd->SetWindowPos(NULL, nNotSet, nNotSet, nNewWidth, nNewHeight, SWP_NOMOVE);
	}	
}

void CObjectDCLView::FireShowPropertyWizard(CDclControlObject *pArxControl)
{
	if (theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().m_pControl != NULL)
		theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().ShowPropertyDlg();
}

	


void CObjectDCLView::OnProperties() 
{
	if (theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().m_pControl != NULL)
		theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().ShowPropertyDlg();
}


BOOL CObjectDCLView::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar) 
{
	
	
	if (dispid == DISPID_AMBIENT_USERMODE)
	{		
		V_VT(pvar) = VT_BOOL;
		V_BOOL(pvar) = 0;
		return TRUE;
	}
	if (dispid == DISPID_AMBIENT_UIDEAD)
	{		
		V_VT(pvar) = VT_BOOL;
		V_BOOL(pvar) = 1;
		return TRUE;
	}

	/*
	IOleControl *pOleCtrl = (IOleControl*)pSite;
	
	//IDispatch *pDispatch = pSite->GetIDispatch(TRUE);
	
	if (pOleCtrl != NULL)
		pOleCtrl->OnAmbientPropertyChange( dispid );		
	*/
	return CView::OnAmbientProperty(pSite, dispid, pvar);
}

void CObjectDCLView::OnDraw(CDC* pDC) 
{
	HDC hdc = pDC->m_hDC;
	PaintBackGround(hdc);	
	
	if (m_pThisDclForm == NULL)
		return;

	try
	{
		int nCount = m_pThisDclForm->GetControlList().GetCount();		

		if (nCount > 0)
		{
			CDclControlObject *pCtrl = m_pThisDclForm->GetControlProperties();

			if (pCtrl != NULL)
			{
				if (pCtrl->GetBoolProperty(nResizable) == TRUE)
					m_bShowGrip = true;
				else 
					m_bShowGrip = false;
			}
			else
				m_bShowGrip = false;

			if (m_bShowGrip)
			{
				UpdateGripPos();
				// draw size-grip
				DrawFrameControl(hdc, &m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
			}	
		}
	}
	catch(...)
	{
	}
	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);

	
}

void CObjectDCLView::OnEditObjectbrowser() 
{
	if (theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().m_pControl != NULL)
	{
		theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().EditObjectbrowser();
	}
	
}
