// FilteredEditCtrl.h : header file
//

#pragma once

#include "EditCtrl.h"
#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CFilteredEditCtrl window

class CFilteredEditCtrl : public CEditCtrl
{
	CInputFilter* mpFilter;

public:
	CFilteredEditCtrl( CInputFilter* pFilter )
		: CEditCtrl()
		, mpFilter( pFilter )
		{}
	CFilteredEditCtrl( CWnd* pParentWnd, const CRect& rectWnd, UINT nID, CInputFilter* pFilter, DWORD dwStyle = GetDefaultWndStyle() )
		: CEditCtrl( pParentWnd, rectWnd, nID, dwStyle, pFilter )
		, mpFilter( pFilter )
		{}
	virtual ~CFilteredEditCtrl()
		{
			delete mpFilter;
		}

public:
	virtual CInputFilter* GetInputFilter() { return mpFilter; }
	virtual void SetInputFilter( CInputFilter* pFilter ) { delete mpFilter; mpFilter = pFilter; }
};
