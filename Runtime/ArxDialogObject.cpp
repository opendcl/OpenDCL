// ArxDialogObject.cpp : implementation file
//

#include "stdafx.h"
#include "ArxWorkspace.h"
#include "DclFormObject.h"
#include "ModalDlg.h"
#include "ModelessDlg.h"
#include "OptionsTabDlg.h"
#include "ControlBarDlg.h"
#include "CustomFileDlg.h"
#include "PaletteDlg.h"
#include "EventArgs.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDialogObject


CArxDialogObject::CDocReactor::CDocReactor( CArxDialogObject* pDialogobject )
: mpDialogobject( pDialogobject )
{
	TDclControlPtr pProps = pDialogobject->GetSourceForm()->GetControlProperties();
	if( pProps )
	{
		msDocActivatedEvent = pProps->GetStringProperty( Prop::DocEventActivated );
		msEnteringNoDocStateEvent = pProps->GetStringProperty( Prop::DocEventEnteringNoDocState );
		if( !msDocActivatedEvent.IsEmpty() || !msEnteringNoDocStateEvent.IsEmpty() )
			acDocManager->addReactor( this );
	}
}

CArxDialogObject::CDocReactor::~CDocReactor()
{
	if( !msDocActivatedEvent.IsEmpty() || !msEnteringNoDocStateEvent.IsEmpty() )
		acDocManager->removeReactor( this );
}

void CArxDialogObject::CDocReactor::documentActivated(AcApDocument* pActivatedDoc)
{
	if( pActivatedDoc )
		mpDialogobject->GetArxServices()->HandleEvent( msDocActivatedEvent, true, args_null(), pActivatedDoc );
}

void CArxDialogObject::CDocReactor::documentToBeDestroyed(AcApDocument* pDocToDestroy)
{
	if( acDocManager->documentCount() == 1 )
	{
		resbuf rbSDI = { NULL };
		if( acedGetVar( ACRX_T("SDI"), &rbSDI ) != RTNORM || rbSDI.resval.rint != 1 )
		{
			mpDialogobject->OnEnteringNoDocState();
			mpDialogobject->GetArxServices()->HandleEvent( msEnteringNoDocStateEvent, false, args_null(), pDocToDestroy );
		}
	}
}


CArxDialogObject::CArxDialogObject( TDclFormPtr pSourceForm, CWnd* pHostDlg )
: CDialogObject( pSourceForm, &mControlPane, pHostDlg )
, mControlPane( pSourceForm, pHostDlg )
, mArxServices( this )
, mbEnteringNoDocState( false )
, mDocReactor( this )
{
	theArxWorkspace.RegisterDialog( this );
	TraceFmt( _T("> CArxDialogObject::CArxDialogObject(%s [%p], %s [HWND: %p]) [this: %p]\r\n"),
						(LPCTSTR)pSourceForm->GetKeyPath(), (CDclFormObject*)pSourceForm,
						(LPCTSTR)CString(pHostDlg->GetRuntimeClass()->m_lpszClassName),
						pHostDlg->m_hWnd, this );
}

CArxDialogObject::~CArxDialogObject()
{
	theArxWorkspace.UnregisterDialog( this );
	TraceFmt( _T("< CArxDialogObject::~CArxDialogObject() [this: %p]\r\n"), this );
}

void CArxDialogObject::OnEnteringNoDocState()
{
	mbEnteringNoDocState = true;
}

bool CArxDialogObject::IsCloseAllowed( bool bCancelling ) const
{
	resbuf* prbResult = NULL;
	GetArxServices()->HandleEvent( Prop::FormEventCancelClose, prbResult, args_N( bCancelling? 1 : 0 ) );
	if( !prbResult )
		return true;
	bool bDisallow = (prbResult->restype != RTNIL && prbResult->restype != RTVOID);
	acutRelRb( prbResult );
	return !bDisallow;
}

//static
CDialogObject* CArxDialogObject::Create( TDclFormPtr pDclForm, CWnd* pParent /*= NULL*/,
																				 DialogParams* pParams /*= NULL*/ )
{
	CAcAppContextModuleResourceOverride resOverride;
	switch( pDclForm->GetType() )
	{
	case FrmModalDlg: return new CModalDlg( pDclForm, pParent, pParams );
	case FrmModelessDlg: return new CModelessDlg( pDclForm, pParent, pParams );
	case FrmOptionsTab: return new COptionsTabDlg( pDclForm, pParent, pParams );
	case FrmFileDlg: return new CCustomFileDlg( pDclForm, pParent, pParams );
#if defined(_BRXTARGET) && (_BRXTARGET <= 9)
	case FrmControlBar: return new CModelessDlg( pDclForm, pParent, pParams );
	case FrmPaletteDlg: return new CModelessDlg( pDclForm, pParent, pParams );
#else
	case FrmControlBar: return new CControlBarDlg( pDclForm, pParent, pParams );
	case FrmPaletteDlg: return new CPaletteDlg( pDclForm, pParent, pParams );
#endif
	}
	return NULL;
}
