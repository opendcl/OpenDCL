// DclControlObject.cpp : implementation file
//

#include "stdafx.h"
#include "OleControlObject.h"
#include "ControlTypes.h"


COleControlObject::COleControlObject( ControlType type )
: CDclControlObject(type, NULL)
, mpProject( NULL )
{
}


COleControlObject::COleControlObject( CProject* pOwner )
: CDclControlObject(CtlActiveX, NULL)
, mpProject( pOwner )
{
}


COleControlObject::COleControlObject( CProject* pOwner, const CLSID& clsid )
: CDclControlObject(CtlActiveX, NULL)
, mpProject( pOwner )
{
	m_clsid = clsid;
}


COleControlObject::~COleControlObject()
{
}
