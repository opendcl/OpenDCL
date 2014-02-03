// ArxPlotStyleNameComboBoxCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "ArxControlServices.h"
#include "R2006AcUiMRUComboBoxFixup.h"


/////////////////////////////////////////////////////////////////////////////
// CArxPlotStyleNameComboBoxCtrl window

class CArxPlotStyleNameComboBoxCtrl : public CAcUiPlotStyleNamesComboBox, public CDialogControl
{
	CArxControlServices	mArxServices;

public:
	CArxPlotStyleNameComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxPlotStyleNameComboBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual CRect GetWndRect() const;
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyUseVisualStyle( TPropertyPtr pProp ); //Prop::UseVisualStyle

public:

protected:
	DECLARE_MESSAGE_MAP();

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchange();
	afx_msg void OnCbnDropdown();
};
