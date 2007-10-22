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
			CString sSelection;
			pCombo->GetWindowText( sSelection );
			int idxMatch = -1;
			if( pCombo->GetCount() > 0 )
				pCombo->ResetContent();

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
				{
					int idx = pCombo->AddString( pszName );
					if( idxMatch < 0 && sSelection.CompareNoCase( pszName ) == 0 )
						idxMatch = idx;
				}
				pTextStyleTableRecord->close();
			}
			delete pIterator;
			if( idxMatch >= 0 )
				pCombo->SetCurSel( idxMatch );
			return bSuccess;
		}
};
