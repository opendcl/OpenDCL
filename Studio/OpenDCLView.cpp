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
#include "EditorWorkspace.h"
#include "AxInterfaceDescriptor.h"
#include "OleFont.h"
#include "AxContainerCtrl.h"
#include "TabStripCtrl.h"
#include "SharedRes.h"
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


static TDclControlPtr FindArxControlObject(TDclFormPtr pDclForm, CString sControlName, TDclControlPtr pCtrl = NULL )
{
	if (pDclForm == NULL)
		return NULL;

	const TDclControlList& Controls = pDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		if ((*iter)->GetStringProperty(Prop::Name) == sControlName && (*iter) != pCtrl)
			return (*iter);
	}
	return NULL;
}

static bool AddControlPropertyListItem( TPropertyPtr pProp, LPCTSTR pszNewValue ) 
{
	if( !pProp )
		return false;
	assert( pProp->GetStringArrayPtr() != NULL );
	pProp->GetStringArrayPtr()->push_back( pszNewValue );
	return true;
}

static bool AddControlPropertyListItem( TPropertyPtr pProp, int nNewValue ) 
{
	if( !pProp )
		return false;
	assert( pProp->GetIntArrayPtr() != NULL );
	pProp->GetIntArrayPtr()->push_back( nNewValue );
	return true;
}

static TPropertyPtr AddControlHiddenProperty( TDclControlPtr pDclControl,
																																	Prop::Id nID,
																																	LPCTSTR pszValue,
																																	PropertyType type )
{
	TPropertyPtr pProp = pDclControl->AddStringProperty( nID, type, pszValue );
	assert( pProp != NULL );
	if( pProp )
		pProp->SetHidden();
	return pProp;
}


typedef CTypedPtrList< CObList, CDclControlObject* > CClipboardObject;


/////////////////////////////////////////////////////////////////////////////
// COpenDCLView

IMPLEMENT_DYNCREATE(COpenDCLView, CView)

BEGIN_MESSAGE_MAP(COpenDCLView, CView)
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()	
	ON_WM_LBUTTONDBLCLK()
	ON_REGISTERED_MESSAGE(WM_GRIP_STARTDRAG, OnGripStartDrag)
	ON_REGISTERED_MESSAGE(WM_GRIP_DRAGGING, OnGripDragging)
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
	ON_COMMAND(ID_TOOLS_SETLISPSYMBOLNAMES, &COpenDCLView::OnToolsSetlispsymbolnames)
	ON_COMMAND(ID_TOOLS_CLEARLISPSYMBOLNAMES, &COpenDCLView::OnToolsClearlispsymbolnames)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SETLISPSYMBOLNAMES, &COpenDCLView::OnUpdateToolsSetlispsymbolnames)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CLEARLISPSYMBOLNAMES, &COpenDCLView::OnUpdateToolsClearlispsymbolnames)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenDCLView construction/destruction

COpenDCLView::COpenDCLView():CView()//(COpenDCLView::IDD)
, mGripper( this, true )
{
	CWinApp* pApp = AfxGetApp();
	CString sProfileName = theWorkspace.LoadResourceString(IDR_MAINFRAME);
	
	// get the grid spacing
	m_gridSpacing = pApp->GetProfileInt(sProfileName, _T("nGridSpacing"), 8);
    
	NullDrawRect();
	m_ControlId = nControlStartId;
	m_MouseDown = false;
	m_bMoving = false;
	m_SelectedControl.Clear();
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

void COpenDCLView::DrawActiveXCtrl(TDclControlPtr pCtrl, CDC* pDC) 
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
	Invalidate();
}

void COpenDCLView::OnControlToInsertChanged() 
{
	m_SelectedControl.Clear();
}

void COpenDCLView::SelectControl(CString sName) 
{
	TDclControlPtr pArxObject = FindArxControlObject(m_pThisDclForm, sName);
	if (pArxObject == NULL)
		return;
	if (IsInSelection(pArxObject))
		return;

	CControlHolder *pControl = (CControlHolder*)pArxObject->m_pCtrlHolder;

	if (m_SelectedControl.GetTemplate() == NULL)
	{
		m_SelectedControl.Set( pArxObject, pControl, pArxObject->GetZOrder() );
		ClearSelection(false);
		CRect rcCtrl;
		pControl->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		mGripper.MoveTo(rcCtrl);
		FireControlSelected(pArxObject);
	}
	else // add the selected control to the multi selection
	{
		CSelectedControl* pSelection = new CSelectedControl( pArxObject, pControl, pArxObject->GetZOrder() );
		m_SelectedList.AddTail(pSelection);
		pControl->SetSelected();			
		FireControlSelected(NULL);
	}
}

bool COpenDCLView::CheckControlsForSelection( CRect rcSelArea, bool bLookForOne ) 
{
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
	if( !m_SelectedControl.GetTemplate() )
	{
		pZOrderList->ClearSelection();
		ClearSelection();
	}
	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	if( Controls.empty() )
		return false;
	bool bReturn = false;
	//check each control to see whether it intersects the selection rectangle
	size_t idx = Controls.size();
	TDclControlList::const_iterator iter = Controls.end();
	do
	{
		--idx;
		TDclControlPtr pDclControl = *(--iter);
		if( pDclControl->IsDeleted() )
			continue;
		if( pDclControl->GetType() <= CtlForm )
			continue;
		CControlHolder* pControl = (CControlHolder*)pDclControl->m_pCtrlHolder;
		assert( pControl != NULL );
		if( !pControl )
			continue;
		// get the control's position
		CRect rcCtrl;
		pControl->GetWindowRect( &rcCtrl );
		ScreenToClient( rcCtrl );
		if( !CRect().IntersectRect( rcSelArea, rcCtrl ) )
			continue; //no intersection, so skip this control
		if( IsInSelection( pDclControl ) ||  pDclControl == m_SelectedControl.GetTemplate() )
			continue; //it's already selected
		bReturn = true;
		theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar( pDclControl );
		bool bFirstSelectedControl = (m_SelectedControl.GetTemplate() == NULL);
		pZOrderList->SelectItem( pDclControl->GetStringProperty( Prop::Name ), true );

		if( bFirstSelectedControl )
		{
			m_SelectedControl.Set( pDclControl, pControl, idx );
			mGripper.MoveTo( rcCtrl );
			if( bLookForOne )
				return true;
		}
		else
		{
			CSelectedControl* pSelection = new CSelectedControl( pDclControl, pControl, idx );
			m_SelectedList.AddTail( pSelection );
			pControl->SetSelected();
		}
	} while( iter != Controls.begin() );
	return bReturn;
}

bool COpenDCLView::CheckControlsForSelection(CPoint point, UINT nFlags, bool bMouseDown) 
{
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();	
	if (m_SelectedList.GetCount() == 0 && nFlags > 1 && m_SelectedControl.GetIndex() == -1)
		nFlags = 1;
	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	size_t idx = Controls.size();
	TDclControlList::const_iterator iter = Controls.end();
	do
	{
		--idx;
		TDclControlPtr pDclControl = *--iter;
		if (!pDclControl->IsDeleted())
		{
			CControlHolder *pControl = pDclControl->m_pCtrlHolder;
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
					!IsInSelection(pDclControl) &&
					pDclControl != m_SelectedControl.GetTemplate())
				{
					ClearSelection();
					mGripper.Hide();
					pZOrderList->ClearSelection();
					pZOrderList->SelectItem(pDclControl->GetStringProperty(Prop::Name), true);
					// call the method to setup the fonts in the font tool bar.
					theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar(pDclControl);

					m_SelectedControl.Set( pDclControl, pControl, idx );
					mGripper.MoveTo(rcCtrl);
					FireControlSelected(pDclControl);
				}
				else // add the selected control to the multi selection
				{
					if (!IsInSelection(pDclControl) && pDclControl != m_SelectedControl.GetTemplate())
					{
						pZOrderList->SelectItem(pDclControl->GetStringProperty(Prop::Name), false);
						// call the method to setup the fonts in the font tool bar.
						theEditorWorkspace.GetMainFrame()->m_wndDlgBar.AddFontToToolBar(pDclControl);

						CSelectedControl *pSelection = new CSelectedControl( pDclControl, pControl, idx );
						m_SelectedList.AddTail(pSelection);
						pControl->SetSelected();
						if (!bMouseDown)
							FireControlSelected(NULL);
					}
				}
				return true;
			}
		}
	} while( iter != Controls.begin() );
	
	return false;
}

void COpenDCLView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	m_MouseDown = false;
	CView::OnRButtonUp(nFlags, point);
}


void COpenDCLView::MoveControl(CSelectedControl *pSelectedControl, CPoint point)
{
	CRect rcCtrl;
	CRect rcNewPos;

	if (pSelectedControl->GetControlHolder() == NULL)
		return;

	int nXDiff = m_DrawStartPoint.x - point.x;
	int nYDiff = m_DrawStartPoint.y - point.y;

	
	rcNewPos.left = RoundToGridPattern(pSelectedControl->GetTemplate()->GetLongProperty( Prop::Left ) - nXDiff);
	rcNewPos.right = rcNewPos.left + pSelectedControl->GetTemplate()->GetLongProperty( Prop::Width );
	rcNewPos.top = RoundToGridPattern(pSelectedControl->GetTemplate()->GetLongProperty( Prop::Top ) - nYDiff);
	rcNewPos.bottom = rcNewPos.top + pSelectedControl->GetTemplate()->GetLongProperty( Prop::Height );

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
	theWorkspace.SetModified(true);
	m_bMoving = true;
}

void COpenDCLView::RemoveDragRect(CSelectedControl *pSelectedControl)
{
	//if (pSelectedControl->GetControlHolder() == NULL)
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

	if (pSelectedControl->GetControlHolder() == NULL)
		return false;

	rcCtrl = pSelectedControl->m_rcLastDrawn;

    if (rcCtrl.Width() == 0)
		return false;

	pSelectedControl->GetControlHolder()->MoveWindow(rcCtrl, TRUE);
	CRect rc;
	pSelectedControl->GetControlHolder()->GetWindowRect(&rc);
	ScreenToClient(rc);
	if (rcCtrl != rc)
	{
		mGripper.MoveTo(rc);
		// invalidate the last draw rect info Note, this does not mean redraw
		m_SelectedControl.Reset();
		return false;
	}

	// set the position properties
	pSelectedControl->GetTemplate()->SetLongProperty(Prop::Left, rcCtrl.left);
	pSelectedControl->GetTemplate()->SetLongProperty(Prop::Top, rcCtrl.top);
	
	pSelectedControl->GetTemplate()->SetLongProperty(Prop::Width, rcCtrl.Width());
	pSelectedControl->GetTemplate()->SetLongProperty(Prop::Height, rcCtrl.Height());

	CalcControlOffsetDistances(pSelectedControl->GetTemplate(), rcCtrl);
	theWorkspace.SetModified(true);

	return true;
}

void COpenDCLView::CalcControlOffsetDistances(TDclControlPtr pArxObject, const CRect &rcCtrl)
{
	CRect rcThis;
	GetClientRect(&rcThis);

	if (pArxObject->IsDeleted())
		return;

	int nTheLeft = pArxObject->GetLongProperty(Prop::Left);
	int nTheTop = pArxObject->GetLongProperty(Prop::Top);
	
	int nTheWidth = pArxObject->GetLongProperty(Prop::Width);
	int nTheHeight = pArxObject->GetLongProperty(Prop::Height);

	// get the offset boolean flags
	int lLeftFromRight = 0;
	if (pArxObject->GetPropertyObject(Prop::UseLeftFromRight))
	{
		if (pArxObject->GetPropertyObject(Prop::UseLeftFromRight)->GetType() == PropBool)
			lLeftFromRight = pArxObject->GetPropertyObject(Prop::UseLeftFromRight)->GetBooleanValue();
		else
			lLeftFromRight = pArxObject->GetPropertyObject(Prop::UseLeftFromRight)->GetLongValue();
	}

	int lRightFromRight = 0;
	if (pArxObject->GetPropertyObject(Prop::UseLeftFromRight))
	{
		if (pArxObject->GetPropertyObject(Prop::UseRightFromRight)->GetType() == PropBool)
			lRightFromRight = pArxObject->GetPropertyObject(Prop::UseRightFromRight)->GetBooleanValue();
		else
			lRightFromRight = pArxObject->GetPropertyObject(Prop::UseRightFromRight)->GetLongValue();
	}

	int lTopFromBottom = 0;
	if (pArxObject->GetPropertyObject(Prop::UseLeftFromRight))
	{
		if (pArxObject->GetPropertyObject(Prop::UseRightFromRight)->GetType() == PropBool)
			lTopFromBottom = pArxObject->GetPropertyObject(Prop::UseTopFromBottom)->GetBooleanValue();
		else
			lTopFromBottom = pArxObject->GetPropertyObject(Prop::UseTopFromBottom)->GetLongValue();
	}

	int lBottomFromBottom = 0;
	if (pArxObject->GetPropertyObject(Prop::UseLeftFromRight))
		lBottomFromBottom = pArxObject->GetPropertyObject(Prop::UseBottomFromBottom)->GetLongValue();

	// set the offset position properties
	if (lLeftFromRight == 0 || lLeftFromRight == 1)
		pArxObject->SetLongProperty(Prop::LeftFromRight, rcThis.Width() - nTheLeft);
	else if (lLeftFromRight == 2)
		pArxObject->SetLongProperty(Prop::LeftFromRight, nTheLeft - (rcThis.Width()/2));
	else
	{
		CRect rc = GetSplitterRect(lLeftFromRight);
		pArxObject->SetLongProperty(Prop::LeftFromRight, nTheLeft - rc.left);
	}

	if (lRightFromRight == 0 || lRightFromRight == 1)
		pArxObject->SetLongProperty(Prop::RightFromRight, rcThis.Width() - nTheLeft - nTheWidth);
	else
	{
		CRect rc = GetSplitterRect(lRightFromRight);
		pArxObject->SetLongProperty(Prop::RightFromRight, nTheLeft + nTheWidth - rc.left);	
	}

	if (lTopFromBottom == 0 || lTopFromBottom == 1)
		pArxObject->SetLongProperty(Prop::TopFromBottom, rcThis.Height() - nTheTop);
	else
	{
		CRect rc = GetSplitterRect(lTopFromBottom);
		pArxObject->SetLongProperty(Prop::TopFromBottom, nTheTop - rc.top);
	}

	if (lBottomFromBottom == 0 || lBottomFromBottom == 1)
		pArxObject->SetLongProperty(Prop::BottomFromBottom, rcThis.Height() - nTheTop - nTheHeight);
	else
	{
		CRect rc = GetSplitterRect(lBottomFromBottom);
		pArxObject->SetLongProperty(Prop::BottomFromBottom, rc.top - (nTheTop + nTheHeight));
	}
	theWorkspace.SetModified(true);
}

CRect COpenDCLView::GetSplitterRect(int nId)
{
	CRect rc;
	// just in case get the client rect
	GetClientRect( &rc );

	// now find and retrieve the rectangle for the splitter.
	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pCtrl = *iter;
		if( pCtrl->GetType() == CtlSplitter && pCtrl->GetID() == nId )
		{
			rc.SetRect( pCtrl->GetLongProperty( Prop::Left ), pCtrl->GetLongProperty( Prop::Top ),
									pCtrl->GetLongProperty( Prop::Width ), pCtrl->GetLongProperty( Prop::Height ) );

			break;
		}
	}
	return rc;
}

int COpenDCLView::CheckGripsMouseMoveOver(CPoint point) 
{
	HCURSOR hCursor = NULL;
	int nQuadrant = mGripper.HitTest( point, hCursor );
	if( nQuadrant > 0 )
	{
		SetCursor( hCursor );
		return nQuadrant;
	}

	CChildFrame *pFrame = (CChildFrame*)GetParentFrame();
	if (m_StandardCursorID)
		SetCursor(pFrame->m_CrossCursor);		
	else
		SetCursor(pFrame->m_ArrowCursor);
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

	if (m_MouseDown && !m_bDrawing && m_SelectedControl.GetIndex() > -1)
		MoveControl(&m_SelectedControl, point);						

	if (m_MouseDown && !m_bDrawing && m_SelectedList.GetCount() > 0)
	{
		POSITION pos = m_SelectedList.GetHeadPosition();
		while (pos)
		{
			CSelectedControl* pSelControl = m_SelectedList.GetNext(pos);
			if (pSelControl)
				MoveControl(pSelControl, point);						
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


	if (m_bMoving && m_SelectedControl.GetControlHolder() != NULL)
	{
		m_bMoving = false;
		AllRemoveDragRects();

		CRect rcCtrl;
		m_SelectedControl.GetControlHolder()->GetWindowRect(&rcCtrl);
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
			mGripper.MoveTo(m_SelectedControl.m_rcLastDrawn);	
			
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
						if (pSelControl->GetControlHolder() != NULL)
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
				m_SelectedControl.GetTemplate(),
				m_SelectedControl.GetControlHolder());		
			
			pUndo->rcPos = rcCtrl;
			m_UndoList.AddTail(pUndo);
			
			if (!StoreControlsPosition(&m_SelectedControl))
				return;
			
			mGripper.MoveTo(m_SelectedControl.m_rcLastDrawn);	
			
			// invalidate the last draw rect
			m_SelectedControl.Reset();
			for (int i=0; i<m_SelectedList.GetCount(); i++)
			{
				POSITION pos = m_SelectedList.FindIndex(i);
				if (pos != NULL)
				{
					CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
					if (pSelControl != NULL)
						if (pSelControl->GetControlHolder() != NULL)
						{
							// add the event to the undo list
							CUndoActions *pUndo = new CUndoActions(
								uaMoved,
								pSelControl->GetTemplate(),
								pSelControl->GetControlHolder());		
							pUndo->rcPos.left = pSelControl->GetTemplate()->GetLongProperty(Prop::Left);
							pUndo->rcPos.top = pSelControl->GetTemplate()->GetLongProperty(Prop::Top);
							pUndo->rcPos.right = pUndo->rcPos.left + pSelControl->GetTemplate()->GetLongProperty(Prop::Width);
							pUndo->rcPos.bottom = pUndo->rcPos.top + pSelControl->GetTemplate()->GetLongProperty(Prop::Height);
			
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
	
	if (!m_bDrawing && m_SelectedControl.GetTemplate() != NULL)
	{
		if (m_SelectedList.GetCount() != 0)
			FireControlSelected(NULL);
		else
		{
			FireControlSelected(m_SelectedControl.GetTemplate());
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
					FireControlSelected(m_SelectedControl.GetTemplate());
			}
			else
			{
			
				mGripper.Hide();
				ClearSelection();
			}
		}
		else
		{
		
			mGripper.Hide();
			ClearSelection();
		}
		HideSizingRect();
		// draw the focus rect
		m_bDrawing = false;
		CalcAllOffsets();
	}

	
	if (theEditorWorkspace.GetToolBox()->m_nSelectedCtrl < CtlLabel &&
		(m_SelectedControl.GetIndex() == -1 ||
		 m_SelectedControl.GetTemplate() == NULL ||
		 m_SelectedControl.GetControlHolder() == NULL)
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
	if (bResetZOrder)
	{
		CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
		pZOrderList->ClearSelection();
	}
	m_SelectedControl.Clear();
	if (m_SelectedList.GetCount() == 0)
		return;
	mGripper.Hide();		

	for (int i=m_SelectedList.GetCount()- 1; i>=0; i--)
	{
		POSITION pos = m_SelectedList.FindIndex(i);
		if (pos != NULL)
		{
			CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
			if (pSelControl != NULL)
			{
				pSelControl->GetControlHolder()->HideGrips();
				m_SelectedList.RemoveAt(pos);
				pSelControl->Clear();
				delete pSelControl;
			}
		}
	}
	m_SelectedList.RemoveAll();
}

void COpenDCLView::ShowSelection()
{
	POSITION pos = m_SelectedList.GetHeadPosition();
	while( pos )
	{
		CSelectedControl* pSelControl = m_SelectedList.GetNext( pos );
		pSelControl->GetControlHolder()->SetSelected();
	}
}

bool COpenDCLView::IsInSelection(TDclControlPtr pArxObject)
{
	POSITION pos = m_SelectedList.GetHeadPosition();
	while( pos )
	{
		CSelectedControl* pSelControl = m_SelectedList.GetNext( pos );
		if( pSelControl->GetTemplate() == pArxObject )
			return true;
	}
	return false;
}

void COpenDCLView::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	if (m_pThisDclForm == NULL)
		return;

	if (m_MouseDown && !m_bDrawing &&
		m_SelectedControl.GetIndex() > -1)
		return;		
	
	int nQuad = CheckGripsMouseMoveOver(point);
	if (nQuad > -1)
		SetupDragResize(nQuad);
	else
		OnMouseButtonDown(nFlags, point);
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
		m_SelectedControl.Clear();
		mGripper.Hide();
		ClearSelection();
		FireShowFormGrips(TRUE);
	}	
	
	if (m_SelectedControl.GetIndex() != -1 &&
		m_SelectedControl.GetTemplate() != NULL &&
		m_SelectedControl.GetControlHolder() != NULL)
	{
		m_MouseDown = false;
		CRect rcCtrl;
		m_SelectedControl.GetControlHolder()->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		mGripper.MoveTo(rcCtrl);						
		if (m_SelectedList.GetCount() > 0)
			FireControlSelected(NULL);
		else
			FireControlSelected(m_SelectedControl.GetTemplate());
	}
	else
	{
		HideSizingRect();
		m_bDrawing = false;	
		mGripper.Hide();
		ClearSelection();
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
		mGripper.Hide();
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
			m_SelectedControl.Clear();
			mGripper.Hide();
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


TDclControlPtr COpenDCLView::InsertControl(CRect rcPos, ControlType nCtrlToInsert)
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
	
	TDclControlPtr pDclControl = m_pThisDclForm->AddControl( nCtrlToInsert, sControlName );
	assert( pDclControl != NULL );

	if( nCtrlToInsert == CtlActiveX )
	{
		pDclControl->m_clsid = m_clsid;
		pDclControl->m_sLicenseKey = m_sLicenseKey;
	}

	// call the method to add the new control
	CControlHolder *pControl = AddCWndControl( pDclControl, rcPos, true );
	if (pControl == NULL)
	{
		m_pThisDclForm->DeleteControl( pDclControl ); //pDclControl is invalid after this call!
		UpdateZOrderList();
		return NULL;
	}
	
	// add the event to the undo list
	CUndoActions *pUndo = new CUndoActions( uaInsert, pDclControl, pControl );
	m_UndoList.AddTail(pUndo);

	ClearSelection();
	m_SelectedControl.Set( pDclControl, pControl, pDclControl->GetZOrder() );
	pControl->GetWindowRect(&rcPos);
	ScreenToClient(rcPos);
	mGripper.MoveTo(rcPos);

	// Fire the control was inserted event to let the parent know
	FireControlInserted(pDclControl, nCtrlToInsert);

	// call this method to hide the sizing controls
	// and move them to the top of the z-order
	HideSizingRect();
	mGripper.MoveToTop();
	
	// add the new control to the Zorder list
	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
	pZOrderList->ClearSelection();
	pZOrderList->AddControlToList(pDclControl, true, true);
	// call the method to setup the fonts in the font tool bar.
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar(pDclControl);
	return pDclControl;
}

int COpenDCLView::GetNextControlId()
{
	int nHighest = -1;
	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pCtrl = *iter;
		if (pCtrl->GetID() > nHighest)
			nHighest = pCtrl->GetID();
	}
	return nHighest + nControlStartId + 1;
}

CControlHolder* COpenDCLView::AddCWndControl( TDclControlPtr pDclControl, CRect rcPos, bool bForceUpdate )
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
	CWnd* pControlWnd = pControl->GetControlWnd();
	if( pControlWnd && pControlWnd->m_hWnd )
		pControlWnd->GetWindowRect( &rcPos );
	ScreenToClient( &rcPos );
	pDclControl->SetLongProperty( Prop::Left, rcPos.left );
	pDclControl->SetLongProperty( Prop::Top, rcPos.top );
	pDclControl->SetLongProperty( Prop::Width, rcPos.Width() );
	pDclControl->SetLongProperty( Prop::Height, rcPos.Height() );
	CalcControlOffsetDistances( pDclControl, rcPos );

	// if the activeX control's name has not been set yet, we must do it here
	if (pDclControl->GetType() == CtlActiveX)
	{
		CString sName = pDclControl->GetStringProperty(Prop::Name);
		if (sName.IsEmpty())
			// lets get the correct name set here.
			pDclControl->SetStringProperty(Prop::Name, FindNextControlName(pDclControl->GetActiveXTypeName()));		
	}
	
	if (IsWindow(pControl->m_hWnd)) // set the new control to the top of the Z-order
		pControl->SetWindowPos(&CWnd::wndTop, 0, 0, rcPos.Width(), rcPos.Height(), SWP_NOMOVE);
	
	if (pDclControl->IsDeleted())
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
	return 0;
}

void COpenDCLView::OnDclFormNameChange() 
{
	GetDocument()->SetModifiedFlag(TRUE);
}

CString COpenDCLView::FindNextControlName(CString sControlName) 
{
	if( !m_pThisDclForm )
		return CString();
	TDclFormPtr pRoot = m_pThisDclForm;
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

	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pCtrl = *iter;
		CControlHolder *pCtrlHolder = pCtrl->m_pCtrlHolder;
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

void COpenDCLView::ClearChildControls( CControlHolder* pControlHolder ) 
{
	TDclControlPtr pTemplate = pControlHolder->GetTemplate();
	if( pTemplate && pTemplate->GetType() == CtlActiveX )
		pTemplate->SaveToStream( pControlHolder->GetActiveXCtrl() );
}

void COpenDCLView::RefreshChildControl(TDclControlPtr pArxObject, Prop::Id ChangedPropertyID)
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
				ChangedPropertyID = Prop::_All;
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
	case Prop::SmoothProgress:
	case Prop::Sorted:
	case Prop::ImageList:
	case Prop::MultiColumn:
	case Prop::Orientation:
	case Prop::FilterStyle:
	case Prop::Justification:
	case Prop::ComboBoxStyle:
	case Prop::NoIntegralHeight:
	case Prop::HScrollBar:
	case Prop::ShowTicks:		
	case Prop::HasLines:
	case Prop::LinesAtRoot:
	case Prop::HasButtons:
	case Prop::CheckBoxes:
	case Prop::ButtonStyle:
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
	case Prop::Left:
	case Prop::Top:
	case Prop::Width:
	case Prop::Height:
		{
			// call the method to update the arx control's property that was requested to be changed
			pParent->UpdateProperty(ChangedPropertyID);
			ResizeChildControl(pArxObject);
			break;
		}
	case Prop::AutoSize:
	case Prop::Picture:
		{		
			// call the method to update the arx control's property that was requested to be changed
			pParent->UpdateProperty(ChangedPropertyID);
			// check to see if autosize should resize the control
			pParent->AutoSize();
			ResizeChildControl(pArxObject);

			break;
		}
	case Prop::FontName:
	case Prop::FontBold:
	case Prop::FontSize:
	case Prop::FontItalic:
	case Prop::FontUnderline:
	case Prop::FontStrikeout:
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

void COpenDCLView::ResizeChildControl(TDclControlPtr pArxObject)
{
	if (pArxObject == NULL)
		return;

	CControlHolder *pParent = (CControlHolder*)pArxObject->m_pCtrlHolder;
	if (pParent == NULL)
		return;
	CWnd *pControl = pParent->GetControlWnd();
	
	if (pControl == NULL)
		return;
	
	// prepare to resize the child control
	CRect rcControl;

	rcControl.left = pArxObject->GetLongProperty(Prop::Left),
	rcControl.top = pArxObject->GetLongProperty(Prop::Top);
	int nCalcWidth = pArxObject->GetLongProperty(Prop::Width);
	rcControl.right = rcControl.left + pArxObject->GetLongProperty(Prop::Width);
	rcControl.bottom = rcControl.top + pArxObject->GetLongProperty(Prop::Height);
	
	CRect rcThis;
	GetClientRect(&rcThis);
	
	CalcControlOffsetDistances(pArxObject, rcControl);

	// resize this control no mater what property was asked for
	pParent->MoveWindow(rcControl, TRUE);
	if( m_SelectedControl.GetTemplate() == pArxObject )
		mGripper.MoveTo(rcControl );
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

	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL)
		return;

	// if the dcl form is not valid, exit here
	if (m_pThisDclForm == NULL)
		return;
	
	
	ClipBoard.AddTail(m_SelectedControl.GetTemplate());

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->GetIndex() > -1)	
				ClipBoard.AddTail(pSelControl->GetTemplate());
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
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
		return;

	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();

	if (m_SelectedControl.GetTemplate()->GetType() != CtlFileDlgCtrl)
	{

		GetDocument()->SetModifiedFlag(TRUE);

		// to delete first we are going to move it off the visible area
		CRect rc(nm100,nm100,nm99,nm99);
		m_SelectedControl.GetControlHolder()->MoveWindow(rc, TRUE);

		// now we have to ensure it was moved (it's an ActiveX problem to work around because some controls won't be moved)
		CRect rcNew;
		m_SelectedControl.GetControlHolder()->GetWindowRect(&rcNew);
		ScreenToClient(rcNew);

		// if the control was not moved then we hide it. Other wise we can't because some controls destroy themselves if they are hidden.
		if (rc != rcNew)
			m_SelectedControl.GetControlHolder()->ShowWindow(FALSE);
		
		m_SelectedControl.GetTemplate()->SetDeleted();

		// add the new control to the Zorder list
		pZOrderList->RemoveControlFromList(m_SelectedControl.GetTemplate()->GetStringProperty(Prop::Name));
		

		if (m_SelectedControl.GetTemplate()->GetType() == CtlTabStrip)
			FireTabDeleted = true;
			

		mGripper.Hide();

		// add the event to the undo list
		CUndoActions *pUndo = new CUndoActions(
			uaDeleted,
			m_SelectedControl.GetTemplate(),
			m_SelectedControl.GetControlHolder());
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
				if (pSelControl->GetIndex() > -1 && pSelControl->GetTemplate()->GetType() != CtlFileDlgCtrl)	
				{
					pSelControl->GetControlHolder()->HideGrips();
					pSelControl->GetControlHolder()->ShowWindow(SW_HIDE);
					pSelControl->GetTemplate()->SetDeleted();	
					pZOrderList->RemoveControlFromList(pSelControl->GetTemplate()->GetStringProperty(Prop::Name));
					if (m_SelectedControl.GetTemplate()->GetType() == CtlTabStrip)
						FireTabDeleted = true;
					// add the event to the undo list
					CUndoActions *pUndo = new CUndoActions(
						uaDeleted,
						pSelControl->GetTemplate(),
						pSelControl->GetControlHolder());
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
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
		return;

	GetDocument()->SetModifiedFlag(TRUE);

	if (m_SelectedControl.GetTemplate()->GetType() == CtlActiveX)
	{	
		UpdateAxFont(&m_SelectedControl, Prop::FontName, false, -1, sName);		
		// add the event to the undo list
		CUndoActions *pUndo = new CUndoActions(
			uaFontName,
			m_SelectedControl.GetTemplate(),
			m_SelectedControl.GetControlHolder());
		
	}
	else
	{	
		// get the  property object
		TPropertyPtr pProp = m_SelectedControl.GetTemplate()->GetPropertyObject(Prop::FontName);
		
		if (pProp != NULL)
		{
			// add the event to the undo list
			CUndoActions *pUndo = new CUndoActions(
				uaFontName,
				m_SelectedControl.GetTemplate(),
				m_SelectedControl.GetControlHolder());
			
			// and store it's current value
			pUndo->sString = pProp->GetStringValue();
			m_UndoList.AddTail(pUndo);
			
			// update the property 
			pProp->SetStringValue(sName);

			// call the method to update the control
			((CControlHolder*)m_SelectedControl.GetTemplate()->m_pCtrlHolder)->UpdateProperty(Prop::FontName);
		}
	}

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos != NULL)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->GetIndex() > -1)	
			{
				if (pSelControl->GetTemplate()->GetType() == CtlActiveX)
				{	
					UpdateAxFont(pSelControl, Prop::FontName, false, -1, sName);		

					// add the event to the undo list
					CUndoActions *pUndo = new CUndoActions(
						uaFontName,
						pSelControl->GetTemplate(),
						pSelControl->GetControlHolder());
	
				}
				else
				{		
					// get the  property object
					TPropertyPtr pProp = pSelControl->GetTemplate()->GetPropertyObject(Prop::FontName);
					
					if (pProp != NULL)
					{
						// add the event to the undo list
						CUndoActions *pUndo = new CUndoActions(
							uaFontName,
							pSelControl->GetTemplate(),
							pSelControl->GetControlHolder());
		
						// and store it's current value
						pUndo->sString = pProp->GetStringValue();
						m_UndoList.AddTail(pUndo);
						
						// update the property 
						pProp->SetStringValue(sName);
						
						// call the method to update the control
						pSelControl->GetControlHolder()->UpdateProperty(Prop::FontName);
					}
				}
			}
		}		
	}	
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar( m_SelectedControl.GetTemplate() );
}

void COpenDCLView::UpdateAxFont(CSelectedControl *pSelControl, int nId, bool bBool, int nSize, CString sName)
{
	CString	sFontName;
	try
	{
		TPropertyPtr pFontProp = NULL;
		TDclControlPtr pCtrl = pSelControl->GetTemplate();
		const TPropertyList& Props = pCtrl->GetPropertyList();
		for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
		{
			if( (*iter)->GetType() == PropActiveXProp )
			{
				if( (*iter)->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFontDisp ||
						(*iter)->GetAxInterfaceDescriptorPtr()->GetGuid() == IID_IFont )
				{
					pFontProp = (*iter);
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
			case Prop::FontName:
				{
				sFontName = sName;
				break;
				}
			case Prop::FontSize:
				{
				cyFontSize.Lo = nSize * 10000;
				cyFontSize.Hi = 0;
				break;
				}
			case Prop::FontBold:
				{
				m_bBold = bBool;
				break;
				}
			case Prop::FontItalic:
				{
				m_bItalic = bBool;
				break;
				}
			case Prop::FontUnderline:
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
void COpenDCLView::UpdateFontBool(bool bBool, Prop::Id nId)
{
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
		return;

	TPropertyPtr pProp = NULL;
	
	GetDocument()->SetModifiedFlag(TRUE);

	int nAction = 0;
	switch( nId )
	{
	case Prop::FontBold:
		nAction = uaFontBold;
		break;
	case Prop::FontUnderline:
		nAction = uaFontUnderline;
		break;
	case Prop::FontItalic:
		nAction = uaFontItalic;
		break;
	};
	if (m_SelectedControl.GetTemplate()->GetType() == CtlActiveX)
	{	
		UpdateAxFont(&m_SelectedControl, nId, bBool);		
		// add the event to the undo list
		CUndoActions *pUndo = new CUndoActions(
			nAction,
			m_SelectedControl.GetTemplate(),
			m_SelectedControl.GetControlHolder());
	}
	else
	{
		// get the  property object
		pProp = m_SelectedControl.GetTemplate()->GetPropertyObject(nId);
		
		if (pProp != NULL)
		{
			// add the event to the undo list
			CUndoActions *pUndo = new CUndoActions(
				nAction,
				m_SelectedControl.GetTemplate(),
				m_SelectedControl.GetControlHolder());

			// and store it's current value
			pUndo->bBool = pProp->GetBooleanValue();
			m_UndoList.AddTail(pUndo);
			
			// update the property 
			pProp->SetBooleanValue(bBool);

			// call the method to update the control
			((CControlHolder*)m_SelectedControl.GetTemplate()->m_pCtrlHolder)->UpdateProperty(Prop::FontName);
		}
	}

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos != NULL)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->GetIndex() > -1)	
			{
				if (pSelControl->GetTemplate()->GetType() == CtlActiveX)
				{
					UpdateAxFont(pSelControl, nId, bBool);
					// add the event to the undo list
					CUndoActions *pUndo = new CUndoActions(
						nAction,
						pSelControl->GetTemplate(),
						pSelControl->GetControlHolder());
		
				}
				else
				{				
					// get the  property object
					pProp = pSelControl->GetTemplate()->GetPropertyObject(nId);
					
					if (pProp != NULL)
					{						
						// add the event to the undo list
						CUndoActions *pUndo = new CUndoActions(
							nAction,
							pSelControl->GetTemplate(),
							pSelControl->GetControlHolder());

						// and store it's current value
						pUndo->bBool = pProp->GetBooleanValue();
						m_UndoList.AddTail(pUndo);
						
						// update the property 
						pProp->SetBooleanValue(bBool);
						
						// call the method to update the control
						pSelControl->GetControlHolder()->UpdateProperty(Prop::FontName);
					}
				}
			}
		}		
	}	
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar( m_SelectedControl.GetTemplate() );
}

void COpenDCLView::UpdateFontSize(int nSize) 
{
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
		return;

	GetDocument()->SetModifiedFlag(TRUE);
	
	if (m_SelectedControl.GetTemplate()->GetType() == CtlActiveX)
	{	
		UpdateAxFont(&m_SelectedControl, Prop::FontSize, false, nSize);		

		// add the event to the undo list
		CUndoActions *pUndo = new CUndoActions(
			uaFontSize,
			m_SelectedControl.GetTemplate(),
			m_SelectedControl.GetControlHolder());
	
	}
	else
	{
		// get the  property object
		TPropertyPtr pProp = m_SelectedControl.GetTemplate()->GetPropertyObject(Prop::FontSize);

		if (pProp != NULL)
		{
			// add the event to the undo list
			CUndoActions *pUndo = new CUndoActions(
				uaFontSize,
				m_SelectedControl.GetTemplate(),
				m_SelectedControl.GetControlHolder());
			
			// and store it's current value
			pUndo->lLong= pProp->GetLongValue();
			m_UndoList.AddTail(pUndo);
			
			// update the property 
			pProp->SetLongValue(nSize);

			// call the method to update the control
			((CControlHolder*)m_SelectedControl.GetTemplate()->m_pCtrlHolder)->UpdateProperty(Prop::FontName);
		}
	}

	POSITION pos = m_SelectedList.GetHeadPosition();
	while (pos != NULL)
	{
		CSelectedControl *pSelControl = m_SelectedList.GetNext(pos);
		if (pSelControl != NULL)
		{
			if (pSelControl->GetIndex() > -1)	
			{
				if (pSelControl->GetTemplate()->GetType() == CtlActiveX)
				{	
					UpdateAxFont(pSelControl, Prop::FontSize, false, nSize);		

					// add the event to the undo list
					CUndoActions *pUndo = new CUndoActions(
						uaFontSize,
						pSelControl->GetTemplate(),
						pSelControl->GetControlHolder());
				}
				else
				{				
					// get the  property object
					TPropertyPtr pProp = pSelControl->GetTemplate()->GetPropertyObject(Prop::FontSize);
					
					if (pProp != NULL)
					{
						// add the event to the undo list
						CUndoActions *pUndo = new CUndoActions(
							uaFontSize,
							pSelControl->GetTemplate(),
							pSelControl->GetControlHolder());
			
						// and store it's current value
						pUndo->lLong = pProp->GetLongValue();
						m_UndoList.AddTail(pUndo);
						
						// update the property 
						pProp->SetLongValue(nSize);
						
						// call the method to update the control
						pSelControl->GetControlHolder()->UpdateProperty(Prop::FontName);
					}
				}
			}
		}		
	}	
	theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar( m_SelectedControl.GetTemplate() );
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
	
		mGripper.Hide();
		// clear the previous selection
		m_SelectedControl.Clear();
		ClearSelection();
		
		// create a position variable to hold the counter increment
		POSITION pos;	
		
		// set start position for navigating clipboard controls
		pos = ClipBoard.GetHeadPosition();

		// do loop to navigate clip board controls
		while (pos != NULL)
		{
			// get current clipboard control
			TDclControlPtr pCopy = ClipBoard.GetNext(pos);
			pCopy->SetOwnerForm(m_pThisDclForm);
					
			ClearEventProperties(pCopy);
			if (pCopy->GetType() == CtlFileDlgCtrl)
				continue;

			CString sNameOfCopy = FindNextControlName( GetControlName( pCopy ) );
			TPropertyPtr pCaptionProp = pCopy->GetPropertyObject( Prop::Caption );
			if( pCaptionProp && pCaptionProp->GetStringValue() == pCopy->GetKeyName() )
				pCaptionProp->SetStringValue( sNameOfCopy );
			pCopy->SetStringProperty( Prop::Name, sNameOfCopy );
			//if the copy will be a duplicate, offset it from the original and rename it
			if( m_pThisDclForm->FindControl( sNameOfCopy, pCopy->GetType() ) )
			{
				// make the control be offset by nControlOffset
				pCopy->SetLongProperty( Prop::Left, pCopy->GetLongProperty(Prop::Left) + nControlOffset );
				pCopy->SetLongProperty( Prop::Top, pCopy->GetLongProperty(Prop::Top) + nControlOffset );
			}
			
			CRect rcThis;
			GetClientRect(&rcThis);
			
			// check to make sure the control in inside the form
			int nCtrlLeft = pCopy->GetLongProperty(Prop::Left);
			int nCtrlWidth = pCopy->GetLongProperty(Prop::Width);
			if (nCtrlLeft > rcThis.Width())
			{
				nCtrlLeft = rcThis.Width() - nCtrlWidth;
				pCopy->SetLongProperty(Prop::Left, nCtrlLeft);
			}
			
			// check to make sure the control in inside the form
			int nCtrlTop = pCopy->GetLongProperty(Prop::Top);
			int nCtrlHeight = pCopy->GetLongProperty(Prop::Height);
			if (nCtrlTop > rcThis.Height())
			{
				nCtrlTop = rcThis.Height() - nCtrlHeight;
				pCopy->SetLongProperty(Prop::Top, nCtrlTop);
			}

			// add current clipboard control to DclFormObject
			m_pThisDclForm->AddControl( pCopy );
			
			CRect rcPos( nCtrlLeft, nCtrlTop, nCtrlLeft + nCtrlWidth, nCtrlTop + nCtrlHeight );
			// call the method to add the CWnd control 
			CControlHolder *pControl = AddCWndControl(pCopy, rcPos, true);
			if (pControl == NULL)
				return;
	
			// increment counter
			nCount--;
			nCopied++;
			if (nCopied == 1)
				m_SelectedControl.Set( pCopy, pControl, pCopy->GetZOrder() );
			else
			{
				CSelectedControl *pSelCtrl = new CSelectedControl( pCopy, pControl, pCopy->GetZOrder() );
				pControl->SetSelected();
				m_SelectedList.AddTail(pSelCtrl);
			}			
			
			// add the new control to the Zorder list
			pZOrderList->AddControlToList(pCopy, true, (pCopy == m_SelectedControl.GetTemplate()));
			// call the method to setup the fonts in the font tool bar.
			theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetFontToolBar(pCopy);
		}
		// call method to ensure the selection set is highlighted
		CRect rcCtrl;
		if (m_SelectedControl.GetControlHolder() == NULL)
		{
			CalcAllOffsets();
			return;
		}
		m_SelectedControl.GetControlHolder()->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		mGripper.MoveTo(rcCtrl);
		mGripper.MoveToTop();
		ShowSelection();		
		FireControlSelected(m_SelectedControl.GetTemplate());
	}	
		
	CalcAllOffsets();
}

LRESULT COpenDCLView::OnGripStartDrag(WPARAM wParam, LPARAM lParam)
{
	SetupDragResize( (int)wParam );
	return 0;
}

LRESULT COpenDCLView::OnGripDragging(WPARAM wParam, LPARAM lParam)
{
	CPoint pt( LOWORD(lParam), HIWORD(lParam) );
	DragResized( (int)wParam, pt );
	return 0;
}

void COpenDCLView::SetupDragResize(int nQuadrant) 
{
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
	{
		mGripper.Hide();
		return;
	}

	m_bDrawing = false;
	m_MouseDown = true;
	m_nResizeQuadrant = nQuadrant;	
	CRect rcPos;
	m_SelectedControl.GetControlHolder()->GetWindowRect(&rcPos);
	
	// set the x & y coord's
	m_DrawStartPoint.x = rcPos.left;
	m_DrawStartPoint.y = rcPos.top;
	m_rcDraw = rcPos;
	HideSizingRect();

}

void COpenDCLView::ClearEventProperties(TDclControlPtr pCtrl)
{
	pCtrl->ResetProperty(Prop::EventBtnClicked);
	pCtrl->ResetProperty(Prop::EventClicked);
	pCtrl->ResetProperty(Prop::EventFolderChanged);
	pCtrl->ResetProperty(Prop::EventOnHelp);
	pCtrl->ResetProperty(Prop::EventOnTypeChange);
	pCtrl->ResetProperty(Prop::EventEditChanged);
	pCtrl->ResetProperty(Prop::EventDblClicked);
	pCtrl->ResetProperty(Prop::EventSelChanged);      
	pCtrl->ResetProperty(Prop::EventGetDayState);
	pCtrl->ResetProperty(Prop::EventSelect);
	pCtrl->ResetProperty(Prop::EventKillFocus);
	pCtrl->ResetProperty(Prop::EventOutOfMemory);
	pCtrl->ResetProperty(Prop::EventRClick);
	pCtrl->ResetProperty(Prop::EventRDblClick);
	pCtrl->ResetProperty(Prop::EventReturn);
	pCtrl->ResetProperty(Prop::EventSetFocus);
	pCtrl->ResetProperty(Prop::EventBeginLabelEdit);
	pCtrl->ResetProperty(Prop::EventDeleteItem);
	pCtrl->ResetProperty(Prop::EventEndLabelEdit);
	pCtrl->ResetProperty(Prop::EventItemExpanded);
	pCtrl->ResetProperty(Prop::EventItemExpanding);
	pCtrl->ResetProperty(Prop::EventKeyDown);
	pCtrl->ResetProperty(Prop::EventKeyUp);
	pCtrl->ResetProperty(Prop::EventMouseDown);
	pCtrl->ResetProperty(Prop::EventMouseDown);
	pCtrl->ResetProperty(Prop::EventMouseMove);
	pCtrl->ResetProperty(Prop::EventMouseMovedOff);
	pCtrl->ResetProperty(Prop::EventMouseEntered);
	pCtrl->ResetProperty(Prop::EventColumnClick);
	pCtrl->ResetProperty(Prop::EventMouseDblClick);
	pCtrl->ResetProperty(Prop::EventMouseWheel);
	pCtrl->ResetProperty(Prop::EventPaint);
	pCtrl->ResetProperty(Prop::EventNavigateComplete);
  pCtrl->ResetProperty(Prop::EventSelChanging);
	pCtrl->ResetProperty(Prop::EventChanged);
	pCtrl->ResetProperty(Prop::EventMaxText);
  pCtrl->ResetProperty(Prop::EventUpdate);
  pCtrl->ResetProperty(Prop::EventDropDown);
  pCtrl->ResetProperty(Prop::EventScroll);
	pCtrl->ResetProperty(Prop::EventScrolled);
	pCtrl->ResetProperty(Prop::EventReleasedCapture);
	pCtrl->ResetProperty(Prop::FormEventClose);
	pCtrl->ResetProperty(Prop::FormEventInitialize);
	pCtrl->ResetProperty(Prop::FormEventSize);
	pCtrl->ResetProperty(Prop::FormEventShow);
	pCtrl->ResetProperty(Prop::DocEventActivated);
	pCtrl->ResetProperty(Prop::CfgEventCancel);
	pCtrl->ResetProperty(Prop::CfgEventOK);
	pCtrl->ResetProperty(Prop::CfgEventHelp);
	pCtrl->ResetProperty(Prop::CfgEventApply);
	pCtrl->ResetProperty(Prop::EventReturnPressed);
	pCtrl->ResetProperty(Prop::FormEventCancelClose);
	pCtrl->ResetProperty(Prop::FormEventOnOk);
	pCtrl->ResetProperty(Prop::FormEventOnCancel);
	pCtrl->ResetProperty(Prop::OnLMouseEvent);
	pCtrl->ResetProperty(Prop::OnMMouseEvent);
	pCtrl->ResetProperty(Prop::OnRMouseEvent);
	pCtrl->ResetProperty(Prop::DragnDropToAutoCAD);
	pCtrl->ResetProperty(Prop::DragnDropFromControl);        
	pCtrl->ResetProperty(Prop::DragnDropFromAutoCAD);
	pCtrl->ResetProperty(Prop::DragnDropBegin);


}

void COpenDCLView::DragResized(int nQuadrant, CPoint point) 
{
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
	{
		mGripper.Hide();
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
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
	{
		HideSizingRect();
		m_bDrawing = false;	
		mGripper.Hide();
		return;
	}
	m_MouseDown = false;
	
	mGripper.CancelSizing();
	m_nResizeQuadrant = -1;	
	
	// hide the sizing rect.
	HideSizingRect();
	m_bDrawing = false;	
		
	if (m_rcDraw.Width() <= 0 ||
			m_rcDraw.Height() <= 0 ||
			m_rcDraw.right <= 0 ||
			m_rcDraw.bottom <= 0)
		return;

	CControlHolder* pControlHolder = m_SelectedControl.GetControlHolder();
	CRect rcOldPos;
	pControlHolder->GetWindowRect( &rcOldPos );
	ScreenToClient( &rcOldPos );

	// add the event to the undo list
	CUndoActions *pUndo = new CUndoActions( uaMoved, m_SelectedControl.GetTemplate(), pControlHolder);
	pUndo->rcPos = rcOldPos;
	m_UndoList.AddTail( pUndo );

	CWnd *pControl = pControlHolder->GetControlWnd();
	assert( pControl != NULL );
	if( !pControl )
		return;

	CRect rcPos = m_rcDraw;
	ScreenToClient( &rcPos );
	pControlHolder->MoveWindow( &rcPos );
	mGripper.MoveTo( rcPos );
	CRect rcNewPos( 0, 0, rcPos.Width(), rcPos.Height() );
	pControl->MoveWindow( &rcNewPos );
	pControl->GetWindowRect( &rcNewPos );
	ScreenToClient( &rcNewPos );
	pControlHolder->MoveWindow( &rcNewPos );
	pControl->ShowWindow( SW_SHOW );
	pControlHolder->ShowWindow( SW_SHOW );
	pControl->Invalidate();
	pControlHolder->Invalidate();
	InvalidateRect( &CRect( rcOldPos | rcNewPos ), TRUE );
	UpdateWindow();

	// set the position properties
	m_SelectedControl.GetTemplate()->SetLongProperty(Prop::Left, rcNewPos.left);
	m_SelectedControl.GetTemplate()->SetLongProperty(Prop::Top, rcNewPos.top);
	m_SelectedControl.GetTemplate()->SetLongProperty(Prop::Width, rcNewPos.Width());
	m_SelectedControl.GetTemplate()->SetLongProperty(Prop::Height, rcNewPos.Height());
	CalcControlOffsetDistances(m_SelectedControl.GetTemplate(), rcNewPos);
	pControlHolder->AutoSize();

	if (m_SelectedControl.GetTemplate()->GetType() == CtlTabStrip)
		ResizeChildTabPanes();

	FireControlSelected(m_SelectedControl.GetTemplate());
	FireSetUndo();
}

void COpenDCLView::OnSize(UINT nType, int cx, int cy) 
{
	InvalidateRect(&m_rcGripRect);
	CView::OnSize(nType, cx, cy);
	if (m_pThisDclForm != NULL)
	{
		TDclControlPtr pDclProps = m_pThisDclForm->GetControlProperties();
	
		CRect rc;
		if( m_pThisDclForm->UsesClientRect() )
			GetClientRect(&rc);
		else
			GetParent()->GetWindowRect(&rc);
		 
		pDclProps->SetLongProperty(Prop::Width, rc.Width());
		pDclProps->SetLongProperty(Prop::Height, rc.Height());

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
	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		CRect rcPos( pDclControl->GetLongProperty(Prop::Left),
								 pDclControl->GetLongProperty(Prop::Top),
								 pDclControl->GetLongProperty(Prop::Width),
								 pDclControl->GetLongProperty(Prop::Height) );
		CalcControlOffsetDistances( pDclControl, rcPos );
	}
}


// refresh the selected control
void COpenDCLView::RefreshSelectControl() 
{
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
	{
		mGripper.Hide();
		return;
	}
	
	// if the program made it this far, destroy the previous control
	CControlHolder *pParent = m_SelectedControl.GetTemplate()->m_pCtrlHolder;
	if (pParent == NULL)
		return;
	CWnd *pControl = pParent->GetControlWnd();
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
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
	{
		mGripper.Hide();
		return;
	}
	
	if (direction == 0)
		m_SelectedControl.GetControlHolder()->SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);		
	else
		m_SelectedControl.GetControlHolder()->SetWindowPos(&CWnd::wndBottom, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
	
	// call method to adjust the control's placement
	m_pThisDclForm->ReorderControl( m_SelectedControl.GetTemplate(), (direction != 0) );
	
	for (int i=0; i<m_SelectedList.GetCount(); i++)
	{
		POSITION pos = m_SelectedList.FindIndex(i);
		if (pos != NULL)
		{
			CSelectedControl *pSelControl = m_SelectedList.GetAt(pos);
			if (pSelControl != NULL)
			{
				if (pSelControl->GetIndex() > -1)	
				{
					if (direction == 0)
						pSelControl->GetControlHolder()->SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
					else
						pSelControl->GetControlHolder()->SetWindowPos(&CWnd::wndBottom, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
					// call method to adjust the control's placement
					m_pThisDclForm->ReorderControl( pSelControl->GetTemplate(), (direction != 0) );
				}
			}
		}
	}
	mGripper.MoveToTop();
	UpdateZOrderList();
}


void COpenDCLView::DisplayControls(TDclFormPtr pDclForm) 
{
	m_pThisDclForm = pDclForm;
	if (m_pThisDclForm == NULL)
		return;

	UpdateZOrderList();
	
	mGripper.Hide();
	HideSizingRect();

	const TDclControlList& Controls = pDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		if( !pDclControl->IsDeleted() && pDclControl->GetType() > CtlForm )
		{
			CRect rcPos;
			rcPos.left = pDclControl->GetLongProperty(Prop::Left);
			rcPos.top = pDclControl->GetLongProperty(Prop::Top);
			rcPos.right = rcPos.left + pDclControl->GetLongProperty(Prop::Width);
			rcPos.bottom = rcPos.top + pDclControl->GetLongProperty(Prop::Height);
			AddCWndControl( pDclControl, rcPos, false);
		}
	}

	CalcAllOffsets();

	CRect rc;
	if( m_pThisDclForm->UsesClientRect() )
		GetClientRect(&rc);
	else
		GetParent()->GetWindowRect(&rc);
	TDclControlPtr pDclProps = m_pThisDclForm->GetControlProperties();
	pDclProps->SetLongProperty(Prop::Width, rc.Width());
	pDclProps->SetLongProperty(Prop::Height, rc.Height());
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
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
	{
		mGripper.Hide();
		return -1;
	}
	
	if (m_SelectedList.GetCount() > 0)
		return -2;
	else
		return m_SelectedControl.GetIndex();
}

long COpenDCLView::GetSelectedType() 
{
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
	{
		mGripper.Hide();
		return -1;
	}
	

	return m_SelectedControl.GetTemplate()->GetType();
}


void COpenDCLView::OnLostFocus() 
{
	mGripper.Hide();
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
				pUndoAction->m_pArxObject->SetStringProperty(Prop::FontName, pUndoAction->sString);
				
				// call the method to update the control
				((CControlHolder*)m_SelectedControl.GetTemplate()->m_pCtrlHolder)->UpdateProperty(Prop::FontName);

				break;
				}
			case uaFontSize:
				{
				pUndoAction->m_pArxObject->SetLongProperty(Prop::FontSize, pUndoAction->lLong);
				
				// call the method to update the control
				((CControlHolder*)m_SelectedControl.GetTemplate()->m_pCtrlHolder)->UpdateProperty(Prop::FontName);

				break;
				}
			case uaMoved:
				{
				pUndoAction->m_pControl->MoveWindow(pUndoAction->rcPos, TRUE);
				pUndoAction->m_pArxObject->SetLongProperty(Prop::Left, pUndoAction->rcPos.left);
				pUndoAction->m_pArxObject->SetLongProperty(Prop::Top, pUndoAction->rcPos.top);
				pUndoAction->m_pArxObject->SetLongProperty(Prop::Width, pUndoAction->rcPos.Width());
				pUndoAction->m_pArxObject->SetLongProperty(Prop::Height, pUndoAction->rcPos.Height());
				if (m_SelectedControl.GetIndex() == pUndoAction->m_pArxObject->GetZOrder())
					mGripper.MoveTo(pUndoAction->rcPos);
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
						rc.left = pUndoAction->m_pArxObject->GetLongProperty(Prop::Left);
						rc.top = pUndoAction->m_pArxObject->GetLongProperty(Prop::Top);
						rc.right = rc.left + pUndoAction->m_pArxObject->GetLongProperty(Prop::Width);
						rc.bottom = rc.top + pUndoAction->m_pArxObject->GetLongProperty(Prop::Height);
						pUndoAction->m_pControl->MoveWindow(rc, TRUE);
					}
					else
						pUndoAction->m_pControl->ShowWindow(TRUE);

					
				
					pUndoAction->m_pArxObject->SetDeleted( false );
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

					pUndoAction->m_pArxObject->SetDeleted();
					if (pUndoAction->m_pArxObject->GetType() == CtlTabStrip)
						FireTabControlDeleted();
					mGripper.Hide();
					break;
				}
			}				
		}
		delete pUndoAction;
		m_UndoList.RemoveAt(pos);
	}
	
	
}

void COpenDCLView::UpdateControl(TDclControlPtr pArxObject, Prop::Id ChangedPropertyID) 
{
	
	if (m_pThisDclForm == NULL)
		return;

	// call the method to refresh the control
	RefreshChildControl(pArxObject, ChangedPropertyID);

	if (m_SelectedControl.GetIndex() != -1 &&
		m_SelectedControl.GetTemplate() != NULL &&
		m_SelectedControl.GetControlHolder() != NULL)
	{
		CRect rcCtrl;
		try
		{
			if (IsWindow(m_SelectedControl.GetControlHolder()->m_hWnd))
				m_SelectedControl.GetControlHolder()->GetWindowRect(&rcCtrl);
			ScreenToClient(rcCtrl);
			mGripper.MoveTo(rcCtrl);
		}
		catch(...)
		{
		}		
	}
}

void COpenDCLView::CheckPictureRefs() 
{
	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		if( pDclControl->GetPropertyObject(Prop::Picture) )
			pDclControl->m_pCtrlHolder->UpdateProperty( Prop::Picture);
	}
}

void COpenDCLView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDblClk(nFlags, point);
	if (m_pThisDclForm == NULL)
		return;

	// check to see if we are going to try and select controls
	CheckControlsForSelection(point, nFlags, true);

	
	int nArxIndex = m_SelectedControl.GetIndex();

	if (nArxIndex == -1)
		FireShowPropertyWizard(m_pThisDclForm->GetControlProperties());
	else
		FireShowPropertyWizard(m_SelectedControl.GetTemplate());
}

void COpenDCLView::CalcAllOffsets() 
{
	CRect rcThis;
	GetClientRect(&rcThis);
	
	if (m_pThisDclForm == NULL)
		return;

	// we are going to do a loop to ensure each control's offset properties are calculated correctly
	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		if (!pDclControl->IsDeleted())
		{
			CRect rcPos(pDclControl->GetLongProperty(Prop::Left),
									pDclControl->GetLongProperty(Prop::Top),
									pDclControl->GetLongProperty(Prop::Width),
									pDclControl->GetLongProperty(Prop::Height));
			CalcControlOffsetDistances(pDclControl, rcPos);
		}
	}
}

void COpenDCLView::RefreshControlsWithPictures() 
{
	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		if( pDclControl->GetPropertyObject(Prop::Picture) )
			RefreshChildControl( pDclControl, Prop::_Private );
	}
}

void COpenDCLView::ZOrderUpdateOfSelCtrl(short ZIndex) 
{
	
	if (m_SelectedControl.GetIndex() == -1 ||
		m_SelectedControl.GetTemplate() == NULL ||
		m_SelectedControl.GetControlHolder() == NULL)
	{
		mGripper.Hide();
		return;
	}
			
	// call method to adjust the control's placement
	m_pThisDclForm->ReorderControl( m_SelectedControl.GetTemplate(), size_t(ZIndex) );

	mGripper.MoveToTop();
	UpdateZOrderList();
}

bool COpenDCLView::IsTabsEnabled()
{
	if (m_pThisDclForm == NULL)
		return true;

	// do loop to find out if this form has a tab on it.
	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		// if it does, we must return a false to indicate the tab toolbox button should be disabled.
		if (pDclControl->GetType() == CtlTabStrip && !pDclControl->IsDeleted())
			return false;
	}
	// if it doesn't, we must return a true to indicate the tab toolbox button should be enabled.
	return true;
}


void COpenDCLView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	if (pDeactiveView == pActivateView)
	{
		if (theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().m_pControl != m_SelectedControl.GetTemplate())
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
		TDclControlPtr pCurrentCtrl = m_SelectedControl.GetTemplate();
		if( !pCurrentCtrl )
		{
			TDclFormPtr pCurrentForm = ((COpenDCLView*)pActivateView)->m_pThisDclForm;
			if( pCurrentForm )
				pCurrentCtrl = pCurrentForm->GetControlProperties();
		}
		theEditorWorkspace.GetMainFrame()->m_wndDlgBar.SetActiveView(pActivateView);
		CToolBox *pToolBox = theEditorWorkspace.GetToolBox();
		pToolBox->SetActiveView(pActivateView);
		theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties( pCurrentCtrl, this );
	}
}

void COpenDCLView::UpdateZOrderList() 
{
	if( !m_pThisDclForm )
		return;

	CZOrderListCtrl* pZOrderList = theEditorWorkspace.GetZOrderListCtrl();
	pZOrderList->ClearList( this );
	pZOrderList->SetRedraw( FALSE ); // turn drawing off regardless of list mode

	const TDclControlList& Controls = m_pThisDclForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		if( !pDclControl->IsDeleted() && pDclControl->GetType() > CtlForm )
		{
			bool bIsHot = (pDclControl == m_SelectedControl.GetTemplate());
			pZOrderList->AddControlToList( pDclControl,
																		 (bIsHot ||
																			(pDclControl->m_pCtrlHolder && pDclControl->m_pCtrlHolder->IsSelected())),
																		 bIsHot );
		}
	}
	pZOrderList->SetRedraw( TRUE ); // turn drawing back on and update the window
	pZOrderList->Invalidate();
	pZOrderList->UpdateWindow(); 
	
	if( m_pThisDclForm->m_htiTreeItem )
		theEditorWorkspace.GetProjectTreeCtrl()->SelectItem( m_pThisDclForm->m_htiTreeItem );
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

	if (m_SelectedControl.GetIndex() != -1 &&
		m_SelectedControl.GetTemplate() != NULL &&
		m_SelectedControl.GetControlHolder() != NULL)
	{
		CRect rcCtrl;
		if (IsWindow(m_SelectedControl.GetControlHolder()->m_hWnd))
			m_SelectedControl.GetControlHolder()->GetWindowRect(&rcCtrl);
		ScreenToClient(rcCtrl);
		mGripper.MoveTo(rcCtrl);						
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
	pPopup->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, AfxGetApp()->m_pMainWnd );
}

BOOL COpenDCLView::PreTranslateMessage(MSG* pMsg) 
{
	return CView::PreTranslateMessage(pMsg);
}

void COpenDCLView::FireControlSelected(TDclControlPtr pArxControl)
{
	if (pArxControl != NULL)
	{
		// force the property list to be repainted.
		theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties(pArxControl, this);
		if (m_SelectedControl.GetTemplate() != pArxControl)
			m_SelectedControl.Set( pArxControl, pArxControl->m_pCtrlHolder, pArxControl->GetZOrder() );
		CRect rc;
		m_SelectedControl.GetControlHolder()->GetWindowRect(&rc);
		ScreenToClient(rc);
		mGripper.MoveTo(rc);	
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

void COpenDCLView::FireTabControlDeleted()
{
	theEditorWorkspace.GetProjectTreeCtrl()->RemoveChildren( m_pThisDclForm->m_htiTreeItem );
	const TDclFormList& Forms = m_pThisDclForm->GetProject()->GetDclFormList();
	for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
	{
		if( (*iter)->GetParentForm() == m_pThisDclForm )
			(*iter)->SetDeleted();
	}
}

void COpenDCLView::FireSetUndo()
{
	theWorkspace.SetModified(true);
}

void COpenDCLView::FireControlInserted(TDclControlPtr pArxControl, long ControlType)
{
	// have the property list show the new control
	if (pArxControl != NULL)
		theEditorWorkspace.GetPropertyTabs()->DisplaySelectedControlProperties(pArxControl, this);
	// reset the toolbar to the pointer
	m_StandardCursorID = false;
	theEditorWorkspace.GetToolBox()->m_nSelectedCtrl = 1;
	theWorkspace.SetModified(true);

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
//	TEditorProjectPtr pProject = activeProject;
//	
//	for (int i=0; i<3; i++)
//	{
//		TDclFormPtr pNewDclForm = new CDclFormObject( pProject, VdclTabForm );
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
//		TDclControlPtr pDclProperties = pNewDclForm->GetControlProperties();
//		if (pDclProperties != NULL)
//		{
//			pDclProperties->SetLongProperty(Prop::Height, GetSelectedTabClientHeight());
//			pDclProperties->SetLongProperty(Prop::Width, GetSelectedTabClientWidth());
//		}
//
//		// add the new dcl form to the project tree so it's shown there
//		CProjectTreeCtrl *pProjTree = theEditorWorkspace.GetProjectTreeCtrl();
//		pProjTree->AddFormToTree(pNewDclForm, true);
//	}	
//}


bool COpenDCLView::CanRemoveChildTabPane(int nIndex)
{
	if (nIndex == -1)
		return true;
	TDclFormPtr pTabForm = m_pThisDclForm->GetProject()->FindDclTabChildForm(m_pThisDclForm->GetUniqueName(), nIndex);
	if( !pTabForm || pTabForm->GetControlList().size() <= 1 )
		return true;
	if( MessageBox( theWorkspace.LoadResourceString(IDS_QDELETEFORM),
									theWorkspace.LoadResourceString(IDR_MAINFRAME),
									MB_ICONEXCLAMATION | MB_YESNO) == 6 )
		return true;
	return false;
}

void COpenDCLView::ResizeChildTabPanes()
{
	TDclControlPtr pDclControl = m_SelectedControl.GetTemplate();
	assert( pDclControl->GetType() == CtlTabStrip );
	TPropertyPtr pProp = pDclControl->GetPropertyObject(Prop::TabsCaption);
	if( !pProp )
		return;
	int nNewHeight = -1;
	int nNewWidth = -1;
	CTabStripCtrl* pTabStripCtrl = (CTabStripCtrl*)pDclControl->GetControlInstance();
	if( pTabStripCtrl )
	{
		CRect rcTab = pTabStripCtrl->GetUsedArea();
		nNewHeight = rcTab.Height();
		nNewWidth = rcTab.Width();
	}
	for (size_t i=0; i<pProp->GetStringArrayPtr()->size(); i++)
	{
		TDclFormPtr pTabForm = pDclControl->GetOwnerProject()->FindDclTabChildForm(m_pThisDclForm->GetUniqueName(), i);
		if (!pTabForm)
			continue;
		TDclControlPtr pDclProperties = pTabForm->GetControlProperties();
		pDclProperties->SetLongProperty(Prop::Width, nNewWidth);
		pDclProperties->SetLongProperty(Prop::Height, nNewHeight);
		if (pTabForm->m_pMdiChildWnd != NULL)
		{
			CRect rcClient;
			pTabForm->m_pMdiChildWnd->GetClientRect(&rcClient);
			CRect rcWindow;
			pTabForm->m_pMdiChildWnd->GetWindowRect(&rcWindow);
			int nWidth = nNewWidth + (rcWindow.Width() - rcClient.Width());
			int nHeight = nNewHeight + (rcWindow.Height() - rcClient.Height());
			pTabForm->m_pMdiChildWnd->SetWindowPos(NULL, -1, -1, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}	
	theWorkspace.SetModified(true);
}

void COpenDCLView::FireShowPropertyWizard(TDclControlPtr pArxControl)
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

	TDclControlPtr pCtrl = m_pThisDclForm->GetControlProperties();
	if( pCtrl && pCtrl->GetBooleanProperty(Prop::Resizable) )
	{
		UpdateGripPos();
		DrawFrameControl(hdc, &m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}	
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
	theWorkspace.SetModified(true);
	theEditorWorkspace.GetPropertyTabs()->m_PropertiesTabPane.GetPropertiesCtrl().Invalidate();
}

void COpenDCLView::OnToolsClearlispsymbolnames()
{
	CFormVarNameUpdate Dlg( m_pThisDclForm, NULL );
	Dlg.DoModal();
	theWorkspace.SetModified(true);
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
