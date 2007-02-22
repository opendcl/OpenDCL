// DialogObject.cpp : implementation file
//

#include "stdafx.h"
#include "DialogObject.h"
#include "Project.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CDialogObject

//static
ULONG CDialogObject::mnNextFormId = 1;


CDialogObject::CDialogObject( CDclFormObject* pDclForm, CWnd* pHostDlg )
: mnID( GetNextDialogId() )
, mpSourceForm( pDclForm )
, mpProject( pDclForm->GetProject() )
, mpHostDlg( pHostDlg )
{
	if( pDclForm )
		pDclForm->SetFormInstance( this );
}


CDialogObject::~CDialogObject()
{
	if( mpSourceForm )
		mpSourceForm->SetFormInstance( NULL );
}

bool CDialogObject::Show(bool bShow /*= true*/)
{
	::ShowWindow( GetHWnd(), bShow? SW_SHOW : SW_HIDE );
	return true;
}

bool CDialogObject::CenterDialog()
{
	if( IsDockable() )
		return false;
	CRect rcThis;
	GetWindowRect(rcThis);
	int nCtlWidth = rcThis.Width();
	int nCtlHeight = rcThis.Height();
	CPoint pt;
	// get the left and top values to center the form on the screen	
	pt.y =  (::GetSystemMetrics(SM_CYSCREEN) - rcThis.Height()) / 2;
	pt.x =  (::GetSystemMetrics(SM_CXSCREEN) - rcThis.Width()) / 2;
	BOOL bSuccess = ::SetWindowPos(GetHWnd(), NULL, pt.x, pt.y, 0, 0,
																 SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	return (bSuccess != FALSE);
}

bool CDialogObject::ResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsResizable() )
		return false;
	mpSourceForm->GetControlProperties()->SetLongProperty(nWidth, nNewWidth);
	mpSourceForm->GetControlProperties()->SetLongProperty(nHeight, nNewHeight);
	BOOL bSuccess = ::SetWindowPos(GetHWnd(), NULL, 0, 0, nNewWidth, nNewHeight,
																 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER );
	GetControlPane().RecalcLayout();
	return (bSuccess != FALSE);
}

bool CDialogObject::CenterAndResizeDialog( long nNewWidth, long nNewHeight )
{
	if( !IsResizable() )
		return false;
	mpSourceForm->GetControlProperties()->SetLongProperty(nWidth, nNewWidth);
	mpSourceForm->GetControlProperties()->SetLongProperty(nHeight, nNewHeight);
	CPoint pt;
	// get the left and top values to center the form on the screen	
	pt.y =  (::GetSystemMetrics(SM_CYSCREEN) - nNewHeight) / 2;
	pt.x =  (::GetSystemMetrics(SM_CXSCREEN) - nNewWidth) / 2;
	BOOL bSuccess = ::SetWindowPos(GetHWnd(), NULL, pt.x, pt.y, nNewWidth, nNewHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	GetControlPane().RecalcLayout();
	return (bSuccess != FALSE);
}

bool CDialogObject::SetFocus()
{
	::SetFocus( GetHWnd() );
	return true;
}

bool CDialogObject::GetWindowRect( CRect& rcDlg ) const
{
	return (::GetWindowRect( GetHWnd(), &rcDlg ) != FALSE);
}

bool CDialogObject::GetClientRect( CRect& rcDlg ) const
{
	return (::GetClientRect( GetHWnd(), &rcDlg ) != FALSE);
}

bool CDialogObject::SetMinMaxSize( const CSize& min, const CSize& max )
{
	CDclControlObject *pPropObj = mpSourceForm->GetControlProperties();
	assert(pPropObj != NULL);
	if (pPropObj)
	{
		pPropObj->SetLongProperty(nMinDialogWidth, min.cx);
		pPropObj->SetLongProperty(nMinDialogHeight, min.cy);
		pPropObj->SetLongProperty(nMaxDialogWidth, max.cx);
		pPropObj->SetLongProperty(nMaxDialogHeight, max.cy);
	}
	return true;
}
