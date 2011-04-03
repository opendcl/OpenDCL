// OpenDCLDoc.h : interface of the COpenDCLDoc class
//


#pragma once

#include "StudioProject.h"


class COpenDCLDoc : public CDocument
{
	TStudioProjectPtr mpProject;

protected: // create from serialization only
	COpenDCLDoc();
	DECLARE_DYNCREATE(COpenDCLDoc)

// Attributes
public:
	TStudioProjectPtr GetProject() const { return mpProject; }

// Operations
public:

// Overrides
public:
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	virtual void SetModifiedFlag(BOOL bModified = TRUE);
	virtual BOOL SaveModified(); // return TRUE if ok to continue
	virtual BOOL OnNewDocument();
	virtual void OnCloseDocument();
	afx_msg BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);

// Implementation
public:
	virtual ~COpenDCLDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


