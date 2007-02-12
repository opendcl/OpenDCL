#pragma once


#include "PropertyList.h"
#include "DclControlObject.h"


/////////////////////////////////////////////////////////////////////////////
// COleControlObject

class COleControlObject : public CDclControlObject
{
	CProject* mpProject;
protected:
	COleControlObject( const COleControlObject& ); //declared protected to prevent copy construction
	COleControlObject& operator=( const COleControlObject& ); //declared protected to prevent copy construction
public:
	COleControlObject( CProject* pOwner );
	COleControlObject( CProject* pOwner, const CLSID& clsid );
	~COleControlObject();
public:
	virtual const CProject* GetOwnerProject() const { return mpProject; }
	virtual CProject* GetOwnerProject() { return mpProject; }
};
