// ModalDlg.h : header file
//

#pragma once

#include "BaseDlg.h"
#include "Resource.h"

class CDclFormObject;


class CModalDialogX : public CArxDialogObject
{
	friend class CModalDlg;
	CModalDlg* mpOwner;
protected:
	CModalDialogX( CModalDlg& Owner, CDclFormObject* pDclForm );
	~CModalDialogX();

	virtual DclFormType GetType() const;
	virtual bool IsModeless() const { return false; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const;
	virtual HWND GetHWnd() const;
	virtual void CloseDialog(int nStatus);
	virtual INT_PTR DoModal();
	//virtual bool Show(bool bShow = true) { return false; }
	virtual bool SetMinMaxSize( const CSize& min, const CSize& max );
};


/////////////////////////////////////////////////////////////////////////////
// CModalDlg dialog

class CModalDlg : public CBaseDlg
{
	CModalDialogX mDialogX;
	bool mbResizable;

	enum { IDD = IDD_MODALDIALOG };

// Construction
public:
	CModalDlg(CDclFormObject* pSourceForm, CWnd* pParent = NULL, DialogParams* pParams = NULL);
	~CModalDlg();

public:
	virtual CControlPane& GetControlPane() { return mDialogX.GetControlPane(); }
	virtual const CDialogObject& GetDialogObject() const { return mDialogX; }
	virtual CDialogObject& GetDialogObject() { return mDialogX; }
	void SetDclForm(CDclFormObject *pDclFormObject);
	bool IsResizable() const { return mbResizable; }

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
