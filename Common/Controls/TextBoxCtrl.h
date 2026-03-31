// TextBoxCtrl.h : header file
//

#pragma once

#include "FilteredEditCtrl.h"
#include "DialogControl.h"

class CProject;
class CInputFilter;


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl window

class CTextBoxCtrl : public CFilteredEditCtrl, public CDialogControl
{
	bool mbDragging;

public:
	CTextBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, CInputFilter* pFilter = NULL, bool bCreate = true );
	virtual ~CTextBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	void HandleDpiChanged() override; //handle relayed WM_DPICHANGED_AFTERPARENT message
	bool ApplyProperty( TPropertyPtr pProp ) override;
	bool OnApplyBackgroundColor( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return CFilteredEditCtrl::GetColorService(); }
	DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData ) override;
	bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect ) override;
	HBRUSH HandleCtlColor( CDC* pDC, UINT nCtlColor ) override;

protected:
	DECLARE_MESSAGE_MAP();

	BOOL PreTranslateMessage(MSG* pMsg) override;
	void PostNcDestroy() override;
	afx_msg void OnChange();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
