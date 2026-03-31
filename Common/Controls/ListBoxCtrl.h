// ListBoxCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"
#include <set>


/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl window

class CListBoxCtrl : public CListBox, public CDialogControl
{
	CAcadColorService mColorService;
	int mnPrevSel;
	std::set< UINT > msetnDragSource;
	CRect mrcDropInsertMark;
	bool mbIgnoreChange;

// Construction
public:
	CListBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CListBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	bool Create( CWnd* pParentWnd, UINT nID ) override;
	DWORD GetWndStyle() const override;
	void HandleDpiChanged() override; //handle relayed WM_DPICHANGED_AFTERPARENT message
	bool ApplyPropertiesEnum() override;
	void ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast ) override;
	bool ApplyProperty( TPropertyPtr pProp ) override;
	CAcadColorService* GetColorService() override { return &mColorService; }
	DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData ) override;
	DROPEFFECT OnDragEnter( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState ) override;
	DROPEFFECT OnDragOver( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState ) override;
	void OnDragLeave() override;
	bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT& dropEffect ) override;

	virtual bool IsMultiSelect() const { return ((GetStyle() & (LBS_EXTENDEDSEL | LBS_MULTIPLESEL)) != 0); }
	virtual LPCTSTR GetDragTextPrefix() const { return NULL; }

protected:
	virtual void OnListChanged();

protected:
	DECLARE_MESSAGE_MAP()

	void PostNcDestroy() override;
	BOOL PreTranslateMessage(MSG* pMsg) override;
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg LRESULT OnModifyContent( WPARAM wParam, LPARAM lParam );
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnCbnSelchange();
	afx_msg LRESULT OnDpiChanged(WPARAM wParam, LPARAM lParam);
};
