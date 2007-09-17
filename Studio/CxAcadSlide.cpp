// CxAcadSlide.cpp: implementation of the CxAcadSlide class.
//

#include "stdafx.h"
#include "CxAcadSlide.h"
#include "AcadColorTable.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

char SlbHeader[32] = "AutoCAD Slide Library 1.0\015\012\32";
char SldHeader[17] = "AutoCAD Slide\013\010\026";

struct CxSlideEntry
{
	char Name[32];
	DWORD Offset;
};

struct CxSldHeader 
{
	BYTE IdString[17];
	WORD TypeLevel;
	WORD Width;
	WORD Height;
	DWORD Aspect;
	WORD Hardware;
	WORD Test;
};

CxAcadSlide::CxAcadSlide()
{
	m_Redraw = true;
	m_Data = NULL;
	m_Width = 0;
	m_Height = 0;
	m_LowFirst = false;
	m_BkColor = RGB(0,0,0);
	m_FileName;
}

CxAcadSlide::CxAcadSlide(COLORREF backcolor)
{
	m_Redraw = true;
	m_Data = NULL;
	m_Width = 0;
	m_Height = 0;
	m_LowFirst = false;
	m_BkColor = backcolor;
}

CxAcadSlide::~CxAcadSlide()
{
	FreeData();
}

void CxAcadSlide::FreeData()
{
	if(m_Data)
		delete m_Data;
	m_Data = NULL;
}

void CxAcadSlide::SetBackColor(COLORREF newBkColor)
{
	m_BkColor = newBkColor;
	m_Redraw = true;
}

bool CxAcadSlide::Load(CString filename, bool slb, CString slbSldName)
{
	m_FileName = filename;
	// dump the last one
	FreeData();
	m_Data = new CMemFile();
	BYTE IdString[17];
	BYTE Type;
	BYTE Level;
	WORD Width;
	WORD Height;
	DWORD Aspect;
	WORD Hardware;
	WORD Test;
	CFile file;
	if(!file.Open(filename, CFile::modeRead))
	{
		FreeData();
		return false;
	}
	DWORD size = 0;
	if(slb)
	{
		char tmp[32];
		ZeroMemory(tmp, 32);
		if((32 != file.Read(tmp, 32)) || (lstrcmpA(tmp, SlbHeader) != 0))
			return false;
		// strip parenthesis
		CxSlideEntry sldentry;
		UINT_PTR namelen = 0;
		do {
			file.Read(&sldentry, sizeof(CxSlideEntry));
			namelen = lstrlenA(sldentry.Name);
		}
		while((CStringA(slbSldName).CompareNoCase(sldentry.Name) != 0) && (namelen > 0));
		if(namelen > 0)
		{
			CxSlideEntry next;
			// read next entry for offset info
			file.Read(&next, sizeof(CxSlideEntry));
			if(lstrlenA(next.Name) > 0)
				size = next.Offset - sldentry.Offset;
			else
				size = DWORD(file.GetLength()) - sldentry.Offset;
			file.Seek(sldentry.Offset, CFile::begin);
		}
		else
			return false;
	}
	else
		size = DWORD(file.GetLength()) - 31;
	// get header info
	file.Read(&IdString, sizeof(IdString));
	file.Read(&Type, sizeof(Type));
	file.Read(&Level, sizeof(Level));
	file.Read(&Width, sizeof(Width));
	m_Width = Width;//((sldhead.Width >> 8) & 0xFF) | (sldhead.Width << 8);
	file.Read(&Height, sizeof(Height));
	m_Height = Height;//((sldhead.Height >> 8) & 0xFF) | (sldhead.Height << 8);
	file.Read(&Aspect, sizeof(Aspect));
	m_AspectRatio = ((double) Aspect) / 1E7;
	file.Read(&Hardware, sizeof(Hardware));
	file.Read(&Test, sizeof(Test));
	if(Test != 0x1234)
		m_LowFirst = true;
	// get body of slide
	char *buf = new char [size + 1];
	file.Read(buf, size);
//	temp.WriteHuge(buf, size);
	m_Data->Write(buf, size);
	delete buf;
	return true;
}

#define X(x) (int)((hscale * x) + rect.left)
#define Y(y) (int)(rect.Height() - (vscale * y) + rect.top)

void CxAcadSlide::Draw(CDC *pDC, CRect& rect)
{
	if(!m_Data)
		return;
	int oldDC = pDC->SaveDC();
	short lastcolor = -1;
	CPen pen;
	CBrush brush;
	CPoint polypoints[10];
	m_Data->SeekToBegin();
	WORD record, fromX, fromY, toX, toY, lastX, lastY, polycount;
	lastX = lastY = 0;
	float vscale = (float)rect.Height() / (float)m_Height;
	float hscale = (float)rect.Width() / (float)m_Width;
	BYTE color = 7, type, buf[8];
	COLORREF rgb;
	int i = 0;
	while(m_Data->Read(&record, sizeof(record)) == sizeof(record))
	{
		if(m_LowFirst)
			type = LOBYTE(record);
		else
			type = HIBYTE(record);
		switch(type)
		{
		// offset vector
		case 0xFB:
			m_Data->Read(buf, 3);
			if(m_LowFirst)
				fromX =	lastX + HIBYTE(record);
			else
				fromX =	lastX + LOBYTE(record);
			fromY = lastY + buf[0];
			toX = lastX + buf[1];
			toY = lastY + buf[2];
			pDC->MoveTo(X(fromX), Y(fromY));
			pDC->LineTo(X(toX), Y(toY));
			lastX = fromX;
			lastY = fromY;
			break;
		// common endpoint vector
		case 0xFE:
			m_Data->Read(buf, 1);
			if(m_LowFirst)
				toX = lastX + HIBYTE(record);
			else
				toX = lastX + LOBYTE(record);
			toY = lastY + buf[0];
			pDC->MoveTo(X(lastX), Y(lastY));
			pDC->LineTo(X(toX), Y(toY));
			lastX = toX;
			lastY = toY;
			break;
		// solid fill
		case 0xFD:
			m_Data->Read(&polycount, sizeof(polycount));
			m_Data->Read(buf, 2);
			for(i = 0; i < polycount; i++)
			{
				m_Data->Read(buf, 6);
				polypoints[i].x = X(MAKEWORD(buf[2], buf[3]));
				polypoints[i].y = Y(MAKEWORD(buf[4], buf[5]));
			}
			polycount--;
			pDC->Polygon(polypoints, polycount);
			break;
		// new color
		case 0xFF:
			if(m_LowFirst)
				color = HIBYTE(record);
			else
				color = LOBYTE(record);
			if(color == lastcolor)
				break;
			if(color == 7)
			{
				if(m_BkColor < RGB(128,128,128))
					rgb = RGB(255,255,255);
				else
					rgb = RGB(0,0,0);
			}
			else
				rgb = GetRGBColor( color );
			lastcolor = color;
			pen.DeleteObject();
			pen.CreatePen(PS_SOLID, 0, rgb);
			brush.DeleteObject();
			brush.CreateSolidBrush(rgb);
			pDC->SelectObject(&pen);
			pDC->SelectObject(&brush);
			break;
		// end of data
		case 0xFC:
			break;
		default:
			// undefined
			if((type >= 0x80) && (type <= 0xFA))
				break;
			// else vector
			fromX = record;
			m_Data->Read(&fromY, sizeof(WORD));
			m_Data->Read(&toX, sizeof(WORD));
			m_Data->Read(&toY, sizeof(WORD));
			pDC->MoveTo(X(fromX), Y(fromY));
			pDC->LineTo(X(toX), Y(toY));
			lastX = fromX;
			lastY = fromY;
		}
	}
	pDC->RestoreDC(oldDC);
}

