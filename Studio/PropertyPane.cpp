// PropertyPane.cpp : implementation file
//

#include "stdafx.h"
#include "PropertyPane.h"
#include "Workspace.h"
#include "Resource.h"
#include "DclControlTemplate.h"
#include "ControlName.h"
#include "StudioDialogControl.h"
#include "ControlTypes.h"
#include "PropertyIds.h"
#include "StudioWorkspace.h"
#include "StudioFrame.h"

#define ID_TABCTRL 109


/////////////////////////////////////////////////////////////////////////////
// CPropertyPane

CPropertyPane::CPropertyPane()
: m_PropertiesTabPane( mControls )
{
}

CPropertyPane::~CPropertyPane()
{
}


BEGIN_MESSAGE_MAP(CPropertyPane, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, ID_TABCTRL, OnSelchange)	
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPropertyPane message handlers

int CPropertyPane::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | TCS_FOCUSNEVER | TCS_TOOLTIPS	| TCS_TABS | WS_GROUP |
									/*WS_CLIPSIBLINGS	| */WS_CLIPCHILDREN | TCS_MULTILINE;

	if (!m_TabCtrl.Create(dwStyle, CRect(0,0,10,10), this, ID_TABCTRL))
		return -1;

	m_TabCtrl.DeleteAllItems();
	m_TabCtrl.InsertItem( 0, theWorkspace.LoadResourceString( IDS_PROPERTIES ) );
	m_TabCtrl.InsertItem( 1, theWorkspace.LoadResourceString( IDS_EVENTS ) );

	if( !m_font.CreateStockObject(DEFAULT_GUI_FONT) &&
			!m_font.CreatePointFont( 80, _T("MS Shell Dlg"), NULL ) )
		return -1;

	// Create the properties tab 
	m_PropertiesTabPane.Create(IDD_TABPAGE_PROPERTIES, &m_TabCtrl);

	// create the events tab
	m_EventsTabPane.Create(IDD_EVENTS, &m_TabCtrl);

	m_TabCtrl.SetFont(&m_font);
	m_EventsTabPane.SetFont(&m_font);
	m_PropertiesTabPane.SetFont(&m_font);
	m_PropertiesTabPane.GetPropertiesCtrl().SetFont( &m_font );
	m_PropertiesTabPane.ActivateProperty( NULL );
	m_EventsTabPane.ShowWindow( SW_HIDE );
	m_PropertiesTabPane.ShowWindow( SW_SHOW );
	m_PropertiesTabPane.RedrawWindow();
	return 0;
}

void CPropertyPane::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	CRect rc;
	GetClientRect(&rc);
	m_TabCtrl.MoveWindow(rc, FALSE);
	
	CRect rcTab;
	m_TabCtrl.GetClientRect(&rcTab);

	// get the tab control position
	CRect rcItem;
	
	int nTBottom = 0;
	
	for (int i=0; i < m_TabCtrl.GetItemCount(); i++)
	{
		// get the tab's position
		m_TabCtrl.GetItemRect(i, &rcItem);
		
		if (rcItem.bottom > nTBottom)
			nTBottom = rcItem.bottom;
	}
	
	// set the pane's position
	CRect rcNewRect(
		3,
		nTBottom + 3,
		rcTab.Width() - 6,
		rcTab.Height() - 3);

	m_EventsTabPane.MoveWindow(rcNewRect, FALSE);	
	m_PropertiesTabPane.MoveWindow(rcNewRect, FALSE);
	RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN );
}

void CPropertyPane::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int nCurrentSelectedTab = m_TabCtrl.GetCurSel();
	
	switch (nCurrentSelectedTab)
	{
	case 0:
		m_PropertiesTabPane.ShowWindow(SW_SHOW);
		m_EventsTabPane.ShowWindow(SW_HIDE);
		break;
	case 1:
		m_PropertiesTabPane.ShowWindow(SW_HIDE);
		m_EventsTabPane.ShowWindow(SW_SHOW);
		break;
	}
	*pResult = 0;
}

void CPropertyPane::OnActivateDclControl( TDclControlPtr pDclControl ) 
{
	if( !pDclControl )
	{
		if( !m_hWnd )
			return; //shutting down
		mControls.clear();
		m_PropertiesTabPane.GetPropertiesCtrl().OnActivateDclControl( NULL );
		m_PropertiesTabPane.ActivateProperty( NULL );
		m_EventsTabPane.ClearEvents();
		theStudioWorkspace.GetStudioFrame()->GetStatusBar().SetWindowText( _T("") );
		return;
	}
	bool bAlreadyActive = false;
	for( std::vector< TDclControlPtr >::const_iterator iterControl = mControls.begin();
		 iterControl != mControls.end();
		 ++iterControl )
	{
		if( (*iterControl) == pDclControl && iterControl != mControls.end() )
		{ //the control is already in the list
			bAlreadyActive = true;
			break;
		}
	}
	if( !bAlreadyActive )
		mControls.push_back( pDclControl );

	m_PropertiesTabPane.GetPropertiesCtrl().OnActivateDclControl( pDclControl );
	CString sStatusBarText;
	if( mControls.size() == 1 )
	{
		m_EventsTabPane.ClearEvents();
		m_EventsTabPane.UpdateEvents( pDclControl );
		m_EventsTabPane.EnableWindow( TRUE );
		CString sControlType = GetControlDisplayName(pDclControl);
		CString sName = pDclControl->GetVarName();
		if (!sControlType.IsEmpty())
			sStatusBarText.Format( _T("%s [%s]"), (LPCTSTR)sName, (LPCTSTR)sControlType );
		else
			sStatusBarText = sName;
	}
	else
	{
		//m_TabCtrl.SetCurSel( 0 );
		//m_TabCtrl.DeleteItem( 1 );
		//m_EventsTabPane.ShowWindow( SW_HIDE );
		//m_PropertiesTabPane.ShowWindow(TRUE);
		CString sFmt = theWorkspace.LoadResourceString( IDS_MULTISELECTIONSTATUS );
		if( !sFmt.IsEmpty() )
		{
			sStatusBarText.Format( sFmt, mControls.size() );
			m_EventsTabPane.ClearEvents( sStatusBarText );
		}
	}
	theStudioWorkspace.GetStudioFrame()->GetStatusBar().SetWindowText( sStatusBarText );
}

BOOL CPropertyPane::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST )
	{
		if( AfxGetMainWnd()->PreTranslateMessage( pMsg ) )
			return TRUE;
	}
	return CWnd::PreTranslateMessage(pMsg); //bypass CDialog
}

void CPropertyPane::OnDestroy() 
{
	CDialog::OnDestroy();
	
	m_font.DeleteObject();		
}
