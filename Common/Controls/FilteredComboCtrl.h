// FilteredComboCtrl.h : header file
//

#pragma once

#include "ComboCtrl.h"
#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CFilteredComboCtrl window

class CFilteredComboCtrl : public CComboCtrl
{
	CInputFilter* mpFilter;

public:
	CFilteredComboCtrl( CInputFilter* pFilter )
		: CComboCtrl()
		, mpFilter( pFilter )
		{}
	CFilteredComboCtrl( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID, CInputFilter* pFilter )
		: CComboCtrl( pParentWnd, rectWnd, dwComboStyle, nID )
		, mpFilter( pFilter )
		{}
	virtual ~CFilteredComboCtrl()
		{
			delete mpFilter;
		}

public:
	CInputFilter* GetInputFilter() override { return mpFilter; }
};
