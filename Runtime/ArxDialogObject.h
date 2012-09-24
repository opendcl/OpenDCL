#pragma once

#include "DialogObject.h"
#include "ArxControlPane.h"
#include "ArxControlServices.h"


class CArxDialogObject : public CDialogObject
{
	CArxControlPane mControlPane;
	CArxControlServices	mArxServices;
	bool mbEnteringNoDocState;
	class CDocReactor : public AcApDocManagerReactor
	{
		CArxDialogObject* mpDialogobject;
		CString msDocActivatedEvent;
		CString msEnteringNoDocStateEvent;
		AcApDocument* mpLastActivatedDoc;
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
	virtual const CControlPane* GetControlPane() const { return &mControlPane; }
	virtual CControlPane* GetControlPane() { return &mControlPane; }
	virtual const CArxControlServices* GetArxServices() const { return &mArxServices; }

public:
	bool IsEnteringNoDocState() const { return mbEnteringNoDocState; }
	bool IsCloseAllowed( bool bCancelling ) const;

protected:
	friend CDocReactor;
	void OnEnteringNoDocState();

public:
	static CDialogObject* Create( TDclFormPtr pDclForm, CWnd* pParent = NULL,
																DialogParams* pParams = NULL );
};
