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
		CString name;
		AcCmColor color;
		bool frozen;
		bool off;
		_lprop( LPCTSTR pszName, const AcCmColor& Color, bool bFrozen, bool bOff )
			: name( pszName ), color( Color ), frozen( bFrozen ), off( bOff ) {}
	};
	std::vector< _lprop > mrLayerProperties;

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

public:
	virtual bool IsOwnerDrawn() const { return true; }
	virtual UINT GetItemHeight() const { return 16; }
	virtual void DrawItem( CComboBox* pCombo, LPDRAWITEMSTRUCT lpDrawItemStruct )
		{
			size_t idxItem = lpDrawItemStruct->itemID;
			if( idxItem >= mrLayerProperties.size() )
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
			CRect rcImage( rc );
			rcImage.left += 2;
			rcImage.right = rcImage.left + 13;
			rcImage.bottom = rcImage.top + 13;

			const _lprop& lprop = mrLayerProperties.at( idxItem );
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
			int nX = rc.left; // Save for lines
			rc.left += 48; // Text Position
			rc.top++;
			pDC->TextOut( rc.left, rc.top, lprop.name );
			if( lpDrawItemStruct->itemState & ODS_FOCUS )
				pDC->DrawFocusRect( &lpDrawItemStruct->rcItem );
			pDC->RestoreDC( nIndexDC );
		}
	virtual bool PopulateList( CComboBox* pCombo )
		{
			assert( pCombo->GetCount() == 0 );
			mrLayerProperties.clear();

			CAutoDocReadLock CurDocLock;
			AcDbLayerTable* pLayerTable;
			Acad::ErrorStatus es = 
				acdbHostApplicationServices()->workingDatabase()->getSymbolTable( pLayerTable, AcDb::kForRead );
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
					mrLayerProperties.insert( mrLayerProperties.begin() + idx,
																		_lprop( pszName,
																						pLayerTableRecord->color(),
																						pLayerTableRecord->isFrozen(),
																						pLayerTableRecord->isOff() ) );
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
			pCombo->ReleaseDC( pComboDC );
			return bSuccess;
		}
};
