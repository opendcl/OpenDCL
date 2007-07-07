// OptionListBox.h : header file
//

#pragma once

#include "AcadColorListBox.h"


/////////////////////////////////////////////////////////////////////////////
// COptionListBox window

class COptionListBox : public CAcadColorListBox
{
// Construction
public:
	COptionListBox();

// Attributes
public:
	short m_RowHeight;
	short m_NextHeight;
	CStringArray m_TTTList;

// Operations
public:
	void SetRowHeight(int nNewHeight);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~COptionListBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(COptionListBox)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
