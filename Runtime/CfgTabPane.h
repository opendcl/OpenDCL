// CfgTabPane.h : header file
//

#pragma once

#include "ArxDialogObject.h"
#include "Resource.h"

class CControlobject;
class CFontCollection;


/////////////////////////////////////////////////////////////////////////////
// CfgTabPane dialog

class CfgTabPane : public CAcUiTabChildDialog, public CArxDialogObject
{

	enum { IDD = IDD_CFGTAB };

	// Construction
public:
	CfgTabPane(TDclFormPtr pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL) ;
	virtual ~CfgTabPane();

// CDialogObject overrides
public:
	virtual DclFormType GetType() const { return VdclConfigTab; }
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const { return true; }
	//virtual bool IsDirty() const;
	//virtual bool SetDirty( bool bDirty = true );
	virtual void CloseDialog(int nStatus);
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
	virtual bool OnApplyResizable( TPropertyPtr pProp ) { return true; }

// CAcUiTabChildDialog overrides
	virtual void OnMainDialogAPPLY();
  virtual void OnMainDialogCancel();
  virtual void OnMainDialogOK();
  virtual BOOL OnMainDialogHelp();    
	virtual void OnTabActivation (BOOL bActivate) ;
	virtual BOOL OnTabChanging () ;

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	virtual void PostNcDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
