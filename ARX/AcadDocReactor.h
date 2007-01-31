// AcadDocReactor.h : header file
//

#pragma once


/////////////////////////////////////////////////////////////////////////////
// CAcadDocReactor

class CAcadDocReactor  : public AcApDocManagerReactor
{
public:
	CAcadDocReactor(CWnd *pBlockList = NULL, CWnd *pBlockView = NULL);
	~CAcadDocReactor();

	virtual void documentBecameCurrent(AcApDocument* pDoc);
	virtual void documentDestroyed(const char* filename);
	virtual void documentCreated(AcApDocument* pDocCreating);
	virtual void documentActivated(AcApDocument* pActivatedDoc);
	CString m_EventDefun;
	bool m_bRefreshGlobalVariables;
	CWnd *m_pParentBlockList;
	CWnd *m_pParentBlockView;
};
