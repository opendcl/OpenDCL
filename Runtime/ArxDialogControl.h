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

	static TDialogControlPtr CreateComboExControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);	
	static TDialogControlPtr CreateEditControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);
	static TDialogControlPtr CreateComboControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);
	static void UpdateChildControl(CWnd* pControlWnd, TDclControlPtr pControl, CControlPane* pPane, UINT nControlId);
	static void UpdatePropertyInt(CWnd *pWnd, TDclControlPtr pControl, CControlPane* pPane, Prop::Id nID);
	static void UpdateProperty(TDclControlPtr pControl, CControlPane* pPane, UINT nControlId, Prop::Id nID);
	static void UpdateText(TDclControlPtr pControl, CWnd *pWnd, CString sText);
	static void UpdateFont(TDclControlPtr pControl, CWnd *pWnd, CFont *pFont);
	static void UpdateToolTip(TDclControlPtr pControl, CWnd *pWnd);
	static void SetDwgListComboFolderLink(CArxDwgListCtrl *pDwgList);
	static void ResetImageList(TDclControlPtr pControl, CWnd *pWnd, int nID);
};


//This class deletes the control object in its destructor (used for old style and other dynamically 
//created controls)
class CArxAutoDialogControl : public CArxDialogControl
{
public:
	CArxAutoDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControl )
		: CArxDialogControl( pTemplate, pPane, pControl ) {}
	virtual ~CArxAutoDialogControl() { delete mpControl; }
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
};
