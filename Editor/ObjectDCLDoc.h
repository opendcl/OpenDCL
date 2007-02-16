// ObjectDCLDoc.h : interface of the CObjectDCLDoc class
//

#pragma once


class CObjectDCLDoc : public COleDocument
{
protected: // create from serialization only
	CObjectDCLDoc();
	DECLARE_DYNCREATE(CObjectDCLDoc)
public:
	virtual ~CObjectDCLDoc();

// Attributes
public:

// Operations
public:
	void SetGlobalVariableNames( LPCTSTR pszRootName );

	// Overrides
public:
	virtual void Serialize(CArchive& ar);

// Generated message map functions
protected:
	afx_msg void OnFileClose();
	afx_msg BOOL OnNewDocument();
	afx_msg BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnCloseDocument();
	DECLARE_MESSAGE_MAP()

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};
