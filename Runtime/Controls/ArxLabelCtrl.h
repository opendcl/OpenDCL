// ArxLabelCtrl.h : header file
//

#pragma once

#include "LabelCtrl.h"
#include "ArxControlServices.h"
#include "OleOdcDropTarget.h"

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CArxLabelCtrl window

class CArxLabelCtrl : public CLabelCtrl
{
	CArxControlServices	mArxServices;
	COleOdcDropTarget mDropTarget;	

public:
	CArxLabelCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxLabelCtrl();

// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:
	void SetDragnDrop(BOOL bRegister);

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP();

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg __UINT_LRESULT OnNcHitTest(CPoint point);
};
