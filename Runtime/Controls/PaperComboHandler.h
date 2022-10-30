// PaperComboHandler.h : header file
//

#pragma once

#include "ComboHandler.h"
#include "DclControlTemplate.h"
#include "AutoDocLock.h"


/////////////////////////////////////////////////////////////////////////////
// CPaperComboHandler class

class CPaperComboHandler : public CComboHandler
{
	TDclControlPtr mpPrinterCombo;

public:
	CPaperComboHandler( TDclControlPtr pPrinterCombo = NULL ) : mpPrinterCombo( pPrinterCombo ) {}
	virtual ~CPaperComboHandler() {}

public:
	virtual bool PopulateList( CComboBox* pCombo )
		{
			CWaitCursor WaitCursor;
			CAutoDocWriteLock CurDocLock;
			AcDbLayoutManager* pLayoutManager = acdbHostApplicationServices()->layoutManager();
			assert( pLayoutManager != NULL );
			if( !pLayoutManager ) 
				return false;
			CString sLayout = pLayoutManager->findActiveLayout( true );
#if (_ARXTARGET >= 22 || _ZRXTARGET >= 2021 || _GRXTARGET >= 2023 || _BRXTARGET >= 22)
			AcDbObjectId idLayout = pLayoutManager->findLayoutNamed( sLayout );
			if( idLayout.isNull() ) 
				return true;
			AcDbObjectPointer< AcDbLayout > pLayout;
			pLayout.open( idLayout, AcDb::kForWrite );
			if( pLayout.openStatus() != Acad::eOk ) 
				return true;
#else
			AcDbLayout* pLayoutObj = pLayoutManager->findLayoutNamed( sLayout, true );
			if( !pLayoutObj ) 
				return true;
			AcDbObjectPointer< AcDbLayout > pLayout;
			pLayout.acquire( pLayoutObj );
#endif
			AcDbPlotSettingsValidator* pPlotSettingsValidator = acdbHostApplicationServices()->plotSettingsValidator();
			assert( pPlotSettingsValidator != NULL );
			if( !pPlotSettingsValidator ) 
				return true;

			CString sPlotter;
			if( mpPrinterCombo )
			{
				CDialogControl* pDclControl = mpPrinterCombo->GetControlInstance();
				if( pDclControl )
					pDclControl->GetControlWnd()->GetWindowText( sPlotter );
			}
			pPlotSettingsValidator->setPlotCfgName( pLayout.object(), sPlotter );
			pPlotSettingsValidator->refreshLists( pLayout.object() );
			AcArray< const TCHAR* > mediaList;
			Acad::ErrorStatus es = pPlotSettingsValidator->canonicalMediaNameList( pLayout.object(), mediaList );
			if( es != Acad::eOk )
				return false;
			for( int idx = 0; idx < mediaList.length(); ++idx )
			{
				const ACHAR* pszPaperSize;
				pPlotSettingsValidator->getLocaleMediaName( pLayout.object(), idx, pszPaperSize );
				pCombo->AddString( pszPaperSize );
			}
			return true;
		}
	void SetPrinterCombo( TDclControlPtr pCombo ) { mpPrinterCombo = pCombo; }
};
