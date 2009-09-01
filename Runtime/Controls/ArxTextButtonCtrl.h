// ArxTextButton.h : header file
//

#pragma once

#include "TextButtonCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"


/////////////////////////////////////////////////////////////////////////////
// CArxTextButtonCtrl window

class CArxTextButtonCtrl : public CTextButtonCtrl
{
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;

public:
	CArxTextButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxTextButtonCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual CDragDropService* GetDragDropService() { return &mDragDropService; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );

protected:
	afx_msg void OnClicked();
	afx_msg void OnDoubleclicked();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

protected:
	DECLARE_MESSAGE_MAP();
};
