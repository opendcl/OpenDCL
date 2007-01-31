// CfgTabPane.h : header file
//

#pragma once

#include "Resource.h"
#include "ControlPane.h"

class CDclFormObject;
class CDclControlObject;
class CArxDialogControl;
class CControlobject;
class CFontCollection;


/////////////////////////////////////////////////////////////////////////////
// CfgTabPane dialog

class CfgTabPane : public CAcUiTabChildDialog
{
protected:
	CDclFormObject* mpSourceForm;
	CControlPane mControlPane;
	CDclControlObject* mpControl;

public:
	CDclControlObject	*m_ArxControl;
	CString				m_sProjectName;
	CString				m_sDialogName;
	CList<CArxDialogControl*> m_ControlCol;
	CFontCollection		*m_pFontCollection;

	// Construction
public:
	CfgTabPane(CDclFormObject* pSourceForm, CWnd *pParent = NULL, HINSTANCE hDialogResource = NULL) ;
	virtual ~CfgTabPane();

	//Attributes
	const CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	CDclFormObject* GetSourceForm() { return mpSourceForm; }
	const CControlPane& GetControlPane() const { return mControlPane; }
	CControlPane& GetControlPane() { return mControlPane; }
	const CDclControlObject* GetControl() const { return mpControl; }
	CDclControlObject* GetControl() { return mpControl; }

	//virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnMainDialogAPPLY();
  virtual void OnMainDialogCancel();
  virtual void OnMainDialogOK();
  virtual BOOL OnMainDialogHelp();    

	virtual void OnTabActivation (BOOL bActivate) ;
	virtual BOOL OnTabChanging () ;

	// Dialog Data
	enum { IDD = IDD_CFGTAB };

	// Overrides
	protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void PostNcDestroy();

	// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
};
