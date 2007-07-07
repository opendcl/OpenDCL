// Vdcl3DRect.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Vdcl3DRect.h"
/*
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
*/
/////////////////////////////////////////////////////////////////////////////
// Vdcl3DRect

Vdcl3DRect::Vdcl3DRect()
{
}

Vdcl3DRect::~Vdcl3DRect()
{
}


void Vdcl3DRect::PaintRect(CWnd* pParentWnd) 
{
	
	COLORREF TopLeft = ::GetSysColor(COLOR_BTNSHADOW);
	COLORREF BottomRight = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	
	
	pParentWnd->GetDC()->Draw3dRect(190, 260, 190, 260, TopLeft, BottomRight);
}
