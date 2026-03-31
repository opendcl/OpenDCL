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
		void documentActivated(AcApDocument* pActivatedDoc) override;
		void documentToBeDestroyed(AcApDocument* pDocToDestroy) override;
	} mDocReactor;

public:
	CArxDialogObject( TDclFormPtr pSourceForm, CWnd* pHostDlg );
	virtual ~CArxDialogObject();

public:
	const CControlPane* GetControlPane() const override { return &mControlPane; }
	CControlPane* GetControlPane() override { return &mControlPane; }
	const CArxControlServices* GetArxServices() const override { return &mArxServices; }

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
