// FilteredEditEx.h : header file
//

#pragma once

#include "EditEx.h"
#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CFilteredEditEx window

class CFilteredEditEx : public CEditEx
{
	CInputFilter* mpFilter;

public:
	CFilteredEditEx( CInputFilter* pFilter )
		: CEditEx()
		, mpFilter( pFilter )
		{}
	CFilteredEditEx( CWnd* pParentWnd, const CRect& rectWnd, UINT nID, CInputFilter* pFilter )
		: CEditEx( pParentWnd, rectWnd, nID )
		, mpFilter( pFilter )
		{}
	virtual ~CFilteredEditEx()
		{
			delete mpFilter;
		}

public:
	virtual CInputFilter* GetInputFilter() { return mpFilter; }
};
