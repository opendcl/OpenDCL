// ArxSplitterCtrl.h : header file
//

#pragma once

#include "SplitterCtrl.h"
#include "ArxControlServices.h"

#if (_MFC_VER < 0x0800)
#define __UINT_LRESULT UINT
#else
#define __UINT_LRESULT LRESULT
#endif


/////////////////////////////////////////////////////////////////////////////
// CArxSplitterCtrl window

class CArxSplitterCtrl : public CSplitterCtrl
{
	CArxControlServices	mArxServices;

public:
	CArxSplitterCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CArxSplitterCtrl();

protected:
	void SavePosition() const;
	bool ReadPosition();


// DialogControl Interface
public:
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual bool OnApplyProperty( TPropertyPtr pProp );

protected:
	DECLARE_MESSAGE_MAP();

protected:
	afx_msg void OnMove(int x, int y);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
};
