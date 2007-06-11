#pragma once

struct AxArg
{
	VARTYPE vt;
	CString name;
	CLSID clsid;
	AxArg() : vt( VT_EMPTY ) {}
};
