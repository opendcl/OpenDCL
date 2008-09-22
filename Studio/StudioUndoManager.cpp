// StudioUndoManager.cpp : implementation file
//

#pragma once

#include "stdafx.h"
#include "StudioUndoManager.h"
#include "StudioWorkspace.h"


CStudioUndoManager::CStudioUndoManager( CStudioProject* pProject )
: mpProject( pProject )
, mctGroups( 0 )
{
}

CStudioUndoManager::~CStudioUndoManager()
{
	while( !mUndoStack.empty() )
	{
		CUndoAction* pAction = mUndoStack.back();
		mUndoStack.pop_back();
		delete pAction;
	}
}

void CStudioUndoManager::setEnabled( bool bEnabled /*= true*/ )
{
	if( bEnabled != enabled() )
		theStudioWorkspace.OnFlushUndoGroup(); //end any open undo groups
	__super::setEnabled( bEnabled );
}

bool CStudioUndoManager::AddAction( CUndoAction* pAction )
{
	TraceFmt( _T("> CStudioUndoManager::AddAction(%s)\r\n"), (LPCTSTR)pAction->toString() );
	assert( enabled() );
	if( !enabled() && pAction->GetType() != Undo::EndGroup ) //EndGroup still allowed even when disabled
		return false;
	mUndoStack.push_back( pAction );
	if( pAction->GetType() == Undo::BeginGroup )
		++mctGroups;
	else if( pAction->GetType() == Undo::EndGroup )
	{
		assert( mctGroups > 0 );
		if( mctGroups > 0 )
			--mctGroups;
		if( mctGroups == 0 )
			mGroupProps.clear();
	}
	else
		theWorkspace.SetModified();
	return true;
}

bool CStudioUndoManager::Undo( size_t ctActions /*= 1*/ )
{
	TraceFmt( _T("> CStudioUndoManager::Undo(%u)\r\n"), ctActions );
	assert( ctActions > 0 );
	if( ctActions == 0 )
		return true;
	bool bWasEnabled = enabled();
	setEnabled( false );
	TDclControlPtr pActiveDclControl = theStudioWorkspace.GetActiveDclControl();
	theStudioWorkspace.OnFlushUndoGroup(); //end any open undo groups
	assert( /*mctGroups == 0 &&*/ mGroupProps.empty() );
	bool bFailed = false;
	size_t ctGroupsActive = 0;
	while( ctActions > 0 )
	{
		if( mUndoStack.empty() )
			break;
		CUndoAction* pAction = mUndoStack.back();
		TraceFmt( _T("  CUndoAction::Undo(%s)\r\n"), (LPCTSTR)pAction->toString() );
		bool bSuccess = pAction->Undo();
		assert( bSuccess == true );
		if( !bSuccess )
		{
			bFailed = true;
			break;
		}
		if( pAction->GetType() == Undo::EndGroup )
			++ctGroupsActive;
		else if( pAction->GetType() == Undo::BeginGroup )
			--ctGroupsActive;
		mUndoStack.pop_back();
		delete pAction;
		if( ctGroupsActive == 0 )
			--ctActions;
	}
	setEnabled( bWasEnabled );
	TraceFmt( _T("< CStudioUndoManager::Undo()\r\n") );
	return !bFailed;
}

bool CStudioUndoManager::GetUndoableActions( CStringArray& rsUndoActions, size_t ctMax /*= 0*/ )
{
	theStudioWorkspace.OnFlushUndoGroup(); //end any open undo groups
	size_t ctGroupsActive = 0;
	if( ctMax == 0 )
		ctMax = ~0;
	std::deque< CUndoAction* >::const_reverse_iterator iter = mUndoStack.rbegin();
	while( ctMax > 0 && iter != mUndoStack.rend() )
	{
		CUndoAction* pAction = (*iter++);
		if( pAction->GetType() == Undo::EndGroup )
		{
			++ctGroupsActive;
			continue;
		}
		if( pAction->GetType() == Undo::BeginGroup )
			--ctGroupsActive;
		if( ctGroupsActive == 0 )
		{
			rsUndoActions.Add( pAction->GetDisplayName() );
			--ctMax;
		}
	}
	return true;
}

bool CStudioUndoManager::BeginGroup( LPCTSTR pszDisplayName )
{
	if( !enabled() )
		return false;
	return AddAction( new CBeginGroupUA( pszDisplayName ) );
}

bool CStudioUndoManager::EndGroup()
{
	if( !enabled() )
		return false;
	return AddAction( new CEndGroupUA() );
}

bool CStudioUndoManager::SelectForm( TDclFormPtr pForm )
{
	if( !enabled() )
		return false;
	return AddAction( new CSelectFormUA( pForm ) );
}

bool CStudioUndoManager::AddForm( TDclFormPtr pForm )
{
	if( !enabled() )
		return false;
	return AddAction( new CAddFormUA( pForm ) );
}

bool CStudioUndoManager::DeleteForm( TDclFormPtr pForm )
{
	if( !enabled() )
		return false;
	return AddAction( new CDeleteFormUA( pForm ) );
}

bool CStudioUndoManager::SelectControl( TDclControlPtr pDclControl )
{
	if( !enabled() )
		return false;
	return AddAction( new CSelectControlUA( pDclControl ) );
}

bool CStudioUndoManager::AddControl( TDclControlPtr pDclControl )
{
	if( !enabled() )
		return false;
	return AddAction( new CAddControlUA( pDclControl ) );
}

bool CStudioUndoManager::DeleteControl( TDclControlPtr pDclControl )
{
	if( !enabled() )
		return false;
	return AddAction( new CDeleteControlUA( pDclControl ) );
}

bool CStudioUndoManager::ReorderControl( TDclControlPtr pDclControl )
{
	if( !enabled() )
		return false;
	return AddAction( new CReorderControlUA( pDclControl ) );
}

bool CStudioUndoManager::PropertyChange( TPropertyPtr pProperty )
{
	if( !enabled() )
		return false;
	if( mctGroups > 0 )
	{ //the property can be safely skipped if it was already saved since this group started
		if( mGroupProps.find( pProperty ) != mGroupProps.end() )
			return true;
	}
	if( !AddAction( new CPropertyChangeUA( pProperty ) ) )
		return false;
	if( mctGroups > 0 )
		mGroupProps.insert( pProperty );
	return true;
}
