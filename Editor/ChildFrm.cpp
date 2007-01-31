// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "ChildFrm.h"
#include "Resource.h"
#include "Project.h"
#include "EditorWorkspace.h"
#include "PictureObject.h"
#include "ObjectDCLView.h"
#include "PropertyTabPane.h"

#define nDlgStartupOffsetX  -165
#define nDlgStartupOffsetY  -115
#define nDlgHidingOffset  -2000
			

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_CREATE()
	ON_WM_SHOWWINDOW()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_WM_CLOSE()
	ON_WM_NCHITTEST()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	m_StartupSize.cx = -1;
	m_StartupSize.cy = -1;
	m_pChildView = NULL;
	m_Title = CString();
	m_bResizable = true;
	m_ArrowCursor = NULL;
	m_CrossCursor = NULL;
	
	m_ResizeCursor = NULL;
	
	HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ACAD2002), RT_GROUP_ICON);

	m_hIcon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_ACAD2002));

}

CChildFrame::~CChildFrame()
{
	if (m_hIcon != NULL)
		DeleteObject(m_hIcon);

	if (m_ArrowCursor != NULL)
	{
		DeleteObject (m_ArrowCursor);
		m_ArrowCursor = NULL;
	}
	
	if (m_CrossCursor != NULL)
	{
		DeleteObject (m_CrossCursor);
		m_CrossCursor = NULL;
	}
	
	if (m_ResizeCursor != NULL)
	{
		DeleteObject (m_ResizeCursor);
		m_ResizeCursor = NULL;
	}
		
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{

	if( !CMDIChildWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.style = WS_CHILD | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE | WS_THICKFRAME;
	//cs.dwExStyle = NULL;
	return TRUE;
}

void CChildFrame::SetTitleBarIcon(int nPictureID)
{
	// set start position for navigating Pictures
	POSITION pos = activeProject->GetPictureList().GetHeadPosition();

	int nCount = 0;
	bool bFoundIcon = false;

	HICON hIcon = GetIcon(FALSE);
	SetIcon(NULL, FALSE);
	if (m_hIcon != NULL)
	{
		DestroyIcon(m_hIcon);
		m_hIcon = NULL;
	}
	
	if (nPictureID == 0)
	{
		HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ACAD2002), RT_GROUP_ICON);
		m_hIcon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_ACAD2002));
		SetIcon(m_hIcon, FALSE);
		return;
	}
	
	if (nPictureID == -1)
	{
		HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_OPENFOLDER), RT_GROUP_ICON);
		m_hIcon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_OPENFOLDER));
		SetIcon(m_hIcon, FALSE);
		return;
	}
	
	// do loop to navigate Pictures
	while (nCount < activeProject->GetPictureList().GetCount())
	{
		// get position
		pos = activeProject->GetPictureList().FindIndex(nCount);
		// get current Picture in list
		CPictureObject* pPicture = activeProject->GetPictureList().GetNext(pos);
		
		if (pPicture->GetID() == nPictureID)
		{
			// get the icon
			m_hIcon = pPicture->GetIcon();
			// set the icon
			SetIcon(m_hIcon, FALSE);
			// set the found flag to true
			bFoundIcon = true;
		}
		// increment counter
		nCount++;
	}
	nCount = 0;

	// if not icon was found to be specified	
	if (!bFoundIcon)
	{
		HINSTANCE hInstResource = AfxFindResourceHandle(MAKEINTRESOURCE(IDI_ACAD2002), RT_GROUP_ICON);
		m_hIcon = LoadIcon(hInstResource, MAKEINTRESOURCE(IDI_ACAD2002));
		SetIcon(m_hIcon, FALSE);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

int CChildFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyleEx(WS_EX_CLIENTEDGE, NULL, 0);
	ShowWindow(FALSE);
	SetIcon(m_hIcon, FALSE);


	return 0;
}

void CChildFrame::ResetFrame() 
{
	ModifyStyle(NULL,WS_CAPTION, NULL, 0);
	ModifyStyle(NULL,WS_SYSMENU, NULL, 0);
	ModifyStyle(NULL,WS_THICKFRAME, NULL, 0);
	ModifyStyle(WS_CHILD, NULL, 0);	
}
void CChildFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{	
	if (m_pChildView != NULL)
	{
		CObjectDCLView *pChild = (CObjectDCLView*)m_pChildView;
		if (pChild->m_pThisDclForm == NULL)
		{
			CMDIChildWnd::OnShowWindow(bShow, nStatus);
			
			GetWindowRect(&m_rcStartup);
			/*
			AfxGetApp()->GetMainWnd()->GetWindowRect(rcMain);
			m_rcStartup.OffsetRect(rcMain.left * -1,rcMain.top * -1);
			m_rcStartup.OffsetRect(nDlgStartupOffsetX,nDlgStartupOffsetY);
			*/
			
			if (m_StartupSize.cx == -1 && m_StartupSize.cy == -1)
			{
				CRect rcMain;

				GetParent()->GetClientRect(&rcMain);
				CRect rcThis;
				rcThis.left = (rcMain.Width() -100)/2;
				rcThis.top = (rcMain.Height() -50)/2;
				
				ModifyStyle(WS_CAPTION, NULL, 0);
				ModifyStyle(DS_MODALFRAME, NULL, 0);
				ModifyStyle(WS_SYSMENU, NULL, 0);
				ModifyStyle(WS_THICKFRAME, NULL, 0);
				ModifyStyle(NULL, WS_CHILD, 0);
				
				MoveWindow(rcThis.left,rcThis.top, 100,50,TRUE);
			}
			//ShowWindow(FALSE);
			CMDIChildWnd::OnShowWindow(bShow, nStatus);
			return;
		}
	}	
	CMDIChildWnd::OnShowWindow(bShow, nStatus);
}



void CChildFrame::OnUpdateFrameTitle(BOOL bAddToTitle)
{
    bAddToTitle = FALSE;
	m_strTitle = CString();
	//SetWindowText(m_Title);
}



BOOL CChildFrame::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CWinApp* pApp = AfxGetApp();
	
	if (nHitTest == 10 || nHitTest == 11)
	{
		if (m_ResizeCursor != NULL)
		{
			DeleteObject (m_ResizeCursor);
		}
		m_ResizeCursor = pApp->LoadStandardCursor(IDC_SIZEWE);
		::SetCursor(m_ResizeCursor);
		
	}
	else if (nHitTest == 12 || nHitTest == 15)
	{
		if (m_ResizeCursor != NULL)
		{
			DeleteObject (m_ResizeCursor);
		}
		m_ResizeCursor = pApp->LoadStandardCursor(IDC_SIZENS);
		::SetCursor(m_ResizeCursor);
		
	}
	else if (nHitTest == 13 || nHitTest == 17)
	{
		if (m_ResizeCursor != NULL)
		{
			DeleteObject (m_ResizeCursor);
		}
		m_ResizeCursor = pApp->LoadStandardCursor(IDC_SIZENWSE);
		::SetCursor(m_ResizeCursor);		
		
	}
	else if (nHitTest == 14 || nHitTest == 16)
	{
		if (m_ResizeCursor != NULL)
		{
			DeleteObject (m_ResizeCursor);
		}
		m_ResizeCursor = pApp->LoadStandardCursor(IDC_SIZENESW);
		::SetCursor(m_ResizeCursor);		
		
	}
	else if (m_pChildView->m_StandardCursorID)
	{
		if (m_CrossCursor == NULL)
			m_CrossCursor = pApp->LoadStandardCursor(IDC_CROSS);
		::SetCursor(m_CrossCursor);		
	}
	else
	{
		if (m_ArrowCursor == NULL)
			m_ArrowCursor = pApp->LoadStandardCursor(IDC_ARROW);
		::SetCursor(m_ArrowCursor);
	}
	
	return TRUE;	
}

void CChildFrame::OnEditCopy() 
{
	m_pChildView->OnEditCopy();
	
}

void CChildFrame::OnEditCut() 
{
	m_pChildView->OnEditCut();
	
}

void CChildFrame::OnEditDelete() 
{
	m_pChildView->OnEditDelete();
	
}

void CChildFrame::OnEditPaste() 
{
	m_pChildView->OnEditPaste();
	
}

void CChildFrame::OnEditUndo() 
{
	m_pChildView->OnEditUndo();
	
}

void CChildFrame::OnClose() 
{
	theEditorWorkspace.GetPropertyTabs()->ClearControlProperties();
	// here we have to call DestroyWindow instead of OnCloseWindow because the OnCloseWindow Prompts for a save when we don't want one.
	DestroyWindow();
}
