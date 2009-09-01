// ArxGraphicButtonCtrl.h : header file
//
// ARX specific functionality for GraphicButton control

#pragma once

#include "GraphicButtonCtrl.h"
#include "ArxControlServices.h"
#include "ArxDragDropService.h"


/////////////////////////////////////////////////////////////////////////////
// CArxGraphicButtonCtrl window

class CArxGraphicButtonCtrl : public CGraphicButtonCtrl
{
	CArxControlServices	mArxServices;
	CArxDragDropService mDragDropService;

public:
	CArxGraphicButtonCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxGraphicButtonCtrl();

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
