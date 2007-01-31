#pragma once

class CDclControlObject;


class CFontCollection : public CObject
{
	CList<CFont*> mFonts;
public:
	CFontCollection();	
	virtual ~CFontCollection();

public:		
	const CList<CFont*>& GetFontList() const { return mFonts; }
	CList<CFont*>& GetFontList() { return mFonts; }
	const CFont * GetFont(CDclControlObject *pControl, CWnd *pWnd);
	const CFont * GetFont(const CFont *pTargetFont, CWnd *pWnd);
};
