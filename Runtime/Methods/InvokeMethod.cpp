// InvokeMethod.cpp : implementation file
//

#include "stdafx.h"
#include "InvokeMethod.h"
#include "ArgumentsRetrieval.h"

extern bool IsOnlyModalForm();


static Acad::ErrorStatus ExecuteCommand( LPCTSTR pszCommand, bool bShowCommand = false, AcApDocument* pDoc = NULL )
{
	if( !pDoc )
		pDoc = acDocManager->curDocument();
	if( !pDoc )
		return Acad::eNoDocument;
	//CWnd* CmdBarWnd = acedGetAcadDockCmdLine();
	//CmdBarWnd->SetFocus();
	CString sCmd = pszCommand;
	if( sCmd.GetLength() > 256 )
	{
		//see if we can shorten it by replacing the first string with "..."
		int cchStart = sCmd.Find( _T('"') );
		if( cchStart > 0 )
		{
			int cchEnd = sCmd.Find( _T('"'), cchStart + 1 );
			if( cchEnd > cchStart )
				sCmd = sCmd.Left( cchStart + 1 ) + _T("<...>") + sCmd.Mid( cchEnd );
		}
		if( sCmd.GetLength() > 256 ) //still too long?
			return Acad::eStringTooLong;
	}
	Acad::ErrorStatus es = acDocManager->sendStringToExecute( pDoc, sCmd, false, true, bShowCommand );
#if !defined(_ZRXTARGET)
	switch( es )
	{
	case Acad::eNoDocument:
	case Acad::eBufferTooSmall:
		{
			if( !acDocManager->isApplicationContext() )
			{
				if( RTNORM == ads_queueexpr( sCmd.LockBuffer() ) )
					es = Acad::eOk;
			}
		}
		break;
	}
#endif //!defined(_ZRXTARGET)
	return es;
}

static int acedInvokeNoDocStateSafe(const struct resbuf *args, struct resbuf **result)
{
	AcApDocument* pDoc = acDocManager->curDocument();
	//assert( pDoc != NULL );
	if( !pDoc )
		return RTERROR;
	try
	{ //if the invoked function closes the current document, an exception will occur before acedInvoke returns
		int nResult = acedInvoke( args, result );
		assert( nResult == RTNORM );
	}
	catch( ... )
	{}
	return RTNORM;
}


CString FireCancel( LPCTSTR pszLispFunction )
{
	CString sLispFunction = pszLispFunction;
	
	if( sLispFunction.Left( 4 ).CompareNoCase( _T("^C^C") ) != 0 )
		return sLispFunction;

	//acdbHostApplicationServices()->
	Acad::ErrorStatus es = ExecuteCommand( _T("\x1B\x1B") );
	return sLispFunction.Mid( 4 );
}


bool InvokeEventHandler( LPCTSTR pszHandlerLispFunction,
												 const arg_b& args,
												 bool bAsync,
												 AcApDocument* pDoc /*= NULL*/ )
{
	if( !pszHandlerLispFunction || !*pszHandlerLispFunction )
		return true;
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	TraceFmt( _T("![%s] %s (%s )\r\n"), bAsync? _T("Async") : _T("Sync"), pszHandlerLispFunction, (LPCTSTR)args.asString() );
	CString sHandlerLispFunction = FireCancel( pszHandlerLispFunction );
	if( sHandlerLispFunction.IsEmpty() )
		return true;
	if( bAsync )
	{
		bool bShowCommand = (sHandlerLispFunction.Left( 2 ).CompareNoCase( _T("C:") ) != 0);
		CString sCommand;
		CString sArgs = args.asString();
		if( sArgs.IsEmpty() && bShowCommand && sHandlerLispFunction.Left( 1 ) != _T("_") )
			sCommand = sHandlerLispFunction + _T('\n');
		else
			sCommand.Format( _T("(%s%s)\n"), (LPCTSTR)sHandlerLispFunction, (LPCTSTR)sArgs );
		Acad::ErrorStatus es = ExecuteCommand( sCommand, bShowCommand, pDoc );
		assert (es == Acad::eOk );
		if( es != Acad::eOk )
			return false;
	}
	else
	{
		if( pDoc )
		{
			AcApDocManager* pDocMgr = acDocManager;
			if( pDocMgr )
			{
				try
				{
					acDocManager->activateDocument( pDoc );
				}
				catch( ... )
				{
				}
			}
		}
		resbuf* prbArgs = args.asResbuf();
		resbuf rbLispFunction = { prbArgs, RTSTR };
		rbLispFunction.resval.rstring = sHandlerLispFunction.LockBuffer();
		resbuf* prbResult = NULL;
		int stat = acedInvokeNoDocStateSafe( &rbLispFunction, &prbResult );
		if( prbArgs )
			acutRelRb( prbArgs );
		assert (stat == RTNORM );
		if( stat != RTNORM )
			return false;
		if( prbResult )
			acutRelRb( prbResult );
	}
	return true;
}


bool InvokeEventHandler( LPCTSTR pszHandlerLispFunction,
												 const resbuf* prbArgs,
												 resbuf*& prbResult,
												 AcApDocument* pDoc /*= NULL*/ )
{
	assert( pszHandlerLispFunction != NULL );
	assert( *pszHandlerLispFunction != NULL );
	assert( prbResult == NULL );
	CAcAppContextModuleResourceOverride resOverride( acedGetAcadResourceInstance() );
	if( pDoc )
		acDocManager->activateDocument( pDoc );
	TraceFmt( _T("![Sync-Result] %s (%s )\r\n"), pszHandlerLispFunction, _T("<resbuf>") );
	CString sHandlerLispFunction = FireCancel( pszHandlerLispFunction );
	if( sHandlerLispFunction.IsEmpty() )
		return true;
	resbuf rbLispFunction = { const_cast< resbuf* >(prbArgs), RTSTR };
	rbLispFunction.resval.rstring = sHandlerLispFunction.LockBuffer();
	int stat = acedInvokeNoDocStateSafe( &rbLispFunction, &prbResult );
	assert (stat == RTNORM );
	if( stat != RTNORM )
		return false;
	return true;
}


bool InvokeEventHandler( LPCTSTR pszHandlerLispFunction,
												 const arg_b& args,
												 resbuf*& prbResult,
												 AcApDocument* pDoc /*= NULL*/ )
{
	TraceFmt( _T("![Sync-Result] %s (%s )\r\n"), pszHandlerLispFunction, (LPCTSTR)args.asString() );
	resbuf* prbArgs = args.asResbuf();
	bool bStat = InvokeEventHandler( pszHandlerLispFunction, prbArgs, prbResult, pDoc );
	if( prbArgs )
		acutRelRb( prbArgs );
	return bStat;
}
