// ProjectCollection.h : header file
//

#pragma once

class CProject;


class CProjectCollection : public CObject
{
public:
	CProjectCollection();
	void ClearProjects();
	CProject* GetProject(CString ProjectName);
	void AddProject(CProject *pProject);
	void Serialize(CArchive& ar);

// Attributes
public:		
	CList<CProject*> mProjectCollection;	

// Operations
public:

// Implementation
public:
	virtual ~CProjectCollection();

	// Generated message map functions
protected:
	DECLARE_SERIAL(CProjectCollection)
};
