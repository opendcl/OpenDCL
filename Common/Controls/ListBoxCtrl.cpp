// ListBoxCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "ListBoxCtrl.h"
#include "DclControlObject.h"
#include "ControlPane.h"
#include "PropertyObject.h"
#include "PropertyIds.h"


/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl

CListBoxCtrl::CListBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate /*= true*/ )
: CDialogControl( pTemplate, pPane, this )
{
	if( bCreate )
		Create( pPane->GetHostDialog(), nID );
}

CListBoxCtrl::~CListBoxCtrl()
{
}

bool CListBoxCtrl::Create( CWnd* pParentWnd, UINT nID ) 
{
	bool bSuccess = (__super::Create( GetWndStyle(), GetWndRect(), pParentWnd, nID ) != FALSE);

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;

	//// fix up 3D styles
	//ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
	//CListBox::ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
	//ModifyStyleEx(0, WS_VSCROLL);
	//CListBox::ModifyStyleEx(0, WS_VSCROLL);

	return bSuccess;
}

DWORD CListBoxCtrl::GetWndStyle() const
{
	DWORD dwStyle = CDialogControl::GetWndStyle();

	dwStyle |= (WS_CLIPSIBLINGS | LBS_HASSTRINGS | LBS_NOTIFY | LBS_DISABLENOSCROLL | LBS_USETABSTOPS);
	TPropertyPtr pPropNoIntegralHeight = mpTemplate->GetPropertyObject( Prop::NoIntegralHeight );
	if( !pPropNoIntegralHeight || pPropNoIntegralHeight->GetBooleanValue() )
		dwStyle |= LBS_NOINTEGRALHEIGHT;
	if( mpTemplate->GetBooleanProperty( Prop::MultiColumn ) )
		dwStyle |= LBS_MULTICOLUMN;
	if( mpTemplate->GetBooleanProperty( Prop::Sorted ) )
		dwStyle |= LBS_SORT;
	switch( mpTemplate->GetLongProperty( Prop::SelectStyle ) )
	{
	case 1:
		dwStyle |= LBS_EXTENDEDSEL;
		break;
	case 2:
		dwStyle |= LBS_MULTIPLESEL;
		break;
	}
	return dwStyle;
}

bool CListBoxCtrl::OnApplyProperty( TPropertyPtr pProp )
{
	if( !__super::OnApplyProperty( pProp ) )
		return false;
	bool bFailed = false;
	switch( pProp->GetID() )
	{
	case Prop::Sorted:
		{
			if( pProp->GetBooleanValue() )
				ModifyStyle( 0, LBS_SORT, 0 );
			else
				ModifyStyle( LBS_SORT, 0, 0 );
			Invalidate();
		}
		break;
	case Prop::ColumnWidth:
		{
			SetColumnWidth( pProp->GetLongValue() );
		}
		break;
	case Prop::DisableNoScroll:
		{
			if( pProp->GetBooleanValue() )
				ModifyStyle( 0, LBS_DISABLENOSCROLL, SWP_FRAMECHANGED );
			else
				ModifyStyle( LBS_DISABLENOSCROLL, 0, SWP_FRAMECHANGED );
		}
		break;
	case Prop::List:
		{
			ResetContent();
			for (size_t idx = 0; idx < pProp->size(); idx++)
				AddString( pProp->GetStringItem( idx ) );
			Invalidate();
		}
		break;
	}
	return !bFailed;
}


BEGIN_MESSAGE_MAP(CListBoxCtrl, CListBox)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl message handlers

HBRUSH CListBoxCtrl::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	if( !IsWindowEnabled() )
		return NULL;
	pDC->SetBkMode( TRANSPARENT );	
	pDC->SetTextColor( mAcadColorService.GetForegroundColor() );
	return mAcadColorService.GetBackgroundBrush();	
}

void CListBoxCtrl::PostNcDestroy() 
{
	__super::PostNcDestroy();
	delete this;
}
