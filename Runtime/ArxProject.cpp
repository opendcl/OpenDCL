// ArxProject.cpp : implementation file
//

#include "stdafx.h"
#include "ArxProject.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxProject

IMPLEMENT_SERIAL(CArxProject, CProject, 1)

CArxProject::CArxProject()
{
}


CArxProject::CArxProject( LPCTSTR pszKeyName )
: CProject( pszKeyName )
{
}


CArxProject::~CArxProject()
{
}


bool CArxProject::OnExtendTabbedDialog( CAdUiTabExtensionManager* pTabXM ) const
{
	if (!pTabXM)
		return false;
	bool bFailed = false;
	POSITION posDclForm = mDclForms.GetHeadPosition();
	while (posDclForm)
	{
		CDclFormObject *pDclForm = mDclForms.GetNext(posDclForm);
		if (pDclForm && pDclForm->GetType() == VdclConfigTab)
		{
			if (!theArxWorkspace.AddExtensionTab(pDclForm, pTabXM))
				bFailed = true;
		}
	}
	return !bFailed;
}


bool CArxProject::SetProjectLispSymbols( bool bResetToNil /*= false*/ ) const
{
	POSITION posDclForm = mDclForms.GetHeadPosition();
	while( posDclForm != NULL )
	{
		CDclFormObject *pDclForm = mDclForms.GetNext( posDclForm );
		assert( pDclForm != NULL);
		if( !pDclForm )
			continue;
		if( pDclForm->GetParentForm() )
			continue; //ignore child forms, since they will use the same name as their parent
		CDclControlObject* pPropertiesControl = pDclForm->GetControlProperties();
		if( pPropertiesControl )
		{
			// Get the variable name
			CString sVarName = pDclForm->GetControlProperties()->GetStrProperty( nGlobalVarName );
			if( sVarName.IsEmpty() )
				sVarName = pDclForm->GetKeyPath();
			if( !sVarName.IsEmpty() )
			{
				if( bResetToNil )
					theArxWorkspace.ResetLispSymbol( sVarName );
				else
					theArxWorkspace.SetLispSymbol( sVarName, (long)pDclForm );
			}
		}
	}
	return true;
}
