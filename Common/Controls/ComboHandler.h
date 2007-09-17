// ComboHandler.h : header file
//

#pragma once

class CInputFilter;


/////////////////////////////////////////////////////////////////////////////
// CComboHandler class

class CComboHandler
{
public:
	CComboHandler() {}
	virtual ~CComboHandler() {}

public:
	virtual bool IsOwnerDrawn() const { return false; }
	virtual bool IsAutoSorted() const { return true; }
	virtual UINT GetItemHeight() const { return 0; }
	virtual void DrawItem( CComboBox* pCombo, LPDRAWITEMSTRUCT lpDrawItemStruct ) { return; }
	virtual bool PopulateList( CComboBox* pCombo ) = 0;
	virtual void OnDropdownClose( CComboBox* pCombo ) {}
	virtual CInputFilter* GetInputFilter() const { return NULL; }
};
