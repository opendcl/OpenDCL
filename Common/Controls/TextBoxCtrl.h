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
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual void HandleDpiChanged(); //handle relayed WM_DPICHANGED_AFTERPARENT message
	virtual bool ApplyProperty( TPropertyPtr pProp );
	virtual bool OnApplyBackgroundColor( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return CFilteredEditCtrl::GetColorService(); }
	virtual DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData );
	virtual bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect );
	virtual HBRUSH HandleCtlColor( CDC* pDC, UINT nCtlColor );

protected:
	DECLARE_MESSAGE_MAP();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual afx_msg void PostNcDestroy();
	afx_msg void OnChange();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
