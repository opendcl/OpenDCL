// OpenDCLDoc.h : interface of the COpenDCLDoc class
//

#pragma once


class COpenDCLDoc : public COleDocument
{
protected: // create from serialization only
	COpenDCLDoc();
	DECLARE_DYNCREATE(COpenDCLDoc)
public:
	virtual ~COpenDCLDoc();

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
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg BOOL OnNewDocument();
	afx_msg BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg BOOL OnSaveDocument(LPCTSTR lpszPathName);
	afx_msg void OnCloseDocument();

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};
