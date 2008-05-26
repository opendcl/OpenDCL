// ArxDragDropService.cpp : implementation file
//

#include "StdAfx.h"
#include "ArxDragDropService.h"
#include "ArxControlDropTarget.h"
#include "ArxControlAcadDropTarget.h"
#include "DialogControl.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDragDropService

CArxDragDropService::CArxDragDropService( CDialogControl* pDlgControl )
: mpDlgControl( pDlgControl )
, mpDropSource( pDlgControl->GetDropSource() )
, mpDropTarget( pDlgControl->GetDropTarget() )
, mpDropOnAcadTarget( NULL )
, mbMustDeleteDropTarget( false )
, mbRegistered( false )
{
	if( !mpDropTarget )
	{
		mpDropTarget = new CArxControlDropTarget( pDlgControl );
		mbMustDeleteDropTarget = true;
	}
	if( pDlgControl->GetTemplate()->GetBooleanProperty( Prop::DragnDropAllowBegin ) &&
			!pDlgControl->GetTemplate()->GetStringProperty( Prop::DragnDropToAutoCAD ).IsEmpty() )
		mpDropOnAcadTarget = new CArxControlAcadDropTarget( pDlgControl );
}

CArxDragDropService::~CArxDragDropService()
{
	if( mbRegistered )
		RevokeControlAsDropTarget();
	if( mbMustDeleteDropTarget )
		delete mpDropTarget;
	delete mpDropOnAcadTarget;
}

bool CArxDragDropService::RegisterControlAsDropTarget()
{
	if( !mpDropTarget )
		return false;
	mpDropTarget->Register( mpDlgControl->GetControlWnd() );
	mbRegistered = true;
	return true;
}

bool CArxDragDropService::RevokeControlAsDropTarget()
{
	if( !mpDropTarget )
		return false;
	mpDropTarget->Revoke();
	mbRegistered = false;
	return true;
}

DROPEFFECT CArxDragDropService::BeginDragDrop( const CPoint& point )
{
	TDclControlPtr pDclControl = mpDlgControl->GetTemplate();
	if( !pDclControl->GetBooleanProperty( Prop::DragnDropAllowBegin ) )
		return DROPEFFECT_NONE;
	if( mpDropOnAcadTarget )
		acedAddDropTarget( mpDropOnAcadTarget );
	COleDropTarget* pAcadDropTarget = GetCustomAcadDropTarget();
	if( pAcadDropTarget )
	{
		if( !acedStartOverrideDropTarget( pAcadDropTarget ) )
			pAcadDropTarget = NULL;
	}
	DWORD dwEffect = DROPEFFECT_NONE;
	COleDataSource SourceData;
	DWORD dwSupportedDropEffects = mpDlgControl->OnBeginDrag( point, SourceData );
	if( dwSupportedDropEffects != DROPEFFECT_NONE )
	{
		CString sDragBeginEvent = pDclControl->GetStringProperty( Prop::DragnDropBegin );
		if( !sDragBeginEvent.IsEmpty() )
		{
			InvokeMethod( sDragBeginEvent, mpDlgControl->IsAsyncEvents() );
			dwSupportedDropEffects &= ~DROPEFFECT_MOVE;
		}
		CPoint ptScreen( point );
		mpDlgControl->GetControlWnd()->ClientToScreen( &ptScreen );
		CRect rcDragRegion( ptScreen, CSize( GetSystemMetrics( SM_CXDOUBLECLK ) / 2, GetSystemMetrics( SM_CYDOUBLECLK ) / 2 ) );
		dwEffect = SourceData.DoDragDrop( dwSupportedDropEffects, &rcDragRegion, mpDropSource );
	}
	if( pAcadDropTarget )
		acedEndOverrideDropTarget( pAcadDropTarget );
	if( mpDropOnAcadTarget )
		acedRemoveDropTarget( mpDropOnAcadTarget );
	return dwEffect;
}
