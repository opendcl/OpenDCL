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
#include "ParentFileDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDialogObject


CArxDialogObject::CArxDialogObject( CDclFormObject* pDclForm, CWnd* pHostDlg )
: CDialogObject( pDclForm, pHostDlg )
, mControlPane( pDclForm, pHostDlg )
{
	theArxWorkspace.RegisterDialog( this );
}

CArxDialogObject::~CArxDialogObject()
{
	theArxWorkspace.UnregisterDialog( this );
}

//static
CDialogObject* CArxDialogObject::Create( CDclFormObject* pDclForm, CWnd* pParent /*= NULL*/,
																				 DialogParams* pParams /*= NULL*/ )
{
	CAcModuleResourceOverride resOverride;
	switch( pDclForm->GetType() )
	{
	case VdclModal: return &(new CModalVDcl( pDclForm, pParent, pParams ))->GetDialogObject();
	case VdclModeless: return &(new CModelessDlg( pDclForm, pParent, pParams ))->GetDialogObject();
	case VdclConfigTab: return &(new CfgTabPane( pDclForm, pParent, pParams ))->GetDialogObject();
	case VdclDockable:
		{
			if( pParams && !pParams->bResizes ) //fixed size?
				return &(new CDockingDialog( pDclForm, pParent, pParams ))->GetDialogObject();
			return &(new CResizableDockingDialog( pDclForm, pParent, pParams ))->GetDialogObject();
		}
	case VdclFileDialog: return &(new CParentFileDialog( pDclForm, pParent, pParams ))->GetDialogObject();
	}
	return NULL;
}
