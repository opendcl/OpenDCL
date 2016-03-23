// PrinterComboHandler.h : header file
//

#pragma once

#include "ComboHandler.h"
#include "Workspace.h"
#include "AutoDocLock.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// CPrinterComboHandler class

class CPrinterComboHandler : public CComboHandler
{
	CImageList mImageList;
	std::vector< int > mrImage;

public:
	CPrinterComboHandler()
		{
			mImageList.Create( 16, 14, ILC_COLOR32 | ILC_MASK, 2, 1 );
			HMODULE hRes = theWorkspace.GetLocalResourceModule();
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE(IDI_PRINTER) ) );
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE(IDI_PLOTTER) ) );
		}
	virtual ~CPrinterComboHandler() {}

public:
	virtual bool IsOwnerDrawn() const { return true; }
	virtual bool IsAutoSorted() const { return false; }
	virtual void DrawItem( CComboBox* pCombo, LPDRAWITEMSTRUCT lpDrawItemStruct )
		{
			size_t idxItem = lpDrawItemStruct->itemID;
			if( idxItem >= mrImage.size() )
				return;

			CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
			int nIndexDC = pDC->SaveDC();
			CRect rc( lpDrawItemStruct->rcItem );
			CBrush brushFill;
			if( lpDrawItemStruct->itemState & ODS_SELECTED )
			{
				brushFill.CreateSolidBrush( ::GetSysColor( COLOR_HIGHLIGHT ) );
				pDC->SetTextColor( ::GetSysColor( COLOR_HIGHLIGHTTEXT ) );
			}
			else
				brushFill.CreateSolidBrush( pDC->GetBkColor() );
			pDC->SetBkMode( TRANSPARENT );
			pDC->FillRect( rc, &brushFill );
			mImageList.Draw( pDC, mrImage.at( idxItem ), CPoint( rc.left, rc.top ), ILD_TRANSPARENT );
			CString sText;
			pCombo->GetLBText( idxItem, sText );
			pDC->TextOut( rc.left + 18, rc.top, sText );
			if( lpDrawItemStruct->itemState & ODS_FOCUS )
				pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
			pDC->RestoreDC( nIndexDC );
		}
	virtual bool PopulateList( CComboBox* pCombo )
		{
			assert( pCombo->GetCount() == 0 );
			mrImage.clear();

			CWaitCursor WaitCursor;
			CAutoDocReadLock CurDocLock;
			AcDbPlotSettingsValidator* pPlotSettingsValidator = acdbHostApplicationServices()->plotSettingsValidator();
			if( !pPlotSettingsValidator )
				return false;
			assert( pPlotSettingsValidator != NULL );
			AcArray< const TCHAR* > rsDevices;
			pPlotSettingsValidator->plotDeviceList( rsDevices );
			for( int idx = 0; idx < rsDevices.length(); ++idx )
			{
				pCombo->AddString( rsDevices[idx] );
				mrImage.push_back( 0 );
			}

			CString sPlottersPath = GetPlottersPath();
			CFileFind finder;
			BOOL bResult = finder.FindFile( sPlottersPath + _T("\\*.pc3") );
			while( bResult )
			{
				bResult = finder.FindNextFile();
				if( finder.IsDots() )
					continue; // skip . and .. files; otherwise, we'd recurse infinitely!
				CString sPC3Name = finder.GetFileName();
				int nFound = pCombo->FindStringExact( 0, sPC3Name );
				if( nFound > -1 )
					mrImage[nFound] = 1;
				else
				{
					pCombo->AddString( sPC3Name );
					mrImage.push_back( 1 );
				}
			}
			finder.Close();
			return true;
		}
	static LPCTSTR GetPlottersPath()
		{
			static TCHAR szPlottersPath[MAX_PATH] = _T("");
			if( !szPlottersPath[0] )
				acedGetEnv( _T("PrinterConfigDir"), szPlottersPath );
			return szPlottersPath;
		}
};
