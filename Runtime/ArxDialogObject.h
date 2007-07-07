#pragma once

#include "DialogObject.h"
#include "ArxControlPane.h"


class CArxDialogObject : public CDialogObject
{
	// Attributes
protected:
	CArxControlPane mControlPane;
	class CDocReactor : public AcApDocManagerReactor
	{
		CArxDialogObject* mpDialogobject;
		CString msDocActivatedEvent;
	public:
		CDocReactor( CArxDialogObject* pDialogobject );
		~CDocReactor();
		void documentActivated(AcApDocument* pActivatedDoc);
	} mDocReactor;

public:
	CArxDialogObject( CDclFormObject* pSourceForm, CWnd* pHostDlg );
	virtual ~CArxDialogObject();

	// Properties
public:
	virtual const CControlPane& GetControlPane() const { return mControlPane; }
	virtual CControlPane& GetControlPane() { return mControlPane; }

// Operations
public:
	static CDialogObject* Create( CDclFormObject* pDclForm, CWnd* pParent = NULL,
																DialogParams* pParams = NULL );
};
