// UndoManager.h : header file
//

#pragma once

#include "Workspace.h"
#include "PtrTypes.h"

class CUndoAction;


class CUndoManager
{
	bool mbEnabled;

public:
	CUndoManager() : mbEnabled( true ) {}
	virtual ~CUndoManager() {}

	virtual bool enabled() const { return mbEnabled; }
	virtual void setEnabled( bool bEnabled = true ) { mbEnabled = bEnabled; }
	virtual size_t size() const = 0;

	virtual bool AddAction( CUndoAction* pAction ) = 0;
	virtual bool Undo( size_t ctActions = 1 ) = 0;

	virtual bool GetUndoableActions( CStringArray& rsUndoActions, size_t ctMax = 0 ) = 0;

	virtual bool SaveProject() = 0;
	virtual bool BeginGroup( LPCTSTR pszDisplayName ) = 0;
	virtual bool EndGroup() = 0;
	virtual bool SelectForm( TDclFormPtr pForm ) = 0;
	virtual bool AddForm( TDclFormPtr pForm ) = 0;
	virtual bool DeleteForm( TDclFormPtr pForm ) = 0;
	virtual bool SelectControl( TDclControlPtr pDclControl ) = 0;
	virtual bool AddControl( TDclControlPtr pDclControl ) = 0;
	virtual bool DeleteControl( TDclControlPtr pDclControl ) = 0;
	virtual bool ReorderControl( TDclControlPtr pDclControl ) = 0;
	virtual bool PropertyChange( TPropertyPtr pProperty ) = 0;
};


class AutoUndoGroup
{
	CUndoManager* mpUndoManager;
public:
	AutoUndoGroup( CUndoManager* pUndoManager, LPCTSTR pszDisplayName )
		: mpUndoManager( pUndoManager )
		{
			if( pUndoManager )
				pUndoManager->BeginGroup( pszDisplayName );
		}
	AutoUndoGroup( CUndoManager* pUndoManager, UINT nDisplayNameResId )
		: mpUndoManager( pUndoManager )
		{
			if( pUndoManager )
				pUndoManager->BeginGroup( theWorkspace.LoadResourceString( nDisplayNameResId ) );
		}
	~AutoUndoGroup()
		{
			if( mpUndoManager )
				mpUndoManager->EndGroup();
		}
};


class DisableUndoManager
{
	CUndoManager* mpUndoManager;
	bool mbWasEnabled;
public:
	DisableUndoManager( CUndoManager* pUndoManager )
		: mpUndoManager( pUndoManager )
		, mbWasEnabled( true )
		{
			if( pUndoManager )
			{
				mbWasEnabled = pUndoManager->enabled();
				pUndoManager->setEnabled( false );
			}
		}
	~DisableUndoManager()
		{
			if( mpUndoManager )
				mpUndoManager->setEnabled( mbWasEnabled );
		}
};
