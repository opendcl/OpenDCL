#pragma once


#include "PropertyList.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// COleControlObject

class COleControlObject : public CDclControlObject
{
	CProject* mpProject;
public:
	COleControlObject( CProject* pOwner);
	COleControlObject( CProject* pOwner, const CLSID& clsid );
	virtual const CProject* GetOwnerProject() const { return mpProject; }
	virtual CProject* GetOwnerProject() { return mpProject; }
};
