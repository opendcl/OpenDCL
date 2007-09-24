// SelectedControl.h : header file
//

#pragma once

#include "DclControlObject.h"

class CControlHolder;


/////////////////////////////////////////////////////////////////////////////
// SelectedControl window

class CSelectedControl : public CObject
{
	TDclControlPtr mpTemplate;
	CControlHolder* mpControlHolder;
	int mnIndex;
public:
	CRect m_rcLastDrawn;

public:
	CSelectedControl( TDclControlPtr pTemplate = NULL, CControlHolder* pControlHolder = NULL, int idxControl = -1 )
	: mpTemplate( pTemplate )
	, mpControlHolder( pControlHolder )
	, mnIndex( idxControl )
	, m_rcLastDrawn( 0, 0, 0, 0 )
	{
	}
	void Set( TDclControlPtr pTemplate = NULL, CControlHolder* pControlHolder = NULL, int idxControl = -1 )
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
	TDclControlPtr GetTemplate() const { return mpTemplate; }
	CControlHolder* GetControlHolder() const { return mpControlHolder; }
	int GetIndex() const { return mnIndex; }
};
