// OBJECTBROWSER.h : header file
//

#pragma once

#include "ResizableDialog.h"
#include "AutoRichEditCtrl.h"
#include "OleControlObject.h"
#include "Resource.h"
#include <vector>

class CDclFormObject;
class CPropertyObject;
class AxMethodDescriptor;
class AxPropertyDescriptor;

#define nDeLeft		209
#define nDeTop		27
#define nDeRight	72
#define nDeBottom	40
#define nDeButtonHeigth	50

#define nDeIconSize 16


/////////////////////////////////////////////////////////////////////////////
// CObjectBrowser dialog

class CObjectBrowser : public CResizableDialog
{
// Construction
public:
	CObjectBrowser(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	enum { IDD = IDD_OBJECTBROWSER };
	CButton	m_Copy3;
	CStatic	m_MefDef;
	CButton	m_OK;
	CButton	m_Copy2;
	CButton	m_Copy1;
	CAutoRichEditCtrl	m_RichBox;
	CTreeCtrl			m_ListBox;

	CImageList			m_ImageList;
	RefCountedPtr< COleControlObject > m_pControl;
	CDclFormObject		*m_pDclForm;
	CString				m_sDclFormName;
	CString				m_sClipBoardDefun1;
	CString				m_sClipBoardDefun2;
	CString				m_sClipBoardDefun3;
	std::vector< RefCountedPtr< COleControlObject > > m_OleObjectList;
	
// Overrides
	// ClassWizard generated virtual function overrides
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

public:
	void Setup();
	void ResizeControls(int cx=-1, int cy=-1);
	HTREEITEM LoadOleObjectIntoTree(RefCountedPtr< COleControlObject > pControl);
	void SearchMethods(RefCountedPtr< CPropertyObject > pProp);
	void LoadAllAssociatedOleObjects(CDclControlObject *pControl);
	void LoadInfoTree(RefCountedPtr< COleControlObject > pControl, HTREEITEM hParentItem, int nIndex);
	void SelectionChanged(HTREEITEM hItem);
	void LoadMethods(CString sFileName, HTREEITEM hParentItem);
	bool LoadFullMethod(CString sFileName, CString sMethodName, CString &sTitle, CString &sDesc, CString &sDefun1);
	CString StripMethodNameOfBrackets(CString sMethodName);
	CString GetTypeName( VARTYPE vt, AxMethodDescriptor* pMethod = NULL, AxPropertyDescriptor* pProperty = NULL );

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CObjectBrowser)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedListbox(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCopy2();
	afx_msg void OnCopy1();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnCopy3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
