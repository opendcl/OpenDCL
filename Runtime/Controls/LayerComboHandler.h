// LayerComboHandler.h : header file
//

#pragma once

#include "ComboHandler.h"
#include "Workspace.h"
#include "AutoDocLock.h"
#include "Resource.h"
#include <vector>


/////////////////////////////////////////////////////////////////////////////
// CLayerComboHandler class

class CLayerComboHandler : public CComboHandler
{
	CImageList mImageList;
	struct _lprop
	{
		AcCmColor color;
		bool frozen;
		bool off;
		_lprop()
			: frozen( false ), off( false ) {}
		_lprop( const AcCmColor& Color, bool bFrozen, bool bOff )
			: color( Color ), frozen( bFrozen ), off( bOff ) {}
	};
	std::map< CString, _lprop > mmapLayerProperties;

public:
	CLayerComboHandler()
		{
			mImageList.Create( 16, 16, ILC_COLOR32 | ILC_MASK, 4, 1 );
			HMODULE hRes = theWorkspace.GetLocalResourceModule();
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE(IDI_LAYERON) ) );
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE(IDI_LAYEROFF) ) );
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE(IDI_LAYERTHAWED) ) );
			mImageList.Add( LoadIcon( hRes, MAKEINTRESOURCE(IDI_LAYERFROZEN) ) );
		}
	virtual ~CLayerComboHandler() {}

protected:
	bool LookupLayerProps( LPCTSTR pszLayerName, _lprop& props )
	{
		CString sName( pszLayerName );
		sName.MakeUpper();
		std::map< CString, _lprop >::const_iterator iterLayer = mmapLayerProperties.find(sName);
		if( iterLayer == mmapLayerProperties.end() )
			return false;
		props = iterLayer->second;
		return true;
	}
	bool HasLayerProps( LPCTSTR pszLayerName )
	{
		CString sName( pszLayerName );
		sName.MakeUpper();
		std::map< CString, _lprop >::const_iterator iterLayer = mmapLayerProperties.find(sName);
		return (iterLayer != mmapLayerProperties.end());
	}

public:
	virtual bool IsOwnerDrawn() const { return true; }
	virtual UINT GetItemHeight() const { return 16; }
	virtual void DrawItem( CComboBox* pCombo, LPDRAWITEMSTRUCT lpDrawItemStruct )
		{
			CDC *pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
			int nIndexDC = pDC->SaveDC();
			CRect rc( lpDrawItemStruct->rcItem );

			DcTheme Theme( pCombo->m_hWnd, VSCLASS_COMBOBOX, pDC->GetSafeHdc() );
			if (Theme)
			{
				if (lpDrawItemStruct->itemState & ODS_SELECTED)
				{
					pDC->FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT));
					pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				}
				else if (lpDrawItemStruct->itemState & ODS_DISABLED)
				{
					Theme.DrawThemeBackground(CP_BACKGROUND, CBTBS_DISABLED, &rc, NULL);
					pDC->SetTextColor(GetSysColor(COLOR_GRAYTEXT));
				}
				else
				{
					Theme.DrawThemeBackground(CP_BACKGROUND, CBTBS_NORMAL, &rc, NULL);
					pDC->SetTextColor(GetSysColor(COLOR_BTNTEXT));
				}
			}
			else
			{
				if (lpDrawItemStruct->itemState & ODS_SELECTED)
				{
					pDC->FillSolidRect(rc, GetSysColor(COLOR_HIGHLIGHT));
					pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
				}
				else if (lpDrawItemStruct->itemState & ODS_DISABLED)
					pDC->FillSolidRect(rc, GetSysColor(COLOR_3DFACE));
				else
					pDC->FillSolidRect(rc, pDC->GetBkColor());
			}

			int idxItem = (int)lpDrawItemStruct->itemID;
			if(idxItem >= 0)
			{
				CString sName;
				pCombo->GetLBText(idxItem, sName);
				_lprop lprop;
				if( LookupLayerProps(sName, lprop) )
				{
					CRect rcImage( rc );
					rcImage.left += 2;
					rcImage.right = rcImage.left + 13;
					rcImage.bottom = rcImage.top + 13;

					CPoint point;
					CRect rcColor( rcImage );
					rcColor.top++;
					rcColor.bottom++;
					CPen penColorBorder( PS_SOLID, 1, RGB(0,0,0) );
					CPen* pOldPen = pDC->SelectObject( &penColorBorder );
					pDC->Rectangle( &rcColor );
					pDC->SelectObject( pOldPen );
					rcColor.DeflateRect( 1, 1 );
					COLORREF crFill =
						(lprop.color.isByColor()? RGB(lprop.color.red(),lprop.color.green(),lprop.color.blue()) :
																			GetRGBColor( lprop.color.colorIndex() ));
					CBrush brFill( crFill );
					pDC->FillRect( &rcColor, &brFill );
					mImageList.Draw( pDC, lprop.off? 1 : 0, CPoint( rcImage.left + 14, rcImage.top ), ILD_TRANSPARENT );
					mImageList.Draw( pDC, lprop.frozen? 3 : 2, CPoint( rcImage.left + 28, rcImage.top ), ILD_TRANSPARENT );
				}
				rc.left += 48; // Text Position
				rc.top++;
				pDC->SetBkMode(TRANSPARENT);
				pDC->TextOut( rc.left, rc.top, sName );
			}
			if( lpDrawItemStruct->itemState & ODS_FOCUS )
				pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
			pDC->RestoreDC( nIndexDC );
		}
	virtual bool PopulateList( CComboBox* pCombo )
		{
			CString sSelected;
			pCombo->GetWindowText(sSelected);
			//Delete all existing layer names (and leave the items which are not layers)
			for(int idxItem = pCombo->GetCount() - 1; idxItem >= 0; --idxItem)
			{
				CString sName;
				pCombo->GetLBText(idxItem, sName);
				if( HasLayerProps(sName) )
					pCombo->DeleteString(idxItem);
			}
			mmapLayerProperties.clear();

			CAutoDocReadLock CurDocLock;
			AcDbDatabase* pCurDb = acdbHostApplicationServices()->workingDatabase();
			if( !pCurDb )
				return true;
			AcDbLayerTable* pLayerTable;
			Acad::ErrorStatus es =
				pCurDb->getSymbolTable( pLayerTable, AcDb::kForRead );
			if( es != Acad::eOk )
				return false;
			AcDbLayerTableIterator* pIterator = NULL;
			es = pLayerTable->newIterator( pIterator );
			pLayerTable->close();
			if( es != Acad::eOk )
				return false;
			bool bSuccess = true;
			int nMinWidth = 0;
			CDC* pComboDC = pCombo->GetDC();
			for( ; !pIterator->done(); pIterator->step() )
			{
				AcDbLayerTableRecord* pLayerTableRecord = NULL;
				if( pIterator->getRecord( pLayerTableRecord, AcDb::kForRead ) != Acad::eOk )
				{
					bSuccess = false;
					continue;
				}
				const ACHAR* pszName;
				if( pLayerTableRecord->getName( pszName ) != Acad::eOk )
					bSuccess = false;
				else
				{
					int idx = pCombo->AddString( pszName );
					CSize sizeText = pComboDC->GetTextExtent( CString( pszName ) );
					if( sizeText.cx > nMinWidth )
						nMinWidth = sizeText.cx;
					CString sName( pszName );
					sName.MakeUpper();
					mmapLayerProperties[sName] = _lprop( pLayerTableRecord->color(),
																							 pLayerTableRecord->isFrozen(),
																							 pLayerTableRecord->isOff() );
				}
				pLayerTableRecord->close();
			}
			delete pIterator;
			if( bSuccess && nMinWidth > 0 )
			{
				int nCurrent = pCombo->GetDroppedWidth();
				nMinWidth += 50; //to account for images and border
				if( nMinWidth > nCurrent )
					pCombo->SetDroppedWidth( nMinWidth );
			}
			int nSelected = pCombo->FindStringExact( -1, sSelected );
			if( nSelected >= 0 )
				pCombo->SetCurSel( nSelected );
			pCombo->ReleaseDC( pComboDC );
			return bSuccess;
		}
};
