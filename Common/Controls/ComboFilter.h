// ComboFilter.h : header file
//

#pragma once

class CInputFilter;


/////////////////////////////////////////////////////////////////////////////
// CComboFilter class

class CComboFilter : public CComboHandler
{
	CInputFilter* mpFilter;

public:
	CComboFilter( CInputFilter* pFilter = NULL ) : mpFilter( pFilter ) {}
	virtual ~CComboFilter() { delete mpFilter; }

public:
	bool IsAutoSorted() const override { return false; }
	bool PopulateList( CComboBox* pCombo ) override { return false; }
	CInputFilter* GetInputFilter() const override { return mpFilter; }
};
