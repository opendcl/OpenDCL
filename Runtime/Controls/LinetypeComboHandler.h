// LinetypeComboHandler.h : header file
//

#pragma once

#include "ComboHandler.h"
#include "AutoDocLock.h"


/////////////////////////////////////////////////////////////////////////////
// CLinetypeComboHandler class

class CLinetypeComboHandler : public CComboHandler
{
public:
	CLinetypeComboHandler() {}
	virtual ~CLinetypeComboHandler() {}

public:
	virtual bool PopulateList( CComboBox* pCombo )
		{
			assert( pCombo->GetCount() == 0 );

			CAutoDocReadLock CurDocLock;
			AcDbDatabase* pCurDb = acdbHostApplicationServices()->workingDatabase();
			if( !pCurDb )
				return false;
			AcDbLinetypeTable* pLinetypeTable;
			Acad::ErrorStatus es = 
				acdbHostApplicationServices()->workingDatabase()->getSymbolTable( pLinetypeTable, AcDb::kForRead );
			if( es != Acad::eOk )
				return false;
			AcDbLinetypeTableIterator* pIterator = NULL;
			es = pLinetypeTable->newIterator( pIterator );
			pLinetypeTable->close();
			if( es != Acad::eOk )
				return false;
			bool bSuccess = true;
			for( ; !pIterator->done(); pIterator->step() )
			{
				AcDbLinetypeTableRecord* pLinetypeTableRecord = NULL;
				if( pIterator->getRecord( pLinetypeTableRecord, AcDb::kForRead ) != Acad::eOk )
				{
					bSuccess = false;
					continue;
				}
				const ACHAR* pszName;
				if( pLinetypeTableRecord->getName( pszName ) != Acad::eOk )
					bSuccess = false;
				else
					pCombo->AddString( pszName );
				pLinetypeTableRecord->close();
			}
			delete pIterator;
			return bSuccess;
		}
};
