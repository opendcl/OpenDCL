// StudioDialogObject.cpp : implementation of the CStudioDialogObject class
//

#include "stdafx.h"
#include "StudioDialogObject.h"
#include "OpenDCL.h"
#include "OpenDCLDoc.h"
#include "DclFormView.h"
#include "StudioWorkspace.h"
#include "ControlManager.h"
#include "StudioDialogControl.h"
#include "TabStripCtrl.h"
#include "ProjectPane.h"
#include "PropertyPane.h"
#include "ToolboxPane.h"
#include "ZOrderPane.h"
#include "AxContainerCtrl.h"
#include "AxInterfaceDescriptor.h"
#include "OLEFont.h"
#include "PictureObject.h"
#include "ControlName.h"
#include "FormControlManager.h"
#include "PropertyWizard.h"
#include "ControlBrowser.h"
#include "FormVarNameUpdate.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//private clipboard formats
#define CF_OPENDCLCONTROL CF_PRIVATEFIRST


// CStudioDialogObject

BEGIN_MESSAGE_MAP(CStudioDialogObject, CDialog)
	ON_COMMAND(ID_SELECTPOINTERTOOL, &CStudioDialogObject::OnSelectPointerTool)
	ON_UPDATE_COMMAND_UI(ID_SELECTPOINTERTOOL, &CStudioDialogObject::OnUpdateSelectPointerTool)
	ON_COMMAND(ID_SHIFTLEFT, &CStudioDialogObject::OnShiftLeft)
	ON_UPDATE_COMMAND_UI(ID_SHIFTLEFT, &CStudioDialogObject::OnUpdateShiftLeft)
	ON_COMMAND(ID_SHIFTRIGHT, &CStudioDialogObject::OnShiftRight)
	ON_UPDATE_COMMAND_UI(ID_SHIFTRIGHT, &CStudioDialogObject::OnUpdateShiftRight)
	ON_COMMAND(ID_SHIFTUP, &CStudioDialogObject::OnShiftUp)
	ON_UPDATE_COMMAND_UI(ID_SHIFTUP, &CStudioDialogObject::OnUpdateShiftUp)
	ON_COMMAND(ID_SHIFTDOWN, &CStudioDialogObject::OnShiftDown)
	ON_UPDATE_COMMAND_UI(ID_SHIFTDOWN, &CStudioDialogObject::OnUpdateShiftDown)
	ON_COMMAND(ID_EDIT_CUT, &CStudioDialogObject::OnEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, &CStudioDialogObject::OnUpdateEditCut)
	ON_COMMAND(ID_EDIT_DELETE, &CStudioDialogObject::OnEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, &CStudioDialogObject::OnUpdateEditDelete)
	ON_COMMAND(ID_EDIT_PASTE, &CStudioDialogObject::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CStudioDialogObject::OnUpdateEditPaste)
	ON_COMMAND(ID_EDIT_COPY, &CStudioDialogObject::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CStudioDialogObject::OnUpdateEditCopy)
	ON_COMMAND(ID_PROPERTIES, &CStudioDialogObject::OnProperties)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, &CStudioDialogObject::OnUpdateProperties)
	ON_COMMAND(ID_AXPROPERTIES, &CStudioDialogObject::OnAxProperties)
	ON_UPDATE_COMMAND_UI(ID_AXPROPERTIES, &CStudioDialogObject::OnUpdateAxProperties)
	ON_COMMAND(ID_FONTPROPERTIES, &CStudioDialogObject::OnFontProperties)
	ON_UPDATE_COMMAND_UI(ID_FONTPROPERTIES, &CStudioDialogObject::OnUpdateProperties)
	ON_COMMAND(ID_TOOLTIPPROPERTIES, &CStudioDialogObject::OnTooltipProperties)
	ON_UPDATE_COMMAND_UI(ID_TOOLTIPPROPERTIES, &CStudioDialogObject::OnUpdateProperties)
	ON_COMMAND(ID_FORECOLORPROPERTIES, &CStudioDialogObject::OnForeColorProperties)
	ON_UPDATE_COMMAND_UI(ID_FORECOLORPROPERTIES, &CStudioDialogObject::OnUpdateProperties)
	ON_COMMAND(ID_BACKCOLORPROPERTIES, &CStudioDialogObject::OnBackColorProperties)
	ON_UPDATE_COMMAND_UI(ID_BACKCOLORPROPERTIES, &CStudioDialogObject::OnUpdateProperties)
	ON_COMMAND(ID_ALTCOLORPROPERTIES, &CStudioDialogObject::OnAltColorProperties)
	ON_UPDATE_COMMAND_UI(ID_ALTCOLORPROPERTIES, &CStudioDialogObject::OnUpdateProperties)
	ON_COMMAND(ID_IMAGELISTPROPERTIES, &CStudioDialogObject::OnImageListProperties)
	ON_UPDATE_COMMAND_UI(ID_IMAGELISTPROPERTIES, &CStudioDialogObject::OnUpdateProperties)
	ON_COMMAND(ID_CONTROLBROWSER, &CStudioDialogObject::OnEditObjectbrowser)
	ON_UPDATE_COMMAND_UI(ID_CONTROLBROWSER, &CStudioDialogObject::OnUpdateEditObjectbrowser)
	ON_COMMAND(ID_TOOLS_SETLISPSYMBOLNAMES, &CStudioDialogObject::OnToolsSetlispsymbolnames)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_SETLISPSYMBOLNAMES, &CStudioDialogObject::OnUpdateToolsSetlispsymbolnames)
	ON_COMMAND(ID_TOOLS_CLEARLISPSYMBOLNAMES, &CStudioDialogObject::OnToolsClearlispsymbolnames)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_CLEARLISPSYMBOLNAMES, &CStudioDialogObject::OnUpdateToolsClearlispsymbolnames)
	ON_COMMAND(ID_SENDTOFRONT, &CStudioDialogObject::OnBringtofront)
	ON_UPDATE_COMMAND_UI(ID_SENDTOFRONT, &CStudioDialogObject::OnUpdateBringtofront)
	ON_COMMAND(ID_SENDTOBACK, &CStudioDialogObject::OnSendtoback)
	ON_UPDATE_COMMAND_UI(ID_SENDTOBACK, &CStudioDialogObject::OnUpdateSendtoback)
	ON_WM_DESTROY()	
	ON_WM_SIZE()	
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	ON_WM_SIZING()
	ON_WM_NCHITTEST()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_NCRBUTTONUP()
	ON_WM_GETDLGCODE()
	ON_WM_CAPTURECHANGED()
END_MESSAGE_MAP()

// CStudioDialogObject construction/destruction

CStudioDialogObject::CStudioDialogObject( TDclFormPtr pSourceForm, CDclFormView* pFormView, bool bCreate /*= true*/ )
: CDialogObject( pSourceForm, &mControlPane, this )
, mpFormView( pFormView )
, mColorService( long( -16 ) /*COLOR_BTNFACE*/, long( -24 ) /*transparent*/ )
, mControlPane( pSourceForm, this )
, mbDrawing( false )
, mbMoving( false )
, mbSizing( false )
, mbMouseDown( false )
, mnDragQuadrant( -1 )
, mptDragStart( 0, 0 )
, mrcDraw( 0, 0, 0, 0 )
, mrcDrawLast( 0, 0, 0, 0 )
, mnGridSpacing( 0 )
{
	assert( mpSourceForm != NULL );
	mnGridSpacing = theApp.GetGridSpacing();
	if( bCreate )
		Create( pFormView, IDD_STUDIODIALOGOBJECT );
}

CStudioDialogObject::~CStudioDialogObject()
{
}

TStudioProjectPtr CStudioDialogObject::GetStudioProject() const
{
	return TStudioProjectLockedPtr( (CStudioProject*)(CProject*)GetProject() );
}

bool CStudioDialogObject::IsModeless() const
{
	switch( mpSourceForm->GetType() )
	{
	case FrmModalDlg:
	case FrmFileDlg:
		return false;
	};
	return true;
}

bool CStudioDialogObject::IsResizable() const
{
	return mpTemplate->GetBooleanProperty( Prop::AllowResizing );
}

void CStudioDialogObject::CloseDialog(int nStatus /*= -1*/)
{
	if( mpFormView )
		mpFormView->GetParentFrame()->DestroyWindow();
	else if( m_hWnd )
		DestroyWindow();
}

FormType CStudioDialogObject::GetType() const
{
	return mpSourceForm->GetType();
}

bool CStudioDialogObject::CreateModeless( UINT nID )
{
	return Create( mpFormView, nID );
}

DWORD CStudioDialogObject::GetWndStyle() const
{
	DWORD dwStyle = __super::GetWndStyle();

	if( GetType() == FrmFileDlg )
		dwStyle |= WS_CAPTION;

	return dwStyle;
}

bool CStudioDialogObject::Create( CWnd* pParentWnd, UINT nID )
{
	mbIgnoreSizing = true;
	bool bSuccess = (CreateDlg( MAKEINTRESOURCE(nID), pParentWnd ) != FALSE);
	SetControlManager( new CFormControlManager( this ) );
	mbIgnoreSizing = false;

	if( bSuccess )
		SetWindowText( GetWndCaption() );

	if( bSuccess && !ApplyPropertiesEnum() )
		bSuccess = false;
	if( bSuccess )
		ShowWindow( SW_SHOW );

	return bSuccess;
}

bool CStudioDialogObject::OnApplyName( TPropertyPtr pProp )
{
	if( mpSourceForm->m_htiTreeItem )
		theStudioWorkspace.GetProjectPane()->GetTreeCtrl().SetItemText( mpSourceForm->m_htiTreeItem, mpSourceForm->GetKeyName() );
	return true;
}

TDialogControlPtr CStudioDialogObject::CreateNewDialogControl( TDclControlPtr pTemplate, UINT nID )
{
	return CStudioDialogControl::Create( pTemplate, &mControlPane, nID );
}

CPoint& CStudioDialogObject::SnapToGrid( CPoint& pt, bool bLimitToControlArea /*= false*/ ) const
{
	if( mnGridSpacing > 1 )
	{
		LONG X = pt.x + (mnGridSpacing / 2);
		LONG Y = pt.y + (mnGridSpacing / 2);
		pt.x = X - (X % mnGridSpacing);
		pt.y = Y - (Y % mnGridSpacing);
	}
	if( bLimitToControlArea )
	{
		CRect rcControlArea = GetWndRect();
		if( pt.x < 0 )
			pt.x = 0;
		else if( pt.x > rcControlArea.right )
			pt.x = rcControlArea.right;
		if( pt.y < 0 )
			pt.y = 0;
		else if( pt.y > rcControlArea.bottom )
			pt.y = rcControlArea.bottom;
	}
	return pt;
}

void CStudioDialogObject::OnFontChange( const FontSettings& FS, UINT flags /*= fontAll*/ )
{
	if( !FS )
		return;
	CUndoManager* pUndoManager = mpSourceForm->GetUndoManager();
	AutoUndoGroup UndoGroup( pUndoManager, IDS_UNDO_CHANGEFONT );
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		CDialogControl* pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl || pDlgControl == this )
			continue;
		CControlManager* pManager = pDlgControl->GetControlManager();
		if( !pManager )
			continue;
		if( pManager->IsSelected() )
		{
			if( pUndoManager )
				pUndoManager->SelectControl( pDclControl );
			pDclControl->SetFontProperties( FS, flags );
			CStudioDialogControl::UpdateProperty( pDclControl, Prop::FontName );
		}
	}
}

void CStudioDialogObject::OnGridSpacingChange( UINT nGridSpacing )
{
	mnGridSpacing = nGridSpacing;
	Invalidate();
}

int CStudioDialogObject::GetNextControlId()
{
	int nHighest = 10;
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pCtrl = *iter;
		if (pCtrl->GetID() > nHighest)
			nHighest = pCtrl->GetID();
	}
	return nHighest + 1;
}

CString CStudioDialogObject::GetNextControlName( LPCTSTR pszRootName )
{
	if( !mpSourceForm )
		return CString();
	TDclFormPtr pRoot = mpSourceForm;
	while( pRoot->GetParentForm() )
		pRoot = pRoot->GetParentForm();
	CString sName;
	UINT nID = 1;
	do
		sName.Format( _T("%s%u"), pszRootName, nID++ );
	while( pRoot->FindControl( sName ) );
	return sName;
}

CDialogControl* CStudioDialogObject::GetControlAtPoint( const CPoint& pt )
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_reverse_iterator iter = Controls.rbegin();
			 iter != Controls.rend();
			 ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		CDialogControl* pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl )
			continue;
		if( pDlgControl == this )
			continue;
		CControlManager* pManager = pDlgControl->GetControlManager();
		if( pManager && pManager->HitTest( pt ) > HTNOWHERE )
			return pDlgControl;
	}
	return NULL;
}

TDclControlPtr CStudioDialogObject::InsertControl( ControlType type, const CRect& rcControl, bool bActivateNow /*= true*/ )
{
	AutoUndoGroup UndoGroup( mpSourceForm->GetUndoManager(), IDS_UNDO_ADDCONTROL );
	TDclControlPtr pDclControl = NULL;
	switch( type )
	{
	case CtlFileExplorer:
		pDclControl = mpSourceForm->AddControl( CtlFileExplorer, GetControlSimpleName( CtlFileExplorer ), rcControl );
		break;
	case CtlActiveX:
		{
			CLSID clsid;
			CString sLicenseKey;
			CString sFilename;
			theStudioWorkspace.GetToolboxPane()->GetActiveXControlInfo( clsid, sLicenseKey, sFilename );
			pDclControl = mpSourceForm->AddControl( CtlActiveX, GetNextControlName( GetControlSimpleName( clsid ) ), rcControl );
			if( pDclControl )
				pDclControl->SetAxCtrlInfo( clsid, sLicenseKey );
		}
		break;
	case CtlTabStrip:
		assert( mpSourceForm->GetType() != FrmTabPage ); //can't nest tab pages
		if( mpSourceForm->GetType() == FrmTabPage )
			return NULL;
		pDclControl = mpSourceForm->AddControl( type, GetNextControlName( GetControlSimpleName( CtlTabStrip ) ), rcControl );
		if( pDclControl )
		{
			pDclControl->GetPropertyObject( Prop::TabsCaption )->AddStringItem( theWorkspace.LoadResourceString( IDS_TAB1 ) );
			pDclControl->GetPropertyObject( Prop::TabsCaption )->AddStringItem( theWorkspace.LoadResourceString( IDS_TAB2 ) );
			pDclControl->GetPropertyObject( Prop::TabsCaption )->AddStringItem( theWorkspace.LoadResourceString( IDS_TAB3 ) );
			TDclFormPtr pTabPage1 = mpSourceForm->AddChildForm( FrmTabPage );
			pTabPage1->SetTabIndex( 0 );
			theStudioWorkspace.GetProjectPane()->AddFormToTree( pTabPage1, false );
			TDclFormPtr pTabPage2 = mpSourceForm->AddChildForm( FrmTabPage );
			pTabPage2->SetTabIndex( 1 );
			theStudioWorkspace.GetProjectPane()->AddFormToTree( pTabPage2, false );
			TDclFormPtr pTabPage3 = mpSourceForm->AddChildForm( FrmTabPage );
			pTabPage3->SetTabIndex( 2 );
			theStudioWorkspace.GetProjectPane()->AddFormToTree( pTabPage3, false );
			theStudioWorkspace.GetProjectPane()->Invalidate();
		}
		break;
	default:
		pDclControl = mpSourceForm->AddControl( type, GetNextControlName( GetControlSimpleName( type ) ), rcControl );
		break;
	}
	assert( pDclControl != NULL );
	if( pDclControl )
	{
		pDclControl->SetLongProperty( Prop::Left, rcControl.left );
		pDclControl->SetLongProperty( Prop::Top, rcControl.top );
		pDclControl->SetLongProperty( Prop::Width, rcControl.Width() );
		pDclControl->SetLongProperty( Prop::Height, rcControl.Height() );
		TDialogControlPtr pDlgControl = CreateNewDialogControl( pDclControl, GetNextControlId() );
		if( pDlgControl )
		{
			mpControlPane->AddControl( pDlgControl );
			if( bActivateNow )
				theStudioWorkspace.ActivateDclControl( pDclControl, true );
			CControlManager* pManager = pDlgControl->GetControlManager();
			if( pManager )
				pManager->Invalidate();
		}
		else
			mpSourceForm->DeleteControl( pDclControl );
	}
	return pDclControl;
}	

bool CStudioDialogObject::PromptRemoveChildTabPane( int nIndex )
{
	if( nIndex < 0 )
		return true;
	TDclFormPtr pTabForm = mpSourceForm->GetProject()->FindDclTabChildForm(mpSourceForm->GetUniqueName(), nIndex);
	if( !pTabForm || pTabForm->GetControlList().size() <= 1 )
		return true;
	if( MessageBox( theWorkspace.LoadResourceString(IDS_QDELETEFORM),
									theWorkspace.GetAppKey(),
									MB_ICONEXCLAMATION | MB_YESNO) == 6 )
		return true;
	return false;
}

bool CStudioDialogObject::IsOnlyFormActive() const
{
	bool bFormSelected = false;
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_reverse_iterator iter = Controls.rbegin();
			 iter != Controls.rend();
			 ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		CDialogControl* pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl )
			continue;
		CControlManager* pManager = pDlgControl->GetControlManager();
		if( !pManager )
			continue;
		if( pManager->IsSelected() )
		{
			if( pDlgControl == this )
				bFormSelected = true;
			else
				return false;
		}
	}
	return bFormSelected;
}

bool CStudioDialogObject::IsControlSelected( TDclControlPtr pDclControl ) const
{
	CDialogControl* pDlgControl = pDclControl->GetControlInstance();
	if( !pDlgControl )
		return false;
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( !pManager )
		return false;
	return pManager->IsSelected();
}

void CStudioDialogObject::GetSelectedControls( TDclControlList& SelectedControls, bool bIgnoreForm /*= true*/ )
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		CDialogControl* pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl || (bIgnoreForm && pDlgControl == this) )
			continue;
		CControlManager* pManager = pDlgControl->GetControlManager();
		if( !pManager )
			continue;
		if( !pManager->IsSelected() )
			continue;
		SelectedControls.push_back( pDclControl );
	}
}

size_t CStudioDialogObject::CountSelected( bool bIgnoreForm /*= true*/ ) const
{
	size_t ctSelected = 0;
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		CDialogControl* pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl || (bIgnoreForm && pDlgControl == this) )
			continue;
		CControlManager* pManager = pDlgControl->GetControlManager();
		if( !pManager )
			continue;
		if( !pManager->IsSelected() )
			continue;
		++ctSelected;
	}
	return ctSelected;
}

void CStudioDialogObject::SelectControl( TDclControlPtr pDclControl ) 
{
	if( !pDclControl )
	{
		const TDclControlList& Controls = mpSourceForm->GetControlList();
		for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
		{
			TDclControlPtr pDclControl = *iter;
			CDialogControl* pDlgControl = pDclControl->GetControlInstance();
			if( !pDlgControl )
				continue;
			CControlManager* pManager = pDlgControl->GetControlManager();
			if( !pManager )
				continue;
			pManager->SetSelected( false );
		}
		return;
	}
	CDialogControl* pDlgControl = pDclControl->GetControlInstance();
	CControlManager* pControlMgr = pDlgControl? pDlgControl->GetControlManager() : NULL;
	if( pControlMgr )
		pControlMgr->SetSelected();			
}

void CStudioDialogObject::ShiftSelection( long lHoriz, long lVert )
{
	TDclControlList SelectedControls;
	GetSelectedControls( SelectedControls );
	if( SelectedControls.empty() )
		return;
	AutoUndoGroup UndoGroup( mpSourceForm->GetUndoManager(), SelectedControls.size() > 1? IDS_UNDO_MOVECONTROLS : IDS_UNDO_MOVECONTROL );
	for( TDclControlList::const_reverse_iterator iter = SelectedControls.rbegin();
			 iter != SelectedControls.rend();
			 ++iter )
	{
		TDclControlPtr pDclControl = (*iter);
		CRect rcControl = pDclControl->GetWndRect();
		rcControl.OffsetRect( lHoriz, lVert );
		pDclControl->SetLongProperty( Prop::Left, rcControl.left );
		pDclControl->SetLongProperty( Prop::Top, rcControl.top );
		pDclControl->SetLongProperty( Prop::Width, rcControl.Width() );
		pDclControl->SetLongProperty( Prop::Height, rcControl.Height() );
		CStudioDialogControl::UpdateProperty( pDclControl, Prop::Left );
		CStudioDialogControl::UpdateProperty( pDclControl, Prop::Top );
		CStudioDialogControl::UpdateProperty( pDclControl, Prop::Width );
		CStudioDialogControl::UpdateProperty( pDclControl, Prop::Height );
		theStudioWorkspace.ActivateDclControl( pDclControl );
		//pManager->RedrawWindow( NULL, NULL, RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW );
	}
}

bool CStudioDialogObject::HasSelection() const
{
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	for( TDclControlList::const_reverse_iterator iter = Controls.rbegin(); iter != Controls.rend(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		CDialogControl* pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl )
			continue;
		if( pDlgControl == this )
			continue; //ignore the form
		CControlManager* pManager = pDlgControl->GetControlManager();
		if( !pManager )
			continue;
		if( pManager->IsSelected() )
			return true;
	}
	return false;
}

void CStudioDialogObject::OnControlDeleted( TDclControlPtr pDclControl )
{
	if( pDclControl->GetType() == CtlTabStrip )
	{ //delete the children
		theStudioWorkspace.GetProjectPane()->RemoveChildren( mpSourceForm->m_htiTreeItem );
		const TDclFormList& Forms = mpSourceForm->GetProject()->GetDclFormList();
		for( TDclFormList::const_reverse_iterator iter = Forms.rbegin(); iter != Forms.rend(); ++iter )
		{
			TDclFormPtr pChildForm = (*iter);
			if( pChildForm->GetParentForm() == mpSourceForm )
				mpSourceForm->GetProject()->DeleteForm( pChildForm );
		}
	}
	mpControlPane->RemoveControl( pDclControl->GetControlInstance() );
}

UINT CStudioDialogObject::HitTest( const CPoint& point ) const
{
	TPropertyPtr pWidthProp = mpTemplate->GetPropertyObject( Prop::Width );
	if( !pWidthProp || pWidthProp->IsHidden() )
		return HTBORDER; //form doesn't support resizing
	UINT nHitTest = (UINT)const_cast< CStudioDialogObject* >( this )->__super::OnNcHitTest(point);
	switch( nHitTest )
	{
	case HTCAPTION:
	case HTTOP:
	case HTTOPLEFT:
	case HTLEFT:
		return HTBORDER;
	case HTTOPRIGHT:
		return HTRIGHT;
	case HTBOTTOMLEFT:
		return HTBOTTOM;
	case HTCLIENT:
	case HTBORDER:
		{
			CRect rcDlg;
			GetClientRect( &rcDlg );
			GetParent()->ClientToScreen( &rcDlg );
			bool bBottom = (rcDlg.bottom - point.y <= 4); //bottom?
			if( rcDlg.right - point.x <= 4 ) //right?
			{
				if( bBottom )
					return HTBOTTOMRIGHT;
				return HTRIGHT;
			}
			if( bBottom )
				return HTBOTTOM;
		}
		break;
	}
	return nHitTest;
}

void CStudioDialogObject::ZOrderSelectedControls( bool bToFront ) 
{
	TDclControlList SelectedControls;
	GetSelectedControls( SelectedControls );
	if( SelectedControls.empty() )
		return;
	AutoUndoGroup UndoGroup( mpSourceForm->GetUndoManager(), IDS_UNDO_ZORDER );
	for( TDclControlList::const_reverse_iterator iter = SelectedControls.rbegin();
			 iter != SelectedControls.rend();
			 ++iter )
		mpSourceForm->ReorderControl( (*iter), bToFront );
	theStudioWorkspace.ActivateDlgObject( this );
}


// CStudioDialogObject message handlers

BOOL CStudioDialogObject::OnInitDialog()
{
	if( IsModeless() )
		ModifyStyleEx( 0, WS_EX_TOOLWINDOW );
	//ModifyStyleEx( 0, 0x02000000L/*WS_EX_COMPOSITED*/ | 0x08000000L/*WS_EX_NOACTIVATE*/ );

	DWORD_PTR dwStyle = GetWndStyle();
	SetWindowLongPtr( m_hWnd, GWL_STYLE, dwStyle );
	SetWindowPos( NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSENDCHANGING );
	OnFrameChanged();

	CRect rectWindow;
	rectWindow = GetWndRect();
	rectWindow.right += GetNCWidth();
	rectWindow.bottom += GetNCHeight();

	__super::OnInitDialog();

	rectWindow.MoveToX( 0 );
	rectWindow.MoveToY( 0 );
	MoveWindow( &rectWindow, FALSE );
	mbIgnoreSizing = false;

	//create the control pane and the design time controls
	CUndoManager* pUndoManager = mpSourceForm->GetUndoManager();
	if( pUndoManager )
		pUndoManager->BeginGroup( theWorkspace.LoadResourceString( IDS_UNDO_SIZEFORM ) );
	size_t ctActions = pUndoManager? pUndoManager->size() : 0;
	UINT nID = 1000;
	GetControlPane()->CreateControls( nID );
	if( mpSourceForm->GetType() == FrmFileDlg )
	{
		TDclControlPtr pFileDlgCtrl = mpSourceForm->FindFirstControlOfType( CtlFileExplorer );
		if( pFileDlgCtrl )
		{
			TDialogControlPtr pCtrlObj = CreateNewDialogControl( pFileDlgCtrl, GetNextControlId() );
			mpControlPane->AddControl( pCtrlObj );
		}
		else
		{
			CRect rcFileCtrl( 0, 0, 556, 386 );
			InsertControl( CtlFileExplorer, rcFileCtrl, false );
		}
	}
	if( pUndoManager )
	{
		bool bNoChanges = (pUndoManager->size() == ctActions);
		pUndoManager->EndGroup();
		if( bNoChanges )
			pUndoManager->Undo();
	}
	GetControlPane()->RecalcLayout();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CStudioDialogObject::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST )
	{
		if( AfxGetMainWnd()->PreTranslateMessage( pMsg ) )
			return TRUE;
	}
	return CWnd::PreTranslateMessage(pMsg); //bypass CDialog
}

void CStudioDialogObject::OnDestroy() 
{
	GetControlPane()->CleanUpControls();	
	__super::OnDestroy();
}

void CStudioDialogObject::OnSelectPointerTool() 
{
	theStudioWorkspace.GetToolboxPane()->ResetToPointer();
}

void CStudioDialogObject::OnUpdateSelectPointerTool(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable( !theStudioWorkspace.GetToolboxPane()->IsPointer() );
}

void CStudioDialogObject::OnShiftLeft() 
{
	ShiftSelection( -1, 0 );
}

void CStudioDialogObject::OnUpdateShiftLeft(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable( HasSelection() );
}

void CStudioDialogObject::OnShiftRight() 
{
	ShiftSelection( 1, 0 );
}

void CStudioDialogObject::OnUpdateShiftRight(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable( HasSelection() );
}

void CStudioDialogObject::OnShiftUp() 
{
	ShiftSelection( 0, -1 );
}

void CStudioDialogObject::OnUpdateShiftUp(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable( HasSelection() );
}

void CStudioDialogObject::OnShiftDown() 
{
	ShiftSelection( 0, 1 );
}

void CStudioDialogObject::OnUpdateShiftDown(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable( HasSelection() );
}

void CStudioDialogObject::OnEditCut() 
{
	OnEditCopy();
	OnEditDelete();
}

void CStudioDialogObject::OnUpdateEditCut(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable( HasSelection() );
}

void CStudioDialogObject::OnEditDelete() 
{
	TDclControlList SelectedControls;
	GetSelectedControls( SelectedControls );
	if( SelectedControls.empty() )
		return;
	AutoUndoGroup UndoGroup( mpSourceForm->GetUndoManager(), SelectedControls.size() > 1? IDS_UNDO_DELETECONTROLS : IDS_UNDO_DELETECONTROL );
	for( TDclControlList::const_reverse_iterator iter = SelectedControls.rbegin();
			 iter != SelectedControls.rend();
			 ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		CDialogControl* pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl )
			continue;
		CControlManager* pManager = pDlgControl->GetControlManager();
		if( !pManager )
			continue;
		OnControlDeleted( pDclControl );
		pManager->DestroyWindow();
		mpSourceForm->DeleteControl( pDclControl );
	}
	theStudioWorkspace.ActivateDclControl( mpSourceForm->GetControlProperties() );
	theStudioWorkspace.ActivateDlgObject( this );
}

void CStudioDialogObject::OnUpdateEditDelete(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable( HasSelection() );
}

void CStudioDialogObject::OnEditPaste() 
{
	if( !OpenClipboard() )
		return;
	theStudioWorkspace.ActivateDclControl( NULL );
	HANDLE hgMem = GetClipboardData( CF_OPENDCLCONTROL );
	if( hgMem )
	{
		LPBYTE pbData = (LPBYTE)GlobalLock( hgMem );
		CMemFile ClipboardMem( 4096 );
		ClipboardMem.SeekToBegin();
		SIZE_T cbClipboard = *(SIZE_T*)pbData;
		pbData += sizeof(cbClipboard);
		size_t ctControls = *(size_t*)pbData;
		if( ctControls > 0 )
		{
			TDclControlList mNewControls;
			pbData += sizeof(ctControls);
			size_t cbControlData = cbClipboard - sizeof(cbClipboard) - sizeof(ctControls);
			if( cbControlData > 0 )
				ClipboardMem.Write( pbData, cbControlData );
			ClipboardMem.SeekToBegin();
			CArchive ClipboardArchive( &ClipboardMem, CArchive::load );
			AutoUndoGroup UndoGroup( mpSourceForm->GetUndoManager(), ctControls > 1? IDS_UNDO_ADDCONTROLS : IDS_UNDO_ADDCONTROL );
			while( ctControls-- > 0 )
			{
				TDclControlPtr pDclControl = new CDclControlObject( mpSourceForm );
				pDclControl->Serialize( ClipboardArchive );
				size_t ctChildren = 0;
				ClipboardArchive >> ctChildren;
				//std::vector< TDclFormPtr > ChildForms;
				//while( ctChildren-- > 0 )
				//{
				//	TDclFormPtr pChildForm = new CDclFormObject( mpSourceForm->GetProject() );
				//	pChildForm->Serialize( ClipboardArchive );
				//	ChildForms.push_back( pChildForm );
				//}
				bool bDuplicate = (mpSourceForm->FindControl( pDclControl->GetKeyName() ) != NULL);
				switch( pDclControl->GetType() )
				{
				case CtlFileExplorer:
					if( bDuplicate )
					{
						pDclControl = NULL; //can't add a second file dialog control!
						MessageBeep( MB_ICONERROR );
					}
					break;
				case CtlTabStrip:
					if( bDuplicate )
					{
						pDclControl = NULL; //can't add a second tab strip control!
						MessageBox( theWorkspace.LoadResourceString(IDS_NOMORETABS), theWorkspace.GetAppKey(), MB_ICONERROR );
					}
					else if( mpSourceForm->GetType() == FrmTabPage )
					{
						pDclControl = NULL; //can't add a second tab strip control!
						MessageBox( theWorkspace.LoadResourceString(IDS_NOTABWITHINTAB), theWorkspace.GetAppKey(), MB_ICONERROR );
					}
					break;
				}
				if( bDuplicate )
				{
					// if the caption property matches the name, update both
					CString sCaption = pDclControl->GetStringProperty( Prop::Caption );
					CString sOldName = pDclControl->GetStringProperty( Prop::Name );
					CString sNewName = GetNextControlName( GetControlSimpleName( pDclControl->GetType() ) );
					pDclControl->SetStringProperty( Prop::Name, sNewName );
					if( sOldName == sCaption )
						pDclControl->SetStringProperty( Prop::Caption, sNewName );
				}
				mpSourceForm->AddControl( pDclControl, true );
				mNewControls.push_back( pDclControl );
			}
			for( TDclControlList::iterator iter = mNewControls.begin(); iter != mNewControls.end(); ++iter )
			{
				TDclControlPtr pDclControl = *iter;
				//fix up controls that are positioned relative to a non-existent splitter
				long lSplitter = pDclControl->GetLongProperty( Prop::UseTopFromBottom );
				if( lSplitter > 2 && !mpControlPane->HasSplitter( lSplitter ) )
					pDclControl->SetLongProperty( Prop::UseTopFromBottom, 0 );
				lSplitter = pDclControl->GetLongProperty( Prop::UseBottomFromBottom );
				if( lSplitter > 2 && !mpControlPane->HasSplitter( lSplitter ) )
					pDclControl->SetLongProperty( Prop::UseBottomFromBottom, 0 );
				lSplitter = pDclControl->GetLongProperty( Prop::UseLeftFromRight );
				if( lSplitter > 2 && !mpControlPane->HasSplitter( lSplitter ) )
					pDclControl->SetLongProperty( Prop::UseLeftFromRight, 0 );
				lSplitter = pDclControl->GetLongProperty( Prop::UseRightFromRight );
				if( lSplitter > 2 && !mpControlPane->HasSplitter( lSplitter ) )
					pDclControl->SetLongProperty( Prop::UseRightFromRight, 0 );
				TDialogControlPtr pDlgControl = CreateNewDialogControl( pDclControl, GetNextControlId() );
				if( pDlgControl )
				{
					mpControlPane->AddControl( pDlgControl );
					theStudioWorkspace.ActivateDclControl( pDclControl );
				}
				else
					mpSourceForm->DeleteControl( pDclControl );
			}
		}
	}
	CloseClipboard();
}

void CStudioDialogObject::OnUpdateEditPaste(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable( IsClipboardFormatAvailable( CF_OPENDCLCONTROL ) );
}

void CStudioDialogObject::OnEditCopy() 
{
	if( !OpenClipboard() )
		return;
	if( !EmptyClipboard() )
		return;
	DisableUndoManager DisableUndo( mpSourceForm->GetUndoManager() );
	const TDclControlList& Controls = mpSourceForm->GetControlList();
	CMemFile ClipboardMem( 4096 );
	CArchive ClipboardArchive( &ClipboardMem, CArchive::store );
	size_t ctControls = 0;
	for( TDclControlList::const_iterator iter = Controls.begin(); iter != Controls.end(); ++iter )
	{
		TDclControlPtr pDclControl = *iter;
		if( pDclControl->GetType() == _CtlForm )
			continue;
		CDialogControl* pDlgControl = pDclControl->GetControlInstance();
		if( !pDlgControl )
			continue;
		CControlManager* pManager = pDlgControl->GetControlManager();
		if( !pManager )
			continue;
		if( pManager->IsSelected() )
		{
			pDclControl->Serialize( ClipboardArchive );
			++ctControls;
			std::list< const CControlPane* > Children;
			if( pDlgControl->GetChildPanes( Children ) )
			{
				ClipboardArchive << Children.size();
				for( std::list< const CControlPane* >::const_iterator iter = Children.begin();
						 iter != Children.end();
						 ++iter )
				{
					const CControlPane* pChildPane = (*iter);
					TDclFormPtr pChildForm = pChildPane->GetSourceForm();
					pChildForm->Serialize( ClipboardArchive );
				}
			}
			else
				ClipboardArchive << size_t(0);
		}
	}
	ClipboardArchive.Flush();
	ClipboardMem.SeekToBegin();
	SIZE_T cbControlData = (SIZE_T)ClipboardMem.GetLength();
	SIZE_T cbClipboard = cbControlData + sizeof(SIZE_T) + sizeof(ctControls);
	HGLOBAL hgMem = GlobalAlloc( GMEM_MOVEABLE, cbClipboard );
	LPBYTE pbData = (LPBYTE)GlobalLock( hgMem );
	*(SIZE_T*)pbData = cbClipboard;
	pbData += sizeof(size_t);
	*(size_t*)pbData = ctControls;
	pbData += sizeof(ctControls);
	ClipboardMem.Read( pbData, cbControlData );
	GlobalUnlock( hgMem );
	SetClipboardData( CF_OPENDCLCONTROL, hgMem );
	CloseClipboard();
}

void CStudioDialogObject::OnUpdateEditCopy(CCmdUI *pCmdUI) 
{
	pCmdUI->Enable( HasSelection() );
}

void CStudioDialogObject::OnProperties() 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	if( !pActiveControl )
		return;
	CPropertyWizard PropWiz( pActiveControl );
	PropWiz.DoModal();
}

void CStudioDialogObject::OnUpdateProperties(CCmdUI *pCmdUI) 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	if( !pActiveControl )
	{
		pCmdUI->Enable( false );
		return;
	}
	switch( pActiveControl->GetType() )
	{
	case _CtlForm:
		{
			if( pActiveControl->GetPropertyObject( Prop::Name ) )
				break;
			pCmdUI->Enable( false );
			return;
		}
		break;
	case CtlFileExplorer:
		pCmdUI->Enable( false );
		return;
	}
	pCmdUI->Enable( CountSelected() <= 1 );
}

void CStudioDialogObject::OnAxProperties() 
{
	CDialogControl* pActiveCtrl = theStudioWorkspace.GetActiveDclControl()->GetControlInstance();
	if( !pActiveCtrl )
		return;
	CAxContainerCtrl* pAxCtrl = pActiveCtrl->GetActiveXCtrl();
	if( !pAxCtrl )
		return;
	CArray< CLSID, CLSID& > rCtrlPages;
	if( !pAxCtrl->GetPropertyPageCLSIDs( rCtrlPages ) || rCtrlPages.IsEmpty() )
		return;
	CWnd* pFocusWnd = GetFocus();
	CLSID FAR * pPropPages = rCtrlPages.GetData();
	ULONG ctPropPages = rCtrlPages.GetCount();
	LPUNKNOWN pUnknown = pAxCtrl->GetControlUnknown();
	OCPFIPARAMS params = 
	{
		sizeof(OCPFIPARAMS),
		pActiveCtrl->GetHWnd(),
		0,
		0,
		NULL,
		1,
		&pUnknown,
		ctPropPages,
		pPropPages,
		GetUserDefaultLCID(),
		DISPID_UNKNOWN,
	};
	if( S_OK == OleCreatePropertyFrameIndirect( &params ) )
	{
		if( pFocusWnd )
			pFocusWnd->SetFocus();
		pAxCtrl->SaveToStream();
	}
}

void CStudioDialogObject::OnUpdateAxProperties(CCmdUI *pCmdUI) 
{
	TDclControlPtr pDclControl = theStudioWorkspace.GetActiveDclControl();
	pCmdUI->Enable( pDclControl && pDclControl->GetType() == CtlActiveX );
}

void CStudioDialogObject::OnFontProperties() 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	if( !pActiveControl )
		return;
	CPropertyWizard PropWiz( pActiveControl, CPropertyWizard::Font );
	PropWiz.DoModal();
}

void CStudioDialogObject::OnTooltipProperties() 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	if( !pActiveControl )
		return;
	CPropertyWizard PropWiz( pActiveControl, CPropertyWizard::Tooltip );
	PropWiz.DoModal();
}

void CStudioDialogObject::OnForeColorProperties() 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	if( !pActiveControl )
		return;
	CPropertyWizard PropWiz( pActiveControl, CPropertyWizard::ForeColor );
	PropWiz.DoModal();
}

void CStudioDialogObject::OnBackColorProperties() 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	if( !pActiveControl )
		return;
	CPropertyWizard PropWiz( pActiveControl, CPropertyWizard::BackColor );
	PropWiz.DoModal();
}

void CStudioDialogObject::OnAltColorProperties() 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	if( !pActiveControl )
		return;
	CPropertyWizard PropWiz( pActiveControl, CPropertyWizard::AltColor );
	PropWiz.DoModal();
}

void CStudioDialogObject::OnImageListProperties() 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	if( !pActiveControl )
		return;
	CPropertyWizard PropWiz( pActiveControl, CPropertyWizard::ImageList );
	PropWiz.DoModal();
}

void CStudioDialogObject::OnEditObjectbrowser() 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	if( !pActiveControl )
		return;
	CControlBrowser ControlBrowserDlg( pActiveControl );
	ControlBrowserDlg.DoModal();
}

void CStudioDialogObject::OnUpdateEditObjectbrowser(CCmdUI *pCmdUI) 
{
	TDclControlPtr pActiveControl = theStudioWorkspace.GetActiveDclControl();
	switch( pActiveControl->GetType() )
	{
	case _CtlForm:
		{
			if( pActiveControl->GetPropertyObject( Prop::Name ) )
				break;
			pCmdUI->Enable( false );
			return;
		}
		break;
	}
	pCmdUI->Enable( CountSelected( false ) == 1 );
}

void CStudioDialogObject::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CDialogControl* pDlgControl = GetControlAtPoint( point );
	theStudioWorkspace.ActivateDclControl( pDlgControl? pDlgControl->GetTemplate() : GetTemplate(), true );
	CMenu menu;
	if( menu.LoadMenu( IDR_MAINFRAME ) )
	{
		CMenu* pEditMenu = menu.GetSubMenu( 1 );
		if( !pDlgControl || pDlgControl->GetTemplate()->GetType() != CtlActiveX )
			pEditMenu->RemoveMenu( ID_AXPROPERTIES, MF_BYCOMMAND ); //remove the ActiveX wizard menu item
		pEditMenu->SetDefaultItem( ID_CONTROLBROWSER );
		pEditMenu->TrackPopupMenu( TPM_LEFTALIGN, point.x, point.y, AfxGetMainWnd() );
	}
}

void CStudioDialogObject::OnToolsSetlispsymbolnames()
{
	CFormVarNameUpdate Dlg( mpSourceForm, mpSourceForm->GetKeyName() );
	if( IDOK == Dlg.DoModal() )
		theStudioWorkspace.ActivateDclControl( theStudioWorkspace.GetActiveDclControl() );
}

void CStudioDialogObject::OnUpdateToolsSetlispsymbolnames(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( mpSourceForm != NULL );
}

void CStudioDialogObject::OnToolsClearlispsymbolnames()
{
	CFormVarNameUpdate Dlg( mpSourceForm, NULL );
	if( IDOK == Dlg.DoModal() )
		theStudioWorkspace.ActivateDclControl( theStudioWorkspace.GetActiveDclControl() );
}

void CStudioDialogObject::OnUpdateToolsClearlispsymbolnames(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( mpSourceForm != NULL );
}

void CStudioDialogObject::OnBringtofront() 
{
	ZOrderSelectedControls( 0 );
}

void CStudioDialogObject::OnUpdateBringtofront(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( HasSelection() );
}

void CStudioDialogObject::OnSendtoback() 
{
	ZOrderSelectedControls( 1 );
}

void CStudioDialogObject::OnUpdateSendtoback(CCmdUI *pCmdUI)
{
	pCmdUI->Enable( HasSelection() );
}

void CStudioDialogObject::OnSize(UINT nType, int cx, int cy) 
{
	__super::OnSize( nType, cx, cy );
	if( !mbIgnoreSizing )
	{
		ASSERT( mpSourceForm != NULL );
		CRect rcForm;
		GetClientRect( &rcForm );
		mpTemplate->SetLongProperty( Prop::Width, rcForm.Width() );
		mpTemplate->SetLongProperty( Prop::Height, rcForm.Height() );
		mpControlPane->RecalcLayout();
	}
	CControlManager* pManager = GetControlManager();
	if( pManager )
	{
		CRect rcForm;
		GetClientRect( &rcForm );
		pManager->MoveWindow( &rcForm, FALSE );
	}
}

BOOL CStudioDialogObject::OnEraseBkgnd(CDC* pDC)
{
	//__super::OnEraseBkgnd(pDC);

	CRect rcClient;
	GetClientRect( &rcClient );
	COLORREF crBackground = mColorService.IsBackgroundTransparent()?
														mColorService.GetForegroundColor() :
														mColorService.GetBackgroundColor();
	if( mpControlPane->GetThemeHelper() )
	{
		TDclFormPtr pParentForm = mpSourceForm->GetParentForm();
		if( pParentForm )
		{
			TDclControlPtr pTabStrip = pParentForm->FindFirstControlOfType( CtlTabStrip );
			if( pTabStrip && pTabStrip->GetBooleanProperty( Prop::UseVisualStyle ) )
				crBackground = GetSysColor( COLOR_WINDOW );
		}
	}
	pDC->FillSolidRect( &rcClient, crBackground );
	if( mnGridSpacing > 1 )
	{
		SnapToGrid( rcClient.BottomRight() );
		COLORREF crGrid = GetSysColor( COLOR_GRAYTEXT );
		for( int iX = rcClient.Width(); iX > 0; iX -= mnGridSpacing )
		{
			for( int iY = rcClient.Height(); iY > 0; iY -= mnGridSpacing )
				pDC->SetPixel( iX, iY, crGrid );
		}
	}
	return TRUE;
}

void CStudioDialogObject::OnSizing(UINT fwSide, LPRECT pRect)
{
	CPoint ptSnapped( pRect->right - pRect->left - GetNCWidth(), pRect->bottom - pRect->top - GetNCHeight() );
	SnapToGrid( ptSnapped );
	ptSnapped.x += GetNCWidth();
	ptSnapped.y += GetNCHeight();
	CSize szMin( 0, 0 );
	CSize szMax( 0, 0 );
	//for now, min/max sizes are ignored in the editor
	//GetMinMaxSize( szMin, szMax );

	if( fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_LEFT || fwSide == WMSZ_TOPLEFT )
	{
		if (szMin.cx > 0 && ptSnapped.x < szMin.cx)
			pRect->left = pRect->right - szMin.cx;
		else if (szMax.cx > 0 && ptSnapped.x > szMax.cx)
			pRect->left = pRect->right - szMax.cx;
		else
			pRect->left = pRect->right - ptSnapped.x;
	}

	if( fwSide == WMSZ_BOTTOMRIGHT || fwSide == WMSZ_RIGHT || fwSide == WMSZ_TOPRIGHT )
	{
		if (szMin.cx > 0 && ptSnapped.x < szMin.cx)
			pRect->right = pRect->left + szMin.cx;
		else if (szMax.cx > 0 && ptSnapped.x > szMax.cx)
			pRect->right = pRect->left + szMax.cx;
		else
			pRect->right = pRect->left + ptSnapped.x;
	}

	if( fwSide == WMSZ_BOTTOMLEFT || fwSide == WMSZ_BOTTOM || fwSide == WMSZ_BOTTOMRIGHT )
	{
		if (szMin.cy > 0 && ptSnapped.y < szMin.cy)
			pRect->bottom = pRect->top + szMin.cy;
		else if (szMax.cy > 0 && ptSnapped.y > szMax.cy)
			pRect->bottom = pRect->top + szMax.cy;
		else
			pRect->bottom = pRect->top + ptSnapped.y;
	}

	if( fwSide == WMSZ_TOPLEFT || fwSide == WMSZ_TOP || fwSide == WMSZ_TOPRIGHT )
	{
		if (szMin.cy > 0 && ptSnapped.y < szMin.cy)
			pRect->top = pRect->bottom - szMin.cy;
		else if (szMax.cy > 0 && ptSnapped.y > szMax.cy)
			pRect->top = pRect->bottom - szMax.cy;
		else
			pRect->top = pRect->bottom - ptSnapped.y;
	}

	if( !mbSizing )
	{
		mbSizing = true;
		CUndoManager* pUndoManager = mpSourceForm->GetUndoManager();
		if( pUndoManager )
			pUndoManager->BeginGroup( theWorkspace.LoadResourceString( IDS_UNDO_SIZEFORM ) );
	}

	__super::OnSizing(fwSide, pRect);
}

__UINT_LRESULT CStudioDialogObject::OnNcHitTest(CPoint point)
{
	return HitTest( point );
}

void CStudioDialogObject::OnNcLButtonDown(UINT nHitTest, CPoint point)
{
	theStudioWorkspace.ActivateDclControl( mpTemplate );
	__super::OnNcLButtonDown(nHitTest, point);
}

void CStudioDialogObject::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
	theStudioWorkspace.ActivateDclControl( mpTemplate );
	PostMessage( WM_CONTEXTMENU, (WPARAM)m_hWnd, MAKELPARAM(point.x, point.y) );
}

UINT CStudioDialogObject::OnGetDlgCode()
{
	return DLGC_WANTARROWS;

	return __super::OnGetDlgCode();
}

BOOL CStudioDialogObject::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// bypass CDialog::OnCmdMsg during command routing, because it tries to use
	// it's own command router that interferes with the normal MFC routing
	return CWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CStudioDialogObject::OnCaptureChanged(CWnd *pWnd)
{
	if( mbSizing )
	{
		mbSizing = false;
		CUndoManager* pUndoManager = mpSourceForm->GetUndoManager();
		if( pUndoManager )
			pUndoManager->EndGroup();
		theStudioWorkspace.ActivateDclControl( mpTemplate );
	}
	__super::OnCaptureChanged(pWnd);
}
