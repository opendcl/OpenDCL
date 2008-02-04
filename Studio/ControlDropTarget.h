
#pragma once

class CFormControlManager;


// CControlDropTarget command target

class CControlDropTarget : public COleDropTarget
{
	CFormControlManager* mpManager;

public:
	CControlDropTarget( CFormControlManager* pManager );
	virtual ~CControlDropTarget();

protected:
	DECLARE_MESSAGE_MAP()
};


