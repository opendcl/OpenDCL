// OptionsTabPane.h : header file
//

#pragma once

#include "ArxDialogObject.h"
#include "Resource.h"


/////////////////////////////////////////////////////////////////////////////
// COptionsTabPane dialog

class COptionsTabPane : public CAcUiTabChildDialog, public CArxDialogObject
{

	enum { IDD = IDD_CFGTAB };

	// Construction
public:
	COptionsTabPane(TDclFormPtr pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL) ;
	virtual ~COptionsTabPane();

// CDialogObject overrides
public:
	virtual FormType GetType() const { return FrmOptionsTab; }
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const { return true; }
	virtual bool IsDirty() const { return (__super::IsDirty() != FALSE); }
	virtual bool SetDirty( bool bDirty = true ) { __super::SetDirty( bDirty? TRUE : FALSE ); return true; }
	virtual void CloseDialog(int nStatus);
protected:
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }
	virtual bool OnApplyResizable( TPropertyPtr pProp ) { return true; }
	virtual bool IsAsyncEvents() const { return false; }

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
