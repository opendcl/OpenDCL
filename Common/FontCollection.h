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
	CFont* GetFont(CDclControlObject *pControl, CWnd *pWnd);
	CFont* GetFont(const CFont *pTargetFont, CWnd *pWnd);
};
