// ControlDropSource.cpp : implementation file
//

#include "stdafx.h"
#include "ControlDropSource.h"
#include "ControlManager.h"


// CControlDropSource

CControlDropSource::CControlDropSource( CFormControlManager* pManager )
: mpManager( pManager )
{
}

CControlDropSource::~CControlDropSource()
{
}


BEGIN_MESSAGE_MAP(CControlDropSource, COleDropSource)
END_MESSAGE_MAP()



// CControlDropSource message handlers
