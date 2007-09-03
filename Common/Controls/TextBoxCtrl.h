// TextBoxCtrl.h : header file
//

#pragma once

#include "EditEx.h"
#include "DialogControl.h"

class CDclControlObject;
class CProject;


/////////////////////////////////////////////////////////////////////////////
// CTextBoxCtrl window

class CTextBoxCtrl : public CEditEx, public CDialogControl
{

public:
	CTextBoxCtrl( CDclControlObject* pTemplate, CControlPane* pPane, UINT nID, bool bCreate = true );
	virtual ~CTextBoxCtrl();

// DialogControl Interface
public:
	operator TDialogControlPtr () { return TDialogControlLockedPtr( *this ); } //to ensure it doesn't get auto deleted
	virtual bool Create( CWnd* pParentWnd, UINT nID );
	virtual DWORD GetWndStyle() const;
	virtual bool OnApplyProperty( TPropertyPtr pProp );

public:

protected:
	DECLARE_MESSAGE_MAP();

protected:
	afx_msg void PostNcDestroy();
};
