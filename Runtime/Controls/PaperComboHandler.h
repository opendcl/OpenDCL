// PaperComboHandler.h : header file
//

#pragma once

#include "ComboHandler.h"
#include "DclControlObject.h"
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
			CString sSelection;
			pCombo->GetWindowText( sSelection );
			if( pCombo->GetCount() > 0 )
				pCombo->ResetContent();

			CWaitCursor WaitCursor;
			CAutoDocWriteLock CurDocLock;
			AcDbLayoutManager* pLayoutManager = acdbHostApplicationServices()->layoutManager();
			assert( pLayoutManager != NULL );
			CString sLayout = pLayoutManager->findActiveLayout( true );
			AcDbLayout* pLayoutObj = pLayoutManager->findLayoutNamed( sLayout, true );
			if( !pLayoutObj ) 
				return true;
			AcDbObjectPointer< AcDbLayout > pLayout;
			pLayout.acquire( pLayoutObj );
			AcDbPlotSettingsValidator* pPlotSettingsValidator = acdbHostApplicationServices()->plotSettingsValidator();
			assert( pPlotSettingsValidator != NULL );
			if( !pPlotSettingsValidator ) 
				return true;

			int nCurSel = pCombo->GetCurSel();
			if( nCurSel == 0 )
			{
				pPlotSettingsValidator->setPlotCfgName( pLayout.object(), _T("None"), _T("none_user_media") );
				return true;
			}
			else
			{
				CString sPlotter;
				if( mpPrinterCombo )
				{
					CDialogControl* pDclControl = mpPrinterCombo->GetControlInstance();
					if( pDclControl )
						pDclControl->GetControlWnd()->GetWindowText( sPlotter );
				}
				pPlotSettingsValidator->setPlotCfgName( pLayout.object(), sPlotter );
			}
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
			pCombo->SetCurSel( pCombo->FindString( 0, sSelection ) );
			return true;
		}
	void SetPrinterCombo( TDclControlPtr pCombo ) { mpPrinterCombo = pCombo; }
};
