// MainFileDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MainFileDlg.h"
#include "CustomFileDialog.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CMainFileDlg dialog

CMainFileDlg::CMainFileDlg( CCustomFileDialog* pDlgObject, CWnd* pParent /*=NULL*/ )
: CCommonDialog( pParent )
, mpDlgObject( pDlgObject )
, mbInitialized( false )
{
}

CMainFileDlg::~CMainFileDlg()
{
}

void CMainFileDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMainFileDlg, CDialog)
	ON_WM_HELPINFO()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_SIZING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMainFileDlg message handlers

BOOL CMainFileDlg::OnHelpInfo(HELPINFO* pHelpInfo)
{
	mpDlgObject->GetArxServices()->HandleEvent( Prop::EventHelp, false );
	return TRUE;
}

void CMainFileDlg::OnOK() 
{
	if( mpDlgObject->IsClosing() || mpDlgObject->IsCloseAllowed( false ) )
	{
		mpDlgObject->SetClosing();
		__super::OnOK();
	}
	else
		mpDlgObject->SetClosing( false );
}

void CMainFileDlg::OnCancel() 
{
	if( mpDlgObject->IsClosing() || mpDlgObject->IsCloseAllowed( true ) )
	{
		mpDlgObject->SetClosing();
		__super::OnCancel();
	}
	else
		mpDlgObject->SetClosing( false );
}

void CMainFileDlg::OnClose() 
{
	if( mpDlgObject->IsClosing() || mpDlgObject->IsCloseAllowed( true ) )
	{
		mpDlgObject->SetClosing();
		__super::OnClose();
	}
	else
		mpDlgObject->SetClosing( false );
}

void CMainFileDlg::OnDestroy() 
{
	mpDlgObject->SavePosition();
	CRect rcThis;
	GetWindowRect( &rcThis );
	mpDlgObject->GetArxServices()->HandleEvent( Prop::FormEventClose, false, args_NN( rcThis.left, rcThis.top ) );
	__super::OnDestroy();
}

void CMainFileDlg::OnSizing(UINT fwSide, LPRECT pRect) 
{
	int nNewWidth = pRect->right - pRect->left;
	int nNewHeight = pRect->bottom - pRect->top;
	CSize szMin( 0, 0 );
	CSize szMax( 0, 0 );
	mpDlgObject->GetMinMaxSize( szMin, szMax );

	if( fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_LEFT || fwSide == WMSZ_TOPLEFT )
	{
		if (szMin.cx > 0 && nNewWidth < szMin.cx)
			pRect->left = pRect->right - szMin.cx;
		if (szMax.cx > 0 && nNewWidth > szMax.cx)
			pRect->left = pRect->right - szMax.cx;
	}

	if( fwSide == WMSZ_BOTTOMRIGHT || fwSide == WMSZ_RIGHT || fwSide == WMSZ_TOPRIGHT )
	{
		if (szMin.cx > 0 && nNewWidth < szMin.cx)
			pRect->right = pRect->left + szMin.cx;
		if (szMax.cx > 0 && nNewWidth > szMax.cx)
			pRect->right = pRect->left + szMax.cx;
	}

	if( fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMRIGHT )
	{
		if (szMin.cy > 0 && nNewHeight < szMin.cy)
			pRect->bottom = pRect->top + szMin.cy;
		if (szMax.cy > 0 && nNewHeight > szMax.cy)
			pRect->bottom = pRect->top + szMax.cy;
	}

	if( fwSide == WMSZ_TOPLEFT || fwSide == WMSZ_TOP || fwSide == WMSZ_TOPRIGHT )
	{
		if (szMin.cy > 0 && nNewHeight < szMin.cy)
			pRect->top = pRect->bottom - szMin.cy;
		if (szMax.cy > 0 && nNewHeight > szMax.cy)
			pRect->top = pRect->bottom - szMax.cy;
	}

	__super::OnSizing(fwSide, pRect);
}

void CMainFileDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanged(lpwndpos);
	if( mbInitialized && ((lpwndpos->flags & (SWP_NOSIZE | SWP_NOMOVE)) != (SWP_NOSIZE | SWP_NOMOVE)) )
	{
		CRect rcMain;
		GetClientRect( &rcMain );
		rcMain.MoveToXY( 0, 0 );
		mpDlgObject->MoveWindow( &rcMain );
		CRect rcDlg = mpDlgObject->GetEffectiveClientRect();
		int nNewWidth = rcDlg.Width();
		int nNewHeight = rcDlg.Height();
		mpDlgObject->GetTemplate()->SetLongProperty( Prop::Width, nNewWidth );
		mpDlgObject->GetTemplate()->SetLongProperty( Prop::Height, nNewHeight );
		mpDlgObject->GetControlPane()->RecalcLayout();
		mpDlgObject->GetArxServices()->HandleEvent( Prop::FormEventSize, false, args_NN( nNewWidth, nNewHeight ) );
		mpDlgObject->SavePosition();
	}
}

void CMainFileDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if( bShow && !mbInitialized )
	{
		mbInitialized = true;
		mpDlgObject->OnInitializationComplete();
		Invalidate();
	}
	__super::OnShowWindow(bShow, nStatus);
}
