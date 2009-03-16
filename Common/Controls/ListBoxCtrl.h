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
	std::set< UINT > setnDragSource;
	CRect mrcDropInsertMark;
	bool mbIgnoreChange;

// Construction
public:
	CListBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CListBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool ApplyPropertiesEnum();
	virtual void ApplyPropertiesOrder( std::vector< Prop::Id >& ridFirst, std::vector< Prop::Id >& ridLast );
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData );
	virtual DROPEFFECT OnDragEnter( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState );
	virtual DROPEFFECT OnDragOver( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState );
	virtual void OnDragLeave();
	virtual bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT dropEffect );

	bool IsMultiSelect() const { return ((GetStyle() & (LBS_EXTENDEDSEL | LBS_MULTIPLESEL)) != 0); }

protected:
	virtual void OnListChanged();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg LRESULT OnModifyContent( WPARAM wParam, LPARAM lParam );
};
