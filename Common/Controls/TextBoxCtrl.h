// TextBoxCtrl.h : header file
//

#pragma once

#include "FilteredEditCtrl.h"
#include "DialogControl.h"

class CDclControlObject;
class CProject;
class CInputFilter;


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl window

class CTextBoxCtrl : public CFilteredEditCtrl, public CDialogControl
{

public:
	CTextBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, CInputFilter* pFilter = NULL, bool bCreate = true );
	virtual ~CTextBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return CFilteredEditCtrl::GetColorService(); }

public:

protected:
	DECLARE_MESSAGE_MAP();

protected:
	afx_msg void PostNcDestroy();
	afx_msg void OnEnChange();
};
