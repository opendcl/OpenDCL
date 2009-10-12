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
	virtual bool IsAutoSorted() const { return false; }
	virtual bool PopulateList( CComboBox* pCombo ) { return false; }
	virtual CInputFilter* GetInputFilter() const { return mpFilter; }
};
