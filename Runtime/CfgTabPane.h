// CfgTabPane.h : header file
//

#pragma once

#include "Resource.h"
#include "ArxDialogObject.h"

class CDclFormObject;
class CDclControlObject;
class CArxDialogControl;
class CControlobject;
class CFontCollection;


class CConfigTabPaneX : public CArxDialogObject
{
	friend class CfgTabPane;
	CfgTabPane* mpOwner;
protected:
	CConfigTabPaneX( CfgTabPane& Owner, CDclFormObject* pDclForm );
	~CConfigTabPaneX();

	virtual DclFormType GetType() const;
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const { return true; }
	virtual HWND GetHWnd() const;
	virtual bool IsDirty() const;
	virtual bool SetDirty( bool bDirty = true );
	virtual void CloseDialog(int nStatus);
};


/////////////////////////////////////////////////////////////////////////////
// CfgTabPane dialog

class CfgTabPane : public CAcUiTabChildDialog
{
	CConfigTabPaneX mDialogX;

	enum { IDD = IDD_CFGTAB };

	// Construction
public:
	CfgTabPane(CDclFormObject* pSourceForm, CWnd *pParent = NULL, DialogParams* pParams = NULL) ;
	virtual ~CfgTabPane();

public:
	CDialogObject& GetDialogObject() { return mDialogX; }
	const CDialogObject& GetDialogObject() const { return mDialogX; }

	virtual void OnMainDialogAPPLY();
  virtual void OnMainDialogCancel();
  virtual void OnMainDialogOK();
  virtual BOOL OnMainDialogHelp();    
	virtual void OnTabActivation (BOOL bActivate) ;
	virtual BOOL OnTabChanging () ;

	// Overrides
protected:
	virtual void PostNcDestroy();

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnDestroy();
};
