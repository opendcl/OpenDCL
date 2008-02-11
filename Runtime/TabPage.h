// TabPage.h : header file
//

#pragma once

#include "Resource.h"
#include "DialogObject.h"
#include "ArxControlPane.h"

class CDclFormObject;

typedef RefCountedPtr< class CTabPage > TTabPagePtr;


/////////////////////////////////////////////////////////////////////////////
// CTabPage dialog

class CTabPage : public CDialog, public CDialogObject
{
protected:
	CArxControlPane mControlPane;

	enum { IDD = IDD_TABPAGE };

// Construction
public:
	CTabPage( TDclFormPtr pSourceForm, CTabCtrl* pTabCtrl, CRect rectPane, UINT& nId );
	~CTabPage();

	//Attributes
	const CControlPane* GetControlPane() const { return &mControlPane; }
	CControlPane* GetControlPane() { return &mControlPane; }

// CDialogObject overrides
public:
	virtual FormType GetType() const { return FrmTabPage; }
	virtual bool IsModeless() const { return true; }
	virtual bool IsDockable() const { return false; }
	virtual bool IsResizable() const { return false; }
	virtual void CloseDialog(int nStatus) {}
	virtual bool Create( CWnd* pParentWnd, UINT nID ) { return false; }

protected:
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
