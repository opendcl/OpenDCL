// ListBoxDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ListBoxDlg.h"
#include "Workspace.h"
#include "SharedRes.h"
#include "PropertyObject.h"
#include "OpenDCLView.h"
#include "DclFormObject.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "AxPropertyDescriptor.h"
#include "AxInterfaceDescriptor.h"
#include "AxContainerCtrl.h"
#include "ControlHolder.h"
#include "ChildFrm.h"
#include "PropertyListCtrl.h"

#define nPromptForNewPicture  1


/////////////////////////////////////////////////////////////////////////////
// CListBoxDlg dialog


CListBoxDlg::CListBoxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CListBoxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CListBoxDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_nID = CListBoxDlg::IDD;
	m_pParent = pParent;
	m_ListBoxCreated = FALSE;
	m_pAxContainer = NULL;
}	


void CListBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CListBoxDlg)
	DDX_Control(pDX, IDC_LISTBOX, m_ListBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListBoxDlg, CDialog)
	//{{AFX_MSG_MAP(CListBoxDlg)
	ON_LBN_SELCHANGE(IDC_LISTBOX, OnSelchangeListbox)
	ON_LBN_KILLFOCUS(IDC_LISTBOX, OnKillfocusListbox)
	ON_LBN_SETFOCUS(IDC_LISTBOX, OnSetfocusListbox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxDlg message handlers

void CListBoxDlg::OnSelchangeListbox() 
{
	int nInstructions = 0;
	int nSelected;
	int nType = m_pPropObject->GetType();

	// watch for ActiveX properties to be set
	if (nType == PropActiveXProp)
	{
		switch (m_pPropObject->GetAxInterfaceDescriptorPtr()->GetType())
		{
		case VT_BOOL:
		case VT_UI1:
		case VT_I1:
			nType = PropBool;
			break;
		default:
			nType = PropEnum;
			break;
		case VT_DISPATCH:
		case VT_UNKNOWN:
			nType = PropString;
			break;
		
		}
	}

	TCHAR Value[80];
	_ltot(m_ListBox.GetCurSel(), Value, 10);
	

	switch (nType)
	{
	case PropString:
		{
		CString sName;
		int nSel = m_ListBox.GetCurSel();
		m_ListBox.GetText(nSel, sName);
		m_pPropObject->SetStringValue(sName);
		nInstructions = -1;
		if (m_pAxContainer != NULL)
		{
			TDclControlPtr pCtrl = m_pView->m_pThisDclForm->FindControl(sName);
			if (pCtrl != NULL)
			{
				DISPID dispid;
				// get the DISPID
				if (m_pPropObject->GetAxInterfaceDescriptorPtr()->GetPropPutRef())
					dispid = m_pPropObject->GetAxInterfaceDescriptorPtr()->GetPropPutRef()->GetDispId();
				else if (m_pPropObject->GetAxInterfaceDescriptorPtr()->GetPropPut())
					dispid = m_pPropObject->GetAxInterfaceDescriptorPtr()->GetPropPut()->GetDispId();
				
				CAxContainerCtrl *pAxCont = ((CControlHolder*)pCtrl->m_pCtrlHolder)->GetActiveXCtrl();
				CComPtr< IDispatch > pDisp;
				pAxCont->GetOleDispatch( &pDisp );
				// set the image list now.
				//huh? setting the control dispatch as the image list? that can't be right. [ORW]
				m_pAxContainer->GetActiveXCtrl()->SetImageList( dispid, pDisp );
			}
		}
		break;
		}
	case PropEnum:
		{
		if (m_pAxContainer != NULL)
			m_pAxContainer->GetActiveXCtrl()->SetProperty(
				m_pPropObject->GetAxInterfaceDescriptorPtr()->GetGetDescriptor(),
				m_pPropObject->GetAxInterfaceDescriptorPtr()->GetEnumValue(m_ListBox.GetCurSel()) );
		else
			m_pPropObject->SetStringValue(Value);
		break;
		}
	case PropPicture:
		{
		nSelected = m_ListBox.GetCurSel();
		switch (nSelected)
		{
		case 0:
		{
			m_pPropObject->SetLongValue(0);
			if (m_pControl->GetID() == -1)
			{
				CChildFrame *pChildFrm = (CChildFrame*) m_pView->GetParentFrame();				
				pChildFrm->SetTitleBarIcon(0);
			}
			break;
		}
		case 1:
		{
			nInstructions = nPromptForNewPicture;
			break;
		}
		default:
		{
			CString sPictureID;
			m_ListBox.GetText(nSelected, sPictureID);
			m_pPropObject->SetStringValue(sPictureID);

			if (m_pControl->GetID() == -1)
			{
				CChildFrame *pChildFrm = (CChildFrame*) m_pView->GetParentFrame();				
				pChildFrm->SetTitleBarIcon(_tstol(sPictureID));
			}
			break;		
		}		
		}
		break;
		}
		
	case PropBool:
		{
		CString sValue;
		if (m_ListBox.GetCurSel() == 0)
			sValue = theWorkspace.LoadResourceString(IDS_TRUE);
		else
			sValue = theWorkspace.LoadResourceString(IDS_FALSE);
		
		if (m_pAxContainer != NULL)
			m_pAxContainer->GetActiveXCtrl()->SetProperty(
				m_pPropObject->GetAxInterfaceDescriptorPtr()->GetPutDescriptor(),
				(LPCTSTR)sValue );
		else
		{
			m_pPropObject->SetStringValue(sValue);
			if (m_pPropObject->GetID() == Prop::Resizable)
			{
				if (m_ListBox.GetCurSel() == 0)
					m_pView->InvalidateRect(m_pView->m_rcGripRect, TRUE);
				else
					m_pView->Invalidate();
			}
		}
		break;
		}
	}

	((CPropertyListCtrl*)m_pParent)->CloseListBox(nInstructions);
	
}

BOOL CListBoxDlg::Create()
{
	return CDialog::Create(m_nID, m_pParent);
}

void CListBoxDlg::AddString(CString NewString)
{
	m_ListBox.AddString(NewString);
}
void CListBoxDlg::PostNcDestroy() 
{
	//delete this;
}

void CListBoxDlg::MoveWindow(CRect *pRect) 
{
	// move the list box
	CRect rcListBox(0,0,pRect->Width(),pRect->Height());
	m_ListBox.MoveWindow(rcListBox, TRUE);
	
	// get the list box's new size
	CRect rcNewSize;
	m_ListBox.GetWindowRect(&rcNewSize);

	CDialog::MoveWindow(
		pRect->left,
		pRect->top,
		pRect->Width(),
		rcNewSize.Height(),
		TRUE);

}



void CListBoxDlg::OnKillfocusListbox() 
{
	if (IsWindowVisible())
	{
		((CPropertyListCtrl*)m_pParent)->CloseListBox(-1);		
	}
}

void CListBoxDlg::SetPropertyPointer(TPropertyPtr pPropObject)
{
	m_pPropObject = pPropObject;	
}


void CListBoxDlg::OnSetfocusListbox() 
{
	
	
}

BOOL CListBoxDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ListBox.SetFocus();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX PropertyObject Pages should return FALSE
}
