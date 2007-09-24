#pragma once

#include "DialogObject.h"
#include "ArxControlPane.h"


class CArxDialogObject : public CDialogObject
{
	CArxControlPane mControlPane;
	bool mbEnteringNoDocState;
	class CDocReactor : public AcApDocManagerReactor
	{
		CArxDialogObject* mpDialogobject;
		CString msDocActivatedEvent;
		CString msEnteringNoDocStateEvent;
	public:
		CDocReactor( CArxDialogObject* pDialogobject );
		~CDocReactor();
		virtual void documentActivated(AcApDocument* pActivatedDoc);
		virtual void documentToBeDestroyed(AcApDocument* pDocToDestroy);
	} mDocReactor;

public:
	CArxDialogObject( TDclFormPtr pSourceForm, CWnd* pHostDlg );
	virtual ~CArxDialogObject();

public:
	virtual const CControlPane& GetControlPane() const { return mControlPane; }
	virtual CControlPane& GetControlPane() { return mControlPane; }

public:
	bool IsEnteringNoDocState() const { return mbEnteringNoDocState; }

protected:
	friend CDocReactor;
	void OnEnteringNoDocState();

public:
	static CDialogObject* Create( TDclFormPtr pDclForm, CWnd* pParent = NULL,
																DialogParams* pParams = NULL );
};
