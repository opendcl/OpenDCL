#pragma once

struct AxArg
{
	VARTYPE vt;
	CString name;
	CLSID clsid;
	bool optional;
	AxArg() : vt( VT_EMPTY ), optional( false ) {}
};
