// ArxDialogControl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "ArxControlServices.h"

class CComboBoxFolder;
class CArxDwgListCtrl;


class CArxDialogControl : public CDialogControl
{
	// Attributes
protected:
	CArxControlServices	mArxServices;
	
public:
	CArxDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControl );
	virtual ~CArxDialogControl();

	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }

	static TDialogControlPtr Create( TDclControlPtr pTemplate, CControlPane* pPane,
																	 UINT nID, ControlParams* pParams = NULL );

	static void UpdateAllProperties(TDclControlPtr pTemplate);
	static void UpdateProperty(TDclControlPtr pTemplate, Prop::Id id);

protected:
	static TDialogControlPtr CreateImp( TDclControlPtr pTemplate, CControlPane* pPane,
																			UINT nID, ControlParams* pParams = NULL );
	static TDialogControlPtr CreateComboExControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);	
	static TDialogControlPtr CreateEditControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);
	static TDialogControlPtr CreateComboControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);
};


//This class deletes the control object in its destructor (used for old style and other dynamically 
//created controls)
class CAutoArxDialogControl : public CArxDialogControl
{
public:
	CAutoArxDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControlWnd )
		: CArxDialogControl( pTemplate, pPane, pControlWnd ) { UpdateAllProperties( pTemplate ); }
	virtual ~CAutoArxDialogControl() { delete mpControlWnd; }
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
};
