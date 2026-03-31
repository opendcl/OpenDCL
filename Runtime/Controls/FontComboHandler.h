// FontComboHandler.h : header file
//

#pragma once

#include "ComboHandler.h"
#include "Workspace.h"
#include "Resource.h"
#include <string>
#include <map>
#include <set>

#define TRUETYPE_FONT		0x0001
#define PRINTER_FONT		0x0002
#define DEVICE_FONT			0x0004

#if defined(_UNICODE)
typedef std::wstring tstring;
#else
typedef std::string tstring;
#endif //_UNICODE


/////////////////////////////////////////////////////////////////////////////
// CFontComboHandler class

class CFontComboHandler : public CComboHandler
{
	class CFontInfo
	{
		bool	mbInUse;
		DWORD	mnFlags;

	public:
		CFontInfo( DWORD nFlags = 0 ) : mnFlags( nFlags ), mbInUse( false ) {}
		CFontInfo( const CFontInfo& FontInfo )
		{
			mnFlags = FontInfo.mnFlags;
			mbInUse = FontInfo.mbInUse;
		}

		DWORD GetFlags() const { return mnFlags; }
		bool IsInUse() const { return mbInUse; }
		void SetInUse( bool bInUse = true ) { mbInUse = bInUse; }
	};

	CImageList mImageList;
	std::map< tstring, CFontInfo > mmapFonts;
	std::set< tstring > msetHiddenFonts;

public:
	CFontComboHandler()
		{
			mImageList.Create( 15, 13, ILC_COLOR32 | ILC_MASK, 2, 1 );
			HMODULE hRes = theWorkspace.GetLocalResourceModule();
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE( IDI_TTFONT ) ) );
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE( IDI_SHXFONT ) ) );
			HKEY hkFontMgmt;
			if( ERROR_SUCCESS == RegOpenKey( HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Font Management"), &hkFontMgmt ) )
			{
				DWORD dwType;
				DWORD cbVal;
				if( ERROR_SUCCESS == RegQueryValueEx( hkFontMgmt, _T("Inactive Fonts"), NULL, &dwType, NULL, &cbVal ) && dwType == REG_MULTI_SZ )
				{
					cbVal += sizeof(TCHAR);
					TCHAR* pszzVal = new TCHAR[cbVal / sizeof(TCHAR)];
					if( ERROR_SUCCESS == RegQueryValueEx( hkFontMgmt, _T("Inactive Fonts"), NULL, &dwType, (LPBYTE)pszzVal, &cbVal ) && dwType == REG_MULTI_SZ )
					{
						TCHAR* pch = pszzVal;
						while( *pch )
						{
							msetHiddenFonts.insert( pch );
							while( *++pch );
							++pch; //skip to next string
						}
					}
					delete[] pszzVal;
				}
				RegCloseKey( hkFontMgmt );
			}
		}
	virtual ~CFontComboHandler() {}

protected:
	bool IsOwnerDrawn() const override { return true; }
	UINT GetItemHeight() const override { return 16; }
	void DrawItem( CComboBox* pCombo, LPDRAWITEMSTRUCT lpDIS ) override
		{
			ASSERT(lpDIS->CtlType == ODT_COMBOBOX); // We've gotta be a combo
			if( lpDIS->itemID >= (UINT)pCombo->GetCount() )
				return;

			// Lets make a CDC for ease of use
			CDC *pDC = CDC::FromHandle( lpDIS->hDC );

			ASSERT(pDC); // Attached failed

			CRect rc( lpDIS->rcItem );

			// Save off context attributes
			int nIndexDC = pDC->SaveDC();

			CBrush brushFill;

			// Draw selection state
			if (lpDIS->itemState & ODS_SELECTED)
			{
				brushFill.CreateSolidBrush(::GetSysColor(COLOR_HIGHLIGHT));
				pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
			}
			else
				brushFill.CreateSolidBrush(pDC->GetBkColor());

			pDC->SetBkMode(TRANSPARENT);
			pDC->FillRect(rc, &brushFill);
			brushFill.DeleteObject();

			CString sCurFont;
			pCombo->GetLBText( lpDIS->itemID, sCurFont );
			CFontInfo Info( 0 );
			std::map< tstring, CFontInfo >::const_iterator iter = mmapFonts.find( (LPCTSTR)sCurFont );
			if( iter != mmapFonts.end() )
				Info = (iter->second);
			if( Info.GetFlags() == 4 )
				mImageList.Draw( pDC, 0, CPoint( rc.left, rc.bottom - 16 ), ILD_TRANSPARENT );
			else if( Info.GetFlags() == 6 )
				mImageList.Draw( pDC, 1, CPoint( rc.left, rc.bottom - 16 ), ILD_TRANSPARENT );

			int nX = rc.left; // Save for lines

			rc.left += 17; // Text Position
			rc.bottom--;

			pDC->SetTextAlign(TA_LEFT | TA_BASELINE);
			pDC->TextOut( rc.left, rc.bottom - 2, sCurFont );

			if( pCombo->GetItemData( lpDIS->itemID ) == 1 )
			{
				if( pCombo->GetItemData( lpDIS->itemID + 1 ) != 1 )
				{
					// Draw font MRU separator ==============
					TEXTMETRIC tm;
					pDC->GetTextMetrics( &tm );
					pDC->MoveTo( nX, rc.top + tm.tmHeight);
					pDC->LineTo( rc.right, rc.top + tm.tmHeight );
					pDC->MoveTo( nX, rc. top + tm.tmHeight + 2 );
					pDC->LineTo( rc.right, rc.top + tm.tmHeight + 2 );
				}
			}

			// Draw focus rectangle
			if( lpDIS->itemState & ODS_FOCUS )
				pDC->DrawFocusRect( &lpDIS->rcItem );

			// Restore State of context
			pDC->RestoreDC( nIndexDC );
		}
	bool PopulateList( CComboBox* pCombo ) override
		{
			mmapFonts.clear();
			// get the list of existing AutoCAD shx fonts
			CString sFontPath = GetFontPath();
			CFileFind finder;
			BOOL bResult = finder.FindFile(sFontPath + _T("\\*.shx"));
			while (bResult)
			{
				bResult = finder.FindNextFile();
				if (finder.IsDots()) // skip . and .. files
					continue;
				TRY
				{
					CString sFilename = finder.GetFileName().MakeLower();
					CStdioFile fFont( sFontPath + _T("\\") + sFilename, CFile::modeRead | CFile::typeText );
					CString sLine1;
					fFont.ReadString( sLine1 );
					if( sLine1 == _T("AutoCAD-86 unifont 1.0") )
						AddFont( sFilename, 6 );
					//else if( sLine1 == _T("AutoCAD-86 shapes 1.0") )
					//{
					// TODO: figure out how to distinguish between font shape files and non-font shape files
					// as AutoCAD does (e.g. dim.shx is displayed in the STYLE dropdown, genltshp.shx is not.)
					//	AddFont( sFilename, 6 );
					//}
				}
				CATCH( CFileException, e ) {} END_CATCH
			}
			finder.Close();

			EnumerateFonts( pCombo );

			// add the sysytem fonts manually because they are not added otherwise
			pCombo->AddString( _T("MS Shell Dlg"));
			pCombo->AddString( _T("MS Sans Serif"));
			pCombo->AddString( _T("MS Serif"));
			pCombo->AddString( _T("System"));
			pCombo->AddString( _T("Terminal"));
			pCombo->AddString( _T("Small Fonts"));
			pCombo->AddString( _T("Fixedsys"));
			pCombo->AddString( _T("Courier"));

			for( std::map< tstring, CFontInfo >::const_iterator iter = mmapFonts.begin(); iter != mmapFonts.end(); ++iter )
			{
				const tstring& font = iter->first;
				if( msetHiddenFonts.find( font ) != msetHiddenFonts.end() )
					continue;
				LPCTSTR pszFont = font.c_str();
				if( *pszFont == _T('@') && msetHiddenFonts.find( pszFont + 1 ) != msetHiddenFonts.end() )
					continue;
				pCombo->AddString( pszFont );
			}
			return true;
		}
	void OnDropdownClose( CComboBox* pCombo ) override
		{
			CString sFont;
			pCombo->GetWindowText( sFont );
			std::map< tstring, CFontInfo >::iterator iter = mmapFonts.find( (LPCTSTR)sFont );
			if( iter == mmapFonts.end() )
				return;

			// Check the font is not already in use
			if( !iter->second.IsInUse() )
			{
				// Set the font as the top most and selected font
				iter->second.SetInUse();
				pCombo->InsertString( 0, sFont );
				pCombo->SetItemData( 0, 1 );

				// Only hold six MRU'ed fonts, so drop the 7th.
				CString sMRUFont7;
				pCombo->GetLBText( 6, sMRUFont7 );
				if( !sMRUFont7.IsEmpty() )
				{
					std::map< tstring, CFontInfo >::iterator iterFont = mmapFonts.find( (LPCTSTR)sMRUFont7 );
					if( iterFont != mmapFonts.end() )
						iterFont->second.SetInUse( false );
					pCombo->DeleteString( 6 );
				}
			}
			else
			{
				// If font is aleady MRU'ed position it as the first font
				int nSel = pCombo->FindString( -1, sFont );
				if( nSel != CB_ERR )
				{
					// Remove from original position
					pCombo->DeleteString( nSel );
					// Restore to begining of list
					pCombo->InsertString( 0, sFont );
					pCombo->SetItemData( 0, 1 ); // Mark as selected
				}
			}
			pCombo->SetCurSel( 0 );	// Bring as current selection
		}

protected:
	struct _EnumFontState { CFontComboHandler* pHandler; CComboBox* pCombo; };
	static int CALLBACK AFX_EXPORT EnumFamScreenCallBackEx(
			CONST LOGFONT* lplf, CONST TEXTMETRIC* lpntm, DWORD FontType, LPARAM pState )
		{
			// don't put in non-printer raster fonts
			if( FontType & RASTER_FONTTYPE )
				return 1;
			if( FontType & TRUETYPE_FONTTYPE )
			{
				_EnumFontState& state = *(_EnumFontState*)pState;
				state.pHandler->AddFont( lplf->lfFaceName, TRUETYPE_FONTTYPE );
			}
			return 1; // Call me back
		}
	void AddFont( const CString& sFont, DWORD nFlags ) { mmapFonts[(LPCTSTR)sFont] = CFontInfo( nFlags ); }
	CString GetFontPath()
		{
			CString sChar;
			CString sAcadPrefix;
			CString sCompile;
			struct resbuf rb;
			acedGetVar(_T("ACADPREFIX"), &rb);
			sAcadPrefix = rb.resval.rstring;
			for (int i=0; i<sAcadPrefix.GetLength(); i++)
			{
				sChar = sAcadPrefix.Mid(i,1);
				if (sChar != _T(";"))
					sCompile = sCompile + sChar;
				else
				{
					if (sCompile.Right(5) == _T("fonts"))
						return sCompile;
					sCompile = _T("");
				}
			}
			return CString();
		}
	bool EnumerateFonts( CComboBox* pCombo )
		{
			CDC* pDC = pCombo->GetDC();
			LOGFONT lf;
			ZeroMemory( &lf, sizeof(lf) );
			lf.lfCharSet = DEFAULT_CHARSET;
			_EnumFontState state = { this, pCombo };
			int nResult = EnumFontFamiliesEx( pDC->GetSafeHdc(),	// handle to device context
																			 &lf,	// pointer to logical font information
																			 EnumFamScreenCallBackEx,	// pointer to callback function
																			 (LPARAM)&state,	// application-supplied data
																			 0 );
			pCombo->ReleaseDC( pDC );
			if( nResult == 0 )
				return false;
			return true;
		}
};
