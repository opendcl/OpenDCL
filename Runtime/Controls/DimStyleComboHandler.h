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
			CString sSelection;
			pCombo->GetWindowText( sSelection );
			int idxMatch = -1;
			if( pCombo->GetCount() > 0 )
				pCombo->ResetContent();

			CAutoDocReadLock CurDocLock;
			AcDbDimStyleTable* pDimStyleTable;
			Acad::ErrorStatus es = 
				acdbHostApplicationServices()->workingDatabase()->getSymbolTable( pDimStyleTable, AcDb::kForRead );
			if( es != Acad::eOk )
				return false;
			AcDbDimStyleTableIterator* pIterator = NULL;
			es = pDimStyleTable->newIterator( pIterator );
			pDimStyleTable->close();
			if( es != Acad::eOk )
				return false;
			bool bSuccess = true;
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
					int idx = pCombo->AddString( pszName );
					if( idxMatch < 0 && sSelection.CompareNoCase( pszName ) == 0 )
						idxMatch = idx;
				}
				pDimStyleTableRecord->close();
			}
			delete pIterator;
			if( idxMatch >= 0 )
				pCombo->SetCurSel( idxMatch );
			return bSuccess;
		}
};
