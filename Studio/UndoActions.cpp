// UndoActions.cpp : implementation file
//

#include "stdafx.h"
#include "UndoActions.h"
#include "DclFormTemplate.h"
#include "StudioWorkspace.h"
#include "ProjectPane.h"
#include "StudioDialogControl.h"
#include "StudioDialogObject.h"
#include "ControlManager.h"
#include "Resource.h"


CString CSaveProjectUA::GetDisplayName() const
{
	return theWorkspace.LoadResourceString( IDS_UNDO_SAVEPROJECT );
}

bool CSaveProjectUA::Undo()
{
	mpProject->GetDocument()->SetModifiedFlag( FALSE );
	return true;
}

CString CSelectFormUA::GetDisplayName() const
{
	return theWorkspace.LoadResourceString( IDS_UNDO_SELECTFORM );
}

bool CSelectFormUA::Undo()
{
	if( !mpForm )
		return true;
	ASSERT( mpForm->m_htiTreeItem != NULL );
	CProjectPane* pProjTree = theStudioWorkspace.GetProjectPane();
	if( mpForm->m_htiTreeItem )
	{
		CTreeCtrl& TreeCtrl = pProjTree->GetTreeCtrl();
		TreeCtrl.EnsureVisible( mpForm->m_htiTreeItem );
		TreeCtrl.SetFocus();
		TreeCtrl.SelectItem( mpForm->m_htiTreeItem );
	}
	return true;
}

CString CAddFormUA::GetDisplayName() const
{
	return theWorkspace.LoadResourceString( IDS_UNDO_ADDFORM );
}

bool CAddFormUA::Undo()
{
	if( !mpForm )
		return true;
	TProjectPtr pProject = mpForm->GetProject();
	if( !pProject )
		return false;
	pProject->DeleteForm( mpForm );
	ASSERT( mpForm->m_htiTreeItem != NULL );
	CProjectPane* pProjTree = theStudioWorkspace.GetProjectPane();
	if( mpForm->m_htiTreeItem )
		pProjTree->GetTreeCtrl().DeleteItem( mpForm->m_htiTreeItem );
	return true;
}

CString CDeleteFormUA::GetDisplayName() const
{
	return theWorkspace.LoadResourceString( IDS_UNDO_REMOVEFORM );
}

bool CDeleteFormUA::Undo()
{
	if( !mpForm )
		return true;
	TProjectPtr pProject = mpForm->GetProject();
	if( !pProject )
		return false;
	pProject->AddInitializedForm( mpForm );
	CProjectPane* pProjTree = theStudioWorkspace.GetProjectPane();
	if( pProjTree )
		pProjTree->AddFormToTree( mpForm, false );
	return true;
}

CString CSelectControlUA::GetDisplayName() const
{
	return theWorkspace.LoadResourceString( IDS_UNDO_SELECTCONTROL );
}

bool CSelectControlUA::Undo()
{
	if( !mpDclControl )
		return true;
	if( mpDclControl->GetControlInstance() )
		theStudioWorkspace.ActivateDclControl( mpDclControl );
	return true;
}

CString CAddControlUA::GetDisplayName() const
{
	return theWorkspace.LoadResourceString( IDS_UNDO_ADDCONTROL );
}

bool CAddControlUA::Undo()
{
	if( !mpDclControl )
		return true;
	TDclFormPtr pForm = mpDclControl->GetOwnerForm();
	if( !pForm )
		return false;
	CStudioDialogObject* pDlgObjectToReactivate = NULL;
	CDialogControl* pDlgControl = mpDclControl->GetControlInstance();
	if( pDlgControl )
	{
		CControlManager* pManager = pDlgControl->GetControlManager();
		if( pManager )
		{
			if( pManager->IsSelected() )
			{
				theStudioWorkspace.ActivateDclControl( NULL );
				pDlgObjectToReactivate = pManager->GetDialogObject();
			}
			CStudioDialogObject* pDlgObject = pManager->GetDialogObject();
			if( pDlgObject )
				pDlgObject->OnControlDeleted( mpDclControl );
			pManager->DestroyWindow();
		}
	}
	pForm->DeleteControl( mpDclControl );
	if( pDlgObjectToReactivate )
	{
		theStudioWorkspace.ActivateDlgObject( pDlgObjectToReactivate );
		theStudioWorkspace.ActivateDclControl( pForm->GetControlProperties() );
	}
	return true;
}

CDeleteControlUA::CDeleteControlUA( TDclControlPtr pDclControl )
: CUndoAction( Undo::DeleteControl )
, mpDclControl( pDclControl )
, mIdx( CDclFormObject::GetCurrentControlIndex( pDclControl ) )
{}

CString CDeleteControlUA::GetDisplayName() const
{
	return theWorkspace.LoadResourceString( IDS_UNDO_DELETECONTROL );
}

bool CDeleteControlUA::Undo()
{
	if( !mpDclControl )
		return true;
	TDclFormPtr pForm = mpDclControl->GetOwnerForm();
	if( !pForm )
		return false;
	pForm->AddControl( mpDclControl );
	pForm->ReorderControl( mpDclControl, mIdx );
	CStudioDialogObject* pDlgObject = (CStudioDialogObject*)pForm->GetFormInstance();
	if( pDlgObject )
	{
		CControlPane* pPane = pDlgObject->GetControlPane();
		TDialogControlPtr pDlgControl =
			CStudioDialogControl::Create( mpDclControl, pPane, pDlgObject->GetNextControlId() );
		pPane->AddControl( pDlgControl );
		theStudioWorkspace.ActivateDclControl( mpDclControl, true );
	}
	return true;
}

CReorderControlUA::CReorderControlUA( TDclControlPtr pDclControl )
: CUndoAction( Undo::ReorderControl )
, mpDclControl( pDclControl )
, mIdx( CDclFormObject::GetCurrentControlIndex( pDclControl ) )
{}

CString CReorderControlUA::GetDisplayName() const
{
	return theWorkspace.LoadResourceString( IDS_UNDO_REORDERCONTROL );
}

bool CReorderControlUA::Undo()
{
	if( !mpDclControl )
		return true;
	TDclFormPtr pForm = mpDclControl->GetOwnerForm();
	if( !pForm )
		return false;
	pForm->ReorderControl( mpDclControl, mIdx );
	CStudioDialogObject* pDlgObject = (CStudioDialogObject*)pForm->GetFormInstance();
	if( pDlgObject )
	{
		CControlPane* pControlPane = pDlgObject->GetControlPane();
		if( pControlPane )
			pControlPane->ApplyTabOrder();
		if( pDlgObject == theStudioWorkspace.GetActiveDlgObject() )
			theStudioWorkspace.ActivateDlgObject( pDlgObject );
	}
	return true;
}

CPropertyChangeUA::CPropertyChangeUA( TPropertyPtr pProperty )
: CUndoAction( Undo::PropertyChange )
, mpDclControl( pProperty->GetOwnerControl() )
, mId( pProperty->GetID() )
, mValue( 256 )
{
	CArchive Value( &mValue, CArchive::store, 256 );
	const_cast< CPropertyObject* >( &*pProperty )->Serialize( Value );
}

CString CPropertyChangeUA::GetDisplayName() const
{
	return theWorkspace.LoadResourceString( IDS_UNDO_CHANGEPROPERTY );
}

bool CPropertyChangeUA::Undo()
{
	if( !mpDclControl )
		return true;
	TPropertyPtr pProp = mpDclControl->GetPropertyObject( mId );
	if( !pProp )
		return false;
	mValue.SeekToBegin();
	CArchive Value( &mValue, CArchive::load, 256 );
	pProp->Serialize( Value );
	CStudioDialogControl::UpdateProperty( mpDclControl, mId );
	if( mpDclControl == theStudioWorkspace.GetActiveDclControl() )
		theStudioWorkspace.ActivateDclControl( mpDclControl );
	return true;
}
