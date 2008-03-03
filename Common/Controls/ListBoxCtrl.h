// ListBoxCtrl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "AcadColorService.h"


/////////////////////////////////////////////////////////////////////////////
// CListBoxCtrl window

class CListBoxCtrl : public CListBox, public CDialogControl
{
	CAcadColorService mColorService;
	int mnDragSource;
	CRect mrcDropInsertMark;

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
	virtual bool OnApplyProperty( TPropertyPtr pProp );
	virtual CAcadColorService* GetColorService() { return &mColorService; }
	virtual DROPEFFECT OnBeginDrag( const CPoint& point, COleDataSource& SourceData );
	virtual DROPEFFECT OnDragEnter( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState );
	virtual DROPEFFECT OnDragOver( const CPoint& point, COleDataObject* pSourceData, DWORD dwKeyState );
	virtual void OnDragLeave();
	virtual bool OnDrop( const CPoint& point, COleDataObject* pSourceData, DROPEFFECT dropEffect );

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void PostNcDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
};
