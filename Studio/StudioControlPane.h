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
	void ApplyPosition( TDialogControlPtr pDlgControl ) override; //move control window to new position

protected:
	TDialogControlPtr CreateNewDialogControl( TDclControlPtr pTemplate, UINT nID ) override;
	void TabOrderFront( TDialogControlPtr pDlgControl, HDWP hDeferred = NULL ) override;
	void TabOrderBack( TDialogControlPtr pDlgControl, HDWP hDeferred = NULL ) override;
	bool IsInvisibleControlAllowed(  TDialogControlPtr pDlgControl ) const override { return false; }
};
