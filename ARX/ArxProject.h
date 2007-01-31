// ArxProject.h : header file
//

#pragma once

#include "Project.h"
#include "ArxWorkspace.h"

#define activeArxProject ((CArxProject*)theWorkspace.GetActiveProject()) //shortcut to the active project


class CArxProject : public CProject
{
public:
	CArxProject();
	CArxProject( LPCTSTR pszKeyName );
	virtual ~CArxProject();

	//Services
	bool OnExtendTabbedDialog( CAdUiTabExtensionManager* pTabXM ) const;
	bool UpdateGlobalVariables() const;

protected:
	DECLARE_SERIAL(CArxProject)
};
