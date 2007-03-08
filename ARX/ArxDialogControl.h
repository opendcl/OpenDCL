// ArxDialogControl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "ArxControlServices.h"

class CComboBoxFolder;
class CDwgDirList;


class CArxDialogControl : public CDialogControl
{
	// Attributes
protected:
	CArxControlServices	mArxServices;
	
public:
	CArxDialogControl( CDclControlObject* pTemplate, CControlPane* pPane, CWnd* pControl );
	virtual ~CArxDialogControl();

	virtual CArxControlServices* GetArxServices() { return &mArxServices; }

	static TDialogControlPtr Create( CDclControlObject* pTemplate, CControlPane* pPane,
																	 UINT nID, ControlParams* pParams = NULL );

	static TDialogControlPtr CreateComboExControl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nId);	
	static TDialogControlPtr CreateEditControl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nId);
	static TDialogControlPtr CreateComboControl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nId);
	static void UpdateChildControl(CDclControlObject *pControl, CControlPane* pPane, UINT nControlId);
	static void UpdateChildControl(CWnd* pControlWnd, CDclControlObject *pControl, CControlPane* pPane, UINT nControlId);
	static bool IsSelfPopulatedList(CDclControlObject *pControl);
	static void UpdatePropertyInt(CWnd *pWnd, CDclControlObject *pControl, CControlPane* pPane, PropertyId nID);
	static void UpdateProperty(CDclControlObject *pControl, CControlPane* pPane, UINT nControlId, PropertyId nID);
	static void UpdateText(CDclControlObject *pControl, CWnd *pWnd, CString sText);
	static void UpdateFont(CDclControlObject *pControl, CWnd *pWnd, CFont *pFont);
	static void ChangeToolTipText(CDclControlObject *pControl, CWnd *pWnd);
	static void SetDwgListComboFolderLink(CComboBoxFolder *pComboFolder);
	static void SetDwgListComboFolderLink(CDwgDirList *pDwgList);
	static void ResetImageList(CDclControlObject *pControl, CWnd *pWnd, int nID);
};


//This class deletes the control object in its destructor (used for old style and other dynamically 
//created controls)
class CArxAutoDialogControl : public CArxDialogControl
{
public:
	CArxAutoDialogControl( CDclControlObject* pTemplate, CControlPane* pPane, CWnd* pControl )
		: CArxDialogControl( pTemplate, pPane, pControl ) {}
	virtual ~CArxAutoDialogControl() { delete mpControl; }
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
};
