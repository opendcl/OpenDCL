// ArxDialogControl.h : header file
//

#pragma once

#include "DialogControl.h"


class CArxDialogControl : public CDialogControl
{
	// Attributes
protected:
	CString	msLispSymbolName; //the lisp symbol name used for this control
	
public:
	CArxDialogControl( CDclControlObject* pControl, CWnd* pWnd );
	virtual ~CArxDialogControl();

	// Properties
	const CString& GetLispSymbolName() const { return msLispSymbolName; }

	// Implementation
public:
	void CreateGlobalVariables() const;
};
