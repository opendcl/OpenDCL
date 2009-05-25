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
	if( mpDropOnAcadTarget &&
			!pDclControl->GetStringProperty( Prop::DragnDropToAutoCAD ).IsEmpty() )
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
		CSize sizDragRect( GetSystemMetrics( SM_CXDOUBLECLK ), GetSystemMetrics( SM_CYDOUBLECLK ) );
		ptScreen.Offset( -sizDragRect.cx / 2, -sizDragRect.cy / 2 );
		CRect rcDragRegion( ptScreen, sizDragRect );
		dwEffect = SourceData.DoDragDrop( dwSupportedDropEffects, &rcDragRegion, mpDropSource );
	}
	if( pAcadDropTarget )
		acedEndOverrideDropTarget( pAcadDropTarget );
	if( mpDropOnAcadTarget )
		acedRemoveDropTarget( mpDropOnAcadTarget );
	return dwEffect;
}
