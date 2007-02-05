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
	CArxDialogControl( CDclControlObject* pTemplate, RefCountedPtr< CWnd > pControl );
	virtual ~CArxDialogControl();

	// Properties
public:
	const CString& GetLispSymbolName() const { return msLispSymbolName; }

	// Implementation
public:
	virtual void CreateGlobalVariables() const;
};
