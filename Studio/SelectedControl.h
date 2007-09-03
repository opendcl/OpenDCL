// SelectedControl.h : header file
//

#pragma once

class CDclControlObject;
class CControlHolder;


/////////////////////////////////////////////////////////////////////////////
// SelectedControl window

class CSelectedControl : public CObject
{
	CDclControlObject* mpTemplate;
	CControlHolder* mpControlHolder;
	int mnIndex;
public:
	CRect m_rcLastDrawn;

public:
	CSelectedControl( CDclControlObject* pTemplate = NULL, CControlHolder* pControlHolder = NULL, int idxControl = -1 )
	: mpTemplate( pTemplate )
	, mpControlHolder( pControlHolder )
	, mnIndex( idxControl )
	, m_rcLastDrawn( 0, 0, 0, 0 )
	{
	}
	void Set( CDclControlObject* pTemplate = NULL, CControlHolder* pControlHolder = NULL, int idxControl = -1 )
	{	
		mpTemplate = pTemplate;
		mpControlHolder = pControlHolder;
		mnIndex = idxControl;
	}
	void Reset() { m_rcLastDrawn.SetRect( 0, 0, 0, 0 ); }
	void Clear()
	{	
		mpTemplate = NULL;
		mpControlHolder = NULL;
		mnIndex = -1;
	}
	CDclControlObject* GetTemplate() const { return mpTemplate; }
	CControlHolder* GetControlHolder() const { return mpControlHolder; }
	int GetIndex() const { return mnIndex; }
};
