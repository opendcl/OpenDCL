// TextStyleComboHandler.h : header file
//

#pragma once

#include "ComboHandler.h"
#include "AutoDocLock.h"


/////////////////////////////////////////////////////////////////////////////
// CTextStyleComboHandler class

class CTextStyleComboHandler : public CComboHandler
{
public:
	CTextStyleComboHandler() {}
	virtual ~CTextStyleComboHandler() {}

public:
	virtual bool PopulateList( CComboBox* pCombo )
		{
			assert( pCombo->GetCount() == 0 );

			CAutoDocReadLock CurDocLock;
			AcDbTextStyleTable* pTextStyleTable;
			Acad::ErrorStatus es = 
				acdbHostApplicationServices()->workingDatabase()->getSymbolTable( pTextStyleTable, AcDb::kForRead );
			if( es != Acad::eOk )
				return false;
			AcDbTextStyleTableIterator* pIterator = NULL;
			es = pTextStyleTable->newIterator( pIterator );
			pTextStyleTable->close();
			if( es != Acad::eOk )
				return false;
			bool bSuccess = true;
			for( ; !pIterator->done(); pIterator->step() )
			{
				AcDbTextStyleTableRecord* pTextStyleTableRecord = NULL;
				if( pIterator->getRecord( pTextStyleTableRecord, AcDb::kForRead ) != Acad::eOk )
				{
					bSuccess = false;
					continue;
				}
				const ACHAR* pszName;
				if( pTextStyleTableRecord->getName( pszName ) != Acad::eOk )
					bSuccess = false;
				else
					pCombo->AddString( pszName );
				pTextStyleTableRecord->close();
			}
			delete pIterator;
			return bSuccess;
		}
};
