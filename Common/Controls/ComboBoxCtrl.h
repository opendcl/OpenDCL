// ComboBoxCtrl.h : header file
//

#pragma once

#include "FilteredComboCtrl.h"
#include "DialogControl.h"

class CProject;
class CComboHandler;


/////////////////////////////////////////////////////////////////////////////
// CComboBoxCtrl window

class CComboBoxCtrl : public CFilteredComboCtrl, public CDialogControl
{
	CComboHandler* mpHandler;

public:
	CComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, CComboHandler* pHandler = NULL, bool bCreate = true );
	virtual ~CComboBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return CFilteredComboCtrl::GetColorService(); }

public:
	virtual DWORD GetComboStyle() const;
	virtual CComboHandler* GetComboHandler() { return mpHandler; }

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	virtual afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnDropdown();
	afx_msg void OnCloseUp();
};
