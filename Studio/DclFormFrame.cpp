// DclFormFrame.cpp : implementation of the CDclFormFrame class
//
#include "stdafx.h"
#include "OpenDCL.h"
#include "DclFormFrame.h"
#include "DclFormView.h"
#include "StudioWorkspace.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDclFormFrame

IMPLEMENT_DYNCREATE(CDclFormFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CDclFormFrame, CMDIChildWnd)
	ON_WM_NCACTIVATE()
	ON_WM_DESTROY()
	ON_COMMAND(AFX_IDS_SCMINIMIZE, &CDclFormFrame::OnWindowMinimize)
	ON_UPDATE_COMMAND_UI(AFX_IDS_SCMINIMIZE, &CDclFormFrame::OnUpdateWindowMinimize)
	ON_COMMAND(AFX_IDS_SCMAXIMIZE, &CDclFormFrame::OnWindowMaximize)
	ON_UPDATE_COMMAND_UI(AFX_IDS_SCMAXIMIZE, &CDclFormFrame::OnUpdateWindowMaximize)
	ON_COMMAND(AFX_IDS_SCRESTORE, &CDclFormFrame::OnWindowRestore)
	ON_UPDATE_COMMAND_UI(AFX_IDS_SCRESTORE, &CDclFormFrame::OnUpdateWindowRestore)
END_MESSAGE_MAP()


// CDclFormFrame construction/destruction

CDclFormFrame::CDclFormFrame()
: mpDlgObject( NULL )
{
	// TODO: add member initialization code here
}

CDclFormFrame::~CDclFormFrame()
{
}

BOOL CDclFormFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs
	cs.style |= (WS_MAXIMIZE | WS_SYSMENU);
	cs.style &= ~(FWS_ADDTOTITLE | FWS_PREFIXTITLE);
	return __super::PreCreateWindow(cs);
}


// CDclFormFrame diagnostics

#ifdef _DEBUG
void CDclFormFrame::AssertValid() const
{
	__super::AssertValid();
}

void CDclFormFrame::Dump(CDumpContext& dc) const
{
	__super::Dump(dc);
}

#endif //_DEBUG


// CDclFormFrame message handlers


void CDclFormFrame::OnDestroy()
{
	if( mpDlgObject )
		theStudioWorkspace.DeactivateDlgObject( mpDlgObject );
	__super::OnDestroy();
}

BOOL CDclFormFrame::OnNcActivate(BOOL bActive)
{
	BOOL bResult = __super::OnNcActivate(bActive);
	if( bActive && mpDlgObject && mpDlgObject != theStudioWorkspace.GetActiveDlgObject() )
		theStudioWorkspace.ActivateDlgObject( mpDlgObject );
	return bResult;
}

void CDclFormFrame::OnWindowMinimize()
{
	ShowWindow( SW_SHOWMINNOACTIVE );
}

void CDclFormFrame::OnUpdateWindowMinimize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (theStudioWorkspace.GetActiveFormView() != NULL) && !IsIconic() );
}

void CDclFormFrame::OnWindowMaximize()
{
	MDIMaximize();
}

void CDclFormFrame::OnUpdateWindowMaximize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (theStudioWorkspace.GetActiveFormView() != NULL) && !IsZoomed() );
}

void CDclFormFrame::OnWindowRestore()
{
	MDIRestore();
}

void CDclFormFrame::OnUpdateWindowRestore(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( (theStudioWorkspace.GetActiveFormView() != NULL) && (IsIconic() || IsZoomed()) );
}
