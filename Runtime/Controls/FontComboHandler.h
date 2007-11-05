// FontComboHandler.h : header file
//

#pragma once

#include "ComboHandler.h"
#include "Workspace.h"
#include "SharedRes.h"
#include <string>
#include <map>

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

public:
	CFontComboHandler()
		{
			mImageList.Create( 15, 13, ILC_COLOR4 | ILC_MASK, 3, 1 );
			HMODULE hRes = theWorkspace.GetResourceModule();
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE( IDI_TRUEFONT ) ) );
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE( IDI_ACADFONT ) ) );
		}
	virtual ~CFontComboHandler() {}

protected:
	virtual bool IsOwnerDrawn() const { return true; }
	virtual UINT GetItemHeight() const { return 16; }
	virtual void DrawItem( CComboBox* pCombo, LPDRAWITEMSTRUCT lpDIS )
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
				mImageList.Draw( pDC, 0, CPoint( rc.left,rc.top ), ILD_TRANSPARENT );
			else if( Info.GetFlags() == 6 )
				mImageList.Draw( pDC, 1, CPoint( rc.left,rc.top ), ILD_TRANSPARENT );
			
			int nX = rc.left; // Save for lines

			rc.left += 17; // Text Position
			
			pDC->TextOut( rc.left, rc.top, sCurFont );
			
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
	virtual bool PopulateList( CComboBox* pCombo )
		{
			assert( pCombo->GetCount() == 0 );
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
					if( sLine1 == _T("AutoCAD-86 unifont 1.0") /*|| sLine1 == _T("AutoCAD-86 shapes 1.0")*/ )
						AddFont( sFilename, 6 );
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
				pCombo->AddString( iter->first.c_str() );
			return true;
		}
	virtual void OnDropdownClose( CComboBox* pCombo )
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
					std::map< tstring, CFontInfo >::iterator iter = mmapFonts.find( (LPCTSTR)sMRUFont7 );
					if( iter != mmapFonts.end() )
						iter->second.SetInUse( false );
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
	static int CALLBACK AFX_EXPORT EnumFamScreenCallBackEx(
			CONST LOGFONT* lplf, CONST TEXTMETRIC* lpntm, DWORD FontType, LPARAM pThis )
		{
			// don't put in non-printer raster fonts
			if( FontType & RASTER_FONTTYPE )
				return 1;
			if( FontType & TRUETYPE_FONTTYPE )
				((CFontComboHandler*)pThis)->AddFont( lplf->lfFaceName, TRUETYPE_FONTTYPE );
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
			if( !EnumFontFamiliesEx( pDC->GetSafeHdc(),	// handle to device context
															 &lf,	// pointer to logical font information
															 EnumFamScreenCallBackEx,	// pointer to callback function
															 (LPARAM)this,	// application-supplied data
															 0 ) )
				return false;
			pCombo->ReleaseDC( pDC );	
			return true;
		}
};
