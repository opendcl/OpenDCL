#include "stdafx.h"
#include "StdDropSource.h"

SCODE
CStdDropSource::QueryContinueDrag(BOOL fEsc, DWORD grfKeyState)
{
	if (fEsc)
		return  DRAGDROP_S_CANCEL;

	if (!(grfKeyState & MK_LBUTTON)) // No longer pressed
		return DRAGDROP_S_DROP;

	return S_OK;
}

SCODE CStdDropSource::GiveFeedback(DWORD dwEffect)
{
	return DRAGDROP_S_USEDEFAULTCURSORS;
}

