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
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	CRect GetWndRect() const override;
	DWORD GetWndStyle() const override;
	void ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast ) override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return CFilteredComboCtrl::GetColorService(); }

public:
	virtual DWORD GetComboStyle() const;
	virtual CComboHandler* GetComboHandler() { return mpHandler; }
	virtual const CComboHandler* GetComboHandler() const { return mpHandler; }

protected:
	virtual void OnListChanged();

protected:
	DECLARE_MESSAGE_MAP();

protected:
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam) override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void PostNcDestroy() override;
	void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) override;
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnCbnDropdown();
	afx_msg void OnCbnCloseup();
	afx_msg LRESULT OnModifyContent( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnResetContent( WPARAM wParam, LPARAM lParam );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
