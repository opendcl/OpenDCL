// DialogObject.cpp : implementation file
//

#include "stdafx.h"
#include "DialogObject.h"
#include "DclFormObject.h"
#include "ModalVDcl.h"
#include "ModelessDlg.h"
#include "CfgTabPane.h"
#include "DockingDialog.h"
#include "ResizableDockingDialog.h"
#include "ParentFileDialog.h"


/////////////////////////////////////////////////////////////////////////////
// CDialogObject

CDialogObject::CDialogObject()
{
	m_pDockingDialog = NULL;
	m_pResizableDockingDialog = NULL;
	m_pModalDialog = NULL;
	m_pModelessDialog = NULL;
	m_pConfigTabPane = NULL;
	m_pFileDialog = NULL;
	m_nId = 0;
	m_pProject = NULL;
}


CDialogObject::~CDialogObject()
{
}


HWND CDialogObject::GetFormHWnd() const
{
	switch (nType)
	{
	case VdclModal:
		{
			return m_pModalDialog->m_hWnd;
			break;
		}
	case VdclModeless:
		{
			return m_pModelessDialog->m_hWnd;
			break;
		}
	case VdclConfigTab:
		{
			return m_pConfigTabPane->m_hWnd;
			break;
		}
	case VdclDockable:
		{
			if (m_pDockingDialog)
				return m_pDockingDialog->m_hWnd;
			return m_pResizableDockingDialog->m_hWnd;
			break;
		}
	case VdclFileDialog:
		{
			return m_pFileDialog->m_hWnd;
			break;
		}
	}
	return NULL;
}


const CControlPane& CDialogObject::GetControlPane() const
{
	switch (nType)
	{
	case VdclModal:
		{
			return m_pModalDialog->GetControlPane();
			break;
		}
	case VdclModeless:
		{
			return m_pModelessDialog->GetControlPane();
			break;
		}
	case VdclConfigTab:
		{
			return m_pConfigTabPane->GetControlPane();
			break;
		}
	case VdclDockable:
		{
			if (m_pDockingDialog)
				return m_pDockingDialog->GetControlPane();
			return m_pResizableDockingDialog->GetControlPane();
			break;
		}
	}
	static CControlPane emptyPane(NULL);
	return emptyPane;
}


CControlPane& CDialogObject::GetControlPane()
{
	switch (nType)
	{
	case VdclModal:
		{
			return m_pModalDialog->GetControlPane();
			break;
		}
	case VdclModeless:
		{
			return m_pModelessDialog->GetControlPane();
			break;
		}
	case VdclConfigTab:
		{
			return m_pConfigTabPane->GetControlPane();
			break;
		}
	case VdclDockable:
		{
			if (m_pDockingDialog)
				return m_pDockingDialog->GetControlPane();
			return m_pResizableDockingDialog->GetControlPane();
			break;
		}
	}
	static CControlPane emptyPane(NULL);
	return emptyPane;
}
