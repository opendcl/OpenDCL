#include "StdAfx.h"
#include "AcadSld.h"
#include "AcadColorTable.h"


static const CHAR szSlbHeader[] = "AutoCAD Slide Library 1.0\015\012\032\0\0\0";
static const CHAR szSldHeader[] = "AutoCAD Slide\015\012\032";

struct SlbEntry
{
	CHAR szName[32];
	DWORD dwOffset;
};


CAcadSld::CAcadSld(void)
: msizeSlide( -1, -1 )
, mdblAspect( 0 )
, meEndian( unknown )
{
}

CAcadSld::~CAcadSld(void)
{
}

CPoint CAcadSld::SldToClient( const CPoint& ptSld, const CRect& rcTarget )
{
	double dblHScale = (double)rcTarget.Width() / (double)msizeSlide.cx;
	double dblVScale = (double)rcTarget.Height() / (double)msizeSlide.cy;
	return CPoint( rcTarget.left + int(dblHScale * ptSld.x), rcTarget.bottom - int(dblVScale * ptSld.y) );
}

bool CAcadSld::Load( LPCTSTR pszFilename, LPCTSTR pszSlide /*= NULL*/ )
{
	mData.SetLength( 0 );
	msSlideName.Empty();
	meEndian = unknown;
	if( !pszFilename || !*pszFilename )
		return true;
	CFile fileSlide;
	if( !fileSlide.Open( pszFilename, CFile::modeRead | CFile::shareDenyWrite | CFile::osSequentialScan ) )
		return false;
	DWORD cbSlide = 0;
	if( pszSlide && *pszSlide )
	{ //slide from slide library (.slb file)
		CHAR* pszHeader = new CHAR[sizeof(szSlbHeader)];
		int cchHeader = fileSlide.Read( pszHeader, sizeof(szSlbHeader) );
		bool bHeaderOk = (cchHeader == (sizeof(szSlbHeader)) && memcmp( pszHeader, szSlbHeader, sizeof(szSlbHeader) ) == 0);
		delete [] pszHeader;
		if( !bHeaderOk )
			return false;
		CStringA sSlide( pszSlide );
		SlbEntry Sld;
		do
		{
			if( sizeof(SlbEntry) != fileSlide.Read( &Sld, sizeof(SlbEntry) ) )
				return false;
			if( !Sld.szName || !*Sld.szName )
				return false;
		} while( sSlide.CompareNoCase( Sld.szName ) != 0 );
		SlbEntry SldNext;
		if( sizeof(SlbEntry) != fileSlide.Read( &SldNext, sizeof(SlbEntry) ) )
			return false;
		if( !SldNext.szName || !*SldNext.szName )
			cbSlide = DWORD(fileSlide.GetLength()) - Sld.dwOffset;
		else
			cbSlide = SldNext.dwOffset - Sld.dwOffset;
		fileSlide.Seek( Sld.dwOffset, CFile::begin );
	}
	else
	{ //plain .sld file
		cbSlide = DWORD(fileSlide.GetLength());
	}
	CHAR* pszSldHeader = new CHAR[sizeof(szSldHeader)];
	int cchHeader = fileSlide.Read( pszSldHeader, sizeof(szSldHeader) );
	bool bHeaderOk = (cchHeader == (sizeof(szSldHeader)) && memcmp( pszSldHeader, szSldHeader, sizeof(szSldHeader) ) == 0);
	delete [] pszSldHeader;
	if( !bHeaderOk )
		return false;
	cbSlide -= sizeof(szSldHeader);
	BYTE type;
	if( sizeof(type) != fileSlide.Read( &type, sizeof(type) ) )
		return false;
	assert( type == 86 );
	if( type != 86 )
		return false;
	cbSlide -= sizeof(type);
	BYTE format;
	if( sizeof(format) != fileSlide.Read( &format, sizeof(format) ) )
		return false;
	switch( format )
	{
	case 1:
	case 2:
		break;
	default:
		assert( false );
		return false;
	}
	cbSlide -= sizeof(format);
	unsigned short nWidth;
	if( sizeof(nWidth) != fileSlide.Read( &nWidth, sizeof(nWidth) ) )
		return false;
	assert( nWidth < 0x1000 ); //this is pretty big -- it may be invalid
	cbSlide -= sizeof(nWidth);
	unsigned short nHeight;
	if( sizeof(nHeight) != fileSlide.Read( &nHeight, sizeof(nHeight) ) )
		return false;
	assert( nHeight < 0x1000 ); //this is pretty big -- it may be invalid
	cbSlide -= sizeof(nHeight);
	double aspect;
	unsigned short hfill;
	Endian endian = unknown;
	switch( format )
	{
	case 1:
		{
			float fAspect;
			if( sizeof(fAspect) != fileSlide.Read( &fAspect, sizeof(fAspect) ) )
				return false;
			cbSlide -= sizeof(fAspect);
			aspect = fAspect;
			if( sizeof(hfill) != fileSlide.Read( &hfill, sizeof(hfill) ) )
				return false;
			assert( hfill == 0 || hfill == 2 );
			cbSlide -= sizeof(hfill);
			fileSlide.Seek( 1, CFile::current ); //skip filler byte
			cbSlide -= 1;
		}
		break;
	case 2:
		{
			unsigned long lAspect;
			if( sizeof(lAspect) != fileSlide.Read( &lAspect, sizeof(lAspect) ) )
				return false;
			cbSlide -= sizeof(lAspect);
			aspect = (double(lAspect) / 1E7);
			if( sizeof(hfill) != fileSlide.Read( &hfill, sizeof(hfill) ) )
				return false;
			assert( hfill == 0 || hfill == 2 );
			cbSlide -= sizeof(hfill);
			unsigned short nTest;
			if( sizeof(nTest) != fileSlide.Read( &nTest, sizeof(nTest) ) )
				return false;
			switch( nTest )
			{
			case 0x1234:
				endian = little;
				break;
			case 0x3412:
				endian = big;
				break;
			default:
				return false;
			}
			cbSlide -= sizeof(nTest);
		}
		break;
	}
	BYTE* bufSlide = new BYTE [cbSlide];
	bool bGotSlide = (cbSlide == cbSlide != fileSlide.Read( bufSlide, cbSlide ));
	if( bGotSlide )
		mData.Write( bufSlide, cbSlide );
	delete [] bufSlide;
	if( !bGotSlide )
		return false;
	if( pszSlide && *pszSlide )
		msSlideName.Format( _T("%s(%s)"), pszFilename, pszSlide );
	else
		msSlideName = pszFilename;
	msizeSlide.SetSize( nWidth, nHeight );
	mdblAspect = aspect;
	meEndian = endian;
	return true;
}

bool CAcadSld::Draw( CDC* pDC, const CRect& rcDest )
{
	assert( pDC != NULL );
	if( !pDC )
		return false;
	if( mData.GetLength() == 0 )
		return true;
	mData.SeekToBegin();
	int nOldDC = pDC->SaveDC();
	pDC->IntersectClipRect( &rcDest );

	BYTE nColor = 7; //color 7 is "safe" (i.e. white on a dark background, black otherwise)
	COLORREF crColor = GetSafeRGBColor( nColor, getBackgroundColor() );
	CPen pen( PS_SOLID, 0, crColor );
	CBrush brush( crColor );
	pDC->SelectObject( &pen );
	pDC->SelectObject( &brush );

	CPoint ptCursor( 0, 0 );
	CArray< CPoint > rptSolid;
	unsigned short cptExpected = 0;

	bool bEndOfData = false;
	WORD wRecord;
	while( !bEndOfData && mData.Read( &wRecord, sizeof(wRecord) ) == sizeof(wRecord) )
	{
		BYTE type = (meEndian == big)? LOBYTE(wRecord) : HIBYTE(wRecord);
		BYTE lowbyte = (meEndian == big)? HIBYTE(wRecord) : LOBYTE(wRecord);
		switch( type )
		{
		case 0xF7: //undocumented 2-byte record
			{
			}
			break;
		case 0xFB: //offset vector
			{
				CHAR bytes[3];
				if( sizeof(bytes) == mData.Read( bytes, sizeof(bytes) ) )
				{
					CPoint ptFrom( ptCursor.x + (CHAR)lowbyte, ptCursor.y + bytes[0] );
					CPoint ptTo( bytes[1], bytes[2] );
					pDC->MoveTo( SldToClient( ptCursor, rcDest ) );
					pDC->LineTo( SldToClient( ptTo, rcDest ) );
					ptCursor = ptFrom;
				}
				else
					bEndOfData = true;
			}
			break;
		case 0xFC: //end of data
			bEndOfData = true;
			break;
		case 0xFD: //solid fill
			{
				assert( lowbyte == 0 );
				short x;
				short y;
				if( sizeof(x) == mData.Read( &x, sizeof(x) ) &&
						sizeof(y) == mData.Read( &y, sizeof(y) ) )
				{
					if( y >= 0 )
					{
						rptSolid.Add( SldToClient( CPoint( x, y ), rcDest ) );
						assert( rptSolid.GetCount() <= 10 ); //should never be more than 10
						assert( rptSolid.GetCount() <= cptExpected ); //exceeded specified count!
					}
					else if( rptSolid.IsEmpty() )
					{
						assert( x > 0 ); //count must be positive & non-zero
						cptExpected = x;
					}
					else
					{
						//assert( x == 0 ); //should be zero for ending record
						assert( rptSolid.GetCount() + 1 == cptExpected ); //count should match
						pDC->Polygon( rptSolid.GetData(), rptSolid.GetCount() );
						rptSolid.RemoveAll();
					}
				}
				else
					bEndOfData = true;
			}
			break;
		case 0xFE: //common endpoint vector
			{
				char y;
				if( sizeof(y) == mData.Read( &y, sizeof(y) ) )
				{
					CPoint ptTo( (CHAR)lowbyte, y );
					pDC->MoveTo( SldToClient( ptCursor, rcDest ) );
					pDC->LineTo( SldToClient( ptTo, rcDest ) );
					ptCursor = ptTo;
				}
				else
					bEndOfData = true;
			}
			break;
		case 0xFF: //new color
			{
				if( lowbyte == nColor )
					break; //no-op
				nColor = lowbyte;
				crColor = GetSafeRGBColor( nColor, getBackgroundColor() );
				pen.DeleteObject();
				pen.CreatePen( PS_SOLID, 0, crColor );
				brush.DeleteObject();
				brush.CreateSolidBrush( crColor );
				pDC->SelectObject( &pen );
				pDC->SelectObject( &brush );
			}
			break;
		default:
			{
				assert( type < 0x80 );
				if( type >= 0x80 )
					bEndOfData = true; //undefined type, so exit now
				// else vector
				signed short y;
				signed short x2;
				signed short y2;
				if( sizeof(y) == mData.Read( &y, sizeof(y) ) &&
						sizeof(x2) == mData.Read( &x2, sizeof(x2) ) &&
						sizeof(y2) == mData.Read( &y2, sizeof(y2) ) )
				{
					CPoint ptFrom( (type << 8) | lowbyte, y );
					CPoint ptTo( x2, y2 );
					pDC->MoveTo( SldToClient( ptFrom, rcDest ) );
					pDC->LineTo( SldToClient( ptTo, rcDest ) );
					ptCursor = ptFrom;
				}
				else
					bEndOfData = true;
			}
			break;
		}
	}
	pDC->RestoreDC( nOldDC );
	return (mData.GetPosition() == mData.GetLength());
}
