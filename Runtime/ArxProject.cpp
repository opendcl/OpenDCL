// ArxProject.cpp : implementation file
//

#include "stdafx.h"
#include "ArxProject.h"
#include "ArxWorkspace.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxProject

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
	for( TDclFormList::const_iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
	{
		if( (*iter)->GetType() == FrmOptionsTab && !theArxWorkspace.AddExtensionTab( *iter, pTabXM ) )
			bFailed = true;
	}
	return !bFailed;
}


bool CArxProject::SetProjectLispSymbols( bool bResetToNil /*= false*/ ) const
{
	for( TDclFormList::const_iterator iter = mDclForms.begin(); iter != mDclForms.end(); ++iter )
	{
		if( (*iter)->GetParentForm() )
			continue; //ignore child forms, since they will use the same name as their parent
		CString sVarName = (*iter)->GetVarName();
		if( !sVarName.IsEmpty() )
		{
			if( bResetToNil )
				theArxWorkspace.ResetLispSymbol( sVarName );
			else
				theArxWorkspace.SetLispSymbol( sVarName, (const CDclControlObject*)(*iter)->GetControlProperties(), odcl::ptrDclControl );
		}
	}
	return true;
}
