// DclFormView.cpp : implementation of the CDclFormView class
//

#include "stdafx.h"
#include "DclFormView.h"
#include "StudioDialogObject.h"
#include "StudioWorkspace.h"
#include "StudioFrame.h"
#include "OpenDCLDoc.h"
#include "ControlManager.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDclFormView

IMPLEMENT_DYNCREATE(CDclFormView, CScrollView)

BEGIN_MESSAGE_MAP(CDclFormView, CScrollView)
	ON_COMMAND(ID_WINDOW_NEXTFORM, &CDclFormView::OnWindowNextForm)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_NEXTFORM, &CDclFormView::OnUpdateWindowNextForm)
	ON_COMMAND(ID_WINDOW_PREVFORM, &CDclFormView::OnWindowPrevForm)
	ON_UPDATE_COMMAND_UI(ID_WINDOW_PREVFORM, &CDclFormView::OnUpdateWindowPrevForm)
END_MESSAGE_MAP()


// CDclFormView construction/destruction

CDclFormView::CDclFormView()
: mpDialogObject( NULL )
{
}

CDclFormView::CDclFormView( TDclFormPtr pSourceForm )
: mpDialogObject( new CStudioDialogObject( pSourceForm, this, false ) )
{
}

CDclFormView::~CDclFormView()
{
	CFrameWnd* pFrame = theStudioWorkspace.GetStudioFrame();
	if( pFrame->GetActiveView() == this )
		pFrame->SetActiveView( theStudioWorkspace.GetProjectPane() );
	if( mpDialogObject )
		delete mpDialogObject;
}

TProjectPtr CDclFormView::GetProject() const
{
	TDclFormPtr pForm = GetSourceForm();
	if( !pForm )
		return NULL;
	return pForm->GetProject();
}

void CDclFormView::SetSourceForm( TDclFormPtr pSourceForm )
{
	mpDialogObject = new CStudioDialogObject( pSourceForm, this );
}

TDclFormPtr CDclFormView::GetSourceForm() const
{
	if( !mpDialogObject )
		return NULL;
	return mpDialogObject->GetSourceForm();
}

BOOL CDclFormView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	//cs.style &= ~(FWS_ADDTOTITLE | FWS_PREFIXTITLE);
	if( !__super::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

// CDclFormView drawing

void CDclFormView::OnDraw(CDC* /*pDC*/)
{
	COpenDCLDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
}

void CDclFormView::OnInitialUpdate()
{
	__super::OnInitialUpdate();
	ModifyStyleEx( WS_EX_CLIENTEDGE, 0, 0 );
}

void CDclFormView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	if( bActivate && mpDialogObject )
	{
		CString sViewTitle;
		mpDialogObject->GetWindowText( sViewTitle );
		GetParentFrame()->SetWindowText( sViewTitle );
	}
	__super::OnActivateView( bActivate, pActivateView, pDeactiveView );
}

// CDclFormView diagnostics

#ifdef _DEBUG
void CDclFormView::AssertValid() const
{
	__super::AssertValid();
}

void CDclFormView::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

COpenDCLDoc* CDclFormView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenDCLDoc)));
	return (COpenDCLDoc*)m_pDocument;
}
#endif //_DEBUG


// CDclFormView message handlers


void CDclFormView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CRect rcDlgObject = mpDialogObject->GetEffectiveWindowRect();
	CSize sizeView( rcDlgObject.Width(), rcDlgObject.Height() );
	sizeView.cx += 400;
	sizeView.cx += 400;
	SetScrollSizes( MM_TEXT, sizeView );
}

BOOL CDclFormView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// include the dialog object in the command routing chain if this view is the active view
	if( theStudioWorkspace.GetStudioFrame()->GetActiveView() == this )
	{
		if( nID != IDOK && nID != IDCANCEL )
		{
			if( mpDialogObject->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
				return TRUE;
		}
	}

	// route commands through the project pane as well
	if( theStudioWorkspace.GetProjectPane()->OnCmdMsg( nID, nCode, pExtra, pHandlerInfo ) )
		return TRUE;

	return CScrollView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CDclFormView::OnWindowNextForm()
{
	CDocument* pDoc = GetDocument();
	if( !pDoc )
		return;

	CStudioFrame* pStudioFrame = theStudioWorkspace.GetStudioFrame();
	CProjectPane* pProjectPane = theStudioWorkspace.GetProjectPane();
	CView* pNextView = NULL;
	CView* pFirstView = NULL;
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos )
	{
		CView* pView = pDoc->GetNextView( pos );
		if( pView == pProjectPane )
			continue;
		if( pView == this )
		{
			pNextView = pDoc->GetNextView( pos );
			if( !pNextView )
				pNextView = pFirstView;
			break;
		}
		if( !pFirstView )
			pFirstView = pView;
	}
	if( pNextView )
	{
		pNextView->GetParentFrame()->ActivateFrame();
		pStudioFrame->SetActiveView( pNextView );
	}
}

void CDclFormView::OnUpdateWindowNextForm(CCmdUI *pCmdUI)
{
	size_t ctViews = 0;
	CProjectPane* pProjectPane = theStudioWorkspace.GetProjectPane();
	CDocument* pDoc = GetDocument();
	if( pDoc )
	{
		POSITION pos = pDoc->GetFirstViewPosition();
		while( pos )
		{
			CView* pView = pDoc->GetNextView( pos );
			if( pView != pProjectPane )
				++ctViews;
		}
	}
	pCmdUI->Enable( (ctViews > 1) );
}

void CDclFormView::OnWindowPrevForm()
{
	CDocument* pDoc = GetDocument();
	if( !pDoc )
		return;

	CStudioFrame* pStudioFrame = theStudioWorkspace.GetStudioFrame();
	CProjectPane* pProjectPane = theStudioWorkspace.GetProjectPane();
	CView* pPrevView = NULL;
	POSITION pos = pDoc->GetFirstViewPosition();
	while( pos )
	{
		CView* pView = pDoc->GetNextView( pos );
		if( pView == pProjectPane )
			continue;
		if( pView == this )
		{
			if( !pPrevView )
			{
				while( pos )
					pPrevView = pDoc->GetNextView( pos ); //get last in list
			}
			break;
		}
		pPrevView = pView;
	}
	if( pPrevView )
	{
		pPrevView->GetParentFrame()->ActivateFrame();
		pStudioFrame->SetActiveView( pPrevView );
	}
}

void CDclFormView::OnUpdateWindowPrevForm(CCmdUI *pCmdUI)
{
	size_t ctViews = 0;
	CProjectPane* pProjectPane = theStudioWorkspace.GetProjectPane();
	CDocument* pDoc = GetDocument();
	if( pDoc )
	{
		POSITION pos = pDoc->GetFirstViewPosition();
		while( pos )
		{
			CView* pView = pDoc->GetNextView( pos );
			if( pView != pProjectPane )
				++ctViews;
		}
	}
	pCmdUI->Enable( (ctViews > 1) );
}
