// ArxProject.h : header file
//

#pragma once

#include "Project.h"
#include "RefCountedPtr.h"


#define activeArxProject ((TArxProjectPtr)theWorkspace.GetActiveProject()) //shortcut to the active project

class CArxProject : public CProject
{
public:
	CArxProject();
	CArxProject( LPCTSTR pszKeyName );
	virtual ~CArxProject();

	//Overrides

	//Services
	bool OnExtendTabbedDialog( CAdUiTabExtensionManager* pTabXM ) const;
	bool SetProjectLispSymbols( bool bResetToNil = false ) const;

protected:
	DECLARE_SERIAL(CArxProject)
};

typedef RefCountedPtr< CArxProject > TArxProjectPtr;
typedef LockedPtr< CArxProject > TArxProjectLockedPtr;
