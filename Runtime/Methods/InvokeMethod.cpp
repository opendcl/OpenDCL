// InvokeMethod.cpp : implementation file
//

#include "stdafx.h"
#include "InvokeMethod.h"
#include "ArgumentsRetrieval.h"

extern bool IsOnlyModalForm();


//This VL.Application code is left here for possible future use. It could not be used for
//determining at runtime how many arguments the tree control's OnDropFromControl event
//handler expects because handles won't make it through the COM portal. [ORW 2014-04-24]
//
//
//extern "C" IDispatch* AcadGetIDispatch( BOOL bAddRef );
//
//
//DISPID GetIdOfName(IDispatch* pDisp, LPOLESTR szMethod )
//{
//	DISPID dispid;
//	HRESULT hr = pDisp->GetIDsOfNames(IID_NULL, &szMethod, 1, LOCALE_SYSTEM_DEFAULT, &dispid);
//	if( FAILED(hr) )
//		return -1;
//	return dispid;
//}
//
//IDispatch* GetInterfaceObject( LPOLESTR szProgId )
//{
//	static CComPtr<IDispatch> pAcadDisp( AcadGetIDispatch( TRUE ) );
//	if(!pAcadDisp)
//		return NULL;
//	static DISPID dispidGetInterfaceObject = GetIdOfName( pAcadDisp, L"GetInterfaceObject");
//	if( dispidGetInterfaceObject < 0 )
//		return NULL;
//
//	HRESULT hr;
//	VARIANT vResult;
//	VariantInit( &vResult );
//	VARIANTARG vProgId = { VT_BSTR };
//	vProgId.bstrVal = bstr_t( szProgId );
//	DISPPARAMS dispArgs = {&vProgId, NULL, 1, 0};
//	EXCEPINFO stException = {};
//	hr = pAcadDisp->Invoke( dispidGetInterfaceObject,
//													IID_NULL,
//													LOCALE_SYSTEM_DEFAULT,
//													DISPATCH_METHOD,
//													&dispArgs,
//													&vResult,
//													&stException,
//													NULL );
//	VariantClear( &vProgId );
//	if(FAILED(hr))
//		return NULL;
//	IDispatch* pDisp = NULL;
//	if( vResult.vt == VT_DISPATCH )
//		pDisp = vResult.pdispVal;
//	VariantClear( &vResult );
//	return pDisp;
//}
//
//IDispatch* VlGetApplication()
//{
//	static bool bFailed = false;
//	if( bFailed )
//		return NULL; //bail quickly if we already tried and failed
//	IDispatch* pAcad = GetInterfaceObject( L"VL.Application.16" );
//	if( !pAcad )
//	{
//		resbuf rbAcadVer = {NULL};
//		int nResult = acedGetVar( _T("ACADVER"), &rbAcadVer );
//		CStringW sVerSuffix;
//		if( nResult == RTNORM )
//		{
//			sVerSuffix = L'.';
//			sVerSuffix += CStringW( rbAcadVer.resval.rstring ).SpanExcluding( L". " );
//		}
//		bstr_t sVLApp = L"VL.Application";
//		sVLApp += (LPCWSTR)sVerSuffix;
//		pAcad = GetInterfaceObject( sVLApp );
//		if( !pAcad )
//		{
//			bFailed = true;
//		#ifdef _DEBUG
//			OutputDebugString( _T("* Warning: OpenDCL could not obtain VL.Application interface!") );
//		#endif
//		}
//	}
//	return pAcad;
//}
//
//IDispatch* VlGetActiveDocument()
//{
//	CComPtr<IDispatch> pVlAppDisp( VlGetApplication() );
//	if( !pVlAppDisp )
//		return NULL;
//	static DISPID dispidActiveDocument = GetIdOfName( pVlAppDisp, L"ActiveDocument");
//	if( dispidActiveDocument < 0 )
//		return NULL;
//
//	HRESULT hr;
//	VARIANT vResult;
//	VariantInit( &vResult );
//	DISPPARAMS dispArgs = {NULL, NULL, 0, 0};
//	EXCEPINFO stException = {};
//	hr = pVlAppDisp->Invoke( dispidActiveDocument,
//													 IID_NULL,
//													 LOCALE_SYSTEM_DEFAULT,
//													 DISPATCH_PROPERTYGET,
//													 &dispArgs,
//													 &vResult,
//													 &stException,
//													 NULL );
//	if(FAILED(hr))
//		return NULL;
//	IDispatch* pDisp = NULL;
//	if( vResult.vt == VT_DISPATCH )
//		pDisp = vResult.pdispVal;
//	VariantClear( &vResult );
//	return pDisp;
//}
//
//IDispatch* VlGetActiveDocumentFunctionsCollection()
//{
//	CComPtr<IDispatch> pVlActiveDocDisp( VlGetActiveDocument() );
//	if( !pVlActiveDocDisp )
//		return NULL;
//	DISPID dispidFunctions = GetIdOfName( pVlActiveDocDisp, L"Functions");
//	if( dispidFunctions < 0 )
//		return NULL;
//
//	HRESULT hr;
//	VARIANT vResult;
//	VariantInit( &vResult );
//	DISPPARAMS dispArgs = {NULL, NULL, 0, 0};
//	EXCEPINFO stException = {};
//	hr = pVlActiveDocDisp->Invoke( dispidFunctions,
//																 IID_NULL,
//																 LOCALE_SYSTEM_DEFAULT,
//																 DISPATCH_PROPERTYGET,
//																 &dispArgs,
//																 &vResult,
//																 &stException,
//																 NULL );
//	if(FAILED(hr))
//		return NULL;
//	IDispatch* pDisp = NULL;
//	if( vResult.vt == VT_DISPATCH )
//		pDisp = vResult.pdispVal;
//	VariantClear( &vResult );
//	return pDisp;
//}
//
//IDispatch* VlGetActiveDocumentFunction( LPOLESTR szFunctionName )
//{
//	CComPtr<IDispatch> pFunctionsDisp( VlGetActiveDocumentFunctionsCollection() );
//	if(!pFunctionsDisp)
//		return NULL;
//	DISPID dispidItem = GetIdOfName( pFunctionsDisp, L"Item");
//	if( dispidItem < 0 )
//		return NULL;
//
//	HRESULT hr;
//	VARIANT vResult;
//	VariantInit( &vResult );
//	VARIANTARG vFunctionName = { VT_BSTR };
//	vFunctionName.bstrVal = bstr_t( szFunctionName );
//	DISPPARAMS dispArgs = {&vFunctionName, NULL, 1, 0};
//	EXCEPINFO stException = {};
//	hr = pFunctionsDisp->Invoke( dispidItem,
//															 IID_NULL,
//															 LOCALE_SYSTEM_DEFAULT,
//															 DISPATCH_PROPERTYGET,
//															 &dispArgs,
//															 &vResult,
//															 &stException,
//															 NULL );
//	VariantClear( &vFunctionName );
//	if(FAILED(hr))
//		return NULL;
//	IDispatch* pDisp = NULL;
//	if( vResult.vt == VT_DISPATCH )
//		pDisp = vResult.pdispVal;
//	VariantClear( &vResult );
//	return pDisp;
//}
//
//HRESULT VlEvalFunction( LPOLESTR szFunctionName, DISPPARAMS& stArgs, VARIANT& vResult )
//{
//	CComPtr<IDispatch> pFunctionDisp( VlGetActiveDocumentFunction( szFunctionName ) );
//	if(!pFunctionDisp)
//		return DISP_E_UNKNOWNINTERFACE;
//
//	HRESULT hr;
//	EXCEPINFO stException = {};
//	hr = pFunctionDisp->Invoke( DISPID_VALUE,
//															IID_NULL,
//															LOCALE_SYSTEM_DEFAULT,
//															DISPATCH_METHOD,
//															&stArgs,
//															&vResult,
//															&stException,
//															NULL );
//	return hr;
//}


class DelayedCommand : public AcEdInputContextReactor
{
	AcApDocument* mpDoc;
	CString sCmd;
public:
	DelayedCommand( AcApDocument* pDoc, LPCTSTR pszCommand ) : mpDoc( pDoc ), sCmd( pszCommand )
	{
		mpDoc->inputPointManager()->addInputContextReactor( this );
	}
	~DelayedCommand()
	{
		mpDoc->inputPointManager()->removeInputContextReactor( this );
	}
	virtual void beginQuiescentState()
	{
		ads_queueexpr( sCmd.LockBuffer() );
		delete this;
	}
};

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
	switch( es )
	{
	case Acad::eNoDocument:
	case Acad::eBufferTooSmall:
		{
			new DelayedCommand( pDoc, sCmd.LockBuffer() );
			es = Acad::eOk;
		}
		break;
	}
	return es;
}

static int acedInvokeNoDocStateSafe(const struct resbuf *args, struct resbuf **result)
{
	//VARIANT vResult;
	//DISPPARAMS stArgs = {NULL, NULL, 0, 0}; //TODO: convert resbuf list to VARARG arguments
	//HRESULT hr = VlEvalFunction(bstr_t(args->resval.rstring), stArgs, vResult);
	//if( SUCCEEDED(hr) )
	//	return RTNORM;

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
