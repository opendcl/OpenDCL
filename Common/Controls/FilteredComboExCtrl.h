// FilteredComboExCtrl.h : header file
//

#pragma once

#include "ComboExCtrl.h"
#include "InputFilter.h"


/////////////////////////////////////////////////////////////////////////////
// CFilteredExComboCtrl window

class CFilteredComboExCtrl : public CComboExCtrl
{
	//CInputFilter* mpFilter;

public:
	CFilteredComboExCtrl( CInputFilter* pFilter )
		: CComboExCtrl()
		//, mpFilter( pFilter )
		{}
	CFilteredComboExCtrl( CWnd* pParentWnd, const CRect& rectWnd, DWORD dwComboStyle, UINT nID, CInputFilter* pFilter )
		: CComboExCtrl( pParentWnd, rectWnd, dwComboStyle, nID )
		//, mpFilter( pFilter )
		{}
	virtual ~CFilteredComboExCtrl()
		{
			//delete mpFilter;
		}

public:
	//CInputFilter* GetInputFilter() override { return mpFilter; }
};
