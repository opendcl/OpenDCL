#pragma once

class CDclFormObject;
class CModalVDcl;
class CDockingDialog;
class CResizableDockingDialog;
class CModelessDlg;
class CfgTabPane;
class CParentFileDialog;
class CArxProject;
class CControlPane;


//This looks like a container for pointers to all of the possible types of dialog
//boxes.
class CDialogObject : public CObject
{
public:
	CDialogObject();
	virtual ~CDialogObject();

// Attributes
public:		
	int						nType;
	int						m_nId;
	CString					m_sFileName;
	CString					m_sDialogName;
	CDclFormObject			*m_pDialogObject;
	CModalVDcl				*m_pModalDialog;
	CDockingDialog			*m_pDockingDialog;
	CResizableDockingDialog	*m_pResizableDockingDialog;
	CModelessDlg			*m_pModelessDialog;
	CfgTabPane				*m_pConfigTabPane;
	CParentFileDialog		*m_pFileDialog;
	CArxProject* m_pProject;

// Operations
public:
	HWND GetFormHWnd() const;
	const CControlPane& GetControlPane() const;
	CControlPane& GetControlPane();

// Implementation
protected:
};
