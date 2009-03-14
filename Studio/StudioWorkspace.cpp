// StudioWorkspace.cpp : implementation file
//

#include "stdafx.h"
#include "StudioWorkspace.h"
#include "StudioProject.h"
#include "OpenDCL.h"
#include "OpenDCLDoc.h"
#include "StudioFrame.h"
#include "DclFormView.h"
#include "ZOrderPane.h"
#include "StudioDialogObject.h"
#include "ControlManager.h"


CWorkspace* theWorkspacePtr()
{
	static CStudioWorkspace workspace;
	return &workspace;
}


//function used by CLocalResourceModuleOverride defined in StdAfx.h
HMODULE appLocalResModule(void)
{
	return theWorkspace.GetLocalResourceModule();
}


CStudioWorkspace::CStudioWorkspace()
: mpStudioFrame( (CStudioFrame*)((COpenDCLApp*)AfxGetApp())->m_pMainWnd )
, mpActiveDlgObject( NULL )
, mpActiveDclControl( NULL )
{
}

CStudioWorkspace::~CStudioWorkspace()
{
}

CString CStudioWorkspace::GetLanguage(void) const
{
	CString sLanguage = __super::GetLanguage();
	if( !sLanguage.IsEmpty() )
		return sLanguage;
	return _T("ENU");
}

TStudioProjectPtr CStudioWorkspace::GetActiveProject() const
{
	COpenDCLDoc* pDoc = (COpenDCLDoc*)GetActiveDocument();
	if( !pDoc )
		return NULL;
	return pDoc->GetProject();
}

void CStudioWorkspace::SetModified( bool bModified )
{
	__super::SetModified( bModified );
}

CDocument* CStudioWorkspace::GetActiveDocument() const
{
	if( !GetStudioFrame() )
		return NULL;
	return GetStudioFrame()->GetActiveDocument();
}

CProjectPane* CStudioWorkspace::GetProjectPane() const
{
	if( !GetStudioFrame() )
		return NULL;
	return &GetStudioFrame()->GetProjectPane();
}

CPropertyPane* CStudioWorkspace::GetPropertyPane() const
{
	if( !GetStudioFrame() )
		return NULL;
	return &GetStudioFrame()->GetPropertyPane();
}

CToolboxPane* CStudioWorkspace::GetToolboxPane() const
{
	if( !GetStudioFrame() )
		return NULL;
	return &GetStudioFrame()->GetToolboxPane();
}

CZOrderPane* CStudioWorkspace::GetZOrderPane() const
{
	if( !GetStudioFrame() )
		return NULL;
	return &GetStudioFrame()->GetZOrderPane();
}

CString CStudioWorkspace::GetActiveProjectName() const
{
	TStudioProjectPtr pProject = GetActiveProject();
	if( !pProject )
		return CString();
	return pProject->GetKeyName();
}

FontSettings CStudioWorkspace::GetDefaultFontSettings() const
{
	return theApp.GetDefaultFontSettings();
}

void CStudioWorkspace::OnGridSpacingChange( UINT nGridSpacing )
{
	CDocument* pDoc = GetActiveDocument();
	if( pDoc )
	{
		CProjectPane* pProjectPane = GetProjectPane();
		POSITION pos = pDoc->GetFirstViewPosition();
		while( pos )
		{
			CView* pView = pDoc->GetNextView( pos );
			if( pView == pProjectPane )
				continue;
			ASSERT_KINDOF(CDclFormView, pView);
			CDclFormView* pFormView = (CDclFormView*)pView;
			pFormView->GetDialogObject()->OnGridSpacingChange( nGridSpacing );
		}
	}
}

void CStudioWorkspace::OnFlushUndoGroup()
{
	CPropertyPane* pPropertyPane = GetPropertyPane();
	if( pPropertyPane )
		pPropertyPane->OnFlushUndoGroup();
}

void CStudioWorkspace::ActivateProject( TStudioProjectPtr pProject, CDocument* pDoc )
{
	CProjectPane* pProjectPane = GetProjectPane();
	if( !pProjectPane )
		return;
	if( pDoc )
	{
		if( !pProjectPane->GetDocument() )
			pDoc->AddView( pProjectPane );
	}
	pProjectPane->OnActivateProject( pProject );
}

void CStudioWorkspace::ActivateDlgObject( CStudioDialogObject* pDlgObject )
{
	TraceFmt( _T("> CStudioWorkspace::ActivateDlgObject(%s [%p])\r\n"),
						(LPCTSTR)pDlgObject->GetSourceForm()->GetKeyPath(),
						pDlgObject );
	if( pDlgObject ) //can be NULL
	{
		if( pDlgObject->m_hWnd )
		{
			pDlgObject->OnUpdateZOrder();
			CZOrderPane* pZOrderPane = GetZOrderPane();
			if( pZOrderPane )
				pZOrderPane->OnActivateDlgObject( pDlgObject );
		}
	}
	else
		DeactivateDlgObject( mpActiveDlgObject );
	CProjectPane* pProjectPane = GetProjectPane();
	if( pProjectPane )
		pProjectPane->OnActivateForm( pDlgObject->GetSourceForm() );
	CToolboxPane* pToolbox = GetToolboxPane();
	if( pToolbox )
		pToolbox->ActivateDlgObject( pDlgObject );
	bool bSetFormProps = (pDlgObject != mpActiveDlgObject);
	mpActiveDlgObject = pDlgObject;
	SetActiveFormView( pDlgObject? pDlgObject->GetFormView() : NULL );
	GetStudioFrame()->SetActiveView( pDlgObject->GetFormView(), TRUE );
	if( bSetFormProps )
	{
		ActivateDclControl( NULL );
		ActivateDclControl( pDlgObject->GetSourceForm()->GetControlProperties() );
	}
}

void CStudioWorkspace::DeactivateDlgObject( CStudioDialogObject* pDlgObject )
{
	TraceFmt( _T("> CStudioWorkspace::DeactivateDlgObject(%s [%p])\r\n"),
						(LPCTSTR)(pDlgObject? pDlgObject->GetSourceForm()->GetKeyPath() : NULL),
						pDlgObject );
	if( pDlgObject != mpActiveDlgObject )
		return; //no-op
	CZOrderPane* pZOrderPane = GetZOrderPane();
	if( pZOrderPane )
		pZOrderPane->OnActivateDlgObject( NULL );
	CPropertyPane* pPropertyPane = GetPropertyPane();
	if( pPropertyPane )
		pPropertyPane->OnActivateDclControl( NULL );
	CToolboxPane* pToolbox = GetToolboxPane();
	if( pToolbox )
		pToolbox->ActivateDlgObject( NULL );
	mpActiveDlgObject = NULL;
	SetActiveFormView( NULL );
}

void CStudioWorkspace::ActivateDclControl( TDclControlPtr pDclControl, bool bDeactivateCurrent /*= false*/ )
{
	TraceFmt( _T("> CStudioWorkspace::ActivateDclControl(%s [%p], %s)\r\n"),
						(LPCTSTR)(pDclControl? pDclControl->GetKeyPath() : NULL),
						(const CDclControlObject*)pDclControl,
						bDeactivateCurrent? _T("true") : _T("false") );
	if( pDclControl && bDeactivateCurrent )
		ActivateDclControl( NULL, false );
	if( pDclControl == mpActiveDclControl )
	{
		CPropertyPane* pPropertyPane = GetPropertyPane();
		if( pPropertyPane )
			pPropertyPane->OnActivateDclControl( pDclControl ); //display updated property values
		mpStudioFrame->GetFontToolbar().OnActivateDclControl( pDclControl );
		return; //no-op
	}
	CToolboxPane* pToolbox = GetToolboxPane();
	if( pToolbox )
		pToolbox->ActivateDclControl( pDclControl );
	mpActiveDclControl = pDclControl;
	if( pDclControl ) //can be NULL
	{
		if( pDclControl->GetType() == _CtlForm )
		{
			ActivateDclControl( NULL ); //always reset before selecting form
			mpActiveDclControl = pDclControl;
		}
		TDclFormPtr pForm = pDclControl->GetOwnerForm();
		CStudioDialogObject* pDlgObject = (CStudioDialogObject*)pForm->GetFormInstance();
		assert( pDlgObject == mpActiveDlgObject ); //make sure the form is activated first!
		CZOrderPane* pZOrderPane = GetZOrderPane();
		if( pZOrderPane )
			pZOrderPane->OnActivateDclControl( pDclControl );
		CPropertyPane* pPropertyPane = GetPropertyPane();
		if( pPropertyPane )
			pPropertyPane->OnActivateDclControl( pDclControl );
		mpStudioFrame->GetFontToolbar().OnActivateDclControl( pDclControl );
		pDlgObject->OnActivateDclControl( pDclControl );
		//if( pDclControl->GetType() != _CtlForm )
		//{
		//	CToolboxPane* pToolbox = GetToolboxPane();
		//	if( pToolbox )
		//		pToolbox->ResetToPointer();
		//}
	}
	else
	{
		CZOrderPane* pZOrderPane = GetZOrderPane();
		if( pZOrderPane )
			pZOrderPane->OnActivateDclControl( NULL );
		CPropertyPane* pPropertyPane = GetPropertyPane();
		if( pPropertyPane )
			pPropertyPane->OnActivateDclControl( NULL );
		mpStudioFrame->GetFontToolbar().OnActivateDclControl( NULL );
		if( mpActiveDlgObject )
			mpActiveDlgObject->OnActivateDclControl( NULL );
	}
}
