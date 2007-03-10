// CxAcadSlide.h: interface for the CxAcadSlide class.
//

#pragma once

const int nModelSpace = -22;
const int nPaperSpace = -23;


class CxAcadSlide : public CObject
{
public:
	void FreeData();
	bool m_Redraw;
	double m_AspectRatio;
	CxAcadSlide();
	CxAcadSlide(COLORREF backcolor);
	virtual ~CxAcadSlide();
	bool Load(CString filename, bool slb, CString slbSldName);
	void Draw(CDC *pDC, CRect& rect);
	void Draw(HDC hdc, CRect& rect);
	void SetBackColor(COLORREF newBkColor);
	WORD m_Height;
	WORD m_Width;
	CString m_FileName;
	COLORREF m_BkColor;

protected:
	bool m_LowFirst;
	CMemFile *m_Data;
	BYTE Level;
};
