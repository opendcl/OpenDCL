// ArxDialogObject.cpp : implementation file
//

#include "stdafx.h"
#include "ArxWorkspace.h"
#include "DclFormObject.h"
#include "ModalDlg.h"
#include "ModelessDlg.h"
#include "OptionsTabPane.h"
#include "DockingDialog.h"
#include "CustomFileDialog.h"
#include "PaletteDialog.h"
#include "InvokeMethod.h"


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
		InvokeMethod( msDocActivatedEvent, true, pActivatedDoc );
}

void CArxDialogObject::CDocReactor::documentToBeDestroyed(AcApDocument* pDocToDestroy)
{
	if( acDocManager->documentCount() == 1 )
	{
		mpDialogobject->OnEnteringNoDocState();
		InvokeMethod( msEnteringNoDocStateEvent, false, pDocToDestroy );
	}
}


CArxDialogObject::CArxDialogObject( TDclFormPtr pSourceForm, CWnd* pHostDlg )
: CDialogObject( pSourceForm, &mControlPane, pHostDlg )
, mControlPane( pSourceForm, pHostDlg )
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

//static
CDialogObject* CArxDialogObject::Create( TDclFormPtr pDclForm, CWnd* pParent /*= NULL*/,
																				 DialogParams* pParams /*= NULL*/ )
{
	CAcAppContextModuleResourceOverride resOverride;
	switch( pDclForm->GetType() )
	{
	case FrmModalDlg: return new CModalDlg( pDclForm, pParent, pParams );
	case FrmModelessDlg: return new CModelessDlg( pDclForm, pParent, pParams );
	case FrmOptionsTab: return new COptionsTabPane( pDclForm, pParent, pParams );
	case FrmControlBar: return new CDockingDialog( pDclForm, pParent, pParams );
	case FrmFileDlg: return new CCustomFileDialog( pDclForm, pParent, pParams );
	case FrmPaletteDlg: return new CPaletteDialog( pDclForm, pParent, pParams );
	}
	return NULL;
}
