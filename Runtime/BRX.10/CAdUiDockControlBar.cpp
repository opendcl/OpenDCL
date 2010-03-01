// CAdUiDockControlBar implementation

#include "StdAfx.h"
#include "MFC/CAdUi/CAdUiDockControlBar.h"



LRESULT CAdUiDockControlBar::OnNcHitTest(CPoint pt)
{
	return CControlBar::OnNcHitTest(pt);
}

BOOL CAdUiDockControlBar::OnEraseBkgnd(CDC* pDC)
{
	return CControlBar::OnEraseBkgnd(pDC);
}
