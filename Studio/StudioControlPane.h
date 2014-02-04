// StudioControlPane.h : header file
//

#pragma once

#include "ControlPane.h"
#include "PtrTypes.h"


/////////////////////////////////////////////////////////////////////////////
// CStudioControlPane window

class CStudioControlPane : public CControlPane
{
// Construction
protected:
	CStudioControlPane();
public:
	CStudioControlPane(TDclFormPtr pSourceForm, CWnd* pHostDlg);
	virtual ~CStudioControlPane();

// Operations
public:
	void SetSourceForm( TDclFormPtr pSourceForm );

protected:

// Implementation
public:
	virtual void ApplyPosition( TDialogControlPtr pDlgControl ); //move control window to new position

protected:
	virtual TDialogControlPtr CreateNewDialogControl( TDclControlPtr pTemplate, UINT nID );
	virtual void TabOrderFront( TDialogControlPtr pDlgControl, HDWP hDeferred = NULL );
	virtual void TabOrderBack( TDialogControlPtr pDlgControl, HDWP hDeferred = NULL );
	virtual bool IsInvisibleControlAllowed(  TDialogControlPtr pDlgControl ) const { return false; }
};
