// ArxControlServices.h : header file
//
// The purpose of this class is to expose ARX specific services from a CDialogControl object. Usage 
// should consist of including a member of this class in the ARX control class, then exposing it by 
// overriding CDialogControl::GetArxServices() and returning an instance of this class.

#pragma once

#include "PtrTypes.h"


class CArxControlServices
{
	TDclControlPtr mpTemplate;
	CString	msLispSymbolName; //the lisp symbol name being used for this control
	
public:
	CArxControlServices( TDclControlPtr pTemplate );
	virtual ~CArxControlServices();

	// Properties
public:
	const CString& GetLispSymbolName() const { return msLispSymbolName; }
	virtual void SetLispSymbol( bool bResetToNil = false ) const;
};
