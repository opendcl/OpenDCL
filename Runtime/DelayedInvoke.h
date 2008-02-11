// DelayedInvoke.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CDelayedInvoke

class CDelayedInvoke
{
	CString msCallback;
	UINT_PTR mnTimerId;
public:
	CDelayedInvoke( LPCTSTR pszCallbackName, UINT cMsDelay );
	~CDelayedInvoke();
};
