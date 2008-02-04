// ControlDropTarget.cpp : implementation file
//

#include "stdafx.h"
#include "ControlDropTarget.h"
#include "DialogObject.h"


// CControlDropTarget

CControlDropTarget::CControlDropTarget( CFormControlManager* pManager )
: mpManager( pManager )
{
}

CControlDropTarget::~CControlDropTarget()
{
}


BEGIN_MESSAGE_MAP(CControlDropTarget, COleDropTarget)
END_MESSAGE_MAP()



// CControlDropTarget message handlers
