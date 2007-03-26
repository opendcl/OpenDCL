// ParentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ParentDlg.h"
#include "InvokeMethod.h"
#include "DclControlObject.h"
#include "PropertyIds.h"


const UINT WM_FILEDLG_GETFILENAME = RegisterWindowMessage( _T("odcl.FileDialog.GetFileName") );
const UINT WM_FILEDLG_GETFILETITLE = RegisterWindowMessage( _T("odcl.FileDialog.GetFileTitle") );
const UINT WM_FILEDLG_GETFILEEXT = RegisterWindowMessage( _T("odcl.FileDialog.GetFileExt") );
const UINT WM_FILEDLG_GETFILEPATH = RegisterWindowMessage( _T("odcl.FileDialog.GetFilePath") );
const UINT WM_FILEDLG_GETFOLDERPATH = RegisterWindowMessage( _T("odcl.FileDialog.GetFolderPath") );
const UINT WM_FILEDLG_GETFOLDERNAME = RegisterWindowMessage( _T("odcl.FileDialog.GetFolderName") );
const UINT WM_FILEDLG_GETSELECTEDFILECOUNT = RegisterWindowMessage( _T("odcl.FileDialog.GetSelectedFileCount") );
const UINT WM_FILEDLG_GETSELECTEDFILES = RegisterWindowMessage( _T("odcl.FileDialog.GetSelectedFiles") );


// CFileDialogX interface implementation
CFileDialogX::CFileDialogX( CParentDlg& Owner, CDclFormObject* pDclForm )
: CArxDialogObject( pDclForm, &Owner )
, mpOwner( &Owner )
{
}

CFileDialogX::~CFileDialogX()
{
}

DclFormType CFileDialogX::GetType() const
{
	return VdclFileDialog;
}

HWND CFileDialogX::GetHWnd() const
{
	return mpOwner->m_hWnd;
}

void CFileDialogX::CloseDialog(int nStatus) const
{
	mpOwner->EndDialog( nStatus );
	mpOwner->SendMessage(WM_CLOSE, 0, 0);
	mpOwner->SendMessage(NM_CLICK, (WPARAM)nStatus, 0);
}

INT_PTR CFileDialogX::DoModal()
{
	return mpOwner->DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CParentDlg dialog

CParentDlg::CParentDlg( CDclFormObject* pSourceForm, CWnd* pParent /*=NULL*/, DialogParams* pParams /*= NULL*/ )
: CCommonDialog(pParent)
, mDialogX( *this, pSourceForm )
{
	m_szGripSize.cx = GetSystemMetrics(SM_CXVSCROLL);
	m_szGripSize.cy = GetSystemMetrics(SM_CYHSCROLL);
}

CParentDlg::~CParentDlg()
{
}

void CParentDlg::DoDataExchange(CDataExchange* pDX)
{
	CCommonDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParentDlg, CCommonDialog)
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_MESSAGE(WM_FILEDLG_GETFILENAME, OnGetFileName)
	ON_MESSAGE(WM_FILEDLG_GETFILETITLE, OnGetFileTitle)
	ON_MESSAGE(WM_FILEDLG_GETFILEEXT, OnGetFileExt)
	ON_MESSAGE(WM_FILEDLG_GETFILEPATH, OnGetFilePath)
	ON_MESSAGE(WM_FILEDLG_GETFOLDERPATH, OnGetFolderPath)
	ON_MESSAGE(WM_FILEDLG_GETFOLDERNAME, OnGetFolderName)
	ON_MESSAGE(WM_FILEDLG_GETSELECTEDFILECOUNT, OnGetSelectedFileCount)
	ON_MESSAGE(WM_FILEDLG_GETSELECTEDFILES, OnGetSelectedFiles)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParentDlg message handlers

void CParentDlg::OnCancel() 
{
	CCommonDialog::OnCancel();
}

void CParentDlg::OnOK() 
{	
	CCommonDialog::OnOK();	
}


void CParentDlg::OnDestroy() 
{
	InvokeMethodIntInt(mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventClose), -1, -1, false);	
	CCommonDialog::OnDestroy();
}

void CParentDlg::OnSize(UINT nType, int cx, int cy) 
{
	CCommonDialog::OnSize(nType, cx, cy);
	
	if (CWnd::IsWindowVisible())	
	{
		CRect rcThis;
		if( mDialogX.GetSourceForm()->UsesClientRect() )
			GetClientRect( &rcThis );
		else
			GetWindowRect( &rcThis );
		// call methods to invoke the event
		InvokeMethodIntInt(
			mDialogX.GetSourceForm()->GetControlProperties()->GetStrProperty(nFormEventSize), 
			rcThis.Width(),
			rcThis.Height(),
			false);	
	}

	UpdateGripPos();

	mDialogX.GetControlPane().RecalcLayout();
}

void CParentDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_bShowGrip && !IsZoomed())
	{
		// draw size-grip
		dc.DrawFrameControl(&m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}
	
}


UINT CParentDlg::OnNcHitTest(CPoint point) 
{
	CPoint pt = point;
	ScreenToClient(&pt);

	// if in size grip and in client area
	if (m_bShowGrip && m_rcGripRect.PtInRect(pt) &&
		pt.x >= 0 && pt.y >= 0)
		return HTBOTTOMRIGHT;
	
	return CDialog::OnNcHitTest(point);
}


void CParentDlg::UpdateGripPos()
{
	// size-grip goes bottom right in the client area
	
	if (!m_bShowGrip)
		return;
	
	//InvalidateRect(&m_rcGripRect);
	HDC hdc = ::GetDC(m_hWnd);
	
	// draw the solid rectangle
	::SetBkColor(hdc, ::GetSysColor(COLOR_BTNFACE));
	::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &m_rcGripRect, NULL, 0, NULL);
	
	
	GetClientRect(&m_rcGripRect);

	m_rcGripRect.left = m_rcGripRect.right - m_szGripSize.cx;
	m_rcGripRect.top = m_rcGripRect.bottom - m_szGripSize.cy;

	// draw size-grip
	DrawFrameControl(hdc, &m_rcGripRect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

	// then releasing the DC itself
	::ReleaseDC(m_hWnd, hdc);
	
}

// protected members

void CParentDlg::ShowSizeGrip(BOOL bShow)
{
	if (m_bShowGrip != bShow)
	{
		m_bShowGrip = bShow;
		InvalidateRect(&m_rcGripRect);
	}
}

LRESULT CParentDlg::OnGetFileName( WPARAM wParam, LPARAM lParam )
{
	if( !m_pMainChild )
		return 0;
	return m_pMainChild->SendMessage( WM_FILEDLG_GETFILENAME, wParam, lParam );
}

LRESULT CParentDlg::OnGetFileTitle( WPARAM wParam, LPARAM lParam )
{
	if( !m_pMainChild )
		return 0;
	return m_pMainChild->SendMessage( WM_FILEDLG_GETFILETITLE, wParam, lParam );
}

LRESULT CParentDlg::OnGetFileExt( WPARAM wParam, LPARAM lParam )
{
	if( !m_pMainChild )
		return 0;
	return m_pMainChild->SendMessage( WM_FILEDLG_GETFILEEXT, wParam, lParam );
}

LRESULT CParentDlg::OnGetFilePath( WPARAM wParam, LPARAM lParam )
{
	if( !m_pMainChild )
		return 0;
	return m_pMainChild->SendMessage( WM_FILEDLG_GETFILEPATH, wParam, lParam );
}

LRESULT CParentDlg::OnGetFolderPath( WPARAM wParam, LPARAM lParam )
{
	if( !m_pMainChild )
		return 0;
	return m_pMainChild->SendMessage( WM_FILEDLG_GETFOLDERPATH, wParam, lParam );
}

LRESULT CParentDlg::OnGetFolderName( WPARAM wParam, LPARAM lParam )
{
	if( !m_pMainChild )
		return 0;
	return m_pMainChild->SendMessage( WM_FILEDLG_GETFOLDERNAME, wParam, lParam );
}

LRESULT CParentDlg::OnGetSelectedFileCount( WPARAM wParam, LPARAM lParam )
{
	if( !m_pMainChild )
		return 0;
	return m_pMainChild->SendMessage( WM_FILEDLG_GETSELECTEDFILECOUNT, wParam, lParam );
}

LRESULT CParentDlg::OnGetSelectedFiles( WPARAM wParam, LPARAM lParam )
{
	if( !m_pMainChild )
		return 0;
	return m_pMainChild->SendMessage( WM_FILEDLG_GETSELECTEDFILES, wParam, lParam );
}
