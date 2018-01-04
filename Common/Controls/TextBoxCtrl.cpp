#include "stdafx.h"
#include "TextBoxCtrl.h"
#include "DclControlTemplate.h"
#include "ControlPane.h"


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl

CTextBoxCtrl::CTextBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID,
														CInputFilter* pFilter /*= NULL*/, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
, CFilteredEditCtrl( pFilter )
, mbDragging( false )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CTextBoxCtrl::~CTextBoxCtrl()
{
}

bool CTextBoxCtrl::Create( CWnd* pParentWnd, UINT nID )
{
	bool bSuccess = __super::Create( pParentWnd, GetWndRect(), nID, GetWndStyle(), GetInputFilter() );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	return bSuccess;
}

DWORD CTextBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();
	if( mpTemplate->GetBooleanProperty( Prop::ReturnAsTab ) ||
			!mpTemplate->GetStringProperty( Prop::EventReturnPressed ).IsEmpty() )
		dwStyle |= (ES_WANTRETURN);
	switch( mpTemplate->GetLongProperty( Prop::Justification ) )
	{
	case 0:
		dwStyle |= ES_LEFT;
		break;
	case 1:
		dwStyle |= ES_CENTER;
		break;
	case 2:
		dwStyle |= ES_RIGHT;
		break;
	}
	if( mpTemplate->GetBooleanProperty( Prop::AutoVScroll ) )
		dwStyle |= ES_AUTOVSCROLL;
	if( mpTemplate->GetBooleanProperty( Prop::AutoHScroll ) )
		dwStyle |= ES_AUTOHSCROLL;
	return dwStyle;
}

void CTextBoxCtrl::HandleDpiChanged()
{
	__super::HandleDpiChanged();
	ApplyProperty( mpTemplate->GetPropertyObject( Prop::MarginLeft ) );
}

bool CTextBoxCtrl::ApplyProperty( TPropertyPtr pProp )
{
	if( !__super::ApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::FilterStyle: //can't currently change style at runtime -- 2007-09-10 [ORW]
		break;
	case Prop::Justification:
		{
			switch( pProp->GetLongValue() )
			{
			case 0:
				ModifyStyle( (ES_CENTER | ES_RIGHT), ES_LEFT, 0 );
				break;
			case 1:
				ModifyStyle( (ES_LEFT | ES_RIGHT), ES_CENTER, 0 );
				break;
			case 2:
				ModifyStyle( (ES_LEFT | ES_CENTER), ES_RIGHT, 0 );
				break;
			}
			OnNeedRepaint();
			break;
		}
	case Prop::Text:
		{
			SetWindowText( pProp->GetStringValue() );
			OnNeedRepaint();
			break;
		}
	case Prop::AutoVScroll:
		{
			if( pProp->GetBooleanValue() )
				ModifyStyle( 0, ES_AUTOVSCROLL, 0 );
			else
				ModifyStyle( ES_AUTOVSCROLL, 0, 0 );
			break;
		}
	case Prop::AutoHScroll:
		{
			if( pProp->GetBooleanValue() )
				ModifyStyle( 0, ES_AUTOHSCROLL, 0 );
			else
				ModifyStyle( ES_AUTOHSCROLL, 0, 0 );
			break;
		}
	case Prop::TextLimit:
		{
			SetLimitText( pProp->GetLongValue() );
			break;
		}
	case Prop::MarginLeft:
		{
			SetMargins( FromDIP( pProp->GetLongValue() ), FromDIP( mpTemplate->GetLongProperty( Prop::MarginRight ) ) );
			break;
		}
	case Prop::MarginRight:
		{
			if( !IsEnumeratingProperties() )
				SetMargins( FromDIP( mpTemplate->GetLongProperty( Prop::MarginLeft ) ), FromDIP( pProp->GetLongValue() ) );
			break;
		}
	case Prop::ReadOnly:
		{
			SetReadOnly( pProp->GetBooleanValue() );
			break;
		}
	}
	return !bFailed;
}

bool CTextBoxCtrl::OnApplyBackgroundColor( TPropertyPtr pProp )
{
	CAcadColorService* pColorService = GetColorService();
	if( pColorService )
		pColorService->SetBackgroundColor( pProp->GetLongValue() );
	OnNeedRepaint( true );
	return true;
}

DROPEFFECT CTextBoxCtrl::OnBeginDrag( const CPoint& point, COleDataSource& SourceData )
{
	DROPEFFECT dwEffect = __super::OnBeginDrag( point, SourceData );
	int nStartChar = -1;
	int nEndChar = -1;
	GetSel( nStartChar, nEndChar );
	if( nStartChar != nEndChar )
	{
		CString sText;
		GetWindowText( sText );
		if( nEndChar < nStartChar )
		{
			int nTemp = nEndChar;
			nEndChar = nStartChar;
			nStartChar = nTemp;
		}
		sText = sText.Mid( nStartChar, nEndChar - nStartChar );
		CStringA sTextA( sText );
		SIZE_T cchText = sTextA.GetLength() + 1;
		HGLOBAL hData = GlobalAlloc( GHND, cchText );
		if( hData )
		{
			lstrcpynA( (char*)GlobalLock( hData ), sTextA, cchText );
			GlobalUnlock( hData );
			SourceData.CacheGlobalData( CF_TEXT, hData );
		}
	}

	return (dwEffect | DROPEFFECT_MOVE | DROPEFFECT_COPY);
}

bool CTextBoxCtrl::OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect )
{
	HGLOBAL hData = pSourceData->GetGlobalData( CF_TEXT );
	if( !hData )
		return __super::OnDrop( point, pSourceData, dropEffect );
	CStringA sTextA = (char*)GlobalLock( hData );
	GlobalUnlock( hData );
	GlobalFree( hData );
	CPoint ptIns = point;
	if( mbDragging )
	{
		if( dropEffect == DROPEFFECT_MOVE )
		{
			DWORD posIns = CharFromPos( point );
			int nInsPos = LOWORD(posIns);
			int nLine = HIWORD(posIns);
			if( nLine > 0 )
				nInsPos += LineIndex( nLine );
			int nStartChar = -1;
			int nEndChar = -1;
			GetSel( nStartChar, nEndChar );
			ReplaceSel( _T(""), TRUE );
			if( nInsPos > nEndChar )
			{
				nInsPos -= (nEndChar - nStartChar);
				ptIns = PosFromChar( nInsPos );
				if( ptIns.x == -1 && ptIns.y == -1 )
					ptIns = point; //dropped after last character, so just tack it on the end
			}
			else if( nInsPos >= nStartChar )
				ptIns = PosFromChar( nStartChar );
		}
		mbDragging = false;
	}
	SendMessage( WM_LBUTTONDOWN, 0, MAKELPARAM(ptIns.x,ptIns.y) );
	SendMessage( WM_LBUTTONUP, 0, MAKELPARAM(ptIns.x,ptIns.y) );
	ReplaceSel( CString( sTextA ), TRUE );
	return true;
}

HBRUSH CTextBoxCtrl::HandleCtlColor( CDC* pDC, UINT nCtlColor )
{
	if( (GetStyle() & ES_READONLY) )
		return NULL;
	return __super::HandleCtlColor( pDC, nCtlColor );
}

BEGIN_MESSAGE_MAP(CTextBoxCtrl, CFilteredEditCtrl)
	ON_CONTROL_REFLECT(EN_CHANGE, &CTextBoxCtrl::OnChange)
	ON_WM_LBUTTONDOWN()
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_DPICHANGED_AFTERPARENT, &CTextBoxCtrl::OnDpiChanged)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl message handlers

LRESULT CTextBoxCtrl::OnDpiChanged(WPARAM wParam, LPARAM lParam)
{
	HandleDpiChanged();
	return 0;
}

BOOL CTextBoxCtrl::PreTranslateMessage(MSG* pMsg)
{
	GetToolTipCtrl().RelayEvent(pMsg);
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN )
	{
		if( mpTemplate->GetBooleanProperty( Prop::ReturnAsTab ) )
			pMsg->wParam = VK_TAB;
	}
	return __super::PreTranslateMessage(pMsg);
}

void CTextBoxCtrl::PostNcDestroy()
{
	__super::PostNcDestroy();
	delete this;
}

void CTextBoxCtrl::OnChange()
{
	CString sText;
	GetWindowText( sText );
	if( mpTemplate->GetStringProperty( Prop::Text ) != sText )
		mpTemplate->SetStringProperty( Prop::Text, sText );
}

void CTextBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( mpTemplate->GetBooleanProperty( Prop::DragnDropAllowBegin ) && nFlags == MK_LBUTTON )
	{
		//only start drag if there is a selection
		int nStartChar = -1;
		int nEndChar = -1;
		GetSel( nStartChar, nEndChar );
		if( nStartChar != nEndChar )
		{
			mbDragging = true;
			DROPEFFECT dwDropEffect = BeginDragDrop( point );
			if( mbDragging ) //if drop was on this control, mbDragging gets reset to false
			{
				PostMessage( WM_LBUTTONUP, 0, MAKELPARAM(point.x,point.y) );
				if( dwDropEffect == DROPEFFECT_MOVE )
					ReplaceSel( _T(""), TRUE );
				mbDragging = false;
			}
			else
				return;
		}
	}
	__super::OnLButtonDown(nFlags, point);
}

HBRUSH CTextBoxCtrl::CtlColor(CDC* pDC, UINT nCtlColor)
{
	HBRUSH hbrBackground = HandleCtlColor( pDC, nCtlColor );
	if( hbrBackground )
		return hbrBackground;
	if( (GetStyle() & ES_READONLY) )
	{
		COLORREF crReadOnly = GetSysColor( COLOR_BTNFACE );
		pDC->SetBkColor( crReadOnly );
		static CBrush mbrReadOnly( crReadOnly );
		return mbrReadOnly;
	}
	return NULL;
	return CAcadColorService::GetTransparentBrush();
}

BOOL CTextBoxCtrl::OnEraseBkgnd(CDC* pDC)
{
	if( (GetStyle() & ES_READONLY) )
		return __super::OnEraseBkgnd( pDC );
	if( HandleEraseBkgnd( pDC ) )
		return TRUE;
	return TRUE;
	return __super::OnEraseBkgnd( pDC );
}
