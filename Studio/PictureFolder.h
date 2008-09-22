// PictureFolder.h : header file
//

#pragma once

#include "Resource.h"
#include "PictureBox.h"
#include "StudioProject.h"
#include "PtrTypes.h"
#include <set>
#include <list>

class CDclFormObject;


/////////////////////////////////////////////////////////////////////////////
// CPictureFolder dialog

class CPictureFolder : public CDialog
{
	TStudioProjectPtr mpProject;
	std::set< int > msetIdsToDelete;
	std::list< TPicturePtr > mlistPicsToAdd;

// Construction
public:
	CPictureFolder(TStudioProjectPtr pProject, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPictureFolder();

// Dialog Data
	//{{AFX_DATA(CPictureFolder)
	enum { IDD = IDD_PICTUREFOLDER };
	CListBox	m_PictureList;
	CPictureBox	m_PictureBox;
	//}}AFX_DATA
	
public:
	void UpdateSingleFileDialog();
	void MultiFileDialog();
	TPicturePtr GetSelectedPictureObject();
	void CheckPictureRefs();
	void SearchPictureRefs(TDclFormPtr pDclObject);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPictureFolder)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPictureFolder)
	afx_msg void OnAdd();
	afx_msg void OnDelete();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePicturelist();
	afx_msg void OnUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
