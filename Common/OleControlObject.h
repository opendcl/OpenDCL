#pragma once


#include "DclControlObject.h"
#include "Project.h"


/////////////////////////////////////////////////////////////////////////////
// COleControlObject

class COleControlObject : public CDclControlObject
{
	TProjectLockedPtr mpProject;
protected:
	COleControlObject( const COleControlObject& ); //declared protected to prevent copy construction
	COleControlObject& operator=( const COleControlObject& ); //declared protected to prevent copy construction
public:
	COleControlObject( ControlType type ); //special constructor for temporary controls used in the object browser
	COleControlObject( CProject* pOwner );
	COleControlObject( CProject* pOwner, const CLSID& clsid );
	~COleControlObject();
public:
	virtual const TProjectPtr GetOwnerProject() const { return mpProject; }
	virtual TProjectPtr GetOwnerProject() { return mpProject; }
};
