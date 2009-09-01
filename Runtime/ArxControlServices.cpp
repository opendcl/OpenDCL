// ArxControlServices.cpp : implementation file
//

#include "stdafx.h"
#include "ArxControlServices.h"
#include "ArxWorkspace.h"
#include "DialogControl.h"
#include "InvokeMethod.h"


/////////////////////////////////////////////////////////////////////////////
// CArxControlServices

CArxControlServices::CArxControlServices( CDialogControl* pDlgControl )
: mpDlgControl( pDlgControl )
, msLispSymbolName( pDlgControl->GetTemplate()->GetVarName() )
{
	SetLispSymbol();
}

CArxControlServices::~CArxControlServices()
{
	SetLispSymbol( true );
}

void CArxControlServices::SetLispSymbol( bool bResetToNil /*= false*/ ) const
{
	if( msLispSymbolName.IsEmpty() )
		return;
	if( !bResetToNil )
		theArxWorkspace.SetLispSymbol( msLispSymbolName, (const CDclControlObject*)mpDlgControl->GetTemplate(), odcl::ptrDclControl );
	else
		theArxWorkspace.ResetLispSymbol( msLispSymbolName );
}

bool CArxControlServices::HandleEvent( LPCTSTR pszHandlerName, resbuf*& prbResult, const resbuf* prbArgs ) const
{
	TDclControlPtr pTemplate = mpDlgControl->GetTemplate();
	if( !pTemplate->GetControlInstance() )
		return true;
	if( !pszHandlerName || !pszHandlerName )
		return false;
	InvokeEventHandler( pszHandlerName, prbArgs, prbResult, NULL );
	return (pTemplate->GetControlInstance() == NULL); //return true to abort if the event handler destroyed the control
}

bool CArxControlServices::HandleEvent( LPCTSTR pszHandlerName, resbuf*& prbResult, const arg_b& args /*= args_null()*/ ) const
{
	TDclControlPtr pTemplate = mpDlgControl->GetTemplate();
	if( !pTemplate->GetControlInstance() )
		return true;
	if( !pszHandlerName || !pszHandlerName )
		return false;
	InvokeEventHandler( pszHandlerName, args, prbResult, NULL );
	return (pTemplate->GetControlInstance() == NULL); //return true to abort if the event handler destroyed the control
}

bool CArxControlServices::HandleEvent( LPCTSTR pszHandlerName, bool bAsync, const arg_b& args /*= args_null()*/, AcApDocument* pDoc /*= NULL*/ ) const
{
	TDclControlPtr pTemplate = mpDlgControl->GetTemplate();
	if( !pTemplate->GetControlInstance() )
		return true;
	if( !pszHandlerName || !pszHandlerName )
		return false;
	InvokeEventHandler( pszHandlerName, args, bAsync, pDoc );
	return (pTemplate->GetControlInstance() == NULL); //return true to abort if the event handler destroyed the control
}

bool CArxControlServices::HandleEvent( LPCTSTR pszHandlerName, const arg_b& args /*= args_null()*/ ) const
{
	TDclControlPtr pTemplate = mpDlgControl->GetTemplate();
	if( !pTemplate->GetControlInstance() )
		return true;
	if( !pszHandlerName || !pszHandlerName )
		return false;
	InvokeEventHandler( pszHandlerName, args, mpDlgControl->IsAsyncEvents(), NULL );
	return (pTemplate->GetControlInstance() == NULL); //return true to abort if the event handler destroyed the control
}

bool CArxControlServices::HandleEvent( Prop::Id id, resbuf*& prbResult, const arg_b& args /*= args_null()*/ ) const
{
	TDclControlPtr pTemplate = mpDlgControl->GetTemplate();
	if( !pTemplate->GetControlInstance() )
		return true;
	CString sEventHandler = pTemplate->GetStringProperty( id );
	if( sEventHandler.IsEmpty() )
		return false;
	InvokeEventHandler( sEventHandler, args, prbResult, NULL );
	return (pTemplate->GetControlInstance() == NULL); //return true to abort if the event handler destroyed the control
}

bool CArxControlServices::HandleEvent( Prop::Id id, bool bAsync, const arg_b& args /*= args_null()*/ ) const
{
	TDclControlPtr pTemplate = mpDlgControl->GetTemplate();
	if( !pTemplate->GetControlInstance() )
		return true;
	CString sEventHandler = pTemplate->GetStringProperty( id );
	if( sEventHandler.IsEmpty() )
		return false;
	InvokeEventHandler( sEventHandler, args, bAsync, NULL );
	return (pTemplate->GetControlInstance() == NULL); //return true to abort if the event handler destroyed the control
}

bool CArxControlServices::HandleEvent( Prop::Id id, const arg_b& args /*= args_null()*/ ) const
{
	TDclControlPtr pTemplate = mpDlgControl->GetTemplate();
	if( !pTemplate->GetControlInstance() )
		return true;
	CString sEventHandler = pTemplate->GetStringProperty( id );
	if( sEventHandler.IsEmpty() )
		return false;
	InvokeEventHandler( sEventHandler, args, mpDlgControl->IsAsyncEvents(), NULL );
	return (pTemplate->GetControlInstance() == NULL); //return true to abort if the event handler destroyed the control
}
