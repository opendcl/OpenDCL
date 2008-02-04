
#pragma once

class CFormControlManager;


// CControlDropSource command target

class CControlDropSource : public COleDropSource
{
	CFormControlManager* mpManager;

public:
	CControlDropSource( CFormControlManager* pManager );
	virtual ~CControlDropSource();

protected:
	DECLARE_MESSAGE_MAP()
};


