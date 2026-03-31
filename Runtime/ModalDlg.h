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

// Construction
public:
	CModalDlg(TDclFormPtr pSourceForm, CWnd* pParent = NULL, DialogParams* pParams = NULL);
	~CModalDlg();

protected:
	void SetDclForm(TDclFormPtr pDclFormObject);

// CDialogObject overrides
public:
	FormType GetType() const override { return FrmModalDlg; }
	bool IsModeless() const override { return false; }
	bool IsDockable() const override { return false; }
	void CloseDialog(int nStatus) override;
	INT_PTR DoModal() override;
	//bool Show(bool bShow = true) override { return false; }
protected:
	bool Create( CWnd* pParentWnd, UINT nID ) override { return false; }
	bool IsAsyncEvents() const override { return false; }

// Overrides
protected:
	BOOL PreTranslateMessage(MSG* pMsg) override;
	void OnOK() override;
	void OnCancel() override;
	void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()

	BOOL OnInitDialog() override;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};
