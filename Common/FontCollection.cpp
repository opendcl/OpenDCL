// FontCollection.cpp : implementation file
//

#include "stdafx.h"
#include "FontCollection.h"
#include "DclControlObject.h"
#include "PropertyIds.h"


CFontCollection::CFontCollection()
{
}

CFontCollection::~CFontCollection()
{
	while( !mFonts.IsEmpty() )
	{
		CFont* pFont = mFonts.RemoveTail();
		pFont->DeleteObject();
		//delete pFont;
	}
}

const CFont * CFontCollection::GetFont(CDclControlObject *pControl, CWnd *pWnd)
{
	if (pControl->GetPropertyObject(nLabelName) == NULL)
		return NULL;

	LOGFONT stTargetFont = {0};
	lstrcpyn( stTargetFont.lfFaceName, pControl->GetStrProperty(nLabelName), LF_FACESIZE );
	int nFontSize = pControl->GetLngProperty(nLabelSize);
	stTargetFont.lfWeight = (pControl->GetBoolProperty(nLabelBold)? FW_BOLD : FW_NORMAL);
	stTargetFont.lfItalic = pControl->GetBoolProperty(nLabelItalic);
	stTargetFont.lfUnderline = pControl->GetBoolProperty(nLabelUnderline);
	stTargetFont.lfStrikeOut = pControl->GetBoolProperty(nLabelStrikeOut);
	BOOL bFontSizeStyle = pControl->GetBoolProperty(nFontSizeStyle);
	CPropertyObject *pFontSizeStyle = pControl->GetPropertyObject(nFontSizeStyle); //this is odd [ORW]
	if( bFontSizeStyle || pFontSizeStyle )
	{
		HWND hwndControl = (pWnd? pWnd->m_hWnd : NULL);
		HDC hDC = ::GetDC( hwndControl );
		int nPixelsY = GetDeviceCaps( hDC, LOGPIXELSY );
		::ReleaseDC( hwndControl, hDC );
		stTargetFont.lfHeight = -::MulDiv( nFontSize,	nPixelsY, 72 );
	}
	else
		stTargetFont.lfHeight = nFontSize;

	POSITION pos = mFonts.GetHeadPosition();
	while( pos )
	{
		CFont* pFont = mFonts.GetNext( pos );
		LOGFONT stLogFont;
		pFont->GetLogFont( &stLogFont );
		if( stTargetFont.lfFaceName != stLogFont.lfFaceName )
			continue;
		if( stTargetFont.lfHeight != stLogFont.lfHeight )
			continue;
		if( (stTargetFont.lfWeight == FW_BOLD) != (stLogFont.lfWeight == FW_BOLD) )
			continue;
		if( (stTargetFont.lfItalic != FALSE) != (stLogFont.lfItalic != FALSE) )
			continue;
		if( (stTargetFont.lfUnderline != FALSE) != (stLogFont.lfUnderline != FALSE) )
			continue;
		if( (stTargetFont.lfStrikeOut != FALSE) != (stLogFont.lfStrikeOut != FALSE) )
			continue;
		return pFont;
	}

	if( lstrcmpi( stTargetFont.lfFaceName, _T("kanji") ) == 0 )
		stTargetFont.lfCharSet = SHIFTJIS_CHARSET;
	else
		stTargetFont.lfCharSet = DEFAULT_CHARSET;
	stTargetFont.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	stTargetFont.lfOutPrecision = OUT_DEFAULT_PRECIS;
	stTargetFont.lfQuality = DEFAULT_QUALITY;
	stTargetFont.lfPitchAndFamily = DEFAULT_PITCH;
	stTargetFont.lfEscapement = 0;
	stTargetFont.lfOrientation = 0;
	CFont* pFont = new CFont;
	pFont->CreateFontIndirect( &stTargetFont );
	mFonts.AddTail( pFont );
	return pFont;
}

const CFont * CFontCollection::GetFont(const CFont *pTargetFont, CWnd *pWnd)
{
	LOGFONT stTargetFont;
	const_cast<CFont*>(pTargetFont)->GetLogFont( &stTargetFont );

	POSITION pos = mFonts.GetHeadPosition();
	while( pos )
	{
		CFont* pFont = mFonts.GetNext( pos );
		LOGFONT stLogFont;
		pFont->GetLogFont( &stLogFont );
		if( stTargetFont.lfFaceName != stLogFont.lfFaceName )
			continue;
		if( stTargetFont.lfHeight != stLogFont.lfHeight )
			continue;
		if( (stTargetFont.lfWeight == FW_BOLD) != (stLogFont.lfWeight == FW_BOLD) )
			continue;
		if( (stTargetFont.lfItalic != FALSE) != (stLogFont.lfItalic != FALSE) )
			continue;
		if( (stTargetFont.lfUnderline != FALSE) != (stLogFont.lfUnderline != FALSE) )
			continue;
		if( (stTargetFont.lfStrikeOut != FALSE) != (stLogFont.lfStrikeOut != FALSE) )
			continue;
		return pFont;
	}

	CFont* pFont = new CFont;
	pFont->CreateFontIndirect( &stTargetFont );
	mFonts.AddTail( pFont );
	return pFont;
}
