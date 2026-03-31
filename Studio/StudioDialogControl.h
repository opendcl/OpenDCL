// StudioDialogControl.h : header file
//

#pragma once

#include "DialogControl.h"
#include "ControlPane.h"


class CStudioDialogControl : public CDialogControl
{
	// Attributes
protected:

public:
	CStudioDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControlWnd );
	virtual ~CStudioDialogControl();

public:
	static TDialogControlPtr Create( TDclControlPtr pTemplate, CControlPane* pPane,
																	 UINT nID, ControlParams* pParams = NULL );

	static void UpdateAllProperties( TDclControlPtr pTemplate );
	static void UpdateProperty( TDclControlPtr pTemplate, Prop::Id id );

protected:
	static TDialogControlPtr CreateImp( TDclControlPtr pTemplate, CControlPane* pPane,
																			UINT nID, ControlParams* pParams = NULL );
	static TDialogControlPtr CreateComboExControl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);	
	static TDialogControlPtr CreateEditControl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);
	static TDialogControlPtr CreateComboControl( TDclControlPtr pTemplate, CControlPane* pPane, UINT nId);
};


//This class deletes the control object in its destructor (used for old style and other dynamically 
//created controls)
class CAutoStudioDialogControl : public CStudioDialogControl
{
public:
	CAutoStudioDialogControl( TDclControlPtr pTemplate, CControlPane* pPane, CWnd* pControlWnd )
		: CStudioDialogControl( pTemplate, pPane, pControlWnd ) { UpdateAllProperties( pTemplate ); }
	virtual ~CAutoStudioDialogControl() { delete mpControlWnd; }
	bool Create( CWnd* pParentWnd, UINT nID ) override { return false; }
};
