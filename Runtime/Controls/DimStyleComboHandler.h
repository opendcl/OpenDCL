// DimStyleComboHandler.h : header file
//

#pragma once

#include "ComboHandler.h"
#include "AutoDocLock.h"


/////////////////////////////////////////////////////////////////////////////
// CDimStyleComboHandler class

class CDimStyleComboHandler : public CComboHandler
{
public:
	CDimStyleComboHandler() {}
	virtual ~CDimStyleComboHandler() {}

public:
	virtual bool PopulateList( CComboBox* pCombo )
		{
			assert( pCombo->GetCount() == 0 );

			CAutoDocReadLock CurDocLock;
			AcDbDatabase* pCurDb = acdbHostApplicationServices()->workingDatabase();
			if( !pCurDb )
				return false;
			AcDbDimStyleTable* pDimStyleTable;
			Acad::ErrorStatus es = 
				pCurDb->getSymbolTable( pDimStyleTable, AcDb::kForRead );
			if( es != Acad::eOk )
				return false;
			AcDbDimStyleTableIterator* pIterator = NULL;
			es = pDimStyleTable->newIterator( pIterator );
			pDimStyleTable->close();
			if( es != Acad::eOk )
				return false;
			bool bSuccess = true;
			int nMinWidth = 0;
			CDC* pComboDC = pCombo->GetDC();
			for( ; !pIterator->done(); pIterator->step() )
			{
				AcDbDimStyleTableRecord* pDimStyleTableRecord = NULL;
				if( pIterator->getRecord( pDimStyleTableRecord, AcDb::kForRead ) != Acad::eOk )
				{
					bSuccess = false;
					continue;
				}
				const ACHAR* pszName;
				if( pDimStyleTableRecord->getName( pszName ) != Acad::eOk )
					bSuccess = false;
				else
				{
					pCombo->AddString( pszName );
					CSize sizeText = pComboDC->GetTextExtent( CString( pszName ) );
					if( sizeText.cx > nMinWidth )
						nMinWidth = sizeText.cx;
				}
				pDimStyleTableRecord->close();
			}
			delete pIterator;
			if( bSuccess && nMinWidth > 0 )
			{
				int nCurrent = pCombo->GetDroppedWidth();
				nMinWidth += 4; //to account for border
				if( nMinWidth > nCurrent )
					pCombo->SetDroppedWidth( nMinWidth );
			}
			pCombo->ReleaseDC( pComboDC );
			return bSuccess;
		}
};
