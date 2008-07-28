// StudioUndoManager.h : header file
//

#pragma once

#include "UndoManager.h"
#include <deque>
#include <set>
#include "UndoActions.h"

class CStudioProject;


class CStudioUndoManager : public CUndoManager
{
	CStudioProject* mpProject; //back pointer to owning project
	std::deque< CUndoAction* > mUndoStack;
	size_t mctGroups;
	std::set< TPropertyPtr > mGroupProps;

public:
	CStudioUndoManager( CStudioProject* pProject );
	virtual ~CStudioUndoManager();

	virtual bool IsMultiplePropsChanged() const { return (mGroupProps.size() > 1); }

	// CUndoManager base class implementation
	virtual void setEnabled( bool bEnabled = true );
	virtual size_t size() const { return mUndoStack.size(); }
	virtual bool AddAction( CUndoAction* pAction );
	virtual bool Undo( size_t ctActions = 1 );
	virtual bool GetUndoableActions( CStringArray& rsUndoActions, size_t ctMax = 0 );
	virtual bool BeginGroup( LPCTSTR pszDisplayName );
	virtual bool EndGroup();
	virtual bool SelectForm( TDclFormPtr pForm );
	virtual bool AddForm( TDclFormPtr pForm );
	virtual bool DeleteForm( TDclFormPtr pForm );
	virtual bool SelectControl( TDclControlPtr pDclControl );
	virtual bool AddControl( TDclControlPtr pDclControl );
	virtual bool DeleteControl( TDclControlPtr pDclControl );
	virtual bool ReorderControl( TDclControlPtr pDclControl );
	virtual bool PropertyChange( TPropertyPtr pProperty );
};
