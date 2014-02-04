// SelectedControl.h : header file
//

#pragma once

#include "DclControlTemplate.h"

class CDialogControl;


/////////////////////////////////////////////////////////////////////////////
// SelectedControl window

class CSelectedControl : public CObject
{
	TDclControlPtr mpTemplate;
	CControlManager* mpControlManager;
	int mnIndex;
public:
	CRect m_rcLastDrawn;

public:
	CSelectedControl( TDclControlPtr pTemplate = NULL, CControlManager* pControlManager = NULL, int idxControl = -1 )
	: mpTemplate( pTemplate )
	, mpControlManager( pControlManager )
	, mnIndex( idxControl )
	, m_rcLastDrawn( 0, 0, 0, 0 )
	{
	}
	CSelectedControl( const CSelectedControl& src )
	: mpTemplate( src.mpTemplate )
	, mpControlManager( src.mpControlManager )
	, mnIndex( src.mnIndex )
	, m_rcLastDrawn( src.m_rcLastDrawn )
	{
	}
	void Set( TDclControlPtr pTemplate = NULL, CControlManager* pControlManager = NULL, int idxControl = -1 )
	{	
		mpTemplate = pTemplate;
		mpControlManager = pControlManager;
		mnIndex = idxControl;
	}
	void Reset() { m_rcLastDrawn.SetRect( 0, 0, 0, 0 ); }
	void Clear()
	{	
		mpTemplate = NULL;
		mpControlManager = NULL;
		mnIndex = -1;
	}
	TDclControlPtr GetTemplate() const { return mpTemplate; }
	CControlManager* GetControlManager() const { return mpControlManager; }
	CDialogControl* GetControlInstance() const
		{ return mpTemplate? mpTemplate->GetControlInstance() : NULL; }
	int GetIndex() const { return mnIndex; }
	operator bool() const { return (!!mpControlManager); }
};
