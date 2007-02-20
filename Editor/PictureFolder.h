// PictureFolder.h : header file
//

#pragma once

#include "Resource.h"
#include "PictureBox.h"

class CDclFormObject;
class CPictureObject;
class CProject;


/////////////////////////////////////////////////////////////////////////////
// CPictureFolder dialog

class CPictureFolder : public CDialog
{
	CProject* mpProject;

// Construction
public:
	CPictureFolder(CProject* pProject, CWnd* pParent = NULL);   // standard constructor
	

// Dialog Data
	//{{AFX_DATA(CPictureFolder)
	enum { IDD = IDD_PICTUREFOLDER_DIALOG };
	CListBox	m_PictureList;
	CPictureBox	m_PictureBox;
	//}}AFX_DATA
	
public:
	void UpdateSingleFileDialog();
	void MultiFileDialog();
	CPictureObject *GetSelectedPictureObject();
	void CheckPictureRefs();
	void SearchPictureRefs(CDclFormObject *pDclObject);

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
