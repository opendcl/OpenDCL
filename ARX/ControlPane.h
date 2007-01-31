// ControlPane.h : header file
//

#pragma once

#include "ThemeHelperST.h"

class CProject;
class CDwgDirList;
class CComboBoxFolder;
class CDclControlObject;
class CDclFormObject;
class CFontCollection;
class CArxDialogControl;


/////////////////////////////////////////////////////////////////////////////
// CControlPane window

class CControlPane  : public CObject
{
// Attributes
protected:
	CProject* mpProject;
	CComboBox* mpActivePaperSizesCombo; // //hokey kludge so the printer combo can update the paper size combo [ORW]
	CDclFormObject	*mpSourceForm;
	
public:
	CThemeHelperST		m_ThemeHelper;
	CList<CArxDialogControl*>		*m_pControlCol;
	CRect m_PanePos;
	bool m_bInvokeWithSendString;
	//CList< CDclControlObject* > *m_ArxControlList;
	CString			m_sProjectName;
	CString			m_sDialogName;	
	CWnd			*m_pParentDlg;
	CFontCollection	*m_pFontCollection;
	CWnd			*m_pParentFileDialog;


// Construction
protected:
	CControlPane();
public:
	CControlPane(CDclFormObject* pSourceForm, CProject* pProject = NULL);
	virtual ~CControlPane();

// Operations
public:
	CProject* GetProject() const { return mpProject; }
	CDclFormObject* GetSourceForm() const { return mpSourceForm; }
	void SetFirstControlFocus() const;


	void SetDwgListComboFolderLink(CComboBoxFolder *pComboFolder);
	void SetDwgListComboFolderLink(CDwgDirList *pDwgList);
	CRect GetSplitterRect(int nId);
	void UpdateGlobalVariables();
	void UpdateText(CDclControlObject *pControl, CWnd *pWnd, CString sText);
	void UpdateFont(CDclControlObject *pControl, CWnd *pWnd, CFont *pFont);
	bool IsSelfPopulatedList(CDclControlObject *pControl);
	void SetGrphcBtnsParents(CDclControlObject *pGrphcBtn, CDclControlObject *pOtherBtn, bool bForceRefresh);
	void SetGrphcBtnsParents(bool bForceRefresh);
	void ShowPictureBoxes(BOOL bShow);
	void ChangeToolTipText(CDclControlObject *pControl, CWnd *pWnd);
	void InvalidateControls();
	void DeleteEditControl(CDclControlObject* pControl, CWnd *pWnd);
	void DeleteComboControl(CDclControlObject* pControl, CWnd *pWnd);	
	void CleanUpControls();
	void ResetControlsPos(CDclControlObject *pControl, int cx, int cy, bool bRefreshOthers = false);
	void ResetControlsPos(CDclControlObject *pControl, bool bRefreshOthers = false);
	void ResetControlsPos2(CDclControlObject *pControl, int cx, int cy);
	void ResetControlsPos2(CDclControlObject *pControl);
	CWnd *FindControl(CString sControlName, int nControlType) const;
	bool FindControl(HWND hwndControl, /*out*/ CString& sControlName) const; //if found, returns true & sets sControlName
	CDclControlObject *FindArxObject(CString sControlName) const;
	CDclFormObject *FindDclObject(CString sControlName) const;
	void SizeChanged(int cx, int cy, bool bRefreshOthers = false);
	void ZOrderFront(CWnd *pControl);
	void UpdateProperty(CDclControlObject *pControl, UINT nControlId, int nID);
	void UpdateProperty(CWnd *pWnd, CDclControlObject *pControl, int nID);
	void UpdateChildControl(CDclControlObject *pControl, UINT nControlId);
	void UpdateChildControl(CWnd *pWnd, CDclControlObject *pControl, UINT nControlId);
	void ResetImageList(CDclControlObject *pControl, CWnd *pWnd, int nID);
	UINT CreateEachControl(CDclControlObject* pControl, int nId);
	UINT CreateControls(const CDclFormObject *pDclForm, UINT nId);
	//CFont *GetFont();
	void CreateComboControl(CDclControlObject* pControl, CWnd *pParentWnd, int nId);
	void CreateComboExControl(CDclControlObject* pControl, CWnd *pParentWnd, int nId);	
	void CreateEditControl(CDclControlObject* pControl, CWnd *pParentWnd, int nId);
	void CreateGraphicButton(CDclControlObject* pControl, CWnd *pParentWnd, int nId);
	UINT CreateTabPanes(const CDclFormObject *pDclForm, CWnd *pControl, UINT nId, CRect rcTab);
	void ShowWindow(BOOL bShow);
	void ZOrderBack(CWnd *pControl);
	void Refresh() { SizeChanged( m_PanePos.Width(), m_PanePos.Height() ); }
// Implementation
public:
};
