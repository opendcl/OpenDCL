// ArxControlPane.h : header file
//

#pragma once

#include "ControlPane.h"

class CDwgDirList;
class CComboBoxFolder;


/////////////////////////////////////////////////////////////////////////////
// CArxControlPane window

class CArxControlPane : public CControlPane
{
// Attributes

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
	CDclFormObject* FindDclObject(CString sControlName) const;

// Implementation
public:
	virtual void CleanUpControls();
	virtual bool FindControl(HWND hwndControl, /*out*/ CString& sControlName) const; //if found, returns true & sets sControlName
	virtual TDialogControlPtr FindControl(HWND hwndControl) const;
	virtual TDialogControlPtr FindControl( LPCTSTR pszControlName, ControlType type = CtlInvalid ) const;
	virtual void SetGlobalLispSymbols( bool bResetToNil = false );

protected:
	virtual TDialogControlPtr CreateNewDialogControl( CDclControlObject* pTemplate, UINT nID );
};
