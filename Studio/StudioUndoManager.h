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

	virtual bool IsMultiplePropsChanged() const;

	// CUndoManager base class implementation
	void setEnabled( bool bEnabled = true ) override;
	size_t size() const override { return mUndoStack.size(); }
	bool AddAction( CUndoAction* pAction ) override;
	bool Undo( size_t ctActions = 1 ) override;
	bool GetUndoableActions( CStringArray& rsUndoActions, size_t ctMax = 0 ) override;
	bool SaveProject() override;
	bool BeginGroup( LPCTSTR pszDisplayName ) override;
	bool EndGroup() override;
	bool SelectForm( TDclFormPtr pForm ) override;
	bool AddForm( TDclFormPtr pForm ) override;
	bool DeleteForm( TDclFormPtr pForm ) override;
	bool SelectControl( TDclControlPtr pDclControl ) override;
	bool AddControl( TDclControlPtr pDclControl ) override;
	bool DeleteControl( TDclControlPtr pDclControl ) override;
	bool ReorderControl( TDclControlPtr pDclControl ) override;
	bool PropertyChange( TPropertyPtr pProperty ) override;
};
