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
	bool mbIgnoreChange;

public:
	CComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, CComboHandler* pHandler = NULL, bool bCreate = true );
	virtual ~CComboBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;
	virtual void ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast );
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return CFilteredComboCtrl::GetColorService(); }

public:
	virtual DWORD GetComboStyle() const;
	virtual CComboHandler* GetComboHandler() { return mpHandler; }
	virtual const CComboHandler* GetComboHandler() const { return mpHandler; }

protected:
	virtual void OnListChanged();

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	virtual afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnCbnDropdown();
	afx_msg void OnCbnCloseup();
	afx_msg LRESULT OnModifyContent( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnResetContent( WPARAM wParam, LPARAM lParam );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
