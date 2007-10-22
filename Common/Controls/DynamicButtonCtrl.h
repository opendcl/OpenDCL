// DynamicButtonCtrl.h : header file
//
// Button control designed to be created and destroyed dynamically (in a grid control cell, for example)

#pragma once

#include "XPStyleButtonST.h"


/////////////////////////////////////////////////////////////////////////////
// CDynamicButtonCtrl window

class CDynamicButtonCtrl : public CXPStyleButtonST
{
	CWnd* mpParentWnd;

public:
	CDynamicButtonCtrl( CWnd* pParentWnd, const CRect& rectWnd, UINT nID );
	virtual ~CDynamicButtonCtrl();

protected:
	DECLARE_MESSAGE_MAP();

protected:
	afx_msg void OnClicked();	
};
