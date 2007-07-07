// ArxDialogObject.cpp : implementation file
//

#include "stdafx.h"
#include "ArxProject.h"
#include "DclFormObject.h"
#include "ModalVDcl.h"
#include "ModelessDlg.h"
#include "CfgTabPane.h"
#include "DockingDialog.h"
#include "ResizableDockingDialog.h"
#include "CustomFileDialog.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDialogObject


CArxDialogObject::CDocReactor::CDocReactor( CArxDialogObject* pDialogobject )
: mpDialogobject( pDialogobject )
{
	CDclControlObject* pProps = pDialogobject->GetSourceForm()->GetControlProperties();
	if( pProps )
	{
		msDocActivatedEvent = pProps->GetStrProperty( nDocEventActivated );
		if( !msDocActivatedEvent.IsEmpty() )
			acDocManager->addReactor( this );
	}
}

CArxDialogObject::CDocReactor::~CDocReactor()
{
	if( !msDocActivatedEvent.IsEmpty() )
		acDocManager->removeReactor( this );
}

void CArxDialogObject::CDocReactor::documentActivated(AcApDocument* pActivatedDoc)
{
	if( pActivatedDoc )
		InvokeMethod( msDocActivatedEvent, true, pActivatedDoc );
}


CArxDialogObject::CArxDialogObject( CDclFormObject* pSourceForm, CWnd* pHostDlg )
: CDialogObject( pSourceForm, pHostDlg )
, mControlPane( pSourceForm, pHostDlg )
, mDocReactor( this )
{
	theArxWorkspace.RegisterDialog( this );
	TraceFmt( _T("> CArxDialogObject::CArxDialogObject(%s [%08X], %s [HWND: %08X]) [this: %08X]\r\n"),
						pSourceForm->GetKeyPath(), pSourceForm, CString(pHostDlg->GetRuntimeClass()->m_lpszClassName),
						pHostDlg->m_hWnd, (long)this );
}

CArxDialogObject::~CArxDialogObject()
{
	theArxWorkspace.UnregisterDialog( this );
	TraceFmt( _T("< CArxDialogObject::~CArxDialogObject() [this: %08X]\r\n"), (long)this );
}

//static
CDialogObject* CArxDialogObject::Create( CDclFormObject* pDclForm, CWnd* pParent /*= NULL*/,
																				 DialogParams* pParams /*= NULL*/ )
{
	CAcAppContextModuleResourceOverride resOverride;
	switch( pDclForm->GetType() )
	{
	case VdclModal: return &(new CModalVDcl( pDclForm, pParent, pParams ))->GetDialogObject();
	case VdclModeless: return &(new CModelessDlg( pDclForm, pParent, pParams ))->GetDialogObject();
	case VdclConfigTab: return &(new CfgTabPane( pDclForm, pParent, pParams ))->GetDialogObject();
	case VdclDockable:
		{
			RefCountedPtr< CPropertyObject > pPropResizable = pDclForm->GetControlProperties()->GetPropertyObject( nResizable );
			if( pPropResizable && !pPropResizable->GetBooleanValue() ) //fixed size?
				return &(new CDockingDialog( pDclForm, pParent, pParams ))->GetDialogObject();
			return &(new CResizableDockingDialog( pDclForm, pParent, pParams ))->GetDialogObject();
		}
	case VdclFileDialog: return &(new CCustomFileDialog( pDclForm, pParent, pParams ))->GetDialogObject();
	}
	return NULL;
}
