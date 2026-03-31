// ArxDragDropService.cpp : implementation file
//

#include "StdAfx.h"
#include "ArxDragDropService.h"
#include "ArxControlDropTarget.h"
#include "ArxControlAcadDropTarget.h"
#include "DialogControl.h"
#include "ArxControlServices.h"

#if (defined(_BRXTARGET) && (_BRXTARGET <= 9)) || defined(_GRXTARGET)
BOOL acedStartOverrideDropTarget(COleDropTarget* pTarget)
{
	return FALSE;
}
BOOL acedEndOverrideDropTarget(COleDropTarget* pTarget)
{
	return FALSE;
}
BOOL acedAddDropTarget(COleDropTarget* pTarget)
{
	return FALSE;
}
BOOL acedRemoveDropTarget(COleDropTarget* pTarget)
{
	return FALSE;
}
#endif

//custom drop source implemented just like the MFC default drop source except this
//one dispatches mouse messages during the drag wait loop instead of eating them
class OdclDropSource : public COleDropSource
{
	CWnd* mpCtrl;
public:
	OdclDropSource( CWnd* pCtrl )
		: COleDropSource()
		, mpCtrl( pCtrl )
		{}
	operator COleDropSource* () { return this; }
	BOOL OnBeginDrag(CWnd* pWnd) override
	{
		ASSERT_VALID(this);

		m_bDragStarted = FALSE;

		// opposite button cancels drag operation
		m_dwButtonCancel = 0;
		m_dwButtonDrop = 0;
		if (GetKeyState(VK_LBUTTON) < 0)
		{
			m_dwButtonDrop |= MK_LBUTTON;
			m_dwButtonCancel |= MK_RBUTTON;
		}
		else if (GetKeyState(VK_RBUTTON) < 0)
		{
			m_dwButtonDrop |= MK_RBUTTON;
			m_dwButtonCancel |= MK_LBUTTON;
		}

		DWORD dwLastTick = GetTickCount();
		mpCtrl->SetCapture();

		while (!m_bDragStarted)
		{
			// some applications steal capture away at random times
			if (CWnd::GetCapture() != mpCtrl)
				break;

			MSG msg;
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				if( msg.message == WM_QUIT )
				{
					PostQuitMessage( msg.wParam );
					break;
				}

				TranslateMessage(&msg);
				DispatchMessage(&msg);

				// check for button cancellation (any button down will cancel)
				if (msg.message == WM_LBUTTONUP || msg.message == WM_RBUTTONUP ||
					msg.message == WM_LBUTTONDOWN || msg.message == WM_RBUTTONDOWN)
					break;

				// check for keyboard cancellation
				if( msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE )
					break;

				// check for drag start transition
				m_bDragStarted = !m_rectStartDrag.PtInRect(msg.pt);
				if( m_bDragStarted )
					break;
			}

			// if the user sits here long enough, we eventually start the drag
			if (GetTickCount() - dwLastTick > nDragDelay)
			{
				m_bDragStarted = TRUE;
				break;
			}
		}
		ReleaseCapture();

		return m_bDragStarted;
	}
};



/////////////////////////////////////////////////////////////////////////////
// CArxDragDropService

CArxDragDropService::CArxDragDropService( CDialogControl* pDlgControl )
: mpDlgControl( pDlgControl )
, mpDropSource( pDlgControl->GetDropSource() )
, mpDropTarget( pDlgControl->GetDropTarget() )
, mbMustDeleteDropTarget( false )
, mpDropOnAcadTarget( pDlgControl->GetDropOnAcadTarget() )
, mbMustDeleteDropOnAcadTarget( false )
, mbRegistered( false )
{
	if( !mpDropTarget )
	{
		mpDropTarget = new CArxControlDropTarget( pDlgControl );
		mbMustDeleteDropTarget = true;
	}
	if( !mpDropOnAcadTarget )
	{
		mpDropOnAcadTarget = new CArxControlAcadDropTarget( pDlgControl );
		mbMustDeleteDropOnAcadTarget = true;
	}
}

CArxDragDropService::~CArxDragDropService()
{
	if( mbRegistered )
		RevokeControlAsDropTarget();
	if( mbMustDeleteDropTarget )
		delete mpDropTarget;
	if( mbMustDeleteDropOnAcadTarget )
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
			(!mbMustDeleteDropOnAcadTarget ||
			 !pDclControl->GetStringProperty( Prop::DragnDropToAutoCAD ).IsEmpty()) )
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
		const CArxControlServices* pArxServices = mpDlgControl->GetArxServices();
		if( pArxServices )
		{
			resbuf* prbResult = NULL;
			bool bCancel = pArxServices->HandleEvent( Prop::DragnDropBegin, prbResult );
			if( !bCancel && prbResult && prbResult->restype == RTSHORT )
			{
				switch( prbResult->resval.rint )
				{
				case DROPEFFECT_NONE:
					bCancel = true;
					break;
				case DROPEFFECT_COPY:
					dwSupportedDropEffects = DROPEFFECT_COPY;
					break;
				case DROPEFFECT_MOVE:
					dwSupportedDropEffects = DROPEFFECT_MOVE;
					break;
				case DROPEFFECT_LINK:
					dwSupportedDropEffects = DROPEFFECT_LINK;
					break;
				}
			}
			acutRelRb( prbResult );
			if( bCancel )
				return dwEffect;
		}
		CPoint ptScreen( point );
		mpDlgControl->GetControlWnd()->ClientToScreen( &ptScreen );
		CSize sizDragRect( GetSystemMetrics( SM_CXDOUBLECLK ), GetSystemMetrics( SM_CYDOUBLECLK ) );
		ptScreen.Offset( -sizDragRect.cx / 2, -sizDragRect.cy / 2 );
		CRect rcDragRegion( ptScreen, sizDragRect );
		dwEffect = SourceData.DoDragDrop( dwSupportedDropEffects, &rcDragRegion, mpDropSource? mpDropSource : OdclDropSource( mpDlgControl->GetControlWnd() ) );
	}
	if( pAcadDropTarget )
		acedEndOverrideDropTarget( pAcadDropTarget );
	if( mpDropOnAcadTarget )
		acedRemoveDropTarget( mpDropOnAcadTarget );
	return dwEffect;
}
