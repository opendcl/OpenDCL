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

protected:
	static TDialogControlPtr CreateImp( TDclControlPtr pTemplate, CControlPane* pPane,
																			UINT nID, ControlParams* pParams = NULL );
	static TDialogControlPtr CreateComboExControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);	
	static TDialogControlPtr CreateEditControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);
	static TDialogControlPtr CreateComboControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);
};
