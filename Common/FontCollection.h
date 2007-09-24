#pragma once

#include "PtrTypes.h"


class CFontCollection : public CObject
{
	CList<CFont*> mFonts;
public:
	CFontCollection();	
	virtual ~CFontCollection();

public:		
	const CList<CFont*>& GetFontList() const { return mFonts; }
	CList<CFont*>& GetFontList() { return mFonts; }
	CFont* GetFont(TDclControlPtr pControl, CWnd *pWnd);
	CFont* GetFont(const CFont *pTargetFont, CWnd *pWnd);
};
