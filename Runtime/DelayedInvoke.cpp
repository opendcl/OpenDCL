// DelayedInvoke.cpp : implementation file
//

#include "stdafx.h"
#include "DelayedInvoke.h"
#include "InvokeMethod.h"


static void CALLBACK DelayedInvokeProc( HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime )
{
	CDelayedInvoke* pHandler = (CDelayedInvoke*)nIDEvent;
	delete pHandler;
}

/////////////////////////////////////////////////////////////////////////////
// CDelayedInvoke

CDelayedInvoke::CDelayedInvoke( LPCTSTR pszCallbackName, UINT cMsDelay )
: msCallback( pszCallbackName )
, mnTimerId( 0 )
{
	mnTimerId = AfxGetMainWnd()->SetTimer( (UINT_PTR)this, cMsDelay, DelayedInvokeProc );
}

CDelayedInvoke::~CDelayedInvoke()
{
	AfxGetMainWnd()->KillTimer( mnTimerId );
	InvokeEventHandler( msCallback, args_null(), true );
}
