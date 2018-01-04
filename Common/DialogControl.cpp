// DialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "DialogControl.h"
#include "DclControlTemplate.h"
#include "AcadColorService.h"
#include "ControlPane.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "Workspace.h"
#include "ToolTips.h"
#include "UndoManager.h"
#include "DragDropService.h"
#include "DialogObject.h"
#include "ThemeAPI.h"
#include <algorithm>


/////////////////////////////////////////////////////////////////////////////
// CDialogControl

CDialogControl::CDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControlWnd )
: mpTemplate( pTemplate )
, mpControlWnd( pControlWnd )
, mpControlPane( pPane )
, mbEnumProps( false )
, mpControlManager( NULL )
{
	if( mpTemplate )
	{
		pTemplate->SetControlInstance( this );
		TPropertyPtr pToolTipBalloon = pTemplate->GetPropertyObject(Prop::ToolTipBalloon);
		mToolTip.SetDefaultSizes( !pToolTipBalloon || pToolTipBalloon->GetBooleanValue() );
	}
}

CDialogControl::~CDialogControl()
{
	if( mpTemplate )
		mpTemplate->SetControlInstance( NULL );
}

ControlType CDialogControl::GetControlType() const
{
	if( mpTemplate )
		return mpTemplate->GetType();
	return _CtlInvalid;
}

const WndTheme& CDialogControl::GetTheme() const
{
	OnThemeRequested( const_cast< CDialogControl* >(this)->mWndTheme );
	return mWndTheme;
}

void CDialogControl::OnThemeChanged()
{
	bool bHadTheme = (NULL != mWndTheme.GetWindowTheme());
	mWndTheme.Close();
	if( bHadTheme )
		OnNeedRepaint( true );
}

void CDialogControl::OnThemeRequested( WndTheme& Theme ) const
{
	Theme.Attach( NULL, GetHWnd() );
}

HBRUSH CDialogControl::HandleCtlColor( CDC* pDC, UINT nCtlColor )
{
	if( !mpControlWnd->IsWindowEnabled() )
		return NULL;
	CAcadColorService* pColorService = GetColorService();
	if( !pColorService )
		return NULL;
	pDC->SetTextColor( pColorService->GetForegroundColor() );
	if( pColorService->IsBackgroundNotSet() )
		return NULL;
	if( pColorService->IsBackgroundTransparent() )
	{
		CDialogObject* pHostDlg = mpControlPane->GetDialogObject();
		if( pHostDlg )
		{
			CAcadColorService* pDlgColor = pHostDlg->GetColorService();
			if( pDlgColor )
			{
				if( pDlgColor->IsBackgroundNotSet() )
				{
					if( !pDlgColor->IsBackgroundTransparent() )
					{
						pDC->SetBkColor( pDlgColor->GetBackgroundColor() );
						pDC->SetBkMode( OPAQUE );
					}
					else
						pDC->SetBkMode( TRANSPARENT );
					int nDCInfo = pDC->SaveDC();
					HBRUSH hbrBackground = (HBRUSH)pHostDlg->GetControlWnd()->SendMessage( WM_CTLCOLORDLG, (WPARAM)pDC, (LPARAM)pHostDlg->GetControlWnd()->m_hWnd );
					if (nDCInfo != 0 )
						pDC->RestoreDC(nDCInfo);
					if( hbrBackground )
						return hbrBackground;
				}
				else if( !pDlgColor->IsBackgroundTransparent() )
				{
					pDC->SetBkColor( pDlgColor->GetBackgroundColor() );
					pDC->SetBkMode( OPAQUE );
					return pDlgColor->GetBackgroundBrush();
				}
			}
		}
		pDC->SetBkMode( TRANSPARENT );
		return NULL;
	}
	pDC->SetBkColor( pColorService->GetBackgroundColor() );
	pDC->SetBkMode( OPAQUE );
	return pColorService->GetBackgroundBrush();
}

BOOL CDialogControl::HandleEraseBkgnd( CDC* pDC )
{
	//TraceFmt( _T("# CDialogControl(%s)::HandleEraseBkgnd(%s)\r\n"),
	//					asString( this ),
	//					asString( pDC ) );
	if( !pDC )
		return FALSE;
	CRect rcClip;
	pDC->GetClipBox( &rcClip );
	CRect rcClient;
	mpControlWnd->GetClientRect( &rcClient );
	if( !rcClip.IntersectRect( &rcClip, &rcClient ) )
		return TRUE;
	if( mpControlWnd->GetExStyle() & WS_EX_TRANSPARENT )
	{
		pDC->SetBkMode( TRANSPARENT );
		return TRUE;
	}
	if( !mpControlWnd->IsWindowEnabled() && !GetTheme().IsThemeActive() )
	{
		pDC->FillSolidRect( &rcClip, GetSysColor( COLOR_INACTIVEBORDER ) );
		return TRUE;
	}
	CAcadColorService* pColorService = GetColorService();
	if( !pColorService )
		return FALSE;
	if( pColorService->IsBackgroundNotSet() )
		return FALSE;
	if( pColorService->IsBackgroundTransparent() )
	{
		CWnd* pParent = mpControlWnd->GetParent();
		if( pParent )
		{
			int nDCInfo = pDC->SaveDC();
			HBRUSH hbrBackground = (HBRUSH)pParent->SendMessage( WM_CTLCOLORDLG, (WPARAM)pDC, (LPARAM)pParent->m_hWnd );
			pDC->RestoreDC(nDCInfo);
			if( !hbrBackground )
				return FALSE;
			pDC->FillRect( &rcClip, CBrush::FromHandle( hbrBackground ) );
		}
		return TRUE;
	}
	else if( mpTemplate->GetPropertyObject( Prop::BackgroundColor ) )
	{
		pDC->FillSolidRect( &rcClip, pColorService->GetBackgroundColor() );
		return TRUE;
	}
	return FALSE;
}

DROPEFFECT CDialogControl::BeginDragDrop( const CPoint& point )
{
	CDragDropService* pDragDropService = GetDragDropService();
	if( !pDragDropService )
		return DROPEFFECT_NONE;
	return pDragDropService->BeginDragDrop( point );
}

DROPEFFECT CDialogControl::OnBeginDrag( const CPoint& point, COleDataSource& SourceData )
{
	UINT CF_DclControl = CDragDropService::GetDclControlClipboardFormat();
	HGLOBAL hDclControlPtr = GlobalAlloc( GHND, sizeof(CDclControlTemplate*) );
	if( !hDclControlPtr )
		return DROPEFFECT_NONE;
	*(CDclControlTemplate**)GlobalLock( hDclControlPtr ) = &(*mpTemplate);
	GlobalUnlock( hDclControlPtr );
	SourceData.CacheGlobalData( CF_DclControl, hDclControlPtr );
	DWORD dwDropEffect = DROPEFFECT_COPY;

	CString sText;
	mpControlWnd->GetWindowText( sText );
	if( sText.IsEmpty() )
		return dwDropEffect;
	CStringA sTextA( sText );
	SIZE_T cchText = sTextA.GetLength() + 1;
	HGLOBAL hData = GlobalAlloc( GHND, cchText );
	if( !hData )
		return dwDropEffect;
	lstrcpynA( (char*)GlobalLock( hData ), sTextA, cchText );
	GlobalUnlock( hData );
	SourceData.CacheGlobalData( CF_TEXT, hData );
	return dwDropEffect;
}

DROPEFFECT CDialogControl::OnDragEnter( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState )
{
	return OnDragOver( point, pSourceData, dwKeyState );
}

DROPEFFECT CDialogControl::OnDragOver( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState )
{
	if( (dwKeyState & MK_CONTROL) != MK_CONTROL )
		return DROPEFFECT_MOVE;    
	return DROPEFFECT_COPY;
}

bool CDialogControl::OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect )
{
	return false;
}

CWnd* CDialogControl::GetTopLevelWnd()
{
	CWnd* pTopLevelWnd = mpControlWnd;
	if( !pTopLevelWnd->m_hWnd )
		return pTopLevelWnd;
	//if the control is hosted inside another window, find the ancestor
	//that is a child of the host dialog
	CWnd* pHostDlg = GetControlPane()->GetHostDialog();
	assert( pHostDlg->IsChild( pTopLevelWnd ) );
	while( (pTopLevelWnd->GetControlUnknown() || (pTopLevelWnd->GetStyle() & WS_CHILD)) &&
				 pTopLevelWnd->GetParent() != pHostDlg )
		pTopLevelWnd = pTopLevelWnd->GetParent();
	return pTopLevelWnd;
}

bool CDialogControl::IsAsyncEvents() const
{
	if( mpControlPane && mpControlPane->IsModal() )
		return false; // force controls on modal forms to handle events synchronously
	if( mpControlPane && mpControlPane->IsClosing() )
		return false;
	if( mpTemplate )
		return (mpTemplate->GetLongProperty(Prop::EventInvoke) == 1);
	return false;
}

CString CDialogControl::GetKeyName() const
{
	if( mpTemplate )
		return mpTemplate->GetKeyName();
	return CString();
}

CString CDialogControl::GetKeyPath() const
{
	if( mpTemplate )
		return mpTemplate->GetKeyPath();
	return CString();
}

CString CDialogControl::GetVarName() const
{
	if( mpTemplate )
		return mpTemplate->GetVarName();
	return CString();
}

CAxContainerCtrl* CDialogControl::GetActiveXCtrl() const
{
	if( mpTemplate->GetType() != CtlActiveX )
		return NULL;
	return (CAxContainerCtrl*)mpControlWnd;
}

bool CDialogControl::Focus()
{
	if( !mpControlWnd )
		return false;
	mpControlWnd->SetFocus();
	return true;
}

CRect CDialogControl::GetEffectiveWindowRect() const
{
	CRect rcWnd( 0, 0, 0, 0 );
	if( mpControlWnd )
	{
		CWnd* pWndChildOfHost = mpControlWnd;
		//if the control is hosted inside another window, use the parent instead
		bool bWindowLess = (pWndChildOfHost->m_hWnd == NULL);
		if( !bWindowLess )
		{
			if( (pWndChildOfHost->GetControlUnknown() || (pWndChildOfHost->GetStyle() & WS_CHILD)) &&
					pWndChildOfHost->GetParent() != mpControlPane->GetHostDialog() )
				pWndChildOfHost = pWndChildOfHost->GetParent();
		}
		pWndChildOfHost->GetWindowRect( &rcWnd );
		mpControlPane->GetHostDialog()->ScreenToClient( &rcWnd );
	}
	return rcWnd;
}

CRect CDialogControl::GetEffectiveClientRect() const
{
	CRect rcClient( 0, 0, 0, 0 );
	if( mpControlWnd )
	{
		CWnd* pWndChildOfHost = mpControlWnd;
		//if the control is hosted inside another window, use the parent instead
		bool bWindowLess = (pWndChildOfHost->m_hWnd == NULL);
		if( !bWindowLess )
		{
			if( (pWndChildOfHost->GetStyle() & WS_CHILD) &&
					pWndChildOfHost->GetParent() != mpControlPane->GetHostDialog() )
				pWndChildOfHost = pWndChildOfHost->GetParent();
		}
		pWndChildOfHost->GetClientRect( &rcClient );
	}
	return rcClient;
}

void CDialogControl::HandleDpiChanged()
{
	OnApplyFont( mpTemplate->GetPropertyObject( Prop::FontSize ) );
}

long CDialogControl::FromDIP( long nDIP ) const
{
	mpControlPane->FromDIP( &nDIP, 1 );
	return nDIP;
}

long CDialogControl::ToDIP( long nDDP ) const
{
	mpControlPane->ToDIP( &nDDP, 1 );
	return nDDP;
}

POINT& CDialogControl::FromDIP( POINT& pt ) const
{
	mpControlPane->FromDIP( &pt.x, 2 );
	return pt;
}

POINT& CDialogControl::ToDIP( POINT& pt ) const
{
	mpControlPane->ToDIP( &pt.x, 2 );
	return pt;
}

SIZE& CDialogControl::FromDIP( SIZE& size ) const
{
	mpControlPane->FromDIP( &size.cx, 2 );
	return size;
}

SIZE& CDialogControl::ToDIP( SIZE& size ) const
{
	mpControlPane->ToDIP( &size.cx, 2 );
	return size;
}

RECT& CDialogControl::FromDIP( RECT& rect ) const
{
	mpControlPane->FromDIP( &rect.left, 4 );
	return rect;
}

RECT& CDialogControl::ToDIP( RECT& rect ) const
{
	mpControlPane->ToDIP( &rect.left, 4 );
	return rect;
}

POINT& CDialogControl::FromPhysical( POINT& point ) const
{
	HWND hwnd = GetHWnd();
	DpiAwarenessHelper::PhysicalToLogicalPoint( hwnd, &point );
	return point;
}

POINT& CDialogControl::ToPhysical( POINT& point ) const
{
	HWND hwnd = GetHWnd();
	DpiAwarenessHelper::LogicalToPhysicalPoint( hwnd, &point );
	return point;
}

void CDialogControl::SetPosLeft( long lNewLeft )
{
	mpTemplate->SetLongProperty( Prop::Left, lNewLeft );
	SyncPosLeft();
}

void CDialogControl::SetPosTop( long lNewTop )
{
	mpTemplate->SetLongProperty( Prop::Top, lNewTop );
	SyncPosTop();
}

void CDialogControl::SetPosWidth( long lNewWidth )
{
	mpTemplate->SetLongProperty( Prop::Width, lNewWidth );
	SyncPosRight();
}

void CDialogControl::SetPosHeight( long lNewHeight )
{
	mpTemplate->SetLongProperty( Prop::Height, lNewHeight );
	SyncPosBottom();
}

void CDialogControl::SyncPosLeft()
{
	TPropertyPtr pLeftFromRightProp = mpTemplate->GetPropertyObject( Prop::LeftFromRight );
	if( pLeftFromRightProp )
	{
		long lLeftFromRight = mpTemplate->GetLongProperty( Prop::UseLeftFromRight );
		long lNew = 0;
		switch( lLeftFromRight  )
		{
		case 0:
		case 1: //offset from right edge of control area
			lNew = mpControlPane->GetControlArea().right - mpTemplate->GetLongProperty( Prop::Left );
			break;
		case 2: //offset from center of control area
			lNew = mpTemplate->GetLongProperty( Prop::Left ) - mpControlPane->GetControlArea().CenterPoint().x;
			break;
		default: //offset from splitter
			lNew = mpTemplate->GetLongProperty( Prop::Left ) - mpControlPane->GetSplitterPos( lLeftFromRight ).x;
			break;
		}
		if( pLeftFromRightProp->GetLongValue() != lNew )
			pLeftFromRightProp->SetLongValue( lNew );
	}
}

void CDialogControl::SyncPosTop()
{
	TPropertyPtr pTopFromBottomProp = mpTemplate->GetPropertyObject( Prop::TopFromBottom );
	if( pTopFromBottomProp )
	{
		long lTopFromBottom = mpTemplate->GetLongProperty( Prop::UseTopFromBottom );
		long lNew = 0;
		switch( lTopFromBottom  )
		{
		case 0:
		case 1: //offset from bottom edge of control area
			lNew = mpControlPane->GetControlArea().bottom - mpTemplate->GetLongProperty( Prop::Top );
			break;
		case 2: //offset from center of control area
			lNew = mpTemplate->GetLongProperty( Prop::Top ) - mpControlPane->GetControlArea().CenterPoint().y;
			break;
		default: //offset from splitter
			lNew = mpTemplate->GetLongProperty( Prop::Top ) - mpControlPane->GetSplitterPos( lTopFromBottom ).y;
			break;
		}
		if( pTopFromBottomProp->GetLongValue() != lNew )
			pTopFromBottomProp->SetLongValue( lNew );
	}
}

void CDialogControl::SyncPosRight()
{
	TPropertyPtr pRightFromRightProp = mpTemplate->GetPropertyObject( Prop::RightFromRight );
	if( pRightFromRightProp )
	{
		long lLeft = mpTemplate->GetLongProperty( Prop::Left );
		long lRightFromRight = mpTemplate->GetLongProperty( Prop::UseRightFromRight );
		long lNew = 0;
		switch( lRightFromRight  )
		{
		case 0:
		case 1: //offset from right edge of control area
			lNew = mpControlPane->GetControlArea().right - lLeft - mpTemplate->GetLongProperty( Prop::Width );
			break;
		default: //offset from splitter
			lNew = lLeft + mpTemplate->GetLongProperty( Prop::Width ) - mpControlPane->GetSplitterPos( lRightFromRight ).x;
			break;
		}
		if( pRightFromRightProp->GetLongValue() != lNew )
			pRightFromRightProp->SetLongValue( lNew );
	}
}

void CDialogControl::SyncPosBottom()
{
	TPropertyPtr pBottomFromBottomProp = mpTemplate->GetPropertyObject( Prop::BottomFromBottom );
	if( pBottomFromBottomProp )
	{
		long lTop = mpTemplate->GetLongProperty( Prop::Top );
		long lBottomFromBottom = mpTemplate->GetLongProperty( Prop::UseBottomFromBottom );
		long lNew = 0;
		switch( lBottomFromBottom  )
		{
		case 0:
		case 1: //offset from right edge of control area
			lNew = mpControlPane->GetControlArea().bottom - lTop - mpTemplate->GetLongProperty( Prop::Height );
			break;
		default: //offset from splitter
			lNew = mpControlPane->GetSplitterPos( lBottomFromBottom ).y - mpTemplate->GetLongProperty( Prop::Height ) - lTop;
			break;
		}
		if( pBottomFromBottomProp->GetLongValue() != lNew )
			pBottomFromBottomProp->SetLongValue( lNew );
	}
}

CRect CDialogControl::GetWndRect() const
{
	CPoint pntUpperLeft( mpTemplate->GetLongProperty(Prop::Left), mpTemplate->GetLongProperty(Prop::Top) );
	return CRect( pntUpperLeft.x,
								pntUpperLeft.y,
								pntUpperLeft.x + mpTemplate->GetLongProperty(Prop::Width),
								pntUpperLeft.y + mpTemplate->GetLongProperty(Prop::Height) );
}

DWORD CDialogControl::GetWndStyle() const
{
	DWORD dwStyle = WS_CHILD /*| WS_VISIBLE*/ | WS_CLIPSIBLINGS;

	if( mpTemplate->GetBooleanProperty(Prop::IsTabStop) )
		dwStyle |= WS_TABSTOP;
	if( mpTemplate->GetBooleanProperty(Prop::BeginGroup) )
		dwStyle |= WS_GROUP;
	if( mpTemplate->GetBooleanProperty(Prop::VScrollBar) )
		dwStyle |= WS_VSCROLL;

	return dwStyle;
}

CString CDialogControl::GetWndCaption() const
{
	return mpTemplate->GetStringProperty(Prop::Caption);
}

void CDialogControl::OnFrameChanged()
{
	OnNeedRepaint( true, true );
}

void CDialogControl::OnNeedRepaint( bool bRepaintBackground /*= false*/, bool bUpdateNow /*= false*/ ) const
{
	if( !mpControlWnd->m_hWnd )
		return;
	mpControlWnd->Invalidate( bRepaintBackground? TRUE : FALSE );
	if( bUpdateNow )
		mpControlWnd->UpdateWindow();
	return;
/*
	if( bRepaintBackground &&
			((mpControlWnd->GetExStyle() & WS_EX_TRANSPARENT) || !mpControlWnd->IsWindowVisible()) )
	{ //force the control's background to be redrawn if it has a transparent background
		CWnd* pHostDlg = mpControlPane->GetHostDialog();
		if( pHostDlg )
		{
			CRect rcControl;
			mpControlWnd->GetWindowRect( &rcControl );
			if( (pHostDlg->GetExStyle() & WS_EX_TRANSPARENT) )
			{
				CWnd* pHostParent = pHostDlg->GetParent();
				if( pHostParent )
				{
					CRect rcInvalid = rcControl;
					pHostParent->ScreenToClient( &rcInvalid );
					pHostParent->InvalidateRect( &rcInvalid, TRUE );
					if( bUpdateNow )
						pHostParent->UpdateWindow();
				}
			}
			pHostDlg->ScreenToClient( &rcControl );
			pHostDlg->InvalidateRect( &rcControl, TRUE );
			if( bUpdateNow )
				pHostDlg->UpdateWindow();
		}
	}
	if( mpControlWnd )
	{
		if( mpControlWnd->m_hWnd )
		{
			mpControlWnd->Invalidate();
			if( bUpdateNow )
				mpControlWnd->UpdateWindow();
		}
		else
		{
			CWnd* pHostDlg = mpControlPane->GetHostDialog();
			if( pHostDlg )
			{
				CDC* pdc = pHostDlg->GetDC();
				mpControlWnd->PaintWindowlessControls( pdc );
				pHostDlg->ReleaseDC( pdc );
			}
		}
	}
*/
}

CRect CDialogControl::ValidatePosition( const CRect& rcProposed ) const
{
	return rcProposed;
}

void CDialogControl::ApplyPosition()
{
	//TraceFmt( _T("CDialogControl(%s)::ApplyPosition() [%d, %d] @ [%d x %d]\r\n"),
	//					asString( this ),
	//					mpTemplate->GetLongProperty( Prop::Left ),
	//					mpTemplate->GetLongProperty( Prop::Top ),
	//					mpTemplate->GetLongProperty( Prop::Width ),
	//					mpTemplate->GetLongProperty( Prop::Height ) );
	if( mbEnumProps )
		return; //defer
	mpControlPane->ApplyPosition( TDialogControlLockedPtr( this ) );
}

bool CDialogControl::ApplyPropertiesEnum()
{
	bool bSuccess = true;
	mbEnumProps = true;
	std::vector< Prop::Id > ridFirst;
	std::vector< Prop::Id > ridLast;
	ApplyPropertiesOrder( ridFirst, ridLast );
	for( std::vector< Prop::Id >::const_iterator iter = ridFirst.begin();
			 iter != ridFirst.end();
			 ++iter )
		ApplyProperty( mpTemplate->GetPropertyObject( *iter ) );
	const TPropertyList& Props = mpTemplate->GetPropertyList();
	for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
	{
		TPropertyPtr pProp = (*iter);
		Prop::Id id = pProp->GetID();
		if( std::find( ridFirst.begin(), ridFirst.end(), id ) != ridFirst.end() || 
				std::find( ridLast.begin(), ridLast.end(), id ) != ridLast.end() )
			continue; //skip first/last properties
		ApplyProperty( pProp );
	}
	for( std::vector< Prop::Id >::const_iterator iter = ridLast.begin();
			 iter != ridLast.end();
			 ++iter )
		ApplyProperty( mpTemplate->GetPropertyObject( *iter ) );
	mbEnumProps = false;
	ApplyPosition();
	return bSuccess;
}

void CDialogControl::ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast )
{
	//make sure the position offsets are corrected before changing anything else
	ridFirst.push_back( Prop::UseLeftFromRight );
	ridFirst.push_back( Prop::UseRightFromRight );
	ridFirst.push_back( Prop::UseTopFromBottom );
	ridFirst.push_back( Prop::UseBottomFromBottom );
	ridFirst.push_back( Prop::ImageList );
	ridLast.push_back( Prop::AutoSize ); //save autosize for last
}

bool CDialogControl::ApplyProperty( TPropertyPtr pProp )
{
	if( !pProp )
		return false;
	//DisableUndoManager DisableUndo( mpTemplate->GetUndoManager() );
	bool bSuccess = true;
	switch( pProp->GetID() )
	{
	case Prop::Name: if( !OnApplyName( pProp ) ) bSuccess = false; break;
	case Prop::ForegroundColor: if( !OnApplyForegroundColor( pProp ) ) bSuccess = false; break;
	case Prop::BackgroundColor: if( !OnApplyBackgroundColor( pProp ) ) bSuccess = false; break;
	case Prop::BorderStyle: if( !OnApplyBorderStyle( pProp ) ) bSuccess = false; break;
	case Prop::Enabled: if( !OnApplyEnabled( pProp ) ) bSuccess = false; break;
	case Prop::DragnDropAllowDrop: if( !OnApplyDragDropAllowDrop( pProp ) ) bSuccess = false; break;
	case Prop::Visible: if( !OnApplyVisible( pProp ) ) bSuccess = false; break;
	case Prop::Caption: if( !OnApplyCaption( pProp ) ) bSuccess = false; break;
	case Prop::TitleBarText: if( !OnApplyCaption( pProp ) ) bSuccess = false; break;
	case Prop::IsTabStop: if( !OnApplyIsTabStop( pProp ) ) bSuccess = false; break;
	case Prop::BeginGroup: if( !OnApplyBeginGroup( pProp ) ) bSuccess = false; break;
	case Prop::VScrollBar: if( !OnApplyVScrollBar( pProp ) ) bSuccess = false; break;
	case Prop::HScrollBar: if( !OnApplyHScrollBar( pProp ) ) bSuccess = false; break;
	case Prop::UseVisualStyle: if( !OnApplyUseVisualStyle( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipTitle: if( !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipBalloon: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipLine: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipBody: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipPicture: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipAviFileName: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::ToolTipTitleColor: if( !IsEnumeratingProperties() && !OnApplyToolTip( pProp ) ) bSuccess = false; break;
	case Prop::FontName: if( !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontSize: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontBold: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontItalic: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontUnderline: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::FontStrikeout: if( !IsEnumeratingProperties() && !OnApplyFont( pProp ) ) bSuccess = false; break;
	case Prop::Left: if( !OnApplyLeft( pProp ) ) bSuccess = false; break;
	case Prop::Top: if( !OnApplyTop( pProp ) ) bSuccess = false; break;
	case Prop::Width: if( !OnApplyWidth( pProp ) ) bSuccess = false; break;
	case Prop::Height: if( !OnApplyHeight( pProp ) ) bSuccess = false; break;
	case Prop::LeftFromRight: if( !OnApplyLeftFromRight( pProp ) ) bSuccess = false; break;
	case Prop::RightFromRight: if( !OnApplyRightFromRight( pProp ) ) bSuccess = false; break;
	case Prop::TopFromBottom: if( !OnApplyTopFromBottom( pProp ) ) bSuccess = false; break;
	case Prop::BottomFromBottom: if( !OnApplyBottomFromBottom( pProp ) ) bSuccess = false; break;
	case Prop::UseLeftFromRight: if( !OnApplyUseLeftFromRight( pProp ) ) bSuccess = false; break;
	case Prop::UseRightFromRight: if( !OnApplyUseRightFromRight( pProp ) ) bSuccess = false; break;
	case Prop::UseTopFromBottom: if( !OnApplyUseTopFromBottom( pProp ) ) bSuccess = false; break;
	case Prop::UseBottomFromBottom: if( !OnApplyUseBottomFromBottom( pProp ) ) bSuccess = false; break;
	}
	return bSuccess;
}

bool CDialogControl::OnApplyName( TPropertyPtr pProp )
{
	return true;
}

bool CDialogControl::OnApplyForegroundColor( TPropertyPtr pProp )
{
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
		pColorService->SetForegroundColor( pProp->GetLongValue() );
	OnNeedRepaint( false );
	return true;
}

bool CDialogControl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	if( pProp->GetLongValue() == -24 ) //-24 = transparent background
	{
		mpControlWnd->ModifyStyleEx( 0, WS_EX_TRANSPARENT );
		CWnd* pTop = GetTopLevelWnd();
		if( pTop != mpControlWnd )
			pTop->ModifyStyleEx( 0, WS_EX_TRANSPARENT );
	}
	else
	{
		mpControlWnd->ModifyStyleEx( WS_EX_TRANSPARENT, 0 );
		CWnd* pTop = GetTopLevelWnd();
		if( pTop != mpControlWnd )
			pTop->ModifyStyleEx( WS_EX_TRANSPARENT, 0 );
	}
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
		pColorService->SetBackgroundColor( pProp->GetLongValue() );
	OnNeedRepaint( true );
	return true;
}

bool CDialogControl::OnApplyBorderStyle( TPropertyPtr pProp )
{
	switch( pProp->GetLongValue() )
	{
	case 0:
		mpControlWnd->ModifyStyle( WS_BORDER, 0, SWP_FRAMECHANGED );
		mpControlWnd->ModifyStyleEx( (WS_EX_STATICEDGE | WS_EX_CLIENTEDGE), 0, SWP_FRAMECHANGED );
		break;
	case 1:
		mpControlWnd->ModifyStyle( WS_BORDER, 0, SWP_FRAMECHANGED );
		mpControlWnd->ModifyStyleEx( WS_EX_STATICEDGE, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED );
		break;
	case 2:
		mpControlWnd->ModifyStyle( WS_BORDER, 0, SWP_FRAMECHANGED );
		mpControlWnd->ModifyStyleEx( WS_EX_CLIENTEDGE, WS_EX_STATICEDGE, SWP_FRAMECHANGED );
		break;
	}
	OnFrameChanged();
	return true;
}

bool CDialogControl::OnApplyEnabled( TPropertyPtr pProp )
{
	mpControlWnd->EnableWindow( pProp->GetBooleanValue() );
	OnNeedRepaint();
	return true;
}

bool CDialogControl::OnApplyDragDropAllowDrop( TPropertyPtr pProp )
{
	CDragDropService* pDragDropService = GetDragDropService();
	if( !pDragDropService )
		return false;
	if( pProp->GetBooleanValue() )
		pDragDropService->RegisterControlAsDropTarget();
	else
		pDragDropService->RevokeControlAsDropTarget();
	return true;
}

bool CDialogControl::OnApplyVisible( TPropertyPtr pProp )
{
	CControlPane* pPane = GetControlPane();
	if( pPane )
		pPane->ApplyVisibility( TDialogControlLockedPtr( this ) );
	else
		mpControlWnd->ShowWindow( pProp->GetBooleanValue()? SW_SHOW : SW_HIDE );
	return true;
}

bool CDialogControl::OnApplyCaption( TPropertyPtr pProp )
{
	mpControlWnd->SetWindowText( pProp->GetStringValue() );
	OnNeedRepaint( true );
	return true;
}

bool CDialogControl::OnApplyIsTabStop( TPropertyPtr pProp )
{
	if( pProp->GetBooleanValue() )
		mpControlWnd->ModifyStyle( 0, WS_TABSTOP );
	else
		mpControlWnd->ModifyStyle( WS_TABSTOP, 0 );
	return true;
}

bool CDialogControl::OnApplyBeginGroup( TPropertyPtr pProp )
{
	if( pProp->GetBooleanValue() )
		mpControlWnd->ModifyStyle( 0, WS_GROUP );
	else
		mpControlWnd->ModifyStyle( WS_GROUP, 0 );
	return true;
}

bool CDialogControl::OnApplyVScrollBar( TPropertyPtr pProp )
{
	if( pProp->GetBooleanValue() )
		mpControlWnd->ModifyStyle( 0, WS_VSCROLL, SWP_FRAMECHANGED );
	else
		mpControlWnd->ModifyStyle( WS_VSCROLL, 0, SWP_FRAMECHANGED );
	OnFrameChanged();
	return true;
}

bool CDialogControl::OnApplyHScrollBar( TPropertyPtr pProp )
{
	if( pProp->GetBooleanValue() )
		mpControlWnd->ModifyStyle( 0, WS_HSCROLL, SWP_FRAMECHANGED );
	else
		mpControlWnd->ModifyStyle( WS_HSCROLL, 0, SWP_FRAMECHANGED );
	OnFrameChanged();
	return true;
}

bool CDialogControl::OnApplyUseVisualStyle( TPropertyPtr pProp )
{
	if( pProp->GetBooleanValue() )
		GetTheme().SetWindowTheme( NULL, NULL );
	else
		GetTheme().SetWindowTheme( L"", L"" );
	OnNeedRepaint();
	return true;
}

bool CDialogControl::OnApplyToolTip( TPropertyPtr pProp )
{
	GetToolTipCtrl().RemoveAllTools();
	SetToolTipEx( mpControlWnd, GetToolTipCtrl(), mpTemplate );
	return true;
}

bool CDialogControl::OnApplyFont( TPropertyPtr pProp )
{
	mpControlWnd->SetFont( theWorkspace.GetFontCollection().GetFont( mpTemplate, mpControlPane ) );
	OnNeedRepaint();
	return true;
}

bool CDialogControl::OnApplyLeft( TPropertyPtr pProp )
{
	SyncPosLeft();
	ApplyPosition();
	return true;
}

bool CDialogControl::OnApplyTop( TPropertyPtr pProp )
{
	SyncPosTop();
	ApplyPosition();
	return true;
}

bool CDialogControl::OnApplyWidth( TPropertyPtr pProp )
{
	SyncPosRight();
	ApplyPosition();
	return true;
}

bool CDialogControl::OnApplyHeight( TPropertyPtr pProp )
{
	SyncPosBottom();
	ApplyPosition();
	return true;
}

bool CDialogControl::OnApplyLeftFromRight( TPropertyPtr pProp )
{
	TPropertyPtr pLeftProp = mpTemplate->GetPropertyObject( Prop::Left );
	if( !pLeftProp )
		return false;
	long lUseLeftFromRight = mpTemplate->GetLongProperty( Prop::UseLeftFromRight );
	long lNew = 0;
	switch( lUseLeftFromRight  )
	{
	case 0:
	case 1: //offset from right edge of control area
		lNew = mpControlPane->GetControlArea().right - pProp->GetLongValue();
		break;
	case 2: //offset from center of control area
		lNew = mpControlPane->GetControlArea().CenterPoint().x + pProp->GetLongValue();
		break;
	default: //offset from splitter
		lNew = mpControlPane->GetSplitterPos( lUseLeftFromRight ).x + pProp->GetLongValue();
		break;
	}
	if( pLeftProp->GetLongValue() != lNew )
	{
		pLeftProp->SetLongValue( lNew );
		return OnApplyLeft( pLeftProp );
	}
	return true;
}

bool CDialogControl::OnApplyRightFromRight( TPropertyPtr pProp )
{
	TPropertyPtr pWidthProp = mpTemplate->GetPropertyObject( Prop::Width );
	if( !pWidthProp )
		return false;
	long lLeft = mpTemplate->GetLongProperty( Prop::Left );
	long lUseRightFromRight = mpTemplate->GetLongProperty( Prop::UseRightFromRight );
	long lNew = 0;
	switch( lUseRightFromRight  )
	{
	case 0:
	case 1: //offset from right edge of control area
		lNew = mpControlPane->GetControlArea().right - pProp->GetLongValue() - lLeft;
		break;
	default: //offset from splitter
		lNew = mpControlPane->GetSplitterPos( lUseRightFromRight ).x + pProp->GetLongValue() - lLeft;
		break;
	}
	if( pWidthProp->GetLongValue() != lNew )
	{
		pWidthProp->SetLongValue( lNew );
		return OnApplyWidth( pWidthProp );
	}
	return true;
}

bool CDialogControl::OnApplyTopFromBottom( TPropertyPtr pProp )
{
	TPropertyPtr pTopProp = mpTemplate->GetPropertyObject( Prop::Top );
	if( !pTopProp )
		return false;
	long lUseTopFromBottom = mpTemplate->GetLongProperty( Prop::UseTopFromBottom );
	long lNew = 0;
	switch( lUseTopFromBottom  )
	{
	case 0:
	case 1: //offset from bottom edge of control area
		lNew = mpControlPane->GetControlArea().bottom - pProp->GetLongValue();
		break;
	case 2: //offset from center of control area
		lNew = mpControlPane->GetControlArea().CenterPoint().y + pProp->GetLongValue();
		break;
	default: //offset from splitter
		lNew = mpControlPane->GetSplitterPos( lUseTopFromBottom ).y + pProp->GetLongValue();
		break;
	}
	if( pTopProp->GetLongValue() != lNew )
	{
		pTopProp->SetLongValue( lNew );
		return OnApplyTop( pTopProp );
	}
	return true;
}

bool CDialogControl::OnApplyBottomFromBottom( TPropertyPtr pProp )
{
	TPropertyPtr pHeightProp = mpTemplate->GetPropertyObject( Prop::Height );
	if( !pHeightProp )
		return false;
	long lTop = mpTemplate->GetLongProperty( Prop::Top );
	long lUseBottomFromBottom = mpTemplate->GetLongProperty( Prop::UseBottomFromBottom );
	long lNew = 0;
	switch( lUseBottomFromBottom  )
	{
	case 0:
	case 1: //offset from right edge of control area
		lNew = mpControlPane->GetControlArea().bottom - pProp->GetLongValue() - lTop;
		break;
	default: //offset from splitter
		lNew = mpControlPane->GetSplitterPos( lUseBottomFromBottom ).y - pProp->GetLongValue() - lTop;
		break;
	}
	if( pHeightProp->GetLongValue() != lNew )
	{
		pHeightProp->SetLongValue( lNew );
		return OnApplyHeight( pHeightProp );
	}
	return true;
}

bool CDialogControl::OnApplyUseLeftFromRight( TPropertyPtr pProp )
{
	if( pProp->GetLongValue() > 0 )
	{
		TPropertyPtr pLeftFromRightProp = mpTemplate->GetPropertyObject( Prop::LeftFromRight );
		if( !pLeftFromRightProp )
			return false;
		return OnApplyLeftFromRight( pLeftFromRightProp );
	}
	TPropertyPtr pLeftProp = mpTemplate->GetPropertyObject( Prop::Left );
	if( !pLeftProp )
		return false;
	return OnApplyLeft( pLeftProp );
}

bool CDialogControl::OnApplyUseRightFromRight( TPropertyPtr pProp )
{
	if( pProp->GetLongValue() > 0 )
	{
		TPropertyPtr pRightFromRightProp = mpTemplate->GetPropertyObject( Prop::RightFromRight );
		if( !pRightFromRightProp )
			return false;
		return OnApplyRightFromRight( pRightFromRightProp );
	}
	TPropertyPtr pWidthProp = mpTemplate->GetPropertyObject( Prop::Width );
	if( !pWidthProp )
		return false;
	return OnApplyWidth( pWidthProp );
}

bool CDialogControl::OnApplyUseTopFromBottom( TPropertyPtr pProp )
{
	if( pProp->GetLongValue() > 0 )
	{
		TPropertyPtr pTopFromBottomProp = mpTemplate->GetPropertyObject( Prop::TopFromBottom );
		if( !pTopFromBottomProp )
			return false;
		return OnApplyTopFromBottom( pTopFromBottomProp );
	}
	TPropertyPtr pTopProp = mpTemplate->GetPropertyObject( Prop::Top );
	if( !pTopProp )
		return false;
	return OnApplyTop( pTopProp );
}

bool CDialogControl::OnApplyUseBottomFromBottom( TPropertyPtr pProp )
{
	if( pProp->GetLongValue() > 0 )
	{
		TPropertyPtr pBottomFromBottomProp = mpTemplate->GetPropertyObject( Prop::BottomFromBottom );
		if( !pBottomFromBottomProp )
			return false;
		return OnApplyBottomFromBottom( pBottomFromBottomProp );
	}
	TPropertyPtr pHeightProp = mpTemplate->GetPropertyObject( Prop::Height );
	if( !pHeightProp )
		return false;
	return OnApplyHeight( pHeightProp );
}
