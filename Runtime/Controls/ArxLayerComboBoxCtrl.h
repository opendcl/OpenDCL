// ArxLayerComboBoxCtrl.h : header file
//

#pragma once

#include "ArxComboBoxCtrl.h"
#include "LayerComboHandler.h"


/////////////////////////////////////////////////////////////////////////////
// CArxLayerComboBoxCtrl window

class CArxLayerComboBoxCtrl : public CArxComboBoxCtrl
{
	class CDocReactor : public AcApDocManagerReactor
	{
		CArxLayerComboBoxCtrl* mpCombo;
		CComboHandler* mpHandler;
	public:
		CDocReactor( CArxLayerComboBoxCtrl* pCombo )
			: mpCombo( pCombo )
			, mpHandler( pCombo? pCombo->GetComboHandler() : NULL )
			{ if( mpHandler ) acDocManager->addReactor( this ); else mpHandler = NULL; }
		~CDocReactor()
			{ if( mpHandler ) acDocManager->removeReactor( this ); }
		void documentActivated(AcApDocument* pActivatedDoc)
			{ if( mpHandler ) mpHandler->PopulateList( mpCombo ); }
		void documentDestroyed(const char* filename)
			{ if( mpHandler ) mpHandler->PopulateList( mpCombo ); }
	} mDocReactor;
public:
	CArxLayerComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true )
		: CArxComboBoxCtrl( pTemplate, pPane, nID, new CLayerComboHandler, bCreate )
		, mDocReactor( this )
		{}
	virtual ~CArxLayerComboBoxCtrl() {}
};
