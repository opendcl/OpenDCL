// ControlGripper.h : header file
//

#pragma once


#include "GripWnd.h"

class CControlGripper
{
	CWnd* mpParentWnd;
	CGripWnd mGrip1;
	CGripWnd mGrip2;
	CGripWnd mGrip3;
	CGripWnd mGrip4;
	CGripWnd mGrip5;
	CGripWnd mGrip6;
	CGripWnd mGrip7;
	CGripWnd mGrip8;

public:
	CControlGripper( CWnd* pParent, bool bSelected = false );
	virtual ~CControlGripper(void);

protected:
	bool EnsureCreated();

public:
	bool IsVisible() const;
	void Hide();
	void MoveToTop();
	void MoveTo( const CRect& rcPos /*client coordinates*/, bool bShow = true );
	int HitTest( const CPoint& ptTest, HCURSOR& hCursor /*out*/ );
	void CancelSizing();
	void SetHot( bool bHot = true );
};
