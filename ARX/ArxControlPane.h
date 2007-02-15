// ArxControlPane.h : header file
//

#pragma once

#include "ControlPane.h"
#include "ThemeHelperST.h"

class CDwgDirList;
class CComboBoxFolder;


/////////////////////////////////////////////////////////////////////////////
// CArxControlPane window

class CArxControlPane : public CControlPane
{
// Attributes
protected:
	CThemeHelperST mThemeHelper;

// Construction
protected:
	CArxControlPane();
public:
	CArxControlPane(CDclFormObject* pSourceForm, CWnd* pHostDlg);
	virtual ~CArxControlPane();

// Operations
public:
	BOOL PreTranslateOLEMessage(MSG* pMsg) const;


protected:
	CDclFormObject *FindDclObject(CString sControlName) const;

// Implementation
public:
	virtual void CleanUpControls();
	virtual bool FindControl(HWND hwndControl, /*out*/ CString& sControlName) const; //if found, returns true & sets sControlName
	virtual TDialogControlPtr FindControl(HWND hwndControl) const;
	virtual CDclControlObject* FindArxObject(CString sControlName) const;
	virtual CWnd* FindControl(CString sControlName, int nControlType) const;
	virtual class CThemeHelperST* GetThemeHelper() { return &mThemeHelper; }
	virtual bool CreateTabPanes(CDclFormObject* pTemplate, CWnd *pTabControl, UINT& nId, CRect rcTab);
	virtual void SetGlobalLispSymbols( bool bResetToNil = false );

protected:
	virtual TDialogControlPtr CreateNewDialogControl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID );
};
