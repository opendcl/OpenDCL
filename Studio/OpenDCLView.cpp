// OpenDCLView.cpp : implementation of the COpenDCLView class
//

#include "stdafx.h"
#include "OpenDCLView.h"
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
#include "AxContainerCtrl.h"
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
#include "GraphicButtonCtrl.h"
#include "VdclStatic.h"
#include "ListCtrlEx.h"
#include "OleFont.h"
#include "AxPropertyDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "OpenDCL.h"
#include "EditorWorkspace.h"
#include "SharedRes.h"
#include "GraphicButtonCtrl.h"
#include "PictureObject.h"
#include "DclControlProp.h"


LPCTSTR gpszCliboardFormatName = _T("ODCL.Clipboard.Controls");


inline static bool IsVersionFree()
{
	return false;
}


static CString LTOA(int nVal)
{
  CString sLong;
	sLong.Format(_T("%d"), nVal);
  return sLong;
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


typedef CTypedPtrList< CObList, CDclControlObject* > CClipboardObject;
//class CClipboardObject : public CObject
//{
//	DECLARE_SERIAL(CClipboardObject);
//public:
//	CClipboardObject() {}
//	virtual ~CClipboardObject()
//	{
//	}
//
//	CTypedPtrList< CObList, CDclControlObject* > mControls;
//
//	virtual void Serialize(CArchive& ar)
//	{
//		CObject::Serialize( ar );
//		mControls.Serialize(ar);
//	}
//};
//IMPLEMENT_SERIAL(CClipboardObject, CObject, 1);

/////////////////////////////////////////////////////////////////////////////
// COpenDCLView

IMPLEMENT_DYNCREATE(COpenDCLView, CView)

BEGIN_MESSAGE_MAP(COpenDCLView, CView)
	//{{AFX_MSG_MAP(COpenDCLView)
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
	ON_COMMAND(ID_TOOLS_SETLISPSYMBOLNAMES, &COpenDCLView::OnToolsSetlispsymbolnames)
	ON_COMMAND(ID_TOOLS_CLEARLISPSYMBOLNAMES, &COpenDCLView::OnToolsClearlispsymbolnames)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SETLISPSYMBOLNAMES, &COpenDCLView::OnUpdateToolsSetlispsymbolnames)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CLEARLISPSYMBOLNAMES, &COpenDCLView::OnUpdateToolsClearlispsymbolnames)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDCLView construction/destruction

COpenDCLView::COpenDCLView():CView()//(COpenDCLView::IDD)
{
	CWinApp* pApp = AfxGetApp();
	CString sProfileName;
	sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);
	
	// get the grid spacing
	m_gridSpacing = pApp->GetProfileInt(sProfileName, _T("nGridSpacing"), 8);
    
	NullDrawRect();
	m_ControlId = nControlStartId;
	m_bShowGrip = true;
	m_bGripsCreate = false;
	m_MouseDown = false;
	m_bMoving = false;
	m_SelectedControl.m_nIndex = -1;
	m_SelectedList.RemoveAll();
	m_UndoList.RemoveAll();
	m_MouseDown = false;
	m_bDrawing = false;
	m_nResizeQuadrant = -1;
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

COpenDCLView::~COpenDCLView()
{
	if (m_pThisDclForm)
	{
		m_pThisDclForm->m_pMdiChildWnd = NULL;
		m_pThisDclForm = NULL;
	}
	m_SelectedControl.Clear();
		
	ClearSelection();
	ClearUndoList();
	
}

BOOL COpenDCLView::PreCreateWindow(CREATESTRUCT& cs)
{
	
	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_CLIPCHILDREN;
	
	if( !CView::PreCreateWindow(cs) )
		return FALSE;
	
	return TRUE;
}

void COpenDCLView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	ModifyStyleEx(WS_EX_CLIENTEDGE, NULL, 0);
	if (m_pThisDclForm == NULL)
	{
		ResizeParentToFit(false);	
	}	
}


void COpenDCLView::ResizeParentToFit(BOOL bShrinkOnly)
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
// COpenDCLView diagnostics

#ifdef _DEBUG
void COpenDCLView::AssertValid() const
{
	CView::AssertValid();
}

void COpenDCLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

//COpenDCLDoc* COpenDCLView::GetDocument() const // non-debug version is inline
//{
//	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenDCLDoc)));
//	return (COpenDCLDoc*)m_pDocument;
//}

/////////////////////////////////////////////////////////////////////////////
// COpenDCLView message handlers

void COpenDCLView::DrawActiveXCtrl(CDclControlObject *pCtrl, CDC* pDC) 
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
	
	//m_lpViewObject->Draw( DVASPECT_CONTENT, -1, NULL, NULL, NULL, pDC->m_hDC, &rc, NULL, NULL, 0 );
	
	//pCtrlHolder->MoveWindow(rcClient, TRUE);
		
	//pCtrl->m_pCtrlHolder->RedrawWindow();
	//pCtrl->m_pCtrlHolder->Invalidate();
	*/
}



/////////////////////////////////////////////////////////////////////////////
// COpenDCLView message handlers
void COpenDCLView::ClearUndoList()
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

void COpenDCLView::NullDrawRect()
{
	m_bDrawing = false;
	m_rcDraw.top = 0;
	m_rcDraw.left = 0;
	m_rcDraw.right = 0;
	m_rcDraw.bottom = 0;
}
	

void COpenDCLView::PaintBackGround(HDC hdc)
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

void COpenDCLView::OnGridSpacingChanged() 
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

void COpenDCLView::OnControlToInsertChanged() 
{
	m_SelectedControl.m_pArxObject = NULL;
	m_SelectedControl.m_pControl = NULL;
	m_SelectedControl.m_nIndex = -1;
	GetDocument()->SetModifiedFlag(TRUE);
}
void COpenDCLView::HideGrips()
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
void COpenDCLView::MoveGripRectsForward()
{
	m_GripRect1.SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect2.SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect3.SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect4.SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect5.SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect6.SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect7.SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect8.SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);

}
void COpenDCLView::ShowGripRects(BOOL bShow, CRect rcCtrl)
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

		if (m_GripRect1.IsWindowVisible() != bShow)
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

void COpenDCLView::SelectControl(CString sName) 
{
	CDclControlObject* pArxObject = FindArxControlObject(m_pThisDclForm, sName);
	if (pArxObject == NULL)
		return;
	if (IsInSelection(pArxObject))
		return;

	CControlHolder *pControl = (CControlHolder*)pArxObject->m_pCtrlHolder;

	if (m_SelectedControl.m_pArxObject == NULL)
	{
		m_SelectedControl.m_pArxObject = pArxObject;	
		m_SelectedControl.m_pControl = pControl;
		m_SelectedControl.m_nIndex = pArxObject->GetZOrder();
		ClearSelection(false);
		CRect rcCtrl;
		pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		ShowGripRects(TRUE, rcCtrl);
		FireControlSelected(pArxObject);
	}
	else // add the selected control to the multi selection
	{
		CSelectedControl *pSelection = new CSelectedControl;
		pSelection->m_pArxObject = pArxObject;
		pSelection->m_pControl = pControl;
		pSelection->m_nIndex = pArxObject->GetZOrder();
		m_SelectedList.AddTail(pSelection);
		pControl->SetSelected(TRUE);			
		FireControlSelected(NULL);
	}
}

bool COpenDCLView::CheckControlsForSelection( CRect rcSelArea, bool bLookForOne ) 
{
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
	if( !m_SelectedControl.m_pArxObject )
	{
		pZOrderList->ClearSelection();
		ClearSelection();
	}
	bool bReturn = false;
	//check each control to see whether it intersects the selection rectangle
	const CList< CDclControlObject* >& ControlList = m_pThisDclForm->GetControlList();
	int idx = ControlList.GetCount();
	POSITION pos = ControlList.GetTailPosition();
	while( pos )
	{
		--idx;
		CDclControlObject* pDclObject = ControlList.GetPrev( pos );
		assert( pDclObject != NULL );
		if( pDclObject->m_Delete )
			continue;
		if( pDclObject->GetType() <= CtlForm )
			continue;
		CControlHolder* pControl = (CControlHolder*)pDclObject->m_pCtrlHolder;
		assert( pControl != NULL );
		if( !pControl )
			continue;
		// get the control's position
		CRect rcCtrl;
		pControl->GetWindowRect( &rcCtrl );
		ScreenToClient( rcCtrl );
		if( !CRect().IntersectRect( rcSelArea, rcCtrl ) )
			continue; //no intersection, so skip this control
		if( IsInSelection( pDclObject ) ||  pDclObject == m_SelectedControl.m_pArxObject )
			continue; //it's already selected
		bReturn = true;
		theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar( pDclObject );
		bool bFirstSelectedControl = (m_SelectedControl.m_pArxObject == NULL);
		m_SelectedControl.m_pArxObject = pDclObject;
		m_SelectedControl.m_pControl = pControl;
		m_SelectedControl.m_nIndex = idx;
		pZOrderList->SelectItem( pDclObject->GetStrProperty( nName ), true );

		if( bFirstSelectedControl )
		{
			ShowGripRects( TRUE, rcCtrl );
			if( bLookForOne )
				return true;
		}
		else
		{
			CSelectedControl* pSelection = new CSelectedControl( pDclObject, pControl, idx );
			m_SelectedList.AddTail( pSelection );
			pControl->SetSelected( TRUE );
		}
	}
	return bReturn;
}

bool COpenDCLView::CheckControlsForSelection(CPoint point, UINT nFlags, bool bMouseDown) 
{
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();	
	if (m_SelectedList.GetCount() == 0 &&
		nFlags > 1 &&
		m_SelectedControl.m_nIndex == -1)
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

void COpenDCLView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_MouseDown = false;
	FirePopUpMenuPlease(point.x, point.y);
	
	
	CView::OnRButtonUp(nFlags, point);
}


void COpenDCLView::MoveControl(CSelectedControl *pSelectedControl, CPoint point)
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

void COpenDCLView::RemoveDragRect(CSelectedControl *pSelectedControl)
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

void COpenDCLView::AllRemoveDragRects()
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



bool COpenDCLView::StoreControlsPosition(CSelectedControl *pSelectedControl)
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

void COpenDCLView::CalcControlOffsetDistances(CDclControlObject *pArxObject, CRect &rcCtrl)
{
	CRect rcThis;
	GetClientRect(&rcThis);

	if (pArxObject->m_Delete == TRUE)
		return;

	int nTheLeft = pArxObject->GetLongProperty(nLeft);
	int nTheTop = pArxObject->GetLongProperty(nTop);
	
	int nTheWidth = pArxObject->GetLongProperty(nWidth);
	int nTheHeight = pArxObject->GetLongProperty(nHeight);

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
	else if (lLeftFromRight == 2)
		pArxObject->SetLongProperty(nLeftFromRight, nTheLeft - (rcThis.Width()/2));
	else
	{
		CRect rc = GetSplitterRect(lLeftFromRight);
		pArxObject->SetLongProperty(nLeftFromRight, nTheLeft - rc.left);
	}
	

	if (lRightFromRight == 0 || lRightFromRight == 1)
		pArxObject->SetLongProperty(nRightFromRight, rcThis.Width() - nTheLeft - nTheWidth);
	else
	{
		CRect rc = GetSplitterRect(lRightFromRight);
		pArxObject->SetLongProperty(nRightFromRight, nTheLeft + nTheWidth - rc.left);	
	}


	if (lTopFromBottom == 0 || lTopFromBottom == 1)
		pArxObject->SetLongProperty(nTopFromBottom, rcThis.Height() - nTheTop);
	else
	{
		CRect rc = GetSplitterRect(lTopFromBottom);
		pArxObject->SetLongProperty(nTopFromBottom, nTheTop - rc.top);
	}
	

	if (lBottomFromBottom == 0 || lBottomFromBottom == 1)
		pArxObject->SetLongProperty(nBottomFromBottom, rcThis.Height() - nTheTop - nTheHeight);
	else
	{
		CRect rc = GetSplitterRect(lBottomFromBottom);
		pArxObject->SetLongProperty(nBottomFromBottom, rc.top - (nTheTop + nTheHeight));
	}
}

CRect COpenDCLView::GetSplitterRect(int nId)
{
	CRect rc;
	// just in case get the client rect
	GetClientRect( &rc );

	// now find and retrieve the rectangle for the splitter.
	POSITION pos = m_pThisDclForm->GetControlList().GetHeadPosition();
	while( pos != NULL )
	{
		CDclControlObject* pCtrl = m_pThisDclForm->GetControlList().GetNext( pos );
		if( pCtrl->GetType() == CtlSplitter && pCtrl->GetID() == nId )
		{
			rc.SetRect( pCtrl->GetLongProperty( nLeft ), pCtrl->GetLongProperty( nTop ),
									pCtrl->GetLongProperty( nWidth ), pCtrl->GetLongProperty( nHeight ) );

			break;
		}
	}
	return rc;
}

int COpenDCLView::CheckGripsMouseMoveOver(CPoint point) 
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
	return -1;
}
void COpenDCLView::OnMouseMove(UINT nFlags, CPoint point) 
{
	
	CheckGripsMouseMoveOver(point);

	// here we are going to resize the draw rect if 
	// the user is resizing a control
	if (m_MouseDown && !m_bDrawing && m_nResizeQuadrant > -1)
	{
		DragResized(m_nResizeQuadrant, point);
		CView::OnMouseMove(nFlags, point);
		return;
	}

	if (m_MouseDown && !m_bDrawing &&
		m_SelectedControl.m_nIndex > -1)
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

void COpenDCLView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnLButtonUp(nFlags, point);
	
	HideSizingRect();
	
	if (m_pThisDclForm == NULL)
		return;

	m_MouseDown = false;
	
	// here we are going to resize the draw rect if 
	// the user is resizing a control
	if (!m_bDrawing && m_nResizeQuadrant > -1)
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
			nXdiff = nXdiff * -1;
		if (nYdiff < 0)
			nYdiff = nYdiff * -1;
		
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
							pUndo->rcPos.left = pSelControl->m_pArxObject->GetLongProperty(nLeft);
							pUndo->rcPos.top = pSelControl->m_pArxObject->GetLongProperty(nTop);
							pUndo->rcPos.right = pUndo->rcPos.left + pSelControl->m_pArxObject->GetLongProperty(nWidth);
							pUndo->rcPos.bottom = pUndo->rcPos.top + pSelControl->m_pArxObject->GetLongProperty(nHeight);
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
		(m_SelectedControl.m_nIndex == -1 ||
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

void COpenDCLView::ClearSelection(bool bResetZOrder)
{
	AllRemoveDragRects();
	HideSizingRect();
	m_SelectedControl.m_pArxObject = NULL;
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

void COpenDCLView::ShowSelection()
{
	POSITION pos = m_SelectedList.GetHeadPosition();
	while( pos )
	{
		CSelectedControl* pSelControl = m_SelectedList.GetNext( pos );
		((CControlHolder*)pSelControl->m_pControl)->SetSelected( TRUE );
	}
}

bool COpenDCLView::IsInSelection(CDclControlObject *pArxObject)
{
	POSITION pos = m_SelectedList.GetHeadPosition();
	while( pos )
	{
		CSelectedControl* pSelControl = m_SelectedList.GetNext( pos );
		if( pSelControl->m_pArxObject == pArxObject )
			return true;
	}
	return false;
}

void COpenDCLView::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	if (m_pThisDclForm == NULL)
		return;

	if (m_MouseDown && !m_bDrawing &&
		m_SelectedControl.m_nIndex > -1)
		return;		
	
	int nQuad = CheckGripsMouseMoveOver(point);
	if (nQuad > -1)
		SetupDragResize(nQuad);
	else
		OnMouseButtonDown(nFlags, point);

	if (m_SelectedControl.m_nIndex == -1 ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		FireMouseDownEvent();
	}
	
	//
	CView::OnLButtonDown(nFlags, point);
		
}
void COpenDCLView::OnRButtonDown(UINT nFlags, CPoint point) 
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
		m_SelectedControl.m_nIndex = -1;
		HideGrips();
		ClearSelection();
		FireShowFormGrips(TRUE);
	}	
	
	if (m_SelectedControl.m_nIndex != -1 &&
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

void COpenDCLView::OnMouseButtonDown(UINT nFlags, CPoint point) 
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
			m_SelectedControl.m_nIndex = -1;
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
int COpenDCLView::RoundToGridPattern(int Value)
{
	// return value if the grid spacing is too small
	if (m_gridSpacing < nMinGridSpacing)
		return Value;
	
	// return value if we are not inserting a control
	if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl < CtlLabel && !m_bMoving && m_nResizeQuadrant == -1)
		return Value;
			
	// rounds Value up or down to nearest ten
	int NewValue;
  
	NewValue = ((int)(Value / m_gridSpacing)) * m_gridSpacing;
	
	return NewValue;

}

void COpenDCLView::InsertControl(CRect rcPos)
{
	// if the control to insert in not a valid control, exit here
	if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl < CtlLabel)
		return;

	InsertControl(rcPos, (ControlType)theEditorWorkspace.GetToolBox()->m_nSelectedCtrl);
}


CDclControlObject* COpenDCLView::InsertControl(CRect rcPos, ControlType nCtrlToInsert)
{

	// if the dcl form is not valid, exit here
	if (m_pThisDclForm == NULL)
		return NULL;
	
	GetDocument()->SetModifiedFlag(TRUE);
	
	// get the next available name for the control
	CString sControlName;
	if (nCtrlToInsert == CtlFileDlgCtrl)
		sControlName = GetControlName(nCtrlToInsert);
	else if (nCtrlToInsert == CtlActiveX)
		sControlName = FindNextControlName(GetControlName(m_clsid));
	else
		sControlName = FindNextControlName(GetControlName(nCtrlToInsert));
	
	CDclControlObject *pDclControl = m_pThisDclForm->AddControl( nCtrlToInsert, sControlName );
	assert( pDclControl != NULL );

	if( nCtrlToInsert == CtlActiveX )
	{
		pDclControl->m_clsid = m_clsid;
		pDclControl->m_sLicenseKey = m_sLicenseKey;
	}

	// call the method to add the new control
	CWnd *pControl = AddCWndControl( pDclControl, rcPos, true );
	if (pControl == NULL)
	{
		m_pThisDclForm->DeleteControl( pDclControl ); //pDclControl is invalid after this call!
		UpdateZOrderList();
		return NULL;
	}
	
	// add the event to the undo list
	CUndoActions *pUndo = new CUndoActions( uaInsert, pDclControl, pControl );
	m_UndoList.AddTail(pUndo);

	m_SelectedControl.m_pArxObject = pDclControl;
	m_SelectedControl.m_pControl = pControl;
	m_SelectedControl.m_nIndex = pDclControl->GetZOrder();
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
	return pDclControl;
}

int COpenDCLView::GetNextControlId()
{
	int nHighest = -1;
	POSITION pos = m_pThisDclForm->GetControlList().GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject *pCtrl = m_pThisDclForm->GetControlList().GetNext(pos);
		if (pCtrl->GetID() > nHighest)
			nHighest = pCtrl->GetID();
	}
	return nHighest + nControlStartId + 1;
}

CWnd* COpenDCLView::AddCWndControl( CDclControlObject* pDclControl, CRect rcPos, bool bForceUpdate )
{
	if (pDclControl->GetID() < 0)
		return NULL;

	AddDefaultProperties( pDclControl, rcPos.Width(), rcPos.Height() );
	
	// create a new control holder
	CControlHolder *pControl = new CControlHolder(pDclControl);
	
	if (pDclControl->GetID() == -1)
		m_ControlId = GetNextControlId();
	else
		m_ControlId = pDclControl->GetID() + nControlStartId;
	
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
	if (!pControl->CreateNewDialogControl())
	{
		delete pControl;
		return NULL;
	}

	// set the position properties
	pControl->GetWindowRect( &rcPos );
	ScreenToClient( &rcPos );
	pControl->m_pLeftProp->SetLongValue( rcPos.left );
	pControl->m_pTopProp->SetLongValue( rcPos.top );
	pControl->m_pWidthProp->SetLongValue( rcPos.Width() );
	pControl->m_pHeightProp->SetLongValue( rcPos.Height() );
	CalcControlOffsetDistances( pDclControl, rcPos );

	// if the activeX control's name has not been set yet, we must do it here
	if (pDclControl->GetType() == CtlActiveX)
	{
		CString sName = pDclControl->GetStrProperty(nName);
		if (sName.IsEmpty())
			// lets get the correct name set here.
			pDclControl->SetStringProperty(nName, FindNextControlName(pDclControl->GetActiveXTypeName()));		
	}
	
	if (IsWindow(pControl->m_hWnd))
		// set the new control to the top of the Z-order
		pControl->SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	
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
		pControl->ShowWindow(TRUE);

	return pControl;
}

int COpenDCLView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
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

int COpenDCLView::GetDclFormIndex() 
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
	return -1;
}

void COpenDCLView::OnDclFormNameChange() 
{
	GetDclFormIndex();

	GetDocument()->SetModifiedFlag(TRUE);
}

CString COpenDCLView::FindNextControlName(CString sControlName) 
{
	if( !m_pThisDclForm )
		return CString();
	CDclFormObject* pRoot = m_pThisDclForm;
	while( pRoot->GetParentForm() )
		pRoot = pRoot->GetParentForm();
	CString sId;
	sId.Format( _T("%d"), pRoot->GetNextId() );
	return sControlName + sId;
}


void COpenDCLView::OnDestroy() 
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

void COpenDCLView::PreDestroy() 
{
	ClearControls();
	
	CProjectTreeCtrl *pProjectTree = theEditorWorkspace.GetProjectTreeCtrl();
	pProjectTree->RemoveViewPointer(this);
	pProjectTree->SetDocument(NULL);
	theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().DisplayProperties( NULL );

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

void COpenDCLView::ClearControls() 
{
	ClearSelection();
	if (m_pThisDclForm == NULL)
		return;

	if (m_pThisDclForm->GetType() < -1 || m_pThisDclForm->GetType() > 5)
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

void COpenDCLView::ClearChildControls(CControlHolder *pParentControl) 
{
	if (pParentControl->m_ControlType == CtlActiveX)
		pParentControl->GetTemplate()->SaveToStream(pParentControl->GetActiveXCtrl());
}

void COpenDCLView::RefreshChildControl(CDclControlObject *pArxObject, PropertyId ChangedPropertyID)
{
	if (pArxObject == NULL)
		return;

	if (pArxObject->m_pCtrlHolder == NULL)
		return;

	if (!IsWindow(pArxObject->m_pCtrlHolder->m_hWnd))
		return;

	CControlHolder *pParent = (CControlHolder*)pArxObject->m_pCtrlHolder;

	// some controls don't need to be recreated when a -1 is passed back by the properties wizard dialog box
	if (ChangedPropertyID == -1)
	{
		switch (pArxObject->GetType())
		{
			case CtlTabStrip:
			case CtlStdButton:
			case CtlGraphicButton:
			{
				// refresh all properties without creating a new child control
				ChangedPropertyID = (PropertyId)-2;
				break;
			}
		}
	}
	// here we are going to check to see if sertain properties have been requested to be changed
	// if these properties have then we create a new control and destroy the old control.
	// otherwise we just call a method to request the update of that control property
	switch(ChangedPropertyID)
	{
	case -1:
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
				pParent->CreateNewDialogControl();
			else // if an ActiveX control
				ResizeChildControl(pArxObject);				
			break;
		}
	case -2:
		{
			// call the method to update all the properties
			pParent->UpdateChildControl();

			break;
		}
	case nLeft:
	case nTop:
	case nWidth:
	case nHeight:
		{
			// call the method to update the arx control's property that was requested to be changed
			pParent->UpdateProperty(ChangedPropertyID);
			ResizeChildControl(pArxObject);
			break;
		}
	case nAutoSize:
	case nPicture:
		{		
			// call the method to update the arx control's property that was requested to be changed
			pParent->UpdateProperty(ChangedPropertyID);
			// check to see if autosize should resize the control
			pParent->CheckAutoSizeProp();
			ResizeChildControl(pArxObject);

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
			pParent->UpdateProperty(ChangedPropertyID);
			ResizeChildControl(pArxObject);
		
			break;
		}
	default:
		{		
			// call the method to update the arx control's property that was requested to be changed
			pParent->UpdateProperty(ChangedPropertyID);			
			break;
		}
	}
}

void COpenDCLView::ResizeChildControl(CDclControlObject *pArxObject)
{
	if (pArxObject == NULL)
		return;

	CControlHolder *pParent = (CControlHolder*)pArxObject->m_pCtrlHolder;
	if (pParent == NULL)
		return;
	CWnd *pControl = pParent->GetChildControl();
	
	if (pControl == NULL)
		return;
	
	// prepare to resize the child control
	CRect rcControl;

	rcControl.left = pArxObject->GetLongProperty(nLeft),
	rcControl.top = pArxObject->GetLongProperty(nTop);
	int nCalcWidth = pArxObject->GetLongProperty(nWidth);
	rcControl.right = rcControl.left + pArxObject->GetLongProperty(nWidth);
	rcControl.bottom = rcControl.top + pArxObject->GetLongProperty(nHeight);
	
	CRect rcThis;
	GetClientRect(&rcThis);
	
	CalcControlOffsetDistances(pArxObject, rcControl);

	// resize this control no mater what property was asked for
	pParent->MoveWindow(rcControl, TRUE);
	if( m_SelectedControl.m_pArxObject == pArxObject )
		ShowGripRects(TRUE, rcControl );
}

void COpenDCLView::HideSizingRect()
{
	if (m_rcDrawLast.Width() == 0 && m_rcDrawLast.Height() == 0)
		return;

	HDC hdc = ::GetDC(NULL);
	::DrawFocusRect(hdc, m_rcDrawLast);	
	::ReleaseDC(NULL, hdc);
	
	m_rcDrawLast.SetRect(0,0,0,0);
}	

void COpenDCLView::CopyControlToClipBoard() 
{
	CClipboardObject ClipBoard;

	if (m_SelectedControl.m_nIndex == -1 ||
		m_SelectedControl.m_pArxObject == NULL)
		return;

	// if the dcl form is not valid, exit here
	if (m_pThisDclForm == NULL)
		return;
	
	
	ClipBoard.AddTail(m_SelectedControl.m_pArxObject);

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->m_nIndex > -1)	
				ClipBoard.AddTail(pSelControl->m_pArxObject);
		}
	}

   // CG: This block was added by the Clipboard Assistant component
	CSharedFile memFile;
	CArchive ar(&memFile, CArchive::store|CArchive::bNoFlushOnDelete);
	UINT m_nClipboardFormat = RegisterClipboardFormat(gpszCliboardFormatName);

	// serialize data to archive object       
	ClipBoard.Serialize(ar);

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


void COpenDCLView::DeleteSelectedControls() 
{
	bool FireTabDeleted = false;
	if (m_SelectedControl.m_nIndex == -1 ||
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
				if (pSelControl->m_nIndex > -1 && pSelControl->m_pArxObject->GetType() != CtlFileDlgCtrl)	
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

void COpenDCLView::UpdateFont(CString sName) 
{
	if (m_SelectedControl.m_nIndex == -1 ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
		return;

	GetDocument()->SetModifiedFlag(TRUE);

	
	if (m_SelectedControl.m_pArxObject->GetType() == CtlActiveX)
	{	
		UpdateAxFont(&m_SelectedControl, nLabelName, false, -1, sName);		
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
			((CControlHolder*)m_SelectedControl.m_pArxObject->m_pCtrlHolder)->UpdateProperty(nLabelName);
		}
	}

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos != NULL)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->m_nIndex > -1)	
			{
				if (pSelControl->m_pArxObject->GetType() == CtlActiveX)
				{	
					UpdateAxFont(pSelControl, nLabelName, false, -1, sName);		

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
						((CControlHolder*)pSelControl->m_pArxObject->m_pCtrlHolder)->UpdateProperty(nLabelName);
					}
				}
			}
		}		
		
	}	
}

void COpenDCLView::UpdateAxFont(CSelectedControl *pSelControl, int nId, bool bBool, int nSize, CString sName)
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
				if (pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFontDisp ||
						pProp->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFont)
				{
					pFontProp = pProp;
					break;
				}
			}
		}
		if (pFontProp == NULL) 
			return;
		
		CAxContainerCtrl *axContainer = ((CControlHolder*)pCtrl->m_pCtrlHolder)->GetActiveXCtrl();
		COleFont font = axContainer->GetFont(pFontProp->GetAxInterfaceDescriptorPtr()->GetGetDispId());
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
		axContainer->SetFont(pFontProp->GetAxInterfaceDescriptorPtr()->GetPutDispId(), font);
	}
	catch (...)
	{
		return;
	}

}
void COpenDCLView::UpdateFontBool(bool bBool, PropertyId nId)
{
	if (m_SelectedControl.m_nIndex == -1 ||
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
			((CControlHolder*)m_SelectedControl.m_pArxObject->m_pCtrlHolder)->UpdateProperty(nId);
		}
	}

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos != NULL)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->m_nIndex > -1)	
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
						((CControlHolder*)pSelControl->m_pArxObject->m_pCtrlHolder)->UpdateProperty(nId);
					}
				}
			}
		}		
	}	
}

void COpenDCLView::UpdateFontSize(int nSize) 
{
	if (m_SelectedControl.m_nIndex == -1 ||
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
			((CControlHolder*)m_SelectedControl.m_pArxObject->m_pCtrlHolder)->UpdateProperty(nLabelSize);
		}
	}

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos != NULL)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->m_nIndex > -1)	
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
						((CControlHolder*)pSelControl->m_pArxObject->m_pCtrlHolder)->UpdateProperty(nLabelName);
					}
				}
			}
		}		
	}	
}


void COpenDCLView::PasteFromClipBoard()
{
	CClipboardObject ClipBoard; 

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
			ClipBoard.Serialize(ar);
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
	

	int nCount = ClipBoard.GetCount();
	int nCopied = 0;
	
	// if the dcl form is not valid, exit here
	if (m_pThisDclForm == NULL)
		return;
	
	
	if (nCount > 0)
	{		
		CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
		pZOrderList->m_pView = this;
		pZOrderList->ClearSelection();
	
		HideGrips();
		// clear the previous selection
		m_SelectedControl.m_nIndex = -1;
		m_SelectedControl.m_pArxObject = NULL;
		m_SelectedControl.m_pControl = NULL;
		ClearSelection();
		
		// create a position variable to hold the counter increment
		POSITION pos;	
		
		// set start position for navigating clipboard controls
		pos = ClipBoard.GetHeadPosition();

		// do loop to navigate clip board controls
		while (pos != NULL)
		{
			// get current clipboard control
			CDclControlObject* pCopy = ClipBoard.GetNext(pos);
			pCopy->SetOwnerForm(m_pThisDclForm);
					
			ClearEventProperties(pCopy);
			if (pCopy->GetType() == CtlFileDlgCtrl)
				continue;

			CString sNameOfCopy = FindNextControlName( GetControlName( pCopy ) );
			RefCountedPtr< CPropertyObject > pCaptionProp = pCopy->GetPropertyObject( nCaption );
			if( pCaptionProp && pCaptionProp->GetStringValue() == pCopy->GetKeyName() )
				pCaptionProp->SetStringValue( sNameOfCopy );
			pCopy->SetStringProperty( nName, sNameOfCopy );
			//if the copy will be a duplicate, offset it from the original and rename it
			if( m_pThisDclForm->FindControl( sNameOfCopy, pCopy->GetType() ) )
			{
				// make the control be offset by nControlOffset
				pCopy->SetLongProperty( nLeft, pCopy->GetLongProperty(nLeft) + nControlOffset );
				pCopy->SetLongProperty( nTop, pCopy->GetLongProperty(nTop) + nControlOffset );
			}
			
			CRect rcThis;
			GetClientRect(&rcThis);
			
			// check to make sure the control in inside the form
			int nCtrlLeft = pCopy->GetLongProperty(nLeft);
			int nCtrlWidth = pCopy->GetLongProperty(nWidth);
			if (nCtrlLeft > rcThis.Width())
			{
				nCtrlLeft = rcThis.Width() - nCtrlWidth;
				pCopy->SetLongProperty(nLeft, nCtrlLeft);
			}
			
			// check to make sure the control in inside the form
			int nCtrlTop = pCopy->GetLongProperty(nTop);
			int nCtrlHeight = pCopy->GetLongProperty(nHeight);
			if (nCtrlTop > rcThis.Height())
			{
				nCtrlTop = rcThis.Height() - nCtrlHeight;
				pCopy->SetLongProperty(nTop, nCtrlTop);
			}

			// add current clipboard control to DclFormObject
			m_pThisDclForm->AddControl( pCopy );
			
			CRect rcPos( nCtrlLeft, nCtrlTop, nCtrlLeft + nCtrlWidth, nCtrlTop + nCtrlHeight );
			// call the method to add the CWnd control 
			CWnd *pControl = AddCWndControl(pCopy, rcPos, true);
			if (pControl == NULL)
				return;
			
			// add the new control to the Zorder list
			pZOrderList->AddControlToList(sNameOfCopy, pCopy->GetType());
			pZOrderList->SelectItem(sNameOfCopy, false);			
			// call the method to setup the fonts in the font tool bar.
			theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar(pCopy);
	
			// increment counter
			nCount--;
			nCopied++;
			if (nCopied == 1)
			{
				m_SelectedControl.m_pArxObject = pCopy;
				m_SelectedControl.m_pControl = pControl;
				m_SelectedControl.m_nIndex = pCopy->GetZOrder();				
			}
			else
			{
				CSelectedControl *pSelCtrl = new CSelectedControl;
				pSelCtrl->m_pArxObject = pCopy;
				pSelCtrl->m_pControl = pControl;				
				pSelCtrl->m_nIndex = pCopy->GetZOrder();
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
		FireControlSelected(m_SelectedControl.m_pArxObject);
	}	
		
	CalcAllOffsets();
}
void COpenDCLView::SetupDragResize(int nQuadrant) 
{
	if (m_SelectedControl.m_nIndex == -1 ||
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

void COpenDCLView::ClearEventProperties(CDclControlObject *pCtrl)
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
	pCtrl->ResetProperty(nFormEventOnCancel);
	pCtrl->ResetProperty(nOnLMouseEvent);
	pCtrl->ResetProperty(nOnMMouseEvent);
	pCtrl->ResetProperty(nOnRMouseEvent);
	pCtrl->ResetProperty(nDragnDropToAutoCAD);
	pCtrl->ResetProperty(nDragnDropFromControl);        
	pCtrl->ResetProperty(nDragnDropFromAutoCAD);
	pCtrl->ResetProperty(nDragnDropBegin);


}

void COpenDCLView::DragResized(int nQuadrant, CPoint point) 
{
	if (m_SelectedControl.m_nIndex == -1 ||
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
CRect COpenDCLView::CalcResizeRect(int nQuadrant, CPoint point)
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

void COpenDCLView::CompletedDragResize(int nQuadrant, CPoint point) 
{
	if (m_SelectedControl.m_nIndex == -1 ||
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
	m_nResizeQuadrant = -1;	
	
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
	assert( pControl != NULL );
	if( !pControl )
		return;

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
	((CControlHolder*)m_SelectedControl.m_pControl)->CheckAutoSizeProp();
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

void COpenDCLView::UpdateClientHeight(CDclControlObject* pArxObject, CWnd *pControl) 
{
	if (m_SelectedControl.m_pArxObject == NULL)
		return;

	if (m_SelectedControl.m_pArxObject->GetType() != CtlTabStrip)
		return;
	if (!IsWindow(pControl->m_hWnd))
		return;

	size_t nCount = ((CTabCtrl*)pControl)->GetItemCount();
	int nTabHeight = 0;
	while (nCount-- > 0)
	{
		// get the new items' rectangle
		CRect rcTab;
		((CTabCtrl*)pControl)->GetItemRect( nCount, &rcTab);

		// if the height of this tab is greater than the previous max height, make it the new max
		if (rcTab.bottom > nTabHeight)
			nTabHeight = rcTab.bottom;
	}
	
	// get the height of the tab control
	CRect rcTabCtrl;
	((CTabCtrl*)pControl)->GetClientRect(&rcTabCtrl);
	
	// subtract the lowest tab bottom to get the control area height
	pArxObject->m_ClientHeight = rcTabCtrl.Height() - nTabHeight;
	
	// inform the parent a tab control has been resized
	FireTabResized(pArxObject->m_ClientHeight);
}

void COpenDCLView::OnSize(UINT nType, int cx, int cy) 
{
	InvalidateRect(&m_rcGripRect);
	CView::OnSize(nType, cx, cy);
	if (m_pThisDclForm != NULL)
	{
		CDclControlObject *pDclProps = m_pThisDclForm->GetControlProperties();
	
		CRect rc;
		if( m_pThisDclForm->UsesClientRect() )
			GetClientRect(&rc);
		else
			GetParent()->GetWindowRect(&rc);
		 
		pDclProps->SetLongProperty(nWidth, rc.Width());
		pDclProps->SetLongProperty(nHeight, rc.Height());

		GetParentFrame()->GetWindowRect(&rc);

		AdjustOffsets(rc.Width(), rc.Height());

		CPropertyTabPane* pPropTabs = theEditorWorkspace.GetPropertyTabs();
		if (pPropTabs->m_PropertiesTabPane.GetPropertiesCtrl().m_pView == this)
			pPropTabs->m_PropertiesTabPane.GetPropertiesCtrl().Invalidate();
	}
	InvalidateRect(&m_rcGripRect);
}	

void COpenDCLView::UpdateGripPos()
{
	// size-grip goes bottom right in the client area

	GetParentFrame()->GetClientRect(&m_rcGripRect);

	m_rcGripRect.left = m_rcGripRect.right - m_szGripSize.cx;
	m_rcGripRect.top = m_rcGripRect.bottom - m_szGripSize.cy;
}

void COpenDCLView::AdjustOffsets(int cx, int cy) 
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
					pArxObject->GetLongProperty(nLeft),
					pArxObject->GetLongProperty(nTop),
					pArxObject->GetLongProperty(nWidth),
					pArxObject->GetLongProperty(nHeight));

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
void COpenDCLView::RefreshSelectControl() 
{
	if (m_SelectedControl.m_nIndex == -1 ||
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


void COpenDCLView::ZOrdedSelectedControls(short direction) 
{
	if (m_SelectedControl.m_nIndex == -1 ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return;
	}
	
	if (direction == 0)
		m_SelectedControl.m_pControl->SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);		
	else
		m_SelectedControl.m_pControl->SetWindowPos(&CWnd::wndBottom, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	
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
				if (pSelControl->m_nIndex > -1)	
				{
					if (direction == 0)
						pSelControl->m_pControl->SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
					else
						pSelControl->m_pControl->SetWindowPos(&CWnd::wndBottom, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
					// call method to adjust the control's placement
					m_pThisDclForm->ReorderControl( pSelControl->m_pArxObject, (direction != 0) );
				}
			}
		}
	}
	MoveGripRectsForward();
	UpdateZOrderList();
}

short COpenDCLView::GetSelectedTabClientWidth() 
{
	if (m_SelectedControl.m_nIndex == -1 ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return -1;
	}
	
	return (short)m_SelectedControl.m_pArxObject->GetLongProperty(nWidth);
}

short COpenDCLView::GetSelectedTabClientHeight() 
{
	if (m_SelectedControl.m_nIndex == -1 ||
			m_SelectedControl.m_pArxObject == NULL ||
			m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return -1;
	}

	CTabCtrl *pControl = (CTabCtrl*)((CControlHolder*)m_SelectedControl.m_pControl)->GetChildControl();
	if (pControl == NULL)
		return -1;

	UpdateClientHeight( m_SelectedControl.m_pArxObject, pControl );
	return m_SelectedControl.m_pArxObject->m_ClientHeight;
}


void COpenDCLView::DisplayControls(CDclFormObject *pDclForm) 
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
				rcPos.left = pArxObject->GetLongProperty(nLeft);
				rcPos.top = pArxObject->GetLongProperty(nTop);
				rcPos.right = rcPos.left + pArxObject->GetLongProperty(nWidth);
				rcPos.bottom = rcPos.top + pArxObject->GetLongProperty(nHeight);
				
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
	if( m_pThisDclForm->UsesClientRect() )
		GetClientRect(&rc);
	else
		GetParent()->GetWindowRect(&rc);
		 
	CDclControlObject *pDclProps = m_pThisDclForm->GetControlProperties();
	pDclProps->SetLongProperty(nWidth, rc.Width());
	pDclProps->SetLongProperty(nHeight, rc.Height());
}

void COpenDCLView::MoveThisInPosition() 
{
	CRect rcStartup = ((CChildFrame*)GetParentFrame())->m_rcStartup;
	if (rcStartup.left < 0)
		rcStartup.OffsetRect(rcStartup.left * -1, 0);
	
	if (rcStartup.top < 0)
		rcStartup.OffsetRect(0, rcStartup.top * -1);

	GetParentFrame()->MoveWindow(rcStartup,TRUE);
}

short COpenDCLView::GetSelectedArxIndex() 
{
	if (m_SelectedControl.m_nIndex == -1 ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return -1;
	}
	
	if (m_SelectedList.GetCount() > 0)
		return -2;
	else
		return m_SelectedControl.m_nIndex;
}

long COpenDCLView::GetSelectedType() 
{
	if (m_SelectedControl.m_nIndex == -1 ||
		m_SelectedControl.m_pArxObject == NULL ||
		m_SelectedControl.m_pControl == NULL)
	{
		HideGrips();
		return -1;
	}
	

	return m_SelectedControl.m_pArxObject->GetType();
}


void COpenDCLView::OnLostFocus() 
{
	HideGrips();
	HideSizingRect();
	ClearSelection();
	m_StandardCursorID = false;

}

BOOL COpenDCLView::CanUndo() 
{
	if (m_UndoList.GetCount() > 0)
		return TRUE;
	else
		return FALSE;
}

void COpenDCLView::UndoAction() 
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
				((CControlHolder*)m_SelectedControl.m_pArxObject->m_pCtrlHolder)->UpdateProperty(nLabelName);

				break;
				}
			case uaFontSize:
				{
				pUndoAction->m_pArxObject->SetLongProperty(nLabelSize, pUndoAction->lLong);
				
				// call the method to update the control
				((CControlHolder*)m_SelectedControl.m_pArxObject->m_pCtrlHolder)->UpdateProperty(nLabelName);

				break;
				}
			case uaMoved:
				{
				pUndoAction->m_pControl->MoveWindow(pUndoAction->rcPos, TRUE);
				pUndoAction->m_pArxObject->SetLongProperty(nLeft, pUndoAction->rcPos.left);
				pUndoAction->m_pArxObject->SetLongProperty(nTop, pUndoAction->rcPos.top);
				pUndoAction->m_pArxObject->SetLongProperty(nWidth, pUndoAction->rcPos.Width());
				pUndoAction->m_pArxObject->SetLongProperty(nHeight, pUndoAction->rcPos.Height());
				if (m_SelectedControl.m_nIndex == pUndoAction->m_pArxObject->GetZOrder())
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
						rc.left = pUndoAction->m_pArxObject->GetLongProperty(nLeft);
						rc.top = pUndoAction->m_pArxObject->GetLongProperty(nTop);
						rc.right = rc.left + pUndoAction->m_pArxObject->GetLongProperty(nWidth);
						rc.bottom = rc.top + pUndoAction->m_pArxObject->GetLongProperty(nHeight);
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

void COpenDCLView::UpdateControl(CDclControlObject *pArxObject, PropertyId ChangedPropertyID) 
{
	
	if (m_pThisDclForm == NULL)
		return;

	// call the method to refresh the control
	RefreshChildControl(pArxObject, ChangedPropertyID);

	if (m_SelectedControl.m_nIndex != -1 &&
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

short COpenDCLView::ResetDclFormIndex() 
{
	return GetDclFormIndex();
}

void COpenDCLView::CheckPictureRefs() 
{
	
	for (int i=1; i<m_pThisDclForm->GetControlList().GetCount(); i++)
	{
		POSITION pos = m_pThisDclForm->GetControlList().FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pArxObject = m_pThisDclForm->GetControlList().GetAt(pos);
			int nPictureId = pArxObject->GetLongProperty(nPicture);
			if (nPictureId >= 0)
			{
				CWnd *pCtrl = pArxObject->m_pCtrlHolder;				
				((CControlHolder*)pCtrl)->UpdateProperty( nPicture);
			}
		}
	}

}

void COpenDCLView::SelectControl(short nArxControlIndex) 
{
	CDclControlObject* pArxObject = GetArxControlObject(m_pThisDclForm, nArxControlIndex);
	if (pArxObject == NULL)
		return;
	CWnd *pControl = pArxObject->m_pCtrlHolder;
	m_SelectedControl.m_pArxObject = pArxObject;
	m_SelectedControl.m_pControl = pControl;
	m_SelectedControl.m_nIndex = pArxObject->GetZOrder();
	ClearSelection();

	CRect rcCtrl;
	if (pControl != NULL)
	{
		pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		ShowGripRects(TRUE, rcCtrl);
	}
}

void COpenDCLView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDblClk(nFlags, point);
	if (m_pThisDclForm == NULL)
		return;

	// check to see if we are going to try and select controls
	CheckControlsForSelection(point, nFlags, true);

	
	int nArxIndex = m_SelectedControl.m_nIndex;

	if (nArxIndex == -1)
		FireShowPropertyWizard(m_pThisDclForm->GetControlProperties());
	else
		FireShowPropertyWizard(m_SelectedControl.m_pArxObject);
}

void COpenDCLView::CalcAllOffsets() 
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
				int nW = pArxObject->GetLongProperty(nWidth);
				int nL = pArxObject->GetLongProperty(nLeft);

				CRect rcPos(pArxObject->GetLongProperty(nLeft),
							pArxObject->GetLongProperty(nTop),
							pArxObject->GetLongProperty(nWidth) - pArxObject->GetLongProperty(nLeft),
							pArxObject->GetLongProperty(nHeight) - pArxObject->GetLongProperty(nTop));
				// set the offset position properties
				CalcControlOffsetDistances(pArxObject, rcPos);
			}
		}
	}
}

void COpenDCLView::RefreshControlsWithPictures() 
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
					RefreshChildControl(pArxObject, nPrivateProperty);
			}
		}
	}

}

void COpenDCLView::ZOrderUpdateOfSelCtrl(short ZIndex) 
{
	
	if (m_SelectedControl.m_nIndex == -1 ||
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

bool COpenDCLView::IsTabsEnabled()
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


void COpenDCLView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetActiveView(pActivateView);
	CToolBox *pToolBox = theEditorWorkspace.GetToolBox();
	pToolBox->SetActiveView(pActivateView);

	if (pDeactiveView == pActivateView)
	{
		if (theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().m_pControl != m_SelectedControl.m_pArxObject)
		{
			CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
			pZOrderList->ClearList(this);
		}		
	}
	else
		((COpenDCLView*)pDeactiveView)->m_pThisDclForm = NULL;

	if(pDeactiveView)
		((COpenDCLView*)pDeactiveView)->m_StandardCursorID = false;
	
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if (bActivate && pActivateView == this)
	{
		UpdateZOrderList();
		CDclControlObject* pCurrentCtrl = m_SelectedControl.m_pArxObject;
		if( !pCurrentCtrl )
		{
			CDclFormObject* pCurrentForm = ((COpenDCLView*)pActivateView)->m_pThisDclForm;
			if( pCurrentForm )
				pCurrentCtrl = pCurrentForm->GetControlProperties();
		}
		theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties( pCurrentCtrl, this );
	}
}

void COpenDCLView::UpdateZOrderList() 
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
void COpenDCLView::OnBringtofront() 
{
	ZOrdedSelectedControls(0);
	
}

void COpenDCLView::OnSendtoback() 
{
	ZOrdedSelectedControls(1);
	
}

void COpenDCLView::OnEditCut() 
{
	CopyControlToClipBoard();
	DeleteSelectedControls();
	theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
}

void COpenDCLView::OnEditDelete() 
{
	DeleteSelectedControls();
	theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
}

void COpenDCLView::OnEditPaste() 
{
	PasteFromClipBoard();
}

void COpenDCLView::OnEditUndo() 
{
	UndoAction();
}

void COpenDCLView::OnEditCopy() 
{
	CopyControlToClipBoard();
	
}

void COpenDCLView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if (m_pThisDclForm == NULL)
		return;

	if (m_SelectedControl.m_nIndex != -1 &&
		m_SelectedControl.m_pArxObject != NULL &&
		m_SelectedControl.m_pControl != NULL)
	{
		CRect rcCtrl;
		if (IsWindow(m_SelectedControl.m_pControl->m_hWnd))
			m_SelectedControl.m_pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		ShowGripRects(TRUE, rcCtrl);						
		if (point.x == -1 && point.y == -1)
		point = (CPoint) GetMessagePos();

		ScreenToClient(&point);

		CMenu menu;
		CMenu* pPopup;

		// the font popup is stored in a resource
		menu.LoadMenu(IDR_POPUPMENUS);
		pPopup = menu.GetSubMenu(1);
		ClientToScreen(&point);
		pPopup->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, this );			
		return;
	}
	
	if (point.x == -1 && point.y == -1)
		point = (CPoint) GetMessagePos();

	ScreenToClient(&point);

	CMenu menu;
	CMenu* pPopup;

	// the font popup is stored in a resource
	menu.LoadMenu(IDR_POPUPMENUS);
	pPopup = menu.GetSubMenu(2);
	ClientToScreen(&point);
	COpenDCLApp* pApp = (COpenDCLApp*)AfxGetApp();
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, pApp->m_pMainWnd );
}

BOOL COpenDCLView::PreTranslateMessage(MSG* pMsg) 
{
	return CView::PreTranslateMessage(pMsg);
}

void COpenDCLView::FireControlSelected(CDclControlObject *pArxControl)
{
	if (pArxControl != NULL)
	{
		// force the property list to be repainted.
		theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties(pArxControl, this);
		if (m_SelectedControl.m_pArxObject != pArxControl)
		{
			m_SelectedControl.m_pArxObject = pArxControl;
			m_SelectedControl.m_pControl = pArxControl->m_pCtrlHolder;
			m_SelectedControl.m_nIndex = pArxControl->GetZOrder();
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
void COpenDCLView::FireShowFormGrips(BOOL bVisible)
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

void COpenDCLView::FirePopUpMenuPlease(long cx, long cy)
{
}

void COpenDCLView::FireTabResized(short nClientHeight)
{
}

void COpenDCLView::FireMouseDownEvent()
{
}

void COpenDCLView::FireTabControlDeleted()
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

void COpenDCLView::FireSetUndo()
{
}

void COpenDCLView::FireControlInserted(CDclControlObject *pArxControl, long ControlType)
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

//void COpenDCLView::AddTabPanes()
//{
//	COpenDCLApp* pApp = (COpenDCLApp*)AfxGetApp();
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


CDclFormObject * COpenDCLView::AddSingleTabPane(int nIndex)
{
	CDclFormObject* pNewDclForm = NULL;
	COpenDCLApp* pApp = (COpenDCLApp*)AfxGetApp();
	// create a pointer to pass to the list to insert
	CProject *pProject = activeProject;
	
	pNewDclForm = m_pThisDclForm->AddChildForm( VdclTabForm );
	pNewDclForm->SetTabIndex( nIndex );

	CDclControlObject *pDclProperties = pNewDclForm->GetControlProperties();
	assert( pDclProperties != NULL );
	if (pDclProperties != NULL)
	{
		pDclProperties->AddLongProperty(nHeight, PropLong, GetSelectedTabClientHeight(), true);
		pDclProperties->AddLongProperty(nWidth, PropLong, GetSelectedTabClientWidth(), true);
	}
	return pNewDclForm;
}


bool COpenDCLView::CanRemoveChildTabPane(int nIndex)
{
	if (nIndex == -1)
		return true;

	COpenDCLApp* pApp = (COpenDCLApp*)AfxGetApp();
	// create a pointer to pass to the list to insert
	CProject *pProject = activeProject;
	
	RefCountedPtr< CPropertyObject > pProp = m_SelectedControl.m_pArxObject->GetPropertyObject(nTabsCaption);

	POSITION pos = pProject->GetDclFormList().FindIndex(nIndex);
	if (pos != NULL)
	{
		CDclFormObject *pTabForm = pProject->FindDclTabChildForm(m_pThisDclForm->GetUniqueName(), nIndex);
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

void COpenDCLView::RemoveChildTabPane(CDclFormObject *pDclForm)
{
	if (pDclForm == NULL)
		return;
	if (pDclForm->m_htiTreeItem != NULL)
		theEditorWorkspace.GetProjectTreeCtrl()->DeleteItem(pDclForm->m_htiTreeItem);
	pDclForm->GetProject()->DeleteForm( pDclForm );
}

void COpenDCLView::ResizeChildTabPanes()
{
	CDclControlObject* pDclControl = m_SelectedControl.m_pArxObject;
	RefCountedPtr< CPropertyObject > pProp = pDclControl->GetPropertyObject(nTabsCaption);
	if( !pProp )
		return;
	int nNewHeight = GetSelectedTabClientHeight();
	int nNewWidth = GetSelectedTabClientWidth();
	for (size_t i=0; i<pProp->GetStringArrayPtr()->size(); i++)
	{
		CDclFormObject *pTabForm = pDclControl->GetOwnerProject()->FindDclTabChildForm(m_pThisDclForm->GetUniqueName(), i);
		if (!pTabForm)
			continue;
		CDclControlObject *pDclProperties = pTabForm->GetControlProperties();
		pDclProperties->SetLongProperty(nHeight, nNewHeight);
		pDclProperties->SetLongProperty(nWidth, nNewWidth);
		if (pTabForm->m_pMdiChildWnd != NULL)
			pTabForm->m_pMdiChildWnd->SetWindowPos(NULL, -1, -1, nNewWidth, nNewHeight, SWP_NOMOVE);
	}	
}

void COpenDCLView::FireShowPropertyWizard(CDclControlObject *pArxControl)
{
	if (theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().m_pControl != NULL)
		theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().ShowPropertyDlg();
}

void COpenDCLView::OnProperties() 
{
	if (theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().m_pControl != NULL)
		theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().ShowPropertyDlg();
}

BOOL COpenDCLView::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar) 
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

void COpenDCLView::OnDraw(CDC* pDC) 
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

void COpenDCLView::OnEditObjectbrowser() 
{
	if (theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().m_pControl != NULL)
		theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().EditObjectbrowser();
}

void COpenDCLView::OnToolsSetlispsymbolnames()
{
	CFormVarNameUpdate Dlg( m_pThisDclForm, m_pThisDclForm->GetKeyName() );
	Dlg.DoModal();
	theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().Invalidate();
}

void COpenDCLView::OnToolsClearlispsymbolnames()
{
	CFormVarNameUpdate Dlg( m_pThisDclForm, NULL );
	Dlg.DoModal();
	theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().Invalidate();
}

void COpenDCLView::OnUpdateToolsSetlispsymbolnames(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pThisDclForm != NULL );
}

void COpenDCLView::OnUpdateToolsClearlispsymbolnames(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( m_pThisDclForm != NULL );
}
