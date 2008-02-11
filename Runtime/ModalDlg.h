// ModalDlg.h : header file
//

#pragma once

#include "BaseDlg.h"
#include "FormTypes.h"
#include "Resource.h"

class CDclFormObject;


/////////////////////////////////////////////////////////////////////////////
// CModalDlg dialog

class CModalDlg : public CBaseDlg
{

	enum { IDD = IDD_MODALDIALOG };

// Construction
public:
	CModalDlg(TDclFormPtr pSourceForm, CWnd* pParent = NULL, DialogParams* pParams = NULL);
	~CModalDlg();

protected:
	void SetDclForm(TDclFormPtr pDclFormObject);

// CDialogObject overrides
public:
	virtual FormType GetType() const { return FrmModalDlg; }
	virtual bool IsModeless() const { return false; }
	virtual bool IsDockable() const { return false; }
	virtual void CloseDialog(int nStatus);
	virtual INT_PTR DoModal();
	//virtual bool Show(bool bShow = true) { return false; }
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }

// Overrides
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg BOOL OnNotify_ToolTipTitle(UINT  id, NMHDR *pNMHDR, LRESULT *pResult);

protected:
	DECLARE_MESSAGE_MAP()
};
