// ComboHostThemeHelper.h : shared host-theme sync for AcUi combo wrappers
//
#pragma once

#include "DialogControl.h"
#include "HostThemeHelper.h"
#include "PropertyIds.h"
#include "ColorService.h"

// AcUi owner-draw uses GetSysColor / AdUi, not OdclSysColor.
// BRX DrawItem honors GetItemColors. ARX themed DrawItem does not;
// turn off AdUi theming in dark so the same remap can paint.

inline HBRUSH AcUiHostWindowBrush()
{
	static COLORREF s_cr = CLR_NONE;
	static CBrush s_br;
	const COLORREF cr = OdclSysColor( COLOR_WINDOW );
	if( cr != s_cr )
	{
		s_br.DeleteObject();
		s_br.CreateSolidBrush( cr );
		s_cr = cr;
	}
	return (HBRUSH)s_br;
}

inline void SyncAcUiComboHostTheme( CDialogControl* pDlg, CWnd* pWnd )
{
	if( !pWnd || !pWnd->m_hWnd )
		return;
#if defined(ODCL_HOST_COLORTHEME)
	const bool bVisual = (pDlg && pDlg->GetTemplate()
		&& pDlg->GetTemplate()->GetBooleanProperty( Prop::UseVisualStyle ));
#if defined(_ARXTARGET) && !defined(_BRXTARGET)
	CAdUiComboBox* pCombo = DYNAMIC_DOWNCAST( CAdUiComboBox, pWnd );
	if( pCombo )
		pCombo->SetIsThemed( bVisual && !CHostThemeHelper::HostMaps() );
#endif
	LPCWSTR pszTheme = CHostThemeHelper::ThemeClass( bVisual );
	CHostThemeHelper::ApplyTree( pWnd->m_hWnd, pszTheme );
	COMBOBOXINFO cbi = {0};
	cbi.cbSize = sizeof( cbi );
	if( ::GetComboBoxInfo( pWnd->m_hWnd, &cbi ) )
	{
		CHostThemeHelper::InstallNcBorder( cbi.hwndItem );
		CHostThemeHelper::InstallNcBorder( cbi.hwndList );
		if( cbi.hwndList )
			CHostThemeHelper::Apply( cbi.hwndList, CHostThemeHelper::ScrollTheme() );
	}
	pWnd->Invalidate( TRUE );
#else
	UNREFERENCED_PARAMETER( pDlg );
#endif
}

inline void PaintAcUiComboChrome( CWnd* pWnd )
{
#if defined(ODCL_HOST_COLORTHEME)
	if( !pWnd || !pWnd->m_hWnd || !CHostThemeHelper::HostMaps() )
		return;
	COMBOBOXINFO cbi = {0};
	cbi.cbSize = sizeof( cbi );
	if( !::GetComboBoxInfo( pWnd->m_hWnd, &cbi ) )
		return;
	CHostThemeHelper::PaintComboChrome( pWnd->m_hWnd );
	CHostThemeHelper::InstallNcBorder( cbi.hwndList );
#else
	UNREFERENCED_PARAMETER( pWnd );
#endif
}

inline HBRUSH AcUiComboHostCtlColor( CDialogControl* pDlg, CDC* pDC, UINT nCtlColor )
{
	if( pDlg )
	{
		HBRUSH hbr = pDlg->HandleCtlColor( pDC, nCtlColor );
		if( hbr )
			return hbr;
	}
	if( !CHostThemeHelper::HostMaps() || !pDC )
		return NULL;
	pDC->SetTextColor( OdclSysColor( COLOR_WINDOWTEXT ) );
	pDC->SetBkColor( OdclSysColor( COLOR_WINDOW ) );
	pDC->SetBkMode( OPAQUE );
	return AcUiHostWindowBrush();
}

inline bool AcUiIsLightIsland( COLORREF cr )
{
	return GetRValue(cr) >= 240 && GetGValue(cr) >= 240 && GetBValue(cr) >= 240;
}

inline void RemapAcUiComboItemColors( COLORREF& fgColor, COLORREF& bgColor, COLORREF& fillColor )
{
	if( !CHostThemeHelper::HostMaps() )
		return;
	const COLORREF crWin = ::GetSysColor( COLOR_WINDOW );
	const COLORREF crFace = ::GetSysColor( COLOR_BTNFACE );
	const COLORREF crText = ::GetSysColor( COLOR_WINDOWTEXT );
	const COLORREF crBtnText = ::GetSysColor( COLOR_BTNTEXT );
	if( AcUiIsLightIsland( bgColor ) || bgColor == crWin || bgColor == crFace )
		bgColor = OdclSysColor( COLOR_WINDOW );
	if( AcUiIsLightIsland( fillColor ) || fillColor == crWin || fillColor == crFace )
		fillColor = OdclSysColor( COLOR_WINDOW );
	if( fgColor == crText || fgColor == crBtnText
		|| (GetRValue(fgColor) < 80 && GetGValue(fgColor) < 80 && GetBValue(fgColor) < 80) )
		fgColor = OdclSysColor( COLOR_WINDOWTEXT );
}

// ARX dark DrawItem: fill row, SetupForImageDraw, cargo image, remapped text.
// SetupForImageDraw / DrawItemImageFromCargo / DrawTextAndFocusRect are
// protected on CAcUiMRUComboBox, so each CArx* combo friends this template.
// BRX keeps __super::DrawItem (GetItemColors remap is enough).
// ZRX ZwUI.lib exports the helpers from 2017; 2014/2015 declare them only.
// GRX gcui.lib / gcad.lib export them from 2015 (oldest GRX target).
// ARX.16 has no helpers (ODCL_HOST_COLORTHEME is off).
#if defined(_ARXTARGET)
#define ODCL_ACUI_HOST_DRAWITEM 1
#elif defined(_ZRXTARGET) && (_ZRXTARGET >= 2017)
#define ODCL_ACUI_HOST_DRAWITEM 1
#elif defined(_GRXTARGET) && (_GRXTARGET >= 2015)
#define ODCL_ACUI_HOST_DRAWITEM 1
#endif

template<typename TCombo>
inline void OdclAcUiComboHostDrawItem( TCombo* pCombo, LPDRAWITEMSTRUCT lpDrawItemStruct )
{
#if defined(ODCL_HOST_COLORTHEME) && defined(ODCL_ACUI_HOST_DRAWITEM)
	if( !pCombo || !lpDrawItemStruct )
		return;

	COLORREF fgColor = OdclSysColor( COLOR_WINDOWTEXT );
	COLORREF bgColor = OdclSysColor( COLOR_WINDOW );
	COLORREF fillColor = bgColor;
	if( lpDrawItemStruct->itemState & ODS_SELECTED )
	{
		fgColor = OdclSysColor( COLOR_HIGHLIGHTTEXT );
		bgColor = fillColor = OdclSysColor( COLOR_HIGHLIGHT );
	}
	pCombo->GetItemColors( *lpDrawItemStruct, fgColor, bgColor, fillColor );

	CDC* pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
	if( !pDC )
		return;
	CRect rItem( lpDrawItemStruct->rcItem );
	CRect rImage( rItem );
	pDC->FillSolidRect( rItem, fillColor );
	pDC->SetBkColor( bgColor );
	pDC->SetTextColor( fgColor );
	pCombo->SetupForImageDraw( lpDrawItemStruct, pDC, rItem, rImage, fgColor, bgColor );
	RemapAcUiComboItemColors( fgColor, bgColor, fillColor );
	pDC->SetBkColor( bgColor );
	pDC->SetTextColor( fgColor );
	if( (int)lpDrawItemStruct->itemID >= 0 )
		pCombo->DrawItemImageFromCargo( pDC, rImage, (int)lpDrawItemStruct->itemID );
	pCombo->DrawTextAndFocusRect( lpDrawItemStruct, pDC, rItem,
		(int)lpDrawItemStruct->itemID, fgColor, bgColor );
#else
	UNREFERENCED_PARAMETER( pCombo );
	UNREFERENCED_PARAMETER( lpDrawItemStruct );
#endif
}

