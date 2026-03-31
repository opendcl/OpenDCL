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
	BOOL CanCloseFrame(CFrameWnd* pFrame) override;
	void SetModifiedFlag(BOOL bModified = TRUE) override;
	BOOL SaveModified() override; // return TRUE if ok to continue
	BOOL OnNewDocument() override;
	void OnCloseDocument() override;
	BOOL OnOpenDocument(LPCTSTR lpszPathName) override;
	BOOL OnSaveDocument(LPCTSTR lpszPathName) override;
	void Serialize(CArchive& ar) override;

// Implementation
public:
	virtual ~COpenDCLDoc();
#ifdef _DEBUG
	void AssertValid() const override;
	void Dump(CDumpContext& dc) const override;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


