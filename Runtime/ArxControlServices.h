// ArxControlServices.h : header file
//
// The purpose of this class is to expose ARX specific services from a CDialogControl object. Usage 
// should consist of including a member of this class in the ARX control class, then exposing it by 
// overriding CDialogControl::GetArxServices() and returning an instance of this class.

#pragma once

#include "PtrTypes.h"
#include "EventArgs.h"

class CDialogControl;


class CArxControlServices
{
	CDialogControl* mpDlgControl;
	CString	msLispSymbolName; //the lisp symbol name being used for this control
	
public:
	CArxControlServices( CDialogControl* pDlgControl );
	virtual ~CArxControlServices();

	// Properties
public:
	const CString& GetLispSymbolName() const { return msLispSymbolName; }
	virtual void SetLispSymbol( bool bResetToNil = false ) const;
	virtual bool HandleDropOnControl( CWnd* pWnd, COleDataObject* pDataObject, 
																		DROPEFFECT dropEffect, CPoint point ) const;
	bool HandleEvent( LPCTSTR pszHandlerName, resbuf*& prbResult, const resbuf* prbArgs ) const;
	bool HandleEvent( LPCTSTR pszHandlerName, resbuf*& prbResult, const arg_b& args = args_null() ) const;
	bool HandleEvent( LPCTSTR pszHandlerName, bool bAsync, const arg_b& args = args_null(), AcApDocument* pDoc = NULL ) const;
	bool HandleEvent( LPCTSTR pszHandlerName, const arg_b& args = args_null() ) const;
	bool HandleEvent( Prop::Id id, resbuf*& prbResult, const arg_b& args = args_null() ) const;
	bool HandleEvent( Prop::Id id, bool bAsync, const arg_b& args = args_null() ) const;
	bool HandleEvent( Prop::Id id, const arg_b& args = args_null() ) const;
};
