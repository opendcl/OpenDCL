// ImageComboBoxCtrl.h : header file
//

#pragma once

#include "FilteredComboExCtrl.h"
#include "DialogControl.h"

class CProject;
class CComboHandler;


/////////////////////////////////////////////////////////////////////////////
// CImageComboBoxCtrl window

class CImageComboBoxCtrl : public CFilteredComboExCtrl, public CDialogControl
{
	CComboHandler* mpHandler;
	bool mbIgnoreChange;

public:
	CImageComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, CComboHandler* pHandler = NULL, bool bCreate = true );
	virtual ~CImageComboBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyUseVisualStyle( TPropertyPtr pProp ); //Prop::UseVisualStyle
	virtual CAcadColorService* GetColorService() { return CFilteredComboExCtrl::GetColorService(); }

public:
	virtual DWORD GetComboStyle() const;
	virtual CComboHandler* GetComboHandler() { return mpHandler; }
	virtual const CComboHandler* GetComboHandler() const { return mpHandler; }
	int FindString(int nIndexStart, LPCTSTR lpszFind) const;
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const;

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
