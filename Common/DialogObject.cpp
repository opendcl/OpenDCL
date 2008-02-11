// DialogObject.cpp : implementation file
//

#include "stdafx.h"
#include "DialogObject.h"
#include "Workspace.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PictureObject.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CDialogObject

//static
ULONG CDialogObject::mnNextFormId = 1;


CDialogObject::CDialogObject( TDclFormPtr pSourceForm, CControlPane* pPane, CWnd* pHostDlg )
: CDialogControl( pSourceForm? pSourceForm->GetControlProperties() : NULL, pPane, pHostDlg  )
, mnID( GetNextDialogId() )
, mpSourceForm( pSourceForm )
, mpProject( pSourceForm? pSourceForm->GetProject() : NULL )
, mnNCWidth( 0 )
, mnNCHeight( 0 )
, mnMinWidth( 0 )
, mnMinHeight( 0 )
, mnMaxWidth( 0 )
, mnMaxHeight( 0 )
, mbClosing( false )
{
	if( pSourceForm )
		pSourceForm->SetFormInstance( this );
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
	CRect rcDlg = GetEffectiveWindowRect();
	CPoint pt( (::GetSystemMetrics(SM_CXSCREEN) - rcDlg.Width()) / 2,
						 (::GetSystemMetrics(SM_CYSCREEN) - rcDlg.Height()) / 2 );
	bool bSuccess =
		(GetTopLevelWnd()->SetWindowPos( NULL, pt.x, pt.y, 0, 0,
																		 SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER ) != FALSE);
	return true;
}

bool CDialogObject::ResizeDialog( long nNewWidth, long nNewHeight )
{
	mpTemplate->SetLongProperty( Prop::Width, nNewWidth );
	mpTemplate->SetLongProperty( Prop::Height, nNewHeight );
	ApplyPosition();
	return true;
}

bool CDialogObject::CenterAndResizeDialog( long nNewWidth, long nNewHeight )
{
	mpTemplate->SetLongProperty( Prop::Width, nNewWidth );
	mpTemplate->SetLongProperty( Prop::Height, nNewHeight );
	nNewWidth += GetNCWidth();
	nNewHeight += GetNCHeight();
	mbIgnoreSizing = true;
	CPoint pt( (::GetSystemMetrics(SM_CXSCREEN) - nNewHeight) / 2,
						 (::GetSystemMetrics(SM_CYSCREEN) - nNewWidth) / 2 );
	bool bSuccess =
		(GetTopLevelWnd()->SetWindowPos( NULL, pt.x, pt.y, nNewWidth, nNewHeight,
																		 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER ) != FALSE);
	GetControlPane()->RecalcLayout();
	mbIgnoreSizing = false;
	return bSuccess;
}

CRect CDialogObject::GetEffectiveWindowRect() const
{
	CRect rcWnd( 0, 0, 0, 0 );
	if( mpControlWnd )
	{
		mpControlWnd->GetWindowRect( &rcWnd );
		if( (mpControlWnd->GetControlUnknown() || (mpControlWnd->GetStyle() & WS_CHILD)) )
			mpControlWnd->GetParent()->ScreenToClient( &rcWnd );
	}
	return rcWnd;
}

CRect CDialogObject::GetEffectiveClientRect() const
{
	CRect rcClient( 0, 0, 0, 0 );
	if( mpControlWnd )
		mpControlWnd->GetClientRect( &rcClient );
	return rcClient;
}

void CDialogObject::GetMinMaxSize( CSize& szMin, CSize& szMax )
{
	szMin.SetSize( mnMinWidth, mnMinHeight );
	szMax.SetSize( mnMaxWidth, mnMaxHeight );
}

CRect CDialogObject::GetWndRect() const
{
	return CRect( 0, 0,
								mpTemplate->GetLongProperty(Prop::Width),
								mpTemplate->GetLongProperty(Prop::Height) );
}

DWORD CDialogObject::GetWndStyle() const
{
	DWORD dwStyle = WS_CHILD | WS_VISIBLE/* | WS_CLIPSIBLINGS*/;

	assert(mpTemplate != NULL);
	if( mpTemplate->GetBooleanProperty(Prop::TitleBar) )
		dwStyle |= WS_CAPTION;
	if( mpTemplate->GetBooleanProperty(Prop::Resizable) )
		dwStyle |= WS_THICKFRAME;

	return dwStyle;
}

CString CDialogObject::GetWndCaption() const
{
	assert(mpTemplate != NULL);
	if( mpSourceForm->GetParentForm() )
	{
		CString sParentName = mpSourceForm->GetParentName();
		const TDclFormList& Forms = mpSourceForm->GetProject()->GetDclFormList();
		for( TDclFormList::const_iterator iter = Forms.begin(); iter != Forms.end(); ++iter )
		{
			if( sParentName == (*iter)->GetUniqueName() )
			{
				TDclControlPtr pDclControl = (*iter)->FindFirstControlOfType( CtlTabStrip );
				if( !pDclControl )
					break;
				CString sTabCaption = pDclControl->GetPropertyListItem( Prop::TabsCaption, mpSourceForm->GetTabIndex() );
				CString sParent = (*iter)->GetControlProperties()->GetStringProperty(Prop::TitleBarText);
				CString sCaption;
				//sCaption.Format( _T("%s / %s"), (LPCTSTR)sParent, (LPCTSTR)sTabCaption );
				sCaption.Format( _T("%s (%s)"), (LPCTSTR)sTabCaption, (LPCTSTR)sParent );
				return sCaption;
			}
		}
	}
	return mpTemplate->GetStringProperty(Prop::TitleBarText);
}

void CDialogObject::OnFrameChanged()
{
	CRect rectWindow;
	mpControlWnd->GetWindowRect( &rectWindow );
	CRect rectClient;
	mpControlWnd->GetClientRect( &rectClient );
	SetNCWidth( rectWindow.Width() - rectClient.Width() );
	SetNCHeight( rectWindow.Height() - rectClient.Height() );
	OnApplyMinMaxSize( NULL );
	ApplyPosition();
	OnNeedRepaint();
}

void CDialogObject::ApplyPosition()
{
	mbIgnoreSizing = true;
	GetTopLevelWnd()->SetWindowPos( NULL, 0, 0,
																	mpTemplate->GetLongProperty(Prop::Width) + GetNCWidth(),
																	mpTemplate->GetLongProperty(Prop::Height) + GetNCHeight(),
																	SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOOWNERZORDER );
	mpControlPane->RecalcLayout();
	mbIgnoreSizing = false;
}

bool CDialogObject::OnApplyProperty( TPropertyPtr pProp )
{
	if( !pProp )
		return false;
	bool bSuccess = true;
	switch( pProp->GetID() )
	{
	case Prop::Resizable: if( !OnApplyResizable( pProp ) ) bSuccess = false; break;
	case Prop::Width: if( !OnApplyWidth( pProp ) ) bSuccess = false; break;
	case Prop::Height: if( !IsEnumeratingProperties() && !OnApplyHeight( pProp ) ) bSuccess = false; break;
	case Prop::MinDialogWidth: if( !OnApplyMinMaxSize( pProp ) ) bSuccess = false; break;
	case Prop::MinDialogHeight: if( !IsEnumeratingProperties() && !OnApplyMinMaxSize( pProp ) ) bSuccess = false; break;
	case Prop::MaxDialogWidth: if( !IsEnumeratingProperties() && !OnApplyMinMaxSize( pProp ) ) bSuccess = false; break;
	case Prop::MaxDialogHeight: if( !IsEnumeratingProperties() && !OnApplyMinMaxSize( pProp ) ) bSuccess = false; break;
	case Prop::TitleBarText: if( !OnApplyCaption( pProp ) ) bSuccess = false; break;
	case Prop::CfgTabCaption: if( !OnApplyCaption( pProp ) ) bSuccess = false; break;
	case Prop::Icon: if( !OnApplyIcon( pProp ) ) bSuccess = false; break;
	case Prop::TitleBar: if( !OnApplyTitleBar( pProp ) ) bSuccess = false; break;
	default: bSuccess = __super::OnApplyProperty( pProp ); break;
	}
	return bSuccess;
}

bool CDialogObject::OnApplyResizable( TPropertyPtr pProp )
{
	mbIgnoreSizing = true;
	if( pProp->GetBooleanValue() )
		mpControlWnd->ModifyStyle( 0, WS_THICKFRAME, SWP_FRAMECHANGED );
	else
		mpControlWnd->ModifyStyle( WS_THICKFRAME, 0, SWP_FRAMECHANGED );
	mbIgnoreSizing = false;
	OnFrameChanged();
	return true;
}

bool CDialogObject::OnApplyWidth( TPropertyPtr pProp )
{
	ApplyPosition();
	return true;
}

bool CDialogObject::OnApplyHeight( TPropertyPtr pProp )
{
	ApplyPosition();
	return true;
}

bool CDialogObject::OnApplyMinMaxSize( TPropertyPtr pProp )
{
	assert(mpTemplate != NULL);
	CSize szMin( mpTemplate->GetLongProperty(Prop::MinDialogWidth),
							 mpTemplate->GetLongProperty(Prop::MinDialogHeight) );
	CSize szMax( mpTemplate->GetLongProperty(Prop::MaxDialogWidth),
							 mpTemplate->GetLongProperty(Prop::MaxDialogHeight) );
	mnMinWidth = szMin.cx;
	mnMinHeight = szMin.cy;
	mnMaxWidth = szMax.cx;
	mnMaxHeight = szMax.cy;
	if( mpSourceForm->UsesClientRect() )
	{
		if( mnMinWidth > 0 )
			mnMinWidth += mnNCWidth;
		if( mnMinHeight > 0 )
			mnMinHeight += mnNCHeight;
		if( mnMaxWidth > 0 )
			mnMaxWidth += mnNCWidth;
		if( mnMaxHeight > 0 )
			mnMaxHeight += mnNCHeight;
	}
	return true;
}

bool CDialogObject::OnApplyIcon( TPropertyPtr pProp )
{
	DestroyIcon( mpControlWnd->SetIcon( NULL, FALSE ) );
	CPictureObject* pPicture = mpSourceForm->GetProject()->FindPicture( pProp->GetLongValue() );
	if( pPicture )
		mpControlWnd->SetIcon( pPicture->CloneIcon(), FALSE );
	else
		mpControlWnd->SetIcon( CopyIcon( AfxGetApp()->GetMainWnd()->GetIcon( FALSE ) ), FALSE );
	OnFrameChanged();
	return true;
}

bool CDialogObject::OnApplyTitleBar( TPropertyPtr pProp )
{
	mbIgnoreSizing = true;
	if( pProp->GetBooleanValue() )
		mpControlWnd->ModifyStyle( 0, WS_CAPTION, SWP_FRAMECHANGED );
	else
		mpControlWnd->ModifyStyle( WS_CAPTION, 0, SWP_FRAMECHANGED );
	mbIgnoreSizing = false;
	OnFrameChanged();
	return true;
}
