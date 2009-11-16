#pragma once

class CAcadSld
{
	CString msSlideName;
	CSize msizeSlide;
	double mdblAspect;
	enum Endian { unknown, little, big } meEndian;
	CMemFile mData;

public:
	CAcadSld(void);
	virtual ~CAcadSld(void);

protected:
	virtual COLORREF getBackgroundColor() const = 0;

public:
	bool Load( LPCTSTR pszFilename, LPCTSTR pszSlide = NULL );
	bool Draw( CDC* pDC, const CRect& rcDest );
	const CString& GetSlideName() const { return msSlideName; }
	const CSize& GetSlideSize() const { return msizeSlide; }

private:
	CPoint SldToClient( const CPoint& ptSld, const CRect& rcTarget );
};
