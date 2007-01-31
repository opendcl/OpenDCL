// CxAcadSlide.cpp: implementation of the CxAcadSlide class.
//

#include "stdafx.h"
#include "CxAcadSlide.h"
#include "AcadColorTable.h"
#include "Workspace.h"
#include "Resource.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHAR SlbHeader[32] = "AutoCAD Slide Library 1.0\015\012\32";
CHAR SldHeader[17] = "AutoCAD Slide\013\010\026";

struct CxSlideEntry
{
	TCHAR Name[32];
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
	m_FileName = "";
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
	TRACE("GdiGetBatchLimit: %i\n", GdiGetBatchLimit());
	// dump the last one
	FreeData();
	m_Data = new CMemFile();
	BYTE IdString[17];
	BYTE Type;
	WORD Width;
	WORD Height;
	DWORD Aspect;
	WORD Hardware;
	WORD Test;
	CFile file;
	try
	{
		if(!file.Open(filename, CFile::modeRead))
		{
			FreeData();
			return false;
		}
	}
	catch(...)
	{
		FreeData();
		return false;
	}
	DWORD size = 0;
	if(slb)
	{
		char tmp[32];
		ZeroMemory(tmp, sizeof(tmp));
		if((sizeof(tmp) != file.Read(tmp, sizeof(tmp))) || (lstrcmpA(tmp, SlbHeader) != 0))
			return false;
		TRACE("Slide Library Header read...\n");
		// strip parenthesis
		TRACE("Looking for %s...\n", slbSldName);
		CxSlideEntry sldentry;
		int namelen = 0;
		do {
			file.Read(&sldentry, sizeof(CxSlideEntry));
			TRACE("Slide entry for: %s, offset: %i\n", sldentry.Name, sldentry.Offset);
			namelen = _tcslen(sldentry.Name);
		}
		while((slbSldName.CompareNoCase(sldentry.Name) != 0) && (namelen > 0));
		
		if(namelen > 0)
		{
			CxSlideEntry next;
			// read next entry for offset info
			file.Read(&next, sizeof(CxSlideEntry));
			if(_tcslen(next.Name) > 0)
				size = next.Offset - sldentry.Offset;
			else
				size = DWORD(file.GetLength()) - sldentry.Offset - 31;
			file.Seek(sldentry.Offset, CFile::begin);
		}
		else
			return false;
	}
	else
		size = DWORD(file.GetLength()) - 31;
	// get header info
	file.Read(&IdString, sizeof(IdString));
	TRACE("IdString: %s\n", IdString);
	file.Read(&Type, sizeof(Type));
	TRACE("Type: %x\n", Type);
	file.Read(&Level, sizeof(Level));
	TRACE("Level: %x\n", Level);

	if (Level == 1)
		size = size - 3;

	file.Read(&Width, sizeof(Width));
	TRACE(_T("Width: %x\n"), Width);
	m_Width = Width;//((sldhead.Width >> 8) & 0xFF) | (sldhead.Width << 8);
	file.Read(&Height, sizeof(Height));
	TRACE(_T("Height: %x\n"), Height);
	m_Height = Height;//((sldhead.Height >> 8) & 0xFF) | (sldhead.Height << 8);

	if (Level == 2)
		file.Read(&Aspect, sizeof(Aspect));
	else
		file.Read(&Aspect, 8);

	TRACE(_T("Aspect: %i\n"), Aspect);
	m_AspectRatio = ((double) Aspect) / 1E7;
	file.Read(&Hardware, sizeof(Hardware));
	TRACE(_T("Hardware: %x\n"), Hardware);
	
	if (Level == 2)
		file.Read(&Test, sizeof(Test));
	else
		file.Read(&Test, 1);

	// get body of slide
	//char *buf = new char [size + 1];
	BYTE *buf = new BYTE [size + 1]; 
	file.Read(buf, size);

	m_Data->Write(buf, size);
	delete buf;
	
	if (Level == 1) 
	{
		theWorkspace.DisplayAlert(theWorkspace.LoadResourceString(IDS_OLDERSLD));
		if(m_Data)
			delete m_Data;
		m_Data = NULL;
	}
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
	COLORREF rgb;
	rgb = RGB(0,0,0);
	pen.CreatePen(PS_SOLID, 0, rgb);
	brush.CreateSolidBrush(rgb);
			
	CPoint polypoints[10];
	m_Data->SeekToBegin();
	WORD record, fromX, fromY, toX, toY, lastX, lastY, polycount;
	lastX = lastY = 0;
	float vscale = (float)rect.Height() / (float)m_Height;
	float hscale = (float)rect.Width() / (float)m_Width;
	BYTE color = 7, type, buf[8];
	
	int i = 0;

	try
	{
	if(!m_Data)
		return;
	
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
			if(!m_Data)
				return;
	
			m_Data->Read(buf, 3);
			if (Level == 2)
			{
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
			}
			else
			{
				if(m_LowFirst)
					toX =	lastX + HIBYTE(record);
				else
					toX =	lastX + LOBYTE(record);
				toY = lastY + buf[0];
				fromX = lastX + buf[1];
				fromY = lastY + buf[2];
				pDC->MoveTo(X(fromX), Y(fromY));
				pDC->LineTo(X(toX), Y(toY));
				lastX = fromX;
				lastY = fromY;
			}
			break;
		// common endpoint vector
		case 0xFE:
			if(!m_Data)
				return;
	
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
			if(!m_Data)
				return;
	
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
			{
				rgb = RGB(	acadcol[color][0],
					acadcol[color][1],
					acadcol[color][2]);
			}
				//rgb = RGB(acadcol[color][0], acadcol[color][1], acadcol[color][2]);
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
			if(!m_Data)
				return;
	
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
	catch(...)
	{
	}
}


void CxAcadSlide::Draw(HDC hdc, CRect& rect)
{
	CPoint point;
	if(!m_Data)
		return;
	int oldDC = SaveDC(hdc);
	short lastcolor = -1;
	HGDIOBJ pen = NULL;
	HBRUSH brush = NULL;
	
	CPoint polypoints[10];
	if(!m_Data)
		return;
	
	m_Data->SeekToBegin();
	WORD record, fromX, fromY, toX, toY, lastX, lastY, polycount;
	lastX = lastY = 0;
	float vscale = (float)rect.Height() / (float)m_Height;
	float hscale = (float)rect.Width() / (float)m_Width;
	BYTE color = 7, type, buf[8];
	COLORREF rgb;
	int i = 0;
	if(!m_Data)
		return;
	
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
			if(!m_Data)
				return;
	
			m_Data->Read(buf, 3);
			if(m_LowFirst)
				fromX =	lastX + HIBYTE(record);
			else
				fromX =	lastX + LOBYTE(record);
			fromY = lastY + buf[0];
			toX = lastX + buf[1];
			toY = lastY + buf[2];
			
			::MoveToEx(hdc, X(fromX), Y(fromY), &point);
			::LineTo(hdc, X(toX), Y(toY));
			lastX = fromX;
			lastY = fromY;
			break;
		// common endpoint vector
		case 0xFE:
			if(!m_Data)
				return;
	
			m_Data->Read(buf, 1);
			if(m_LowFirst)
				toX = lastX + HIBYTE(record);
			else
				toX = lastX + LOBYTE(record);
			toY = lastY + buf[0];
			::MoveToEx(hdc,X(lastX), Y(lastY), &point);
			::LineTo(hdc,X(toX), Y(toY));
			
			lastX = toX;
			lastY = toY;
			break;
		// solid fill
		case 0xFD:
			if(!m_Data)
				return;
	
			m_Data->Read(&polycount, sizeof(polycount));
			m_Data->Read(buf, 2);
			for(i = 0; i < polycount; i++)
			{
				if(!m_Data)
					return;
	
				m_Data->Read(buf, 6);
				polypoints[i].x = X(MAKEWORD(buf[2], buf[3]));
				polypoints[i].y = Y(MAKEWORD(buf[4], buf[5]));
			}
			polycount--;
			::Polygon(hdc, polypoints, polycount);
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
			{
				rgb = RGB(	acadcol[color][0],
							acadcol[color][1],
							acadcol[color][2]);
			}
				//rgb = RGB(acadcol[color][0], acadcol[color][1], acadcol[color][2]);
			lastcolor = color;
			if (pen)
				DeleteObject(pen);
			pen = ::CreatePen(PS_SOLID, 0, rgb);
			if (brush)
				DeleteObject(brush);
			brush = ::CreateSolidBrush(rgb);
			SelectObject(hdc, pen);
			SelectObject(hdc, brush);
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
			if(!m_Data)
				return;
	
			m_Data->Read(&fromY, sizeof(WORD));
			m_Data->Read(&toX, sizeof(WORD));
			m_Data->Read(&toY, sizeof(WORD));
			::MoveToEx(hdc, X(fromX), Y(fromY), &point);
			::LineTo(hdc, X(toX), Y(toY));
			lastX = fromX;
			lastY = fromY;
		}
	}
	RestoreDC(hdc, oldDC);
}
