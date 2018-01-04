#include "stdafx.h"
#include "ThemeAPI.h"

// Copyright 2013 - 2018 ManuSoft. All Rights Reserved.
// http://www.manusoft.com
//
// A license to use the code in this file for the OpenDCL project has been granted
// by Owen Wengerd and ManuSoft. This code is provided under the terms of the GPL
// license governing OpenDCL. This code may not be used for any other purpose
// without the express written consent of Owen Wengerd or ManuSoft.


class ThemeAPImportTable
{
	bool mbInitialized;
	HMODULE mhmodUxTheme;

public:
	ThemeAPImportTable() : mbInitialized( false ), mhmodUxTheme( NULL ) {}
	virtual ~ThemeAPImportTable() { FreeLibrary( mhmodUxTheme ); }

protected:
	bool check(void*& pfTarget, LPCSTR pszFuncName)
	{
		if( pfTarget )
			return true;
		if( !mbInitialized )
		{
			mbInitialized = true;
			mhmodUxTheme = LoadLibrary( _T("UxTheme.dll") );
		}
		if( mhmodUxTheme )
			pfTarget = GetProcAddress( mhmodUxTheme, pszFuncName );
		return (pfTarget != NULL);
	}

private:
	// API function type declarations
	typedef BOOL (WINAPI *F_BeginPanningFeedback)( HWND hwnd );
	typedef BOOL (WINAPI *F_UpdatePanningFeedback)( HWND hwnd, LONG lTotalOverpanOffsetX, LONG lTotalOverpanOffsetY, BOOL fInInertia );
	typedef BOOL (WINAPI *F_EndPanningFeedback)( HWND hwnd, BOOL fAnimateBack );
	typedef HTHEME (STDAPICALLTYPE *F_OpenThemeData)( HWND hwnd, LPCWSTR pszClassList );
	typedef HTHEME (STDAPICALLTYPE  *F_OpenThemeDataEx)( HWND hwnd, LPCWSTR pszClassList, DWORD dwFlags );
	typedef HRESULT (STDAPICALLTYPE  *F_CloseThemeData)( HTHEME hTheme );
	typedef HRESULT (STDAPICALLTYPE  *F_DrawThemeBackground)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect );
	typedef HRESULT (STDAPICALLTYPE  *F_DrawThemeBackgroundEx)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS *pOptions );
	typedef HRESULT (STDAPICALLTYPE  *F_DrawThemeText)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeBackgroundContentRect)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeBackgroundExtent)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pContentRect, LPRECT pExtentRect );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeBackgroundRegion)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemePartSize)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT prc, enum THEMESIZE eSize, SIZE *psz );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeTextExtent)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchCharCount, DWORD dwTextFlags, LPCRECT pBoundingRect, LPRECT pExtentRect );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeTextMetrics)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, TEXTMETRICW *ptm );
	typedef HRESULT (STDAPICALLTYPE  *F_HitTestThemeBackground)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, DWORD dwOptions, LPCRECT pRect, HRGN hrgn, POINT ptTest, WORD *pwHitTestCode );
	typedef HRESULT (STDAPICALLTYPE  *F_DrawThemeEdge)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect );
	typedef HRESULT (STDAPICALLTYPE  *F_DrawThemeIcon)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HIMAGELIST himl, int iImageIndex );
	typedef BOOL (STDAPICALLTYPE  *F_IsThemePartDefined)( HTHEME hTheme, int iPartId, int iStateId );
	typedef BOOL (STDAPICALLTYPE  *F_IsThemeBackgroundPartiallyTransparent)( HTHEME hTheme, int iPartId, int iStateId );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeColor)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeMetric)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, int *piVal );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeString)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPWSTR pszBuff, int cchMaxBuffChars );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeBool)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, BOOL *pfVal );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeInt)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, int *piVal );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeEnumValue)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, int *piVal );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemePosition)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, POINT *pPoint );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeFont)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LOGFONTW *pFont );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeRect)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPRECT pRect );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeMargins)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS *pMargins );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeIntList)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, struct INTLIST *pIntList );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemePropertyOrigin)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, enum PROPERTYORIGIN *pOrigin );
	typedef HRESULT (STDAPICALLTYPE  *F_SetWindowTheme)( HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeFilename)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPWSTR pszThemeFileName, int cchMaxBuffChars );
	typedef COLORREF (STDAPICALLTYPE  *F_GetThemeSysColor)( HTHEME hTheme, int iColorId );
	typedef HBRUSH (STDAPICALLTYPE  *F_GetThemeSysColorBrush)( HTHEME hTheme, int iColorId );
	typedef BOOL (STDAPICALLTYPE  *F_GetThemeSysBool)( HTHEME hTheme, int iBoolId );
	typedef int (STDAPICALLTYPE  *F_GetThemeSysSize)( HTHEME hTheme, int iSizeId );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeSysFont)( HTHEME hTheme, int iFontId, LOGFONTW *plf );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeSysString)( HTHEME hTheme, int iStringId, LPWSTR pszStringBuff, int cchMaxStringChars );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeSysInt)( HTHEME hTheme, int iIntId, int *piValue );
	typedef BOOL (STDAPICALLTYPE  *F_IsThemeActive)( VOID );
	typedef BOOL (STDAPICALLTYPE  *F_IsAppThemed)( VOID );
	typedef HTHEME (STDAPICALLTYPE  *F_GetWindowTheme)( HWND hwnd );
	typedef HRESULT (STDAPICALLTYPE  *F_EnableThemeDialogTexture)( HWND hwnd, DWORD dwFlags );
	typedef BOOL (STDAPICALLTYPE  *F_IsThemeDialogTextureEnabled)( HWND hwnd );
	typedef DWORD (STDAPICALLTYPE  *F_GetThemeAppProperties)( VOID );
	typedef void (STDAPICALLTYPE  *F_SetThemeAppProperties)( DWORD dwFlags );
	typedef HRESULT (STDAPICALLTYPE  *F_GetCurrentThemeName)( LPWSTR pszThemeFileName, int cchMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR pszSizeBuff, int cchMaxSizeChars );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeDocumentationProperty)( LPCWSTR pszThemeName, LPCWSTR pszPropertyName, LPWSTR pszValueBuff, int cchMaxValChars );
	typedef HRESULT (STDAPICALLTYPE  *F_DrawThemeParentBackground)( HWND hwnd, HDC hdc, const RECT* prc );
	typedef HRESULT (STDAPICALLTYPE  *F_EnableTheming)( BOOL fEnable );
	typedef HRESULT (STDAPICALLTYPE  *F_DrawThemeParentBackgroundEx)( HWND hwnd, HDC hdc, DWORD dwFlags, const RECT* prc );
	typedef HRESULT (STDAPICALLTYPE  *F_SetWindowThemeAttribute)( HWND hwnd, enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute );
	typedef HRESULT (STDAPICALLTYPE  *F_SetWindowThemeNonClientAttributes)(HWND hwnd, DWORD dwMask, DWORD dwAttributes);
	typedef HRESULT (STDAPICALLTYPE  *F_DrawThemeTextEx)( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS *pOptions );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeBitmap)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, ULONG dwFlags, HBITMAP* phBitmap );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeStream)( HTHEME hTheme, int iPartId, int iStateId, int iPropId, VOID **ppvStream, DWORD *pcbStream, HINSTANCE hInst );
	typedef HRESULT (STDAPICALLTYPE  *F_BufferedPaintInit)( VOID );
	typedef HRESULT (STDAPICALLTYPE  *F_BufferedPaintUnInit)( VOID );
	typedef HPAINTBUFFER (STDAPICALLTYPE  *F_BeginBufferedPaint)( HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc );
	typedef HRESULT (STDAPICALLTYPE  *F_EndBufferedPaint)( HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget );
	typedef HRESULT (STDAPICALLTYPE  *F_GetBufferedPaintTargetRect)( HPAINTBUFFER hBufferedPaint, RECT *prc );
	typedef HDC (STDAPICALLTYPE  *F_GetBufferedPaintTargetDC)( HPAINTBUFFER hBufferedPaint );
	typedef HDC (STDAPICALLTYPE  *F_GetBufferedPaintDC)( HPAINTBUFFER hBufferedPaint );
	typedef HRESULT (STDAPICALLTYPE  *F_GetBufferedPaintBits)( HPAINTBUFFER hBufferedPaint, RGBQUAD **ppbBuffer, int *pcxRow );
	typedef HRESULT (STDAPICALLTYPE  *F_BufferedPaintClear)( HPAINTBUFFER hBufferedPaint, const RECT *prc );
	typedef HRESULT (STDAPICALLTYPE  *F_BufferedPaintSetAlpha)( HPAINTBUFFER hBufferedPaint, const RECT *prc, BYTE alpha );
	typedef HRESULT (STDAPICALLTYPE  *F_BufferedPaintStopAllAnimations)( HWND hwnd );
	typedef HANIMATIONBUFFER (STDAPICALLTYPE  *F_BeginBufferedAnimation)( HWND hwnd, HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, struct BP_ANIMATIONPARAMS *pAnimationParams, HDC *phdcFrom, HDC *phdcTo );
	typedef HRESULT (STDAPICALLTYPE  *F_EndBufferedAnimation)( HANIMATIONBUFFER hbpAnimation, BOOL fUpdateTarget );
	typedef BOOL (STDAPICALLTYPE  *F_BufferedPaintRenderAnimation)( HWND hwnd, HDC hdcTarget );
	typedef BOOL (STDAPICALLTYPE  *F_IsCompositionActive)( VOID );
	typedef HRESULT (STDAPICALLTYPE  *F_GetThemeTransitionDuration)( HTHEME hTheme, int iPartId, int iStateIdFrom, int iStateIdTo, int iPropId, DWORD *pdwDuration );

	//auto-init pointer to NULL
	template< typename FType > class FPtr
	{
		FType mpfTarget;
	public:
		FPtr() : mpfTarget( NULL ) {}
		operator FType() const { return mpfTarget; }
		operator void*& () const { return *(void**)&mpfTarget; }
		FType asFPtr() const { return mpfTarget; }
	};


	// API function pointers
	FPtr< F_BeginPanningFeedback > mpfBeginPanningFeedback;
	FPtr< F_UpdatePanningFeedback > mpfUpdatePanningFeedback;
	FPtr< F_EndPanningFeedback > mpfEndPanningFeedback;
	FPtr< F_OpenThemeData > mpfOpenThemeData;
	FPtr< F_OpenThemeDataEx > mpfOpenThemeDataEx;
	FPtr< F_CloseThemeData > mpfCloseThemeData;
	FPtr< F_DrawThemeBackground > mpfDrawThemeBackground;
	FPtr< F_DrawThemeBackgroundEx > mpfDrawThemeBackgroundEx;
	FPtr< F_DrawThemeText > mpfDrawThemeText;
	FPtr< F_GetThemeBackgroundContentRect > mpfGetThemeBackgroundContentRect;
	FPtr< F_GetThemeBackgroundExtent > mpfGetThemeBackgroundExtent;
	FPtr< F_GetThemeBackgroundRegion > mpfGetThemeBackgroundRegion;
	FPtr< F_GetThemePartSize > mpfGetThemePartSize;
	FPtr< F_GetThemeTextExtent > mpfGetThemeTextExtent;
	FPtr< F_GetThemeTextMetrics > mpfGetThemeTextMetrics;
	FPtr< F_HitTestThemeBackground > mpfHitTestThemeBackground;
	FPtr< F_DrawThemeEdge > mpfDrawThemeEdge;
	FPtr< F_DrawThemeIcon > mpfDrawThemeIcon;
	FPtr< F_IsThemePartDefined > mpfIsThemePartDefined;
	FPtr< F_IsThemeBackgroundPartiallyTransparent > mpfIsThemeBackgroundPartiallyTransparent;
	FPtr< F_GetThemeColor > mpfGetThemeColor;
	FPtr< F_GetThemeMetric > mpfGetThemeMetric;
	FPtr< F_GetThemeString > mpfGetThemeString;
	FPtr< F_GetThemeBool > mpfGetThemeBool;
	FPtr< F_GetThemeInt > mpfGetThemeInt;
	FPtr< F_GetThemeEnumValue > mpfGetThemeEnumValue;
	FPtr< F_GetThemePosition > mpfGetThemePosition;
	FPtr< F_GetThemeFont > mpfGetThemeFont;
	FPtr< F_GetThemeRect > mpfGetThemeRect;
	FPtr< F_GetThemeMargins > mpfGetThemeMargins;
	FPtr< F_GetThemeIntList > mpfGetThemeIntList;
	FPtr< F_GetThemePropertyOrigin > mpfGetThemePropertyOrigin;
	FPtr< F_SetWindowTheme > mpfSetWindowTheme;
	FPtr< F_GetThemeFilename > mpfGetThemeFilename;
	FPtr< F_GetThemeSysColor > mpfGetThemeSysColor;
	FPtr< F_GetThemeSysColorBrush > mpfGetThemeSysColorBrush;
	FPtr< F_GetThemeSysBool > mpfGetThemeSysBool;
	FPtr< F_GetThemeSysSize > mpfGetThemeSysSize;
	FPtr< F_GetThemeSysFont > mpfGetThemeSysFont;
	FPtr< F_GetThemeSysString > mpfGetThemeSysString;
	FPtr< F_GetThemeSysInt > mpfGetThemeSysInt;
	FPtr< F_IsThemeActive > mpfIsThemeActive;
	FPtr< F_IsAppThemed > mpfIsAppThemed;
	FPtr< F_GetWindowTheme > mpfGetWindowTheme;
	FPtr< F_EnableThemeDialogTexture > mpfEnableThemeDialogTexture;
	FPtr< F_IsThemeDialogTextureEnabled > mpfIsThemeDialogTextureEnabled;
	FPtr< F_GetThemeAppProperties > mpfGetThemeAppProperties;
	FPtr< F_SetThemeAppProperties > mpfSetThemeAppProperties;
	FPtr< F_GetCurrentThemeName > mpfGetCurrentThemeName;
	FPtr< F_GetThemeDocumentationProperty > mpfGetThemeDocumentationProperty;
	FPtr< F_DrawThemeParentBackground > mpfDrawThemeParentBackground;
	FPtr< F_EnableTheming > mpfEnableTheming;
	FPtr< F_DrawThemeParentBackgroundEx > mpfDrawThemeParentBackgroundEx;
	FPtr< F_SetWindowThemeAttribute > mpfSetWindowThemeAttribute;
	FPtr< F_DrawThemeTextEx > mpfDrawThemeTextEx;
	FPtr< F_GetThemeBitmap > mpfGetThemeBitmap;
	FPtr< F_GetThemeStream > mpfGetThemeStream;
	FPtr< F_BufferedPaintInit > mpfBufferedPaintInit;
	FPtr< F_BufferedPaintUnInit > mpfBufferedPaintUnInit;
	FPtr< F_BeginBufferedPaint > mpfBeginBufferedPaint;
	FPtr< F_EndBufferedPaint > mpfEndBufferedPaint;
	FPtr< F_GetBufferedPaintTargetRect > mpfGetBufferedPaintTargetRect;
	FPtr< F_GetBufferedPaintTargetDC > mpfGetBufferedPaintTargetDC;
	FPtr< F_GetBufferedPaintDC > mpfGetBufferedPaintDC;
	FPtr< F_GetBufferedPaintBits > mpfGetBufferedPaintBits;
	FPtr< F_BufferedPaintClear > mpfBufferedPaintClear;
	FPtr< F_BufferedPaintSetAlpha > mpfBufferedPaintSetAlpha;
	FPtr< F_BufferedPaintStopAllAnimations > mpfBufferedPaintStopAllAnimations;
	FPtr< F_BeginBufferedAnimation > mpfBeginBufferedAnimation;
	FPtr< F_EndBufferedAnimation > mpfEndBufferedAnimation;
	FPtr< F_BufferedPaintRenderAnimation > mpfBufferedPaintRenderAnimation;
	FPtr< F_IsCompositionActive > mpfIsCompositionActive;
	FPtr< F_GetThemeTransitionDuration > mpfGetThemeTransitionDuration;


	#define CHECK(name) return check(mpf##name,#name)? mpf##name.asFPtr()

public:
	// API function stubs
	BOOL WINAPI BeginPanningFeedback( HWND hwnd ) { CHECK(BeginPanningFeedback)(hwnd) : FALSE; }
	BOOL WINAPI UpdatePanningFeedback( HWND hwnd, LONG lTotalOverpanOffsetX, LONG lTotalOverpanOffsetY, BOOL fInInertia ) { CHECK(UpdatePanningFeedback)( hwnd, lTotalOverpanOffsetX, lTotalOverpanOffsetY, fInInertia ) : FALSE; }
	BOOL WINAPI EndPanningFeedback( HWND hwnd, BOOL fAnimateBack ) { CHECK(EndPanningFeedback)( hwnd, fAnimateBack ) : FALSE; }
	HTHEME OpenThemeData( HWND hwnd, LPCWSTR pszClassList ) { CHECK(OpenThemeData)( hwnd, pszClassList ) : NULL; }
	HTHEME OpenThemeDataEx( HWND hwnd, LPCWSTR pszClassList, DWORD dwFlags ) { CHECK(OpenThemeDataEx)( hwnd, pszClassList, dwFlags ) : NULL; }
	HRESULT CloseThemeData( HTHEME hTheme ) { CHECK(CloseThemeData)( hTheme ) : E_NOTIMPL; }
	HRESULT DrawThemeBackground( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect ) { CHECK(DrawThemeBackground)( hTheme, hdc, iPartId, iStateId, pRect, pClipRect ) : E_NOTIMPL; }
	HRESULT DrawThemeBackgroundEx( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS *pOptions ) { CHECK(DrawThemeBackgroundEx)( hTheme, hdc, iPartId, iStateId, pRect, pOptions ) : E_NOTIMPL; }
	HRESULT DrawThemeText( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect ) { CHECK(DrawThemeText)( hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect ) : E_NOTIMPL; }
	HRESULT GetThemeBackgroundContentRect( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect ) { CHECK(GetThemeBackgroundContentRect)( hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect ) : E_NOTIMPL; }
	HRESULT GetThemeBackgroundExtent( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pContentRect, LPRECT pExtentRect ) { CHECK(GetThemeBackgroundExtent)( hTheme, hdc, iPartId, iStateId, pContentRect, pExtentRect ) : E_NOTIMPL; }
	HRESULT GetThemeBackgroundRegion( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion ) { CHECK(GetThemeBackgroundRegion)( hTheme, hdc, iPartId, iStateId, pRect, pRegion ) : E_NOTIMPL; }
	HRESULT GetThemePartSize( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT prc, enum THEMESIZE eSize, SIZE *psz ) { CHECK(GetThemePartSize)( hTheme, hdc, iPartId, iStateId, prc, eSize, psz ) : E_NOTIMPL; }
	HRESULT GetThemeTextExtent( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchCharCount, DWORD dwTextFlags, LPCRECT pBoundingRect, LPRECT pExtentRect ) { CHECK(GetThemeTextExtent)( hTheme, hdc, iPartId, iStateId, pszText, cchCharCount, dwTextFlags, pBoundingRect, pExtentRect ) : E_NOTIMPL; }
	HRESULT GetThemeTextMetrics( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, TEXTMETRICW *ptm ) { CHECK(GetThemeTextMetrics)( hTheme, hdc, iPartId, iStateId, ptm ) : E_NOTIMPL; }
	HRESULT HitTestThemeBackground( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, DWORD dwOptions, LPCRECT pRect, HRGN hrgn, POINT ptTest, WORD *pwHitTestCode ) { CHECK(HitTestThemeBackground)( hTheme, hdc, iPartId, iStateId, dwOptions, pRect, hrgn, ptTest, pwHitTestCode ) : E_NOTIMPL; }
	HRESULT DrawThemeEdge( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect ) { CHECK(DrawThemeEdge)( hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect ) : E_NOTIMPL; }
	HRESULT DrawThemeIcon( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HIMAGELIST himl, int iImageIndex ) { CHECK(DrawThemeIcon)( hTheme, hdc, iPartId, iStateId, pRect, himl, iImageIndex ) : E_NOTIMPL; }
	BOOL IsThemePartDefined( HTHEME hTheme, int iPartId, int iStateId ) { CHECK(IsThemePartDefined)( hTheme, iPartId, iStateId ) : FALSE; }
	BOOL IsThemeBackgroundPartiallyTransparent( HTHEME hTheme, int iPartId, int iStateId ) { CHECK(IsThemeBackgroundPartiallyTransparent)( hTheme, iPartId, iStateId ) : FALSE; }
	HRESULT GetThemeColor( HTHEME hTheme, int iPartId, int iStateId, int iPropId, COLORREF *pColor ) { CHECK(GetThemeColor)( hTheme, iPartId, iStateId, iPropId, pColor ) : E_NOTIMPL; }
	HRESULT GetThemeMetric( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, int *piVal ) { CHECK(GetThemeMetric)( hTheme, hdc, iPartId, iStateId, iPropId, piVal ) : E_NOTIMPL; }
	HRESULT GetThemeString( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPWSTR pszBuff, int cchMaxBuffChars ) { CHECK(GetThemeString)( hTheme, iPartId, iStateId, iPropId, pszBuff, cchMaxBuffChars ) : E_NOTIMPL; }
	HRESULT GetThemeBool( HTHEME hTheme, int iPartId, int iStateId, int iPropId, BOOL *pfVal ) { CHECK(GetThemeBool)( hTheme, iPartId, iStateId, iPropId, pfVal ) : E_NOTIMPL; }
	HRESULT GetThemeInt( HTHEME hTheme, int iPartId, int iStateId, int iPropId, int *piVal ) { CHECK(GetThemeInt)( hTheme, iPartId, iStateId, iPropId, piVal ) : E_NOTIMPL; }
	HRESULT GetThemeEnumValue( HTHEME hTheme, int iPartId, int iStateId, int iPropId, int *piVal ) { CHECK(GetThemeEnumValue)( hTheme, iPartId, iStateId, iPropId, piVal ) : E_NOTIMPL; }
	HRESULT GetThemePosition( HTHEME hTheme, int iPartId, int iStateId, int iPropId, POINT *pPoint ) { CHECK(GetThemePosition)( hTheme, iPartId, iStateId, iPropId, pPoint ) : E_NOTIMPL; }
	HRESULT GetThemeFont( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LOGFONTW *pFont ) { CHECK(GetThemeFont)( hTheme, hdc, iPartId, iStateId, iPropId, pFont ) : E_NOTIMPL; }
	HRESULT GetThemeRect( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPRECT pRect ) { CHECK(GetThemeRect)( hTheme, iPartId, iStateId, iPropId, pRect ) : E_NOTIMPL; }
	HRESULT GetThemeMargins( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS *pMargins ) { CHECK(GetThemeMargins)( hTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins ) : E_NOTIMPL; }
	HRESULT GetThemeIntList( HTHEME hTheme, int iPartId, int iStateId, int iPropId, struct INTLIST *pIntList ) { CHECK(GetThemeIntList)( hTheme, iPartId, iStateId, iPropId, pIntList ) : E_NOTIMPL; }
	HRESULT GetThemePropertyOrigin( HTHEME hTheme, int iPartId, int iStateId, int iPropId, enum PROPERTYORIGIN *pOrigin ) { CHECK(GetThemePropertyOrigin)( hTheme, iPartId, iStateId, iPropId, pOrigin ) : E_NOTIMPL; }
	HRESULT SetWindowTheme( HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList ) { CHECK(SetWindowTheme)( hwnd, pszSubAppName, pszSubIdList ) : E_NOTIMPL; }
	HRESULT GetThemeFilename( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPWSTR pszThemeFileName, int cchMaxBuffChars ) { CHECK(GetThemeFilename)( hTheme, iPartId, iStateId, iPropId, pszThemeFileName, cchMaxBuffChars ) : E_NOTIMPL; }
	COLORREF GetThemeSysColor( HTHEME hTheme, int iColorId ) { CHECK(GetThemeSysColor)( hTheme, iColorId ) : 0; }
	HBRUSH GetThemeSysColorBrush( HTHEME hTheme, int iColorId ) { CHECK(GetThemeSysColorBrush)( hTheme, iColorId ) : NULL; }
	BOOL GetThemeSysBool( HTHEME hTheme, int iBoolId ) { CHECK(GetThemeSysBool)( hTheme, iBoolId ) : FALSE; }
	int GetThemeSysSize( HTHEME hTheme, int iSizeId ) { CHECK(GetThemeSysSize)( hTheme, iSizeId ) : 0; }
	HRESULT GetThemeSysFont( HTHEME hTheme, int iFontId, LOGFONTW *plf ) { CHECK(GetThemeSysFont)( hTheme, iFontId, plf ) : E_NOTIMPL; }
	HRESULT GetThemeSysString( HTHEME hTheme, int iStringId, LPWSTR pszStringBuff, int cchMaxStringChars ) { CHECK(GetThemeSysString)( hTheme, iStringId, pszStringBuff, cchMaxStringChars ) : E_NOTIMPL; }
	HRESULT GetThemeSysInt( HTHEME hTheme, int iIntId, int *piValue ) { CHECK(GetThemeSysInt)( hTheme, iIntId, piValue ) : E_NOTIMPL; }
	BOOL IsThemeActive( VOID ) { CHECK(IsThemeActive)() : FALSE; }
	BOOL IsAppThemed( VOID ) { CHECK(IsAppThemed)() : FALSE; }
	HTHEME GetWindowTheme( HWND hwnd ) { CHECK(GetWindowTheme)( hwnd ) : NULL; }
	HRESULT EnableThemeDialogTexture( HWND hwnd, DWORD dwFlags ) { CHECK(EnableThemeDialogTexture)( hwnd, dwFlags ) : E_NOTIMPL; }
	BOOL IsThemeDialogTextureEnabled( HWND hwnd ) { CHECK(IsThemeDialogTextureEnabled)( hwnd ) : FALSE; }
	DWORD GetThemeAppProperties( VOID ) { CHECK(GetThemeAppProperties)() : 0; }
	void SetThemeAppProperties( DWORD dwFlags ) { CHECK(SetThemeAppProperties)( dwFlags ) : 0; }
	HRESULT GetCurrentThemeName( LPWSTR pszThemeFileName, int cchMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR pszSizeBuff, int cchMaxSizeChars ) { CHECK(GetCurrentThemeName)( pszThemeFileName, cchMaxNameChars, pszColorBuff, cchMaxColorChars, pszSizeBuff, cchMaxSizeChars ) : E_NOTIMPL; }
	HRESULT GetThemeDocumentationProperty( LPCWSTR pszThemeName, LPCWSTR pszPropertyName, LPWSTR pszValueBuff, int cchMaxValChars ) { CHECK(GetThemeDocumentationProperty)( pszThemeName, pszPropertyName, pszValueBuff, cchMaxValChars ) : E_NOTIMPL; }
	HRESULT DrawThemeParentBackground( HWND hwnd, HDC hdc, const RECT* prc ) { CHECK(DrawThemeParentBackground)( hwnd, hdc, prc ) : E_NOTIMPL; }
	HRESULT EnableTheming( BOOL fEnable ) { CHECK(EnableTheming)( fEnable ) : E_NOTIMPL; }
	HRESULT DrawThemeParentBackgroundEx( HWND hwnd, HDC hdc, DWORD dwFlags, const RECT* prc ) { CHECK(DrawThemeParentBackgroundEx)( hwnd, hdc, dwFlags, prc ) : E_NOTIMPL; }
	HRESULT SetWindowThemeAttribute( HWND hwnd, enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute ) { CHECK(SetWindowThemeAttribute)( hwnd, eAttribute, pvAttribute, cbAttribute ) : E_NOTIMPL; }
	__inline HRESULT SetWindowThemeNonClientAttributes(HWND hwnd, DWORD dwMask, DWORD dwAttributes)    
	{
		struct{ DWORD flags; DWORD mask; } wta = { dwAttributes, dwMask };
		return SetWindowThemeAttribute(hwnd, (WINDOWTHEMEATTRIBUTETYPE)1/*WTA_NONCLIENT*/, (void*)&(wta), sizeof(wta));
	}
	HRESULT DrawThemeTextEx( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS *pOptions ) { CHECK(DrawThemeTextEx)( hTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions ) : E_NOTIMPL; }
	HRESULT GetThemeBitmap( HTHEME hTheme, int iPartId, int iStateId, int iPropId, ULONG dwFlags, HBITMAP* phBitmap ) { CHECK(GetThemeBitmap)( hTheme, iPartId, iStateId, iPropId, dwFlags, phBitmap ) : E_NOTIMPL; }
	HRESULT GetThemeStream( HTHEME hTheme, int iPartId, int iStateId, int iPropId, VOID **ppvStream, DWORD *pcbStream, HINSTANCE hInst ) { CHECK(GetThemeStream)( hTheme, iPartId, iStateId, iPropId, ppvStream, pcbStream, hInst ) : E_NOTIMPL; }
	HRESULT BufferedPaintInit( VOID ) { CHECK(BufferedPaintInit)() : E_NOTIMPL; }
	HRESULT BufferedPaintUnInit( VOID ) { CHECK(BufferedPaintUnInit)() : E_NOTIMPL; }
	HPAINTBUFFER BeginBufferedPaint( HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc ) { CHECK(BeginBufferedPaint)( hdcTarget, prcTarget, dwFormat, pPaintParams, phdc ) : NULL; }
	HRESULT EndBufferedPaint( HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget ) { CHECK(EndBufferedPaint)( hBufferedPaint, fUpdateTarget ) : E_NOTIMPL; }
	HRESULT GetBufferedPaintTargetRect( HPAINTBUFFER hBufferedPaint, RECT *prc ) { CHECK(GetBufferedPaintTargetRect)( hBufferedPaint, prc ) : E_NOTIMPL; }
	HDC GetBufferedPaintTargetDC( HPAINTBUFFER hBufferedPaint ) { CHECK(GetBufferedPaintTargetDC)( hBufferedPaint ) : NULL; }
	HDC GetBufferedPaintDC( HPAINTBUFFER hBufferedPaint ) { CHECK(GetBufferedPaintDC)( hBufferedPaint ) : NULL; }
	HRESULT GetBufferedPaintBits( HPAINTBUFFER hBufferedPaint, RGBQUAD **ppbBuffer, int *pcxRow ) { CHECK(GetBufferedPaintBits)( hBufferedPaint, ppbBuffer, pcxRow ) : E_NOTIMPL; }
	HRESULT BufferedPaintClear( HPAINTBUFFER hBufferedPaint, const RECT *prc ) { CHECK(BufferedPaintClear)( hBufferedPaint, prc ) : E_NOTIMPL; }
	HRESULT BufferedPaintSetAlpha( HPAINTBUFFER hBufferedPaint, const RECT *prc, BYTE alpha ) { CHECK(BufferedPaintSetAlpha)( hBufferedPaint, prc, alpha ) : E_NOTIMPL; }
	HRESULT BufferedPaintStopAllAnimations( HWND hwnd ) { CHECK(BufferedPaintStopAllAnimations)( hwnd ) : E_NOTIMPL; }
	HANIMATIONBUFFER BeginBufferedAnimation( HWND hwnd, HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, struct BP_ANIMATIONPARAMS *pAnimationParams, HDC *phdcFrom, HDC *phdcTo ) { CHECK(BeginBufferedAnimation)( hwnd, hdcTarget, prcTarget, dwFormat, pPaintParams, pAnimationParams, phdcFrom, phdcTo ) : NULL; }
	HRESULT EndBufferedAnimation( HANIMATIONBUFFER hbpAnimation, BOOL fUpdateTarget ) { CHECK(EndBufferedAnimation)( hbpAnimation, fUpdateTarget ) : E_NOTIMPL; }
	BOOL BufferedPaintRenderAnimation( HWND hwnd, HDC hdcTarget ) { CHECK(BufferedPaintRenderAnimation)( hwnd, hdcTarget ) : FALSE; }
	BOOL IsCompositionActive( VOID ) { CHECK(IsCompositionActive)() : FALSE; }
	HRESULT GetThemeTransitionDuration( HTHEME hTheme, int iPartId, int iStateIdFrom, int iStateIdTo, int iPropId, DWORD *pdwDuration ) { CHECK(GetThemeTransitionDuration)( hTheme, iPartId, iStateIdFrom, iStateIdTo, iPropId, pdwDuration ) : E_NOTIMPL; }
};


// To enable delayed initialization
static ThemeAPImportTable& getUX()
{
	static ThemeAPImportTable theThemeAPIImportTable;
	return theThemeAPIImportTable;
}
#define UX getUX()

UxTheme::UxTheme() : mhTheme( NULL ), mbMustCloseTheme( false )
{
}

UxTheme::UxTheme( HWND hWnd, LPCWSTR pszClassList )
	: mhTheme( OpenThemeData( hWnd, pszClassList ) ), mbMustCloseTheme( true )
{
}

UxTheme::UxTheme( HWND hWnd, LPCWSTR pszClassList, DWORD dwFlags )
	: mhTheme( OpenThemeDataEx( hWnd, pszClassList, dwFlags ) ), mbMustCloseTheme( true )
{
}

UxTheme::UxTheme( HTHEME hTheme, bool bAutoClose /*= false*/ ) : mhTheme( hTheme ), mbMustCloseTheme( bAutoClose )
{
}

UxTheme::~UxTheme()
{
	if( mbMustCloseTheme && mhTheme )
		CloseThemeData( mhTheme );
}

// Theme API functions

HTHEME UxTheme::OpenThemeData( HWND hwnd, LPCWSTR pszClassList )
{
	return UX.OpenThemeData( hwnd, pszClassList );
}

HTHEME UxTheme::OpenThemeDataEx( HWND hwnd, LPCWSTR pszClassList, DWORD dwFlags )
{
	return UX.OpenThemeDataEx( hwnd, pszClassList, dwFlags );
}

HRESULT UxTheme::CloseThemeData( HTHEME hTheme )
{
	return UX.CloseThemeData( hTheme );
}

BOOL WINAPI UxTheme::BeginPanningFeedback( HWND hwnd )
{
	return UX.BeginPanningFeedback( hwnd );
}

BOOL WINAPI UxTheme::UpdatePanningFeedback( HWND hwnd, LONG lTotalOverpanOffsetX, LONG lTotalOverpanOffsetY, BOOL fInInertia )
{
	return UX.UpdatePanningFeedback( hwnd, lTotalOverpanOffsetX, lTotalOverpanOffsetY, fInInertia );
}

BOOL WINAPI UxTheme::EndPanningFeedback( HWND hwnd, BOOL fAnimateBack )
{
	return UX.EndPanningFeedback( hwnd, fAnimateBack );
}

HRESULT UxTheme::DrawThemeBackground( HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect ) const
{
	return UX.DrawThemeBackground( mhTheme, hdc, iPartId, iStateId, pRect, pClipRect );
}

HRESULT UxTheme::DrawThemeBackgroundEx( HDC hdc, int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS *pOptions ) const
{
	return UX.DrawThemeBackgroundEx( mhTheme, hdc, iPartId, iStateId, pRect, pOptions );
}

HRESULT UxTheme::DrawThemeText( HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect ) const
{
	return UX.DrawThemeText( mhTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect );
}

HRESULT UxTheme::GetThemeBackgroundContentRect( HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect ) const
{
	return UX.GetThemeBackgroundContentRect( mhTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect );
}

HRESULT UxTheme::GetThemeBackgroundExtent( HDC hdc, int iPartId, int iStateId, LPCRECT pContentRect, LPRECT pExtentRect ) const
{
	return UX.GetThemeBackgroundExtent( mhTheme, hdc, iPartId, iStateId, pContentRect, pExtentRect );
}

HRESULT UxTheme::GetThemeBackgroundRegion( HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion ) const
{
	return UX.GetThemeBackgroundRegion( mhTheme, hdc, iPartId, iStateId, pRect, pRegion );
}

HRESULT UxTheme::GetThemePartSize( HDC hdc, int iPartId, int iStateId, LPCRECT prc, enum THEMESIZE eSize, SIZE *psz ) const
{
	return UX.GetThemePartSize( mhTheme, hdc, iPartId, iStateId, prc, eSize, psz );
}

HRESULT UxTheme::GetThemeTextExtent( HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchCharCount, DWORD dwTextFlags, LPCRECT pBoundingRect, LPRECT pExtentRect ) const
{
	return UX.GetThemeTextExtent( mhTheme, hdc, iPartId, iStateId, pszText, cchCharCount, dwTextFlags, pBoundingRect, pExtentRect );
}

HRESULT UxTheme::GetThemeTextMetrics( HDC hdc, int iPartId, int iStateId, TEXTMETRICW *ptm ) const
{
	return UX.GetThemeTextMetrics( mhTheme, hdc, iPartId, iStateId, ptm );
}

HRESULT UxTheme::HitTestThemeBackground( HDC hdc, int iPartId, int iStateId, DWORD dwOptions, LPCRECT pRect, HRGN hrgn, POINT ptTest, WORD *pwHitTestCode ) const
{
	return UX.HitTestThemeBackground( mhTheme, hdc, iPartId, iStateId, dwOptions, pRect, hrgn, ptTest, pwHitTestCode );
}

HRESULT UxTheme::DrawThemeEdge( HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect ) const
{
	return UX.DrawThemeEdge( mhTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect );
}

HRESULT UxTheme::DrawThemeIcon( HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HIMAGELIST himl, int iImageIndex ) const
{
	return UX.DrawThemeIcon( mhTheme, hdc, iPartId, iStateId, pRect, himl, iImageIndex );
}

BOOL UxTheme::IsThemePartDefined( int iPartId, int iStateId ) const
{
	return UX.IsThemePartDefined( mhTheme, iPartId, iStateId );
}

BOOL UxTheme::IsThemeBackgroundPartiallyTransparent( int iPartId, int iStateId ) const
{
	return UX.IsThemeBackgroundPartiallyTransparent( mhTheme, iPartId, iStateId );
}

HRESULT UxTheme::GetThemeColor( int iPartId, int iStateId, int iPropId, COLORREF *pColor ) const
{
	return UX.GetThemeColor( mhTheme, iPartId, iStateId, iPropId, pColor );
}

HRESULT UxTheme::GetThemeMetric( HDC hdc, int iPartId, int iStateId, int iPropId, int *piVal ) const
{
	return UX.GetThemeMetric( mhTheme, hdc, iPartId, iStateId, iPropId, piVal );
}

HRESULT UxTheme::GetThemeString( int iPartId, int iStateId, int iPropId, LPWSTR pszBuff, int cchMaxBuffChars ) const
{
	return UX.GetThemeString( mhTheme, iPartId, iStateId, iPropId, pszBuff, cchMaxBuffChars );
}

HRESULT UxTheme::GetThemeBool( int iPartId, int iStateId, int iPropId, BOOL *pfVal ) const
{
	return UX.GetThemeBool( mhTheme, iPartId, iStateId, iPropId, pfVal );
}

HRESULT UxTheme::GetThemeInt( int iPartId, int iStateId, int iPropId, int *piVal ) const
{
	return UX.GetThemeInt( mhTheme, iPartId, iStateId, iPropId, piVal );
}

HRESULT UxTheme::GetThemeEnumValue( int iPartId, int iStateId, int iPropId, int *piVal ) const
{
	return UX.GetThemeEnumValue( mhTheme, iPartId, iStateId, iPropId, piVal );
}

HRESULT UxTheme::GetThemePosition( int iPartId, int iStateId, int iPropId, POINT *pPoint ) const
{
	return UX.GetThemePosition( mhTheme, iPartId, iStateId, iPropId, pPoint );
}

HRESULT UxTheme::GetThemeFont( HDC hdc, int iPartId, int iStateId, int iPropId, LOGFONTW *pFont ) const
{
	return UX.GetThemeFont( mhTheme, hdc, iPartId, iStateId, iPropId, pFont );
}

HRESULT UxTheme::GetThemeRect( int iPartId, int iStateId, int iPropId, LPRECT pRect ) const
{
	return UX.GetThemeRect( mhTheme, iPartId, iStateId, iPropId, pRect );
}

HRESULT UxTheme::GetThemeMargins( HDC hdc, int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS *pMargins ) const
{
	return UX.GetThemeMargins( mhTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins );
}

HRESULT UxTheme::GetThemeIntList( int iPartId, int iStateId, int iPropId, struct INTLIST *pIntList ) const
{
	return UX.GetThemeIntList( mhTheme, iPartId, iStateId, iPropId, pIntList );
}

HRESULT UxTheme::GetThemePropertyOrigin( int iPartId, int iStateId, int iPropId, enum PROPERTYORIGIN *pOrigin ) const
{
	return UX.GetThemePropertyOrigin( mhTheme, iPartId, iStateId, iPropId, pOrigin );
}

HRESULT UxTheme::SetWindowTheme( HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList )
{
	return UX.SetWindowTheme( hwnd, pszSubAppName, pszSubIdList );
}

HRESULT UxTheme::GetThemeFilename( int iPartId, int iStateId, int iPropId, LPWSTR pszThemeFileName, int cchMaxBuffChars ) const
{
	return UX.GetThemeFilename( mhTheme, iPartId, iStateId, iPropId, pszThemeFileName, cchMaxBuffChars );
}

COLORREF UxTheme::GetThemeSysColor( int iColorId ) const
{
	return UX.GetThemeSysColor( mhTheme, iColorId );
}

HBRUSH UxTheme::GetThemeSysColorBrush( int iColorId ) const
{
	return UX.GetThemeSysColorBrush( mhTheme, iColorId );
}

BOOL UxTheme::GetThemeSysBool( int iBoolId ) const
{
	return UX.GetThemeSysBool( mhTheme, iBoolId );
}

int UxTheme::GetThemeSysSize( int iSizeId ) const
{
	return UX.GetThemeSysSize( mhTheme, iSizeId );
}

HRESULT UxTheme::GetThemeSysFont( int iFontId, LOGFONTW *plf ) const
{
	return UX.GetThemeSysFont( mhTheme, iFontId, plf );
}

HRESULT UxTheme::GetThemeSysString( int iStringId, LPWSTR pszStringBuff, int cchMaxStringChars ) const
{
	return UX.GetThemeSysString( mhTheme, iStringId, pszStringBuff, cchMaxStringChars );
}

HRESULT UxTheme::GetThemeSysInt( int iIntId, int *piValue ) const
{
	return UX.GetThemeSysInt( mhTheme, iIntId, piValue );
}

BOOL UxTheme::IsThemeActive( VOID )
{
	return UX.IsThemeActive();
}

BOOL UxTheme::IsAppThemed( VOID )
{
	return UX.IsAppThemed();
}

HTHEME UxTheme::GetWindowTheme( HWND hwnd )
{
	return UX.GetWindowTheme( hwnd );
}

HRESULT UxTheme::EnableThemeDialogTexture( HWND hwnd, DWORD dwFlags )
{
	return UX.EnableThemeDialogTexture( hwnd, dwFlags );
}

BOOL UxTheme::IsThemeDialogTextureEnabled( HWND hwnd )
{
	return UX.IsThemeDialogTextureEnabled( hwnd );
}

DWORD UxTheme::GetThemeAppProperties( VOID )
{
	return UX.GetThemeAppProperties();
}

void UxTheme::SetThemeAppProperties( DWORD dwFlags )
{
	UX.SetThemeAppProperties( dwFlags );
}

HRESULT UxTheme::GetCurrentThemeName( LPWSTR pszThemeFileName, int cchMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR pszSizeBuff, int cchMaxSizeChars )
{
	return UX.GetCurrentThemeName( pszThemeFileName, cchMaxNameChars, pszColorBuff, cchMaxColorChars, pszSizeBuff, cchMaxSizeChars );
}

HRESULT UxTheme::GetThemeDocumentationProperty( LPCWSTR pszThemeName, LPCWSTR pszPropertyName, LPWSTR pszValueBuff, int cchMaxValChars )
{
	return UX.GetThemeDocumentationProperty( pszThemeName, pszPropertyName, pszValueBuff, cchMaxValChars );
}

HRESULT UxTheme::DrawThemeParentBackground( HWND hwnd, HDC hdc, const RECT* prc )
{
	return UX.DrawThemeParentBackground( hwnd, hdc, prc );
}

HRESULT UxTheme::EnableTheming( BOOL fEnable )
{
	return UX.EnableTheming( fEnable );
}

HRESULT UxTheme::DrawThemeParentBackgroundEx( HWND hwnd, HDC hdc, DWORD dwFlags, const RECT* prc )
{
	return UX.DrawThemeParentBackgroundEx( hwnd, hdc, dwFlags, prc );
}

HRESULT UxTheme::SetWindowThemeAttribute( HWND hwnd, enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute )
{
	return UX.SetWindowThemeAttribute( hwnd, eAttribute, pvAttribute, cbAttribute );
}

HRESULT UxTheme::SetWindowThemeNonClientAttributes( HWND hwnd, DWORD dwMask, DWORD dwAttributes )
{
	return UX.SetWindowThemeNonClientAttributes( hwnd, dwMask, dwAttributes );
}

HRESULT UxTheme::DrawThemeTextEx( HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS *pOptions ) const
{
	return UX.DrawThemeTextEx( mhTheme, hdc, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions );
}

HRESULT UxTheme::GetThemeBitmap( int iPartId, int iStateId, int iPropId, ULONG dwFlags, HBITMAP* phBitmap ) const
{
	return UX.GetThemeBitmap( mhTheme, iPartId, iStateId, iPropId, dwFlags, phBitmap );
}

HRESULT UxTheme::GetThemeStream( int iPartId, int iStateId, int iPropId, VOID **ppvStream, DWORD *pcbStream, HINSTANCE hInst ) const
{
	return UX.GetThemeStream( mhTheme, iPartId, iStateId, iPropId, ppvStream, pcbStream, hInst );
}

HRESULT UxTheme::BufferedPaintInit( VOID )
{
	return UX.BufferedPaintInit();
}

HRESULT UxTheme::BufferedPaintUnInit( VOID )
{
	return UX.BufferedPaintUnInit();
}

HPAINTBUFFER UxTheme::BeginBufferedPaint( HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc )
{
	return UX.BeginBufferedPaint( hdcTarget, prcTarget, dwFormat, pPaintParams, phdc );
}

HRESULT UxTheme::EndBufferedPaint( HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget )
{
	return UX.EndBufferedPaint( hBufferedPaint, fUpdateTarget );
}

HRESULT UxTheme::GetBufferedPaintTargetRect( HPAINTBUFFER hBufferedPaint, RECT *prc )
{
	return UX.GetBufferedPaintTargetRect( hBufferedPaint, prc );
}

HDC UxTheme::GetBufferedPaintTargetDC( HPAINTBUFFER hBufferedPaint )
{
	return UX.GetBufferedPaintTargetDC( hBufferedPaint );
}

HDC UxTheme::GetBufferedPaintDC( HPAINTBUFFER hBufferedPaint )
{
	return UX.GetBufferedPaintDC( hBufferedPaint );
}

HRESULT UxTheme::GetBufferedPaintBits( HPAINTBUFFER hBufferedPaint, RGBQUAD **ppbBuffer, int *pcxRow )
{
	return UX.GetBufferedPaintBits( hBufferedPaint, ppbBuffer, pcxRow );
}

HRESULT UxTheme::BufferedPaintClear( HPAINTBUFFER hBufferedPaint, const RECT *prc )
{
	return UX.BufferedPaintClear( hBufferedPaint, prc );
}

HRESULT UxTheme::BufferedPaintSetAlpha( HPAINTBUFFER hBufferedPaint, const RECT *prc, BYTE alpha )
{
	return UX.BufferedPaintSetAlpha( hBufferedPaint, prc, alpha );
}

HRESULT UxTheme::BufferedPaintStopAllAnimations( HWND hwnd )
{
	return UX.BufferedPaintStopAllAnimations( hwnd );
}

HANIMATIONBUFFER UxTheme::BeginBufferedAnimation( HWND hwnd, HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, struct BP_ANIMATIONPARAMS *pAnimationParams, HDC *phdcFrom, HDC *phdcTo )
{
	return UX.BeginBufferedAnimation( hwnd, hdcTarget, prcTarget, dwFormat, pPaintParams, pAnimationParams, phdcFrom, phdcTo );
}

HRESULT UxTheme::EndBufferedAnimation( HANIMATIONBUFFER hbpAnimation, BOOL fUpdateTarget )
{
	return UX.EndBufferedAnimation( hbpAnimation, fUpdateTarget );
}

BOOL UxTheme::BufferedPaintRenderAnimation( HWND hwnd, HDC hdcTarget )
{
	return UX.BufferedPaintRenderAnimation( hwnd, hdcTarget );
}

BOOL UxTheme::IsCompositionActive( VOID )
{
	return UX.IsCompositionActive();
}

HRESULT UxTheme::GetThemeTransitionDuration( int iPartId, int iStateIdFrom, int iStateIdTo, int iPropId, DWORD *pdwDuration ) const
{
	return UX.GetThemeTransitionDuration( mhTheme, iPartId, iStateIdFrom, iStateIdTo, iPropId, pdwDuration );
}

BOOL WINAPI WndTheme::BeginPanningFeedback() const
{
	return __super::BeginPanningFeedback( mhWnd );
}

BOOL WINAPI WndTheme::UpdatePanningFeedback( LONG lTotalOverpanOffsetX, LONG lTotalOverpanOffsetY, BOOL fInInertia ) const
{
	return __super::UpdatePanningFeedback( mhWnd, lTotalOverpanOffsetX, lTotalOverpanOffsetY, fInInertia );
}

BOOL WINAPI WndTheme::EndPanningFeedback( BOOL fAnimateBack ) const
{
	return __super::EndPanningFeedback( mhWnd, fAnimateBack );
}

HTHEME WndTheme::GetWindowTheme( VOID ) const
{
	return __super::GetWindowTheme( mhWnd );
}

HRESULT WndTheme::SetWindowTheme( LPCWSTR pszSubAppName, LPCWSTR pszSubIdList ) const
{
	return __super::SetWindowTheme( mhWnd, pszSubAppName, pszSubIdList );
}

HRESULT WndTheme::EnableThemeDialogTexture( DWORD dwFlags ) const
{
	return __super::EnableThemeDialogTexture( mhWnd, dwFlags );
}

BOOL WndTheme::IsThemeDialogTextureEnabled() const
{
	return __super::IsThemeDialogTextureEnabled( mhWnd );
}

HRESULT WndTheme::DrawThemeParentBackground( HDC hdc, const RECT* prc ) const
{
	return __super::DrawThemeParentBackground( mhWnd, hdc, prc );
}

HRESULT WndTheme::DrawThemeParentBackgroundEx( HDC hdc, DWORD dwFlags, const RECT* prc ) const
{
	return __super::DrawThemeParentBackgroundEx( mhWnd, hdc, dwFlags, prc );
}

HRESULT WndTheme::SetWindowThemeAttribute( enum WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute ) const
{
	return __super::SetWindowThemeAttribute( mhWnd, eAttribute, pvAttribute, cbAttribute );
}

HRESULT WndTheme::SetWindowThemeNonClientAttributes( DWORD dwMask, DWORD dwAttributes ) const
{
	return __super::SetWindowThemeNonClientAttributes( mhWnd, dwMask, dwAttributes );
}

HRESULT WndTheme::BufferedPaintStopAllAnimations() const
{
	return __super::BufferedPaintStopAllAnimations( mhWnd );
}

HANIMATIONBUFFER WndTheme::BeginBufferedAnimation( HDC hdcTarget, const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, struct BP_ANIMATIONPARAMS *pAnimationParams, HDC *phdcFrom, HDC *phdcTo ) const
{
	return __super::BeginBufferedAnimation( mhWnd, hdcTarget, prcTarget, dwFormat, pPaintParams, pAnimationParams, phdcFrom, phdcTo );
}

BOOL WndTheme::BufferedPaintRenderAnimation( HDC hdcTarget ) const
{
	return __super::BufferedPaintRenderAnimation( mhWnd, hdcTarget );
}

HRESULT DcTheme::DrawThemeBackground( int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect ) const
{
	return __super::DrawThemeBackground( mhDC, iPartId, iStateId, pRect, pClipRect );
}

HRESULT DcTheme::DrawThemeBackgroundEx( int iPartId, int iStateId, LPCRECT pRect, const DTBGOPTS *pOptions ) const
{
	return __super::DrawThemeBackgroundEx( mhDC, iPartId, iStateId, pRect, pOptions );
}

HRESULT DcTheme::DrawThemeText( int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect ) const
{
	return __super::DrawThemeText( mhDC, iPartId, iStateId, pszText, cchText, dwTextFlags, dwTextFlags2, pRect );
}

HRESULT DcTheme::GetThemeBackgroundContentRect( int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect ) const
{
	return __super::GetThemeBackgroundContentRect( mhDC, iPartId, iStateId, pBoundingRect, pContentRect );
}

HRESULT DcTheme::GetThemeBackgroundExtent( int iPartId, int iStateId, LPCRECT pContentRect, LPRECT pExtentRect ) const
{
	return __super::GetThemeBackgroundExtent( mhDC, iPartId, iStateId, pContentRect, pExtentRect );
}

HRESULT DcTheme::GetThemeBackgroundRegion( int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion ) const
{
	return __super::GetThemeBackgroundRegion( mhDC, iPartId, iStateId, pRect, pRegion );
}

HRESULT DcTheme::GetThemePartSize( int iPartId, int iStateId, LPCRECT prc, enum THEMESIZE eSize, SIZE *psz ) const
{
	return __super::GetThemePartSize( mhDC, iPartId, iStateId, prc, eSize, psz );
}

HRESULT DcTheme::GetThemeTextExtent( int iPartId, int iStateId, LPCWSTR pszText, int cchCharCount, DWORD dwTextFlags, LPCRECT pBoundingRect, LPRECT pExtentRect ) const
{
	return __super::GetThemeTextExtent( mhDC, iPartId, iStateId, pszText, cchCharCount, dwTextFlags, pBoundingRect, pExtentRect );
}

HRESULT DcTheme::GetThemeTextMetrics( int iPartId, int iStateId, TEXTMETRICW *ptm ) const
{
	return __super::GetThemeTextMetrics( mhDC, iPartId, iStateId, ptm );
}

HRESULT DcTheme::HitTestThemeBackground( int iPartId, int iStateId, DWORD dwOptions, LPCRECT pRect, HRGN hrgn, POINT ptTest, WORD *pwHitTestCode ) const
{
	return __super::HitTestThemeBackground( mhDC, iPartId, iStateId, dwOptions, pRect, hrgn, ptTest, pwHitTestCode );
}

HRESULT DcTheme::DrawThemeEdge( int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect ) const
{
	return __super::DrawThemeEdge( mhDC, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect );
}

HRESULT DcTheme::DrawThemeIcon( int iPartId, int iStateId, LPCRECT pRect, HIMAGELIST himl, int iImageIndex ) const
{
	return __super::DrawThemeIcon( mhDC, iPartId, iStateId, pRect, himl, iImageIndex );
}

HRESULT DcTheme::GetThemeMetric( int iPartId, int iStateId, int iPropId, int *piVal ) const
{
	return __super::GetThemeMetric( mhDC, iPartId, iStateId, iPropId, piVal );
}

HRESULT DcTheme::GetThemeFont( int iPartId, int iStateId, int iPropId, LOGFONTW *pFont ) const
{
	return __super::GetThemeFont( mhDC, iPartId, iStateId, iPropId, pFont );
}

HRESULT DcTheme::GetThemeMargins( int iPartId, int iStateId, int iPropId, LPCRECT prc, MARGINS *pMargins ) const
{
	return __super::GetThemeMargins( mhDC, iPartId, iStateId, iPropId, prc, pMargins );
}

HRESULT DcTheme::DrawThemeParentBackground( const RECT* prc ) const
{
	return __super::DrawThemeParentBackground( mhDC, prc );
}

HRESULT DcTheme::DrawThemeParentBackgroundEx( DWORD dwFlags, const RECT* prc ) const
{
	return __super::DrawThemeParentBackgroundEx( mhDC, dwFlags, prc );
}

HRESULT DcTheme::DrawThemeTextEx( int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const DTTOPTS *pOptions ) const
{
	return __super::DrawThemeTextEx( mhDC, iPartId, iStateId, pszText, cchText, dwTextFlags, pRect, pOptions );
}

HPAINTBUFFER DcTheme::BeginBufferedPaint( const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, HDC *phdc ) const
{
	return __super::BeginBufferedPaint( mhDC, prcTarget, dwFormat, pPaintParams, phdc );
}

HANIMATIONBUFFER DcTheme::BeginBufferedAnimation( const RECT* prcTarget, enum BP_BUFFERFORMAT dwFormat, struct BP_PAINTPARAMS *pPaintParams, struct BP_ANIMATIONPARAMS *pAnimationParams, HDC *phdcFrom, HDC *phdcTo ) const
{
	return __super::BeginBufferedAnimation( mhDC, prcTarget, dwFormat, pPaintParams, pAnimationParams, phdcFrom, phdcTo );
}

BOOL DcTheme::BufferedPaintRenderAnimation( VOID ) const
{
	return __super::BufferedPaintRenderAnimation( mhDC );
}

HRESULT PaintBufferTheme::EndBufferedPaint( BOOL fUpdateTarget ) const
{
	return __super::EndBufferedPaint( mhPaintBuffer, fUpdateTarget );
}

HRESULT PaintBufferTheme::GetBufferedPaintTargetRect( RECT *prc ) const
{
	return __super::GetBufferedPaintTargetRect( mhPaintBuffer, prc );
}

HDC PaintBufferTheme::GetBufferedPaintTargetDC() const
{
	return __super::GetBufferedPaintTargetDC( mhPaintBuffer );
}

HDC PaintBufferTheme::GetBufferedPaintDC() const
{
	return __super::GetBufferedPaintDC( mhPaintBuffer );
}

HRESULT PaintBufferTheme::GetBufferedPaintBits( RGBQUAD **ppbBuffer, int *pcxRow ) const
{
	return __super::GetBufferedPaintBits( mhPaintBuffer, ppbBuffer, pcxRow );
}

HRESULT PaintBufferTheme::BufferedPaintClear( const RECT *prc ) const
{
	return __super::BufferedPaintClear( mhPaintBuffer, prc );
}

HRESULT PaintBufferTheme::BufferedPaintSetAlpha( const RECT *prc, BYTE alpha ) const
{
	return __super::BufferedPaintSetAlpha( mhPaintBuffer, prc, alpha );
}

