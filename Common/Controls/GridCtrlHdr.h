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

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

protected:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};
