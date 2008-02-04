// ArxLinetypeComboBoxCtrl.h : header file
//

#pragma once

#include "ArxComboBoxCtrl.h"
#include "LinetypeComboHandler.h"


/////////////////////////////////////////////////////////////////////////////
// CArxLinetypeComboBoxCtrl window

class CArxLinetypeComboBoxCtrl : public CArxComboBoxCtrl
{
	class CDocReactor : public AcApDocManagerReactor
	{
		CArxLinetypeComboBoxCtrl* mpCombo;
		CComboHandler* mpHandler;
	public:
		CDocReactor( CArxLinetypeComboBoxCtrl* pCombo )
			: mpCombo( pCombo )
			, mpHandler( pCombo? pCombo->GetComboHandler() : NULL )
			{ if( mpHandler ) acDocManager->addReactor( this ); else mpHandler = NULL; }
		~CDocReactor()
			{ if( mpHandler ) acDocManager->removeReactor( this ); }
		void documentActivated(AcApDocument* pActivatedDoc)
			{ if( mpCombo ) mpCombo->ResetContent(); }
		void documentDestroyed(const char* filename)
			{ if( mpCombo ) mpCombo->ResetContent(); }
	} mDocReactor;
public:
	CArxLinetypeComboBoxCtrl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true )
		: CArxComboBoxCtrl( pTemplate, pPane, nID, new CLinetypeComboHandler, bCreate )
		, mDocReactor( this )
		{}
	virtual ~CArxLinetypeComboBoxCtrl() {}
};
