// InvokeMethod.h : header file
//

#pragma once

#include "EventArgs.h"


bool InvokeEventHandler( LPCTSTR pszHandlerLispFunction, const arg_b& args, bool bAsync, AcApDocument* pDoc = NULL );
bool InvokeEventHandler( LPCTSTR pszHandlerLispFunction, const resbuf* prbArgs, resbuf*& prbResult, AcApDocument* pDoc = NULL );
bool InvokeEventHandler( LPCTSTR pszHandlerLispFunction, const arg_b& args, resbuf*& prbResult, AcApDocument* pDoc = NULL );
