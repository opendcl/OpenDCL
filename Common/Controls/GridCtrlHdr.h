// GridCtrlHdr.h : header file
//

#pragma once

class CGridCtrl;


/////////////////////////////////////////////////////////////////////////////
// CGridCtrlHdr window

class CGridCtrlHdr : public CHeaderCtrl
{
	CGridCtrl* mpParent;

// Construction
public:
	CGridCtrlHdr( CGridCtrl* pParent );
	virtual ~CGridCtrlHdr();

protected:
	DECLARE_MESSAGE_MAP()

// Generated message map functions
protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
