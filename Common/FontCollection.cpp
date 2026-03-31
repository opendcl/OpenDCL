// FontCollection.cpp : implementation file
//

#include "stdafx.h"
#include "FontCollection.h"
#include "DclControlTemplate.h"
#include "PropertyIds.h"
#include "DialogObject.h"


CFontCollection::CFontCollection()
{
}

CFontCollection::~CFontCollection()
{
	while( !mFonts.IsEmpty() )
	{
		CFont* pFont = mFonts.RemoveTail();
		pFont->DeleteObject();
		delete pFont;
	}
}

CFont* CFontCollection::GetFont(TDclControlPtr pControl, CControlPane *pWnd)
{
	if (pControl->GetPropertyObject(Prop::FontName) == NULL)
		return NULL;

	LOGFONT stTargetFont = {0};
	lstrcpyn( stTargetFont.lfFaceName, pControl->GetStringProperty(Prop::FontName), LF_FACESIZE );
	int nFontSize = pControl->GetLongProperty(Prop::FontSize);
	stTargetFont.lfWeight = (pControl->GetBooleanProperty(Prop::FontBold)? FW_BOLD : FW_NORMAL);
	stTargetFont.lfItalic = pControl->GetBooleanProperty(Prop::FontItalic);
	stTargetFont.lfUnderline = pControl->GetBooleanProperty(Prop::FontUnderline);
	stTargetFont.lfStrikeOut = pControl->GetBooleanProperty(Prop::FontStrikeout);if (pWnd)
	if( nFontSize > 0 )
	{ //calculate point size
		HWND hwndControl = (pWnd? pWnd->GetHostDialog()->m_hWnd : ::GetDesktopWindow());
		HDC hDC = ::GetDC( hwndControl );
		int nPixelsY = GetDeviceCaps( hDC, LOGPIXELSY );
		::ReleaseDC( hwndControl, hDC );
		stTargetFont.lfHeight = -::MulDiv( nFontSize, nPixelsY, 72 );
	}
	else
		stTargetFont.lfHeight = (pWnd? pWnd->GetDialogObject()->FromDIP( nFontSize ) : nFontSize);
	TraceFmt( _T("CFontCollection::GetFont(%s) => (%s/%d)\r\n"), asString( pControl ), stTargetFont.lfFaceName, stTargetFont.lfHeight );

	POSITION pos = mFonts.GetHeadPosition();
	while( pos )
	{
		CFont* pFont = mFonts.GetNext( pos );
		LOGFONT stLogFont;
		pFont->GetLogFont( &stLogFont );
		if( lstrcmpi(stTargetFont.lfFaceName, stLogFont.lfFaceName) != 0 )
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

CFont* CFontCollection::GetFont(const CFont *pTargetFont, CWnd *pWnd)
{
	LOGFONT stTargetFont;
	const_cast<CFont*>(pTargetFont)->GetLogFont( &stTargetFont );

	POSITION pos = mFonts.GetHeadPosition();
	while( pos )
	{
		CFont* pFont = mFonts.GetNext( pos );
		LOGFONT stLogFont;
		pFont->GetLogFont( &stLogFont );
		if( lstrcmpi( stTargetFont.lfFaceName, stLogFont.lfFaceName ) != 0 )
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
