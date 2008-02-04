#pragma once

class CControlManager;


// CControlGripper

class CControlGripper : public CStatic
{
	CControlManager* mpManager;
	bool mbThickFrame;

public:
	CControlGripper( CControlManager* pManager );
	virtual ~CControlGripper();

	void SetThickFrame( bool bThick = true ) { mbThickFrame = bThick; }

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnPaint();
};


