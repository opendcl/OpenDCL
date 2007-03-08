// ControlHolder.cpp : implementation file
//

#include "stdafx.h"
#include "ControlHolder.h"
#include "DclControlObject.h"
#include "ControlTypes.h"
#include "AxContainerCtrl.h"
#include "Resource.h"
#include "Workspace.h"
#include "OptionListBox.h"
#include "StaticLink.h"
#include "3DRect.h"
#include "Splitter.h"
#include "PictureBox.h"
#include "RoundSliderCtrl.h"
#include "AcadColorListBox.h"
#include "ColorButton.h"
#include "ColorEdit.h"
#include "VdclGroupBox.h"
#include "GraphicButtonCtrl.h"
#include "VdclStatic.h"
#include "ListCtrlEx.h"
#include "FileDlgCtrls.h"
#include "PictureObject.h"
#include "ObjectDCLView.h"
#include "Project.h"


#define nComboStyle12 12
#define nComboDropHeight 300
#define nDeRoundRangeMin  -179
#define nDeRoundRangeMax   180
#define nDeZeroAngle   90


/////////////////////////////////////////////////////////////////////////////
// CControlHolder

CControlHolder::CControlHolder()
: CControlPane()
, mpTemplate( NULL )
, mpDlgControl( NULL )
{
	m_ControlId = -1;	
	m_bSelected = false;
	m_bActiveXCtrl = false;
	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
		  NULL,
		  ::GetSysColorBrush(COLOR_BTNFACE),
		  NULL);
	
}

CControlHolder::CControlHolder( CDclControlObject* mpTemplate )
: CControlPane( mpTemplate->GetOwnerForm(), this )
, mpTemplate( mpTemplate )
, mpDlgControl( NULL )
{
	m_ControlId = -1;	
	m_bSelected = false;
	m_bActiveXCtrl = false;
	m_ClassName = AfxRegisterWndClass(CS_VREDRAW | CS_HREDRAW,
		  NULL,
		  ::GetSysColorBrush(COLOR_BTNFACE),
		  NULL);
	
}

CControlHolder::~CControlHolder()
{
}

int CControlHolder::GetId()
{
	return ++m_ControlId;
}


BEGIN_MESSAGE_MAP(CControlHolder, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_SHOWWINDOW()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlHolder message handlers

BOOL CControlHolder::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	BOOL bSuccess = CStatic::Create(CString(), dwStyle, rect, pParentWnd, nID);
	if( bSuccess )
		m_ControlId = nID;
	return bSuccess;
}

int CControlHolder::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CStatic::OnCreate(lpCreateStruct) == -1)
		return -1;
	CreateGrips();

	return 0;
}
void CControlHolder::CreateGrips()
{
	int nGridIDCounter = nDeGridIDCounter;
	CRect rcGrip(0,0,nGripSizeConst,nGripSizeConst);
	
	// create all the grip rect and hide them for later selection use
	m_bGripsCreate = m_GripRect1.Create(rcGrip, this, nGridIDCounter);
	m_GripRect1.ShowWindow(FALSE);		
	m_GripRect1.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect2.Create(rcGrip, this, nGridIDCounter);
	m_GripRect2.ShowWindow(FALSE);
	m_GripRect2.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect3.Create(rcGrip, this, nGridIDCounter);
	m_GripRect3.ShowWindow(FALSE);
	m_GripRect3.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect4.Create(rcGrip, this, nGridIDCounter);
	m_GripRect4.ShowWindow(FALSE);
	m_GripRect4.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect5.Create(rcGrip, this, nGridIDCounter);
	m_GripRect5.ShowWindow(FALSE);
	m_GripRect5.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect6.Create(rcGrip, this, nGridIDCounter);
	m_GripRect6.ShowWindow(FALSE);
	m_GripRect6.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect7.Create(rcGrip, this, nGridIDCounter);
	m_GripRect7.ShowWindow(FALSE);
	m_GripRect7.m_bShowAsSelected = true;
	nGridIDCounter++;
	
	m_GripRect8.Create(rcGrip, this, nGridIDCounter);
	m_GripRect8.ShowWindow(FALSE);
	m_GripRect8.m_bShowAsSelected = true;
	

}

void CControlHolder::ReleaseSelection() 
{
	m_bSelected = false;
	// now hide all the grip rect's
	m_GripRect1.ShowWindow(FALSE);
	m_GripRect2.ShowWindow(FALSE);
	m_GripRect3.ShowWindow(FALSE);
	m_GripRect4.ShowWindow(FALSE);
	m_GripRect5.ShowWindow(FALSE);
	m_GripRect6.ShowWindow(FALSE);
	m_GripRect7.ShowWindow(FALSE);
	m_GripRect8.ShowWindow(FALSE);

}
void CControlHolder::ForceGripsForward() 
{
	m_GripRect1.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect2.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect3.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect4.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect5.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect6.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect7.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	m_GripRect8.SetWindowPos(&CWnd::wndTop, 0,0,nNotSet,nNotSet, SWP_NOSIZE|SWP_NOMOVE);
	
}

void CControlHolder::HideGrips() 
{
	m_GripRect1.ShowWindow(FALSE);
	m_GripRect2.ShowWindow(FALSE);
	m_GripRect3.ShowWindow(FALSE);
	m_GripRect4.ShowWindow(FALSE);
	m_GripRect5.ShowWindow(FALSE);
	m_GripRect6.ShowWindow(FALSE);
	m_GripRect7.ShowWindow(FALSE);
	m_GripRect8.ShowWindow(FALSE);
	
}

void CControlHolder::SetSelected(BOOL bSelected) 
{
	if (bSelected == TRUE)
		m_bSelected = true;
	else
		m_bSelected = false;

	CRect rcThis;
	GetWindowRect(&rcThis);


	// set the predetermined pos's
	int MidX = (rcThis.Width() - nGripSizeConst) / 2;
	int MidY = (rcThis.Height() - nGripSizeConst) / 2;
	int RightX = rcThis.Width() - nGripSizeConst;
	int BottomY = rcThis.Height() - nGripSizeConst;

	// set the positions of all the grip rect's
	m_GripRect1.MoveWindow( 
		0, 0, nGripSizeConst, nGripSizeConst, 
		TRUE);
	
	m_GripRect2.MoveWindow( 
		MidX, 0, nGripSizeConst, nGripSizeConst,
		TRUE);

	m_GripRect3.MoveWindow( 
		RightX, 0, nGripSizeConst, nGripSizeConst,
		TRUE);

	m_GripRect4.MoveWindow( 
		0, MidY, nGripSizeConst, nGripSizeConst, 
		TRUE);
		
	m_GripRect5.MoveWindow( 
		RightX, MidY, nGripSizeConst, nGripSizeConst,
		TRUE);

	// set the positions of all the grip rect's
	m_GripRect6.MoveWindow( 
		0, BottomY, nGripSizeConst, nGripSizeConst, 
		TRUE);
	
	m_GripRect7.MoveWindow( 
		MidX, BottomY, nGripSizeConst, nGripSizeConst,
		TRUE);

	m_GripRect8.MoveWindow( 
		RightX, BottomY, nGripSizeConst, nGripSizeConst,
		TRUE);
	
	ForceGripsForward();

	// now show all the grip rect's
	m_GripRect1.ShowWindow(TRUE);
	m_GripRect2.ShowWindow(TRUE);
	m_GripRect3.ShowWindow(TRUE);
	m_GripRect4.ShowWindow(TRUE);
	m_GripRect5.ShowWindow(TRUE);
	m_GripRect6.ShowWindow(TRUE);
	m_GripRect7.ShowWindow(TRUE);
	m_GripRect8.ShowWindow(TRUE);
	
	m_GripRect1.Invalidate();
	m_GripRect2.Invalidate();
	m_GripRect3.Invalidate();
	m_GripRect4.Invalidate();
	m_GripRect5.Invalidate();
	m_GripRect6.Invalidate();
	m_GripRect7.Invalidate();
	m_GripRect8.Invalidate();
	

}

void CControlHolder::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	if (mpTemplate == NULL)
		return;

	if (mpTemplate->GetType() == CtlActiveX)
	{
		CAxContainerCtrl* pContainer = GetActiveXCtrl();
		if (pContainer != NULL)
		{
			pContainer->MoveWindow( 0, 0, cx, cy, TRUE );
			CRect rc;
			pContainer->GetWindowRect( &rc );
			if( rc.Width() != cx || rc.Height() != cy )
			{
				CRect rcThis;
				GetWindowRect( &rcThis );
				GetParent()->ScreenToClient( &rcThis );
				rcThis.right = rcThis.left + rc.Width();
				rcThis.bottom = rcThis.top + rc.Height();
				MoveWindow( rcThis, TRUE );
			}
		}
	}
	else
	{
		CWnd *pControl = GetDlgItem(m_ControlId);
	
		if (pControl != NULL)
		{
			pControl->MoveWindow(
				0,
				0,
				cx,
				cy,
				TRUE);

			CRect rc;
			pControl->GetWindowRect(rc);
			if (rc.Width() != cx || rc.Height() != cy)
			{
				CRect rcThis;
				GetWindowRect(&rcThis);
				GetParent()->ScreenToClient(rcThis);
				rcThis.right = rcThis.left + rc.Width();
				rcThis.bottom = rcThis.top + rc.Height();
				MoveWindow(rcThis, TRUE);
			}
			// force a proper repaint.
			pControl->RedrawWindow(NULL, NULL);
		}
	}
}

void CControlHolder::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (mpTemplate == NULL)
		return;
	if (mpTemplate->GetType() != CtlActiveX)
		return;
	CString sName = mpTemplate->GetActiveXTypeName();
	static CString sText = theWorkspace.LoadResourceString(IDS_ImageListCtrl);
	if (sName != sText)
		return;
	
	CRect rcControl;
	CAxContainerCtrl *pCtrl = GetActiveXCtrl();

	CRect rcClient;
	GetClientRect(&rcClient);
	
	CComPtr< IDispatch > pDispatch;
	if( FAILED(pCtrl->GetOleDispatch( &pDispatch )) )
		return;

	CComPtr< IViewObject2 > pViewObject;
	HRESULT hr = pDispatch->QueryInterface( &pViewObject );
	if (FAILED(hr))
		return; // not supported

	if (pViewObject == NULL)
		return;

	RECTL rc;
	rc.left = rcClient.left;
	rc.right = rcClient.right;
	rc.top = rcClient.top;
	rc.bottom = rcClient.bottom;
	
	pViewObject->Draw( DVASPECT_CONTENT, nNotSet, NULL, NULL, NULL, dc.m_hDC, &rc, NULL, NULL, 0 );
}


void CControlHolder::OnDestroy() 
{
	m_GripRect1.DestroyWindow();
	m_GripRect2.DestroyWindow();
	m_GripRect3.DestroyWindow();
	m_GripRect4.DestroyWindow();
	m_GripRect5.DestroyWindow();
	m_GripRect6.DestroyWindow();
	m_GripRect7.DestroyWindow();
	m_GripRect8.DestroyWindow();
	mpDlgControl = NULL;
	CStatic::OnDestroy();
}

CWnd *CControlHolder::GetChildControl() 
{
	return mpDlgControl? mpDlgControl->GetControl() : NULL;
}



void CControlHolder::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CStatic::OnMouseMove(nFlags, point);
}

void CControlHolder::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CStatic::OnLButtonDblClk(nFlags, point);
}

void CControlHolder::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CStatic::OnLButtonDown(nFlags, point);
}

void CControlHolder::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
//	CStatic::OnLButtonUp(nFlags, point);
}

BOOL CControlHolder::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_KEYUP:
	case WM_MOUSEMOVE:
		{
			pMsg->wParam = NULL;
			pMsg->message = NULL;
			return TRUE;
			break;
		}	
	}
		
	return CStatic::PreTranslateMessage(pMsg);
}


CAxContainerCtrl* CControlHolder::GetActiveXCtrl()
{
	return (CAxContainerCtrl*)GetDlgItem(m_ControlId);
}

void CControlHolder::SetColor(DISPID dispid, unsigned long ulColor)
{
	GetActiveXCtrl()->SetColor(dispid, ulColor);
}
unsigned long CControlHolder::GetColor(DISPID dispid)
{
	return GetActiveXCtrl()->GetColor(dispid);
}

HRESULT CControlHolder::GetProperty(AxPropertyDescriptor *axProp, CString &strReturnValue)
{
	return GetActiveXCtrl()->GetProperty(axProp, strReturnValue);
}

void CControlHolder::SetProperty( AxPropertyDescriptor *axProp, LPCTSTR pszNewValue )
{
	GetActiveXCtrl()->SetProperty( axProp, pszNewValue );
}
	
void CControlHolder::ShowPropertyPages()
{
	GetActiveXCtrl()->ShowPropertyPages();
}

void CControlHolder::LoadPictureFile(DISPID dispid, CString sFile, WORD flag)
{
	GetActiveXCtrl()->LoadPictureFile(dispid, sFile, flag);
}

void CControlHolder::SetPicture(DISPID dispid, LPDISPATCH newValue, WORD flag)
{
	GetActiveXCtrl()->SetPicture(dispid, newValue, flag);
}

BOOL CControlHolder::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar) 
{

	if (dispid == DISPID_AMBIENT_USERMODE)
	{		
		V_VT(pvar) = VT_BOOL;
		V_BOOL(pvar) = 0;
		return TRUE;
	}
	if (dispid == DISPID_AMBIENT_UIDEAD)
	{		
		V_VT(pvar) = VT_BOOL;
		V_BOOL(pvar) = 1;
		return TRUE;
	}	
	return CStatic::OnAmbientProperty(pSite, dispid, pvar);
}

void CControlHolder::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CWnd::OnShowWindow(bShow, nStatus);
	
}


void CControlHolder::SetupTreeControl(CTreeCtrl *pControl)
{
	pControl->DeleteAllItems();

	HTREEITEM   m_rghItem[4];

	// load the sample node text
	CString strNodeText;	
	strNodeText = theWorkspace.LoadResourceString(IDS_SAMPLENODE);


	// create first parrent sample node
	m_rghItem[0] = pControl->InsertItem(
		TVIF_TEXT, 
		strNodeText.GetBuffer(strNodeText.GetLength()),
		0,
		0,
		TVIS_EXPANDEDONCE,
		TVIF_TEXT,
		0,
		NULL,
		0);

	// create first child node
	m_rghItem[1] = pControl->InsertItem(
		TVIF_TEXT, 
		strNodeText.GetBuffer(strNodeText.GetLength()),
		0,
		0,
		TVIS_EXPANDEDONCE ,
		TVIF_TEXT,
		0,
		m_rghItem[0],
		0);

	// create second child node
	m_rghItem[2] = pControl->InsertItem(
		TVIF_TEXT, 
		strNodeText.GetBuffer(strNodeText.GetLength()),
		0,
		0,
		0,
		TVIF_TEXT,
		0,
		m_rghItem[0],
		0);
	
	// create second parrent sample node
	m_rghItem[3] = pControl->InsertItem(
		TVIF_TEXT, 
		strNodeText.GetBuffer(strNodeText.GetLength()),
		0,
		0,
		0,
		TVIF_TEXT,
		0,
		NULL,
		0);

	pControl->Expand(m_rghItem[0], TVE_EXPAND);
}

CSize CControlHolder::GetControlSize(CWnd *pControl, int nControlType)
{
	CSize retSize;
	
	if (pControl != NULL)
	{
		CRect rcControl;
		
		if (nControlType == CtlMonth)
		{
			((CMonthCalCtrl*)pControl)->GetMinReqRect(&rcControl);
			retSize.cx = rcControl.Width();
			retSize.cy = rcControl.Height();	
		}
		else
		{
			pControl->GetWindowRect(&rcControl);
			retSize.cx = rcControl.Width();
			retSize.cy = rcControl.Height();		
		}
	}
	return retSize;
}

CWnd* CControlHolder::CreateComboBox(CDclControlObject *mpTemplate)
{
	CWnd* pNewControl = NULL;
	CRect rcThis;
	GetWindowRect(&rcThis);
	ScreenToClient(rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL 
		| CBS_HASSTRINGS | WS_GROUP | ES_AUTOHSCROLL | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	RefCountedPtr< CPropertyObject > pPropNoIntegralHeight = mpTemplate->GetPropertyObject(nNoIntegralHeight);
	if (!pPropNoIntegralHeight || pPropNoIntegralHeight->GetBooleanValue())
		dwStyle = dwStyle | CBS_NOINTEGRALHEIGHT;

	if (mpTemplate->GetBoolProperty(nSorted) == TRUE)
		dwStyle = dwStyle | CBS_SORT;

	if (mpTemplate->GetLngProperty(nComboBoxStyle) == nComboStyle12)
		mpTemplate->SetLongProperty(nDropDownHeight, nComboDropHeight);
		
	switch (mpTemplate->GetLngProperty(nComboBoxStyle))
	{
		case 0:
		{
			CComboBox *pControl = new CComboBox;
			pControl->Create( dwStyle | CBS_DROPDOWN, rc, this, GetId());
			pControl->SetWindowText(mpTemplate->GetStrProperty(nText));
			pNewControl = pControl;
			break;
		}
		case 1:
		{
			CComboBox *pControl = new CComboBox;
			pControl->Create( dwStyle | CBS_SIMPLE, rc, this, GetId());
			pControl->SetWindowText(mpTemplate->GetStrProperty(nText));
			pNewControl = pControl;
			break;
		}
		case 2:
		{
			CComboBox *pControl = new CComboBox;
			pControl->Create( dwStyle | CBS_DROPDOWNLIST, rc, this, GetId());
			
			CString sText = mpTemplate->GetStrProperty(nText);
			CString sName = mpTemplate->GetStrProperty(nName);
			if (sName != sText)
			{
				pControl->SetWindowText(sName);
				mpTemplate->SetStringProperty(nText, sName);
			}
			pNewControl = pControl;
			break;
		}
		case 9:
		{
			CComboBox *pControl = new CComboBox;
			pControl->Create( dwStyle | CBS_SIMPLE, rc, this, GetId());
			
			CString sDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_0 + 9).Mid(4);
			mpTemplate->SetStringProperty(nText, sDesc);
			pControl->SetWindowText(sDesc);			
			pNewControl = pControl;
			break;
		}
		default:
		{
			CComboBoxEx *pControl = new CComboBoxEx;
			pControl->Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | CBS_DROPDOWN, rc, this, GetId());
			
			int nThisValue = mpTemplate->GetLngProperty(nComboBoxStyle);
			CString sDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_0 + nThisValue);
			if (nThisValue > 9)
				sDesc = sDesc.Mid(5);
			else
				sDesc = sDesc.Mid(4);

			mpTemplate->SetStringProperty(nText, sDesc);
			pControl->SetWindowText(sDesc);
			pNewControl = pControl;
			break;
		}
	}

	CRect rcAfter;
	pNewControl->GetWindowRect(&rcAfter);
	mpTemplate->SetLongProperty(nHeight, rcAfter.Height());
	return pNewControl;
}

CWnd* CControlHolder::CreateTextBox(CDclControlObject *mpTemplate)
{
	CRect rcThis;
	GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|ES_WANTRETURN |WS_CLIPSIBLINGS; 

	if (mpTemplate->GetBoolProperty(nHScrollBar) == TRUE)
		dwStyle = dwStyle | WS_HSCROLL;
	if (mpTemplate->GetBoolProperty(nVScrollBar) == TRUE)
		dwStyle = dwStyle | WS_VSCROLL;	
	if (mpTemplate->GetBoolProperty(nReadOnly) == TRUE)
		dwStyle = dwStyle | ES_READONLY;
	
	// adjust the dwStyle for filter settings
	switch (mpTemplate->GetLngProperty(nFilterStyle))
	{			
	case 2: /* Integer */
		dwStyle = dwStyle | ES_NUMBER|ES_AUTOHSCROLL;
		break;	
	case 5:/*Upper case*/
		dwStyle = dwStyle | ES_UPPERCASE|ES_AUTOHSCROLL;
		break;
	case 6:/*lower case*/
		dwStyle = dwStyle | ES_LOWERCASE|ES_AUTOHSCROLL;
		break;
	case 7:/*password*/
		dwStyle = dwStyle | ES_PASSWORD|ES_AUTOHSCROLL;
		break;
	case 8:/*MultiLine*/			
		dwStyle = dwStyle | ES_MULTILINE;
		break;
	default:
		dwStyle = dwStyle | ES_AUTOHSCROLL;
		break;
	}

	// adjust the justification style				
	switch (mpTemplate->GetLngProperty(nJustification))
	{
	case 0:/*Left*/
		dwStyle = dwStyle | ES_LEFT;
		break;
	case 1:/*Center*/
		dwStyle = dwStyle | ES_CENTER;
		break;
	case 2:/*Right*/
		dwStyle = dwStyle | ES_RIGHT;
		break;
	}

	CColorEdit *pNewControl = new CColorEdit;
	pNewControl->Create( dwStyle, rc, this, GetId());

	if (mpTemplate->GetLngProperty(nFilterStyle) == 7)
		pNewControl->SetPasswordChar(_T('*'));
	else
		pNewControl->SetPasswordChar(_T('\0'));
	
	pNewControl->SetWindowText(mpTemplate->GetStrProperty(nText));

	// fix up 3D styles
	pNewControl->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
	return pNewControl;
}


bool CControlHolder::CreateNewDialogControl()
{
	mpDlgControl = NULL; //this should decrement the previous control's ref count to zero and destroy it
	// create the appropriate control to display
	CWnd* pNewControl = NULL;
	CRect rcThis;
	GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	switch(mpTemplate->GetType())
	{		
	case CtlLabel:
		{
			VdclStatic *pControl = new VdclStatic;
			pControl->Create( mpTemplate->GetStrProperty(nCaption),
												WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPSIBLINGS,
												rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlStdButton:
		{
			CButton *pControl = new CButton;
			pControl->Create( mpTemplate->GetStrProperty(nCaption),
												WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|BS_MULTILINE,
												rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlGraphicButton : return ((mpDlgControl = new CGraphicButtonCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlFrame:
		{
			VdclGroupBox *pControl = new VdclGroupBox;
			pControl->Create( mpTemplate->GetStrProperty(nCaption), rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlTextBox:
		pNewControl = CreateTextBox(mpTemplate);
		break;
	case CtlCheckBox:
		{
			CColorButton *pControl = new CColorButton;
			pControl->Create( mpTemplate->GetStrProperty(nCaption),
												WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | WS_CLIPSIBLINGS,
												rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlOptionButton:
		{
			CColorButton *pControl = new CColorButton;
			pControl->Create( mpTemplate->GetStrProperty(nCaption),
												WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON |WS_CLIPSIBLINGS,
												rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlComboBox:
		pNewControl = CreateComboBox(mpTemplate);
		break;
	case CtlListBox:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | LBS_HASSTRINGS | LBS_NOTIFY |WS_CLIPSIBLINGS;
			if (mpTemplate->GetBoolProperty(nNoIntegralHeight) == TRUE)
				dwStyle = dwStyle | LBS_NOINTEGRALHEIGHT;
			if (mpTemplate->GetBoolProperty(nMultiColumn) == TRUE)
				dwStyle = dwStyle | LBS_MULTICOLUMN;
			if (mpTemplate->GetBoolProperty(nSorted) == TRUE)
				dwStyle = dwStyle | LBS_SORT;		
			if (mpTemplate->GetBoolProperty(nVScrollBar) == TRUE)
				dwStyle = dwStyle | WS_VSCROLL;	
			CAcadColorListBox *pControl = new CAcadColorListBox;
			pControl->Create( dwStyle, rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlScrollBar:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS;
			
			if (rcThis.Width() >= rcThis.Height())
			{
				mpTemplate->SetLongProperty(nOrientation, 0);
				dwStyle = dwStyle | SBS_HORZ;
			}
			else
			{
				mpTemplate->SetLongProperty(nOrientation, 1);
				dwStyle = dwStyle | SBS_VERT;
			}
			CScrollBar *pControl = new CScrollBar;
			pControl->Create( dwStyle, rc, this, GetId());
			pControl->ShowScrollBar(TRUE);
			pNewControl = pControl;
			break;
		}
	case CtlSlider:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | TBS_BOTTOM | WS_CLIPSIBLINGS;
			if (rcThis.Width() >= rcThis.Height())
			{
				mpTemplate->SetLongProperty(nOrientation, 0);
				dwStyle = dwStyle | TBS_HORZ;
			}
			else
			{
				mpTemplate->SetLongProperty(nOrientation, 1);
				dwStyle = dwStyle | TBS_VERT;
			}
			if (mpTemplate->GetBoolProperty(nShowTicks) == TRUE)
				dwStyle = dwStyle | TBS_AUTOTICKS;
			CSliderCtrl *pControl = new CSliderCtrl;
			pControl->Create( dwStyle, rc, this, GetId());
			pControl->SetRange(1, 10, TRUE);
			pControl->SetPos(5);
			pNewControl = pControl;
			break;
		}
	case CtlPictureBox:
		{
			CPictureBox *pControl = new CPictureBox;
			pControl->Create( WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->m_AutoSize = FALSE;
			pNewControl = pControl;
			break;
		}
	case CtlTabStrip:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | TCS_FOCUSNEVER |WS_GROUP |WS_CLIPSIBLINGS;
			if (mpTemplate->GetBoolProperty(nMultiRow))
				dwStyle = dwStyle | TCS_MULTILINE;
			else
				dwStyle = dwStyle | TCS_SINGLELINE;
			CTabCtrl *pControl = new CTabCtrl;
			pControl->Create( dwStyle, rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlMonth:
		{
			CMonthCalCtrl *pControl = new CMonthCalCtrl;
			pControl->Create( WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SizeMinReq(TRUE);
			pNewControl = pControl;
			break;
		}
	case CtlImageComboBox:
		{
			CComboBoxEx *pControl = new CComboBoxEx;
			
			DWORD dwStyle = WS_CHILD|WS_VISIBLE;

			switch (mpTemplate->GetLngProperty(nComboBoxStyle))
			{
				case 0:
				{
					pControl->Create( dwStyle | CBS_DROPDOWN, rc, this, GetId());
					pControl->SetWindowText(mpTemplate->GetStrProperty(nText));
					break;
				}
				case 1:
				{
					pControl->Create( dwStyle | CBS_SIMPLE, rc, this, GetId());
					pControl->SetWindowText(mpTemplate->GetStrProperty(nText));
					break;
				}
				case 2:
				{
					pControl->Create( dwStyle | CBS_DROPDOWNLIST, rc, this, GetId());
					CString sText = mpTemplate->GetStrProperty(nText);
					CString sName = mpTemplate->GetStrProperty(nText);
					if (sName != sText)
					{
						pControl->SetWindowText(sName);
						mpTemplate->SetStringProperty(nText, sName);
					}
					break;
				}	
			}	
			pNewControl = pControl;
			break;
		}
	case CtlAnimate:
		{	
			CPictureBox *pControl = new CPictureBox;
			pControl->Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SetIcon(IDI_ANIMATE);	
			pControl->SetAcadColor(nButtonFace);
			pNewControl = pControl;
			break;
		}
	case Ctl3DRect:
		{
			C3DRect *pControl = new C3DRect;
			pControl->Create( 0, rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlProgress:
		{
			DWORD dwStyle = WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS;
			if (mpTemplate->GetBoolProperty(nSmoothProgress) == TRUE)
				dwStyle = dwStyle | PBS_SMOOTH;
			if (mpTemplate->GetLngProperty(nOrientation) == 1)
				dwStyle = dwStyle | PBS_VERTICAL ;
			CProgressCtrl *pControl = new CProgressCtrl;
			pControl->Create(dwStyle, rc, this, GetId());		
			pNewControl = pControl;
			break;
		}
	case CtlSpinButton:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | UDS_ARROWKEYS |WS_CLIPSIBLINGS ;
			if (rcThis.Width() >= rcThis.Height())
			{
				mpTemplate->SetLongProperty(nOrientation, 0);
				dwStyle = dwStyle | UDS_HORZ;
			}
			else
				mpTemplate->SetLongProperty(nOrientation, 1);
			CSpinButtonCtrl *pControl = new CSpinButtonCtrl;
			pControl->Create( dwStyle, rc, this, GetId() );
			pControl->MoveWindow(rc, TRUE);
			pControl->ShowWindow(TRUE);
			pNewControl = pControl;
			break;
		}
	case CtlStaticURL:
		{
			CStaticLink *pControl = new CStaticLink;
			pControl->m_pControl = mpTemplate;
			pControl->Create( CString(), rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlRoundSlider:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS;
			//if (mpTemplate->GetBoolProperty(nShowTicks) == TRUE)
				//dwStyle = dwStyle;
			CRoundSliderCtrl *pControl = new CRoundSliderCtrl;
			pControl->Create( dwStyle, rc, this, GetId());
			pControl->SetRange(nDeRoundRangeMin, nDeRoundRangeMax, FALSE);
			pControl->SetZero(nDeZeroAngle);
			pControl->SetInverted();
			pNewControl = pControl;
			break;
		}
	case CtlBlockView:
		{
			CPictureBox *pControl = new CPictureBox;	
			pControl->Create(WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SetIcon(IDI_BLOCKVIEW);
			pNewControl = pControl;
			break;
		}
	case CtlSlideView:
		{
			CPictureBox *pControl = new CPictureBox;
			pControl->Create(WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SetIcon(IDI_SLIDEVW);	
			pNewControl = pControl;
			break;
		}
	case CtlTree:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | 
				TVS_DISABLEDRAGDROP | TVS_INFOTIP | WS_TABSTOP  |WS_CLIPSIBLINGS;
			if (mpTemplate->GetBoolProperty(nShowSelectAlways))
				dwStyle = dwStyle | TVS_SHOWSELALWAYS;
			if (mpTemplate->GetBoolProperty(nHasLines))
				dwStyle = dwStyle | TVS_HASLINES;
			if (mpTemplate->GetBoolProperty(nLinesAtRoot))
				dwStyle = dwStyle | TVS_LINESATROOT;
			if (mpTemplate->GetBoolProperty(nHasButtons))
				dwStyle = dwStyle | TVS_HASBUTTONS;
			if (mpTemplate->GetBoolProperty(nEditLabels))
				dwStyle = dwStyle | TVS_EDITLABELS;
			if (mpTemplate->GetBoolProperty(nCheckBoxes))
				dwStyle = dwStyle | TVS_CHECKBOXES;
			CTreeCtrl *pControl = new CTreeCtrl;
			pControl->Create(dwStyle, rc, this, GetId());
			pControl->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);	
			SetupTreeControl(pControl);
			pNewControl = pControl;
			break;
		}
	case CtlHtmlCtrl:
		{
			CPictureBox *pControl = new CPictureBox;
			pControl->Create(WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SetIcon(IDI_HTML);	
			pNewControl = pControl;
			break;
		}
	case CtlDwgPreview:
		{
			CPictureBox *pControl = new CPictureBox;
			pControl->Create(WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SetIcon(IDI_DWGPREVIEW);
			pNewControl = pControl;
			break;
		}
	case CtlGrid:
		{
			CListCtrlEx *pControl = new CListCtrlEx;
			pControl->Create(WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SetIcon(IDI_GRID);
			pNewControl = pControl;
			break;
		}
	case CtlListView:
		{
			CPictureBox *pControl = new CPictureBox;
			pControl->Create(WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SetIcon(IDI_LISTVIEW);	
			pNewControl = pControl;
			break;
		}
	case CtlBlockList:
		{	
			CPictureBox *pControl = new CPictureBox;
			pControl->Create(WS_CHILD|WS_VISIBLE |WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SetIcon(IDI_BLOCKLIST);	
			pNewControl = pControl;
			break;
		}
	case CtlOptionList:
		{	
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | LBS_OWNERDRAWVARIABLE
				| LBS_HASSTRINGS | LBS_NOTIFY |WS_CLIPSIBLINGS;
			if (mpTemplate->GetBoolProperty(nNoIntegralHeight) == TRUE)
				dwStyle = dwStyle | LBS_NOINTEGRALHEIGHT;
			if (mpTemplate->GetBoolProperty(nVScrollBar) == TRUE)
				dwStyle = dwStyle | WS_VSCROLL;	
			COptionListBox *pControl = new COptionListBox;
			pControl->m_RowHeight = (short)mpTemplate->GetLngProperty(nRowHeight);
			pControl->Create(dwStyle, rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlActiveX : 
		return ((mpDlgControl = new CAxContainerCtrl( mpTemplate, this, GetId(), rc, true )) != NULL);
	case CtlDwgList:
		{	
			DWORD dwStyle = WS_CHILD | WS_VISIBLE
				| LBS_HASSTRINGS | LBS_NOTIFY |WS_CLIPSIBLINGS;
			dwStyle |= LBS_NOINTEGRALHEIGHT;
			if (mpTemplate->GetBoolProperty(nMultiColumn) == TRUE)
				dwStyle = dwStyle | LBS_MULTICOLUMN;
			if (mpTemplate->GetBoolProperty(nSorted) == TRUE)
				dwStyle = dwStyle | LBS_SORT;		
			dwStyle = dwStyle | WS_VSCROLL;	
			CAcadColorListBox *pControl = new CAcadColorListBox;
			pControl->Create(dwStyle, rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlSplitter:
		{
			if (rcThis.Width() >= rcThis.Height())
				rc.bottom = rc.top + 6;
			else
				rc.right = rc.left + 6;
			CSplitter *pControl = new CSplitter;
			pControl->m_nStyle = Splitter_DoubleRaised;
			pControl->Create(0, rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlHatch:
		{
			CPictureBox *pControl = new CPictureBox;	
			pControl->Create(WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS, rc, this, GetId());
			pControl->SetIcon(IDI_HATCH);
			pNewControl = pControl;
			break;
		}
	case CtlFileDlgCtrl:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS;
			CFileDlgCtrls *pControl = new CFileDlgCtrls;
			pControl->Create( CString(), dwStyle, rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	}
	
	mpDlgControl = new CAutoDialogControl( mpTemplate, this, pNewControl );
	if( !mpDlgControl )
		return false;
 	UpdateChildControl();
	return true;
}

void CControlHolder::CheckAutoSizeProp()
{
	CWnd *pControl = GetChildControl();
	if (pControl == NULL)
		return;
	
	switch(mpTemplate->GetType())
	{
		/*
	case CtlComboBox:
		{
		if (mpTemplate->GetLngProperty(nComboBoxStyle) == 1)
		{
			CRect rcDropDown;
			CListBox   pListBox;
			//((CComboBox*)pControl)->GetDroppedControlRect(&rcDropDown);
		}
		break;
		}
		*/
	//case CtlGraphicButton:
	//	{
	//	if (mpTemplate->GetBoolProperty(nAutoSize) == TRUE)
	//	{
	//		int nBorderAddition;

	//		switch(mpTemplate->GetLngProperty(nButtonStyle))
	//		{
	//		case 0:
	//			nBorderAddition = 4;
	//			break;
	//		case 1:
	//			nBorderAddition = 0;
	//			break;
	//		default:
	//			return;
	//			break;
	//		}
	//		CPictureObject* pPict = ((CGraphicButtonCtrl*)pControl)->GetPicture();
	//		int nIconWidth = (pPict? pPict->GetWidth() : 16) + nBorderAddition;
	//		int nIconHeight = (pPict? pPict->GetHeight() : 16) + nBorderAddition;
	//		mpTemplate->SetLongProperty( nWidth, nIconWidth );
	//		mpTemplate->SetLongProperty( nHeight, nIconHeight );
	//		CRect rcCtrl;
	//		GetWindowRect(&rcCtrl);
	//		ScreenToClient(rcCtrl);
	//		rcCtrl.right = rcCtrl.left + nIconWidth;
	//		rcCtrl.bottom = rcCtrl.top + nIconHeight;
	//		MoveWindow(rcCtrl, TRUE);
	//		
	//	}
	//	break;
	//	}
	case CtlPictureBox:
		{
		if (mpTemplate->GetBoolProperty(nAutoSize) == TRUE)
		{
			int nBorderAddition;

			switch(mpTemplate->GetLngProperty(nBorderStyle))
			{
			case 0:
				nBorderAddition = 0;
				break;
			case 1:
				nBorderAddition = 4;
				break;
			case 2:
				nBorderAddition = 2;
				break;
			}
			int nOldWidth = mpTemplate->GetLngProperty(nWidth);
			int nOldHeight = mpTemplate->GetLngProperty(nHeight);
			int nWidthDelta = ((CPictureBox*)pControl)->m_cxIcon + nBorderAddition - nOldWidth;
			int nHeightDelta = ((CPictureBox*)pControl)->m_cyIcon + nBorderAddition - nOldHeight;
			if( nWidthDelta != 0 )
				mpTemplate->SetLongProperty(nWidth, nOldWidth + nWidthDelta);
			if( nHeightDelta != 0 )
				mpTemplate->SetLongProperty(nHeight, nOldHeight + nHeightDelta);
			if( nWidthDelta != 0 || nHeightDelta != 0 )
			{
				CRect rectThis;
				GetWindowRect( &rectThis );
				GetParent()->ScreenToClient( &rectThis );
				rectThis.right = rectThis.left + nOldWidth + nWidthDelta;
				rectThis.bottom = rectThis.top + nOldHeight + nHeightDelta;
				CRect rectControl(0, 0, rectThis.Width(), rectThis.Height());
				pControl->MoveWindow( &rectThis, TRUE );
				MoveWindow( &rectThis, TRUE );
			}
		}
		break;
		}
	case CtlActiveX:
		{
			return;
			break;
		}
	default:
		{
			// get the child control
			CSize pCtrlSize = GetControlSize(pControl, mpTemplate->GetType());
	
			if (pCtrlSize.cx != mpTemplate->GetLngProperty(nWidth) ||
				pCtrlSize.cy != mpTemplate->GetLngProperty(nHeight))
			{
				mpTemplate->SetLongProperty(
					nWidth,
					pCtrlSize.cx);
				mpTemplate->SetLongProperty(
					nHeight,
					pCtrlSize.cy
					);
			}
			break;
		}

	}

}


void CControlHolder::UpdateClientHeight() 
{
	if (!mpTemplate || mpTemplate->GetType() != CtlTabStrip)
		return;
	CWnd* pControl = mpDlgControl->GetControl();
	if (!IsWindow(pControl->m_hWnd))
		return;

	size_t nCount = ((CTabCtrl*)pControl)->GetItemCount();
	int nTabHeight = 0;
	while (nCount-- > 0)
	{
		// get the new items' rectangle
		CRect rcTab;
		((CTabCtrl*)pControl)->GetItemRect( nCount, &rcTab);

		// if the height of this tab is greater than the previous max height, make it the new max
		if (rcTab.bottom > nTabHeight)
			nTabHeight = rcTab.bottom;
	}
	
	// get the height of the tab control
	CRect rcTabCtrl;
	((CTabCtrl*)pControl)->GetClientRect(&rcTabCtrl);
	
	// subtract the lowest tab bottom to get the control area height
	mpTemplate->m_ClientHeight = rcTabCtrl.Height() - nTabHeight;
	
	// inform the parent a tab control has been resized
	//FireTabResized(mpTemplate->m_ClientHeight); //no-op
}


void CControlHolder::UpdateProperty(PropertyId nID)
{
	if( !mpDlgControl )
		return;
	RefCountedPtr< CPropertyObject > pProp = mpTemplate->GetPropertyObject(nID);

	//if the control has been redesigned to implement the CDialogControl interface, use that
	switch( mpTemplate->GetType() )
	{
	case CtlActiveX:
	case CtlGraphicButton:
		mpDlgControl->OnApplyProperty( pProp );
		mpDlgControl->GetControl()->ShowWindow( SW_SHOW ); //make it visible even if the 'nVisible' property is false
		return;
	}

	CWnd* pControl = mpDlgControl->GetControl();
	assert( pControl != NULL );
	if( !pControl )
		return;
	// set the appropriate property
	switch(nID)
	{
		case nAutoSize:
			{
			switch (mpTemplate->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControl)->m_AutoSize = mpTemplate->GetBoolProperty(nAutoSize);
				break;
			//case CtlGraphicButton:
			//	((CGraphicButtonCtrl*)pControl)->m_AutoSize = mpTemplate->GetBoolProperty(nAutoSize);
				break;
			}			
			break;
			}
		case nAcadColor:
		{				
			switch (mpTemplate->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControl)->SetAcadColor(mpTemplate->GetLngProperty(nAcadColor));
				break;
			
			case CtlStdButton:
			case CtlOptionButton:
			case CtlCheckBox:
				((CColorButton*)pControl)->SetAcadColor(mpTemplate->GetLngProperty(nAcadColor));
				break;

			case CtlAnimate:
			case CtlListView:
			case CtlGrid:
			case CtlHatch:
			case CtlBlockList:
			case CtlBlockView:
			case CtlDwgPreview:
			case CtlPictureBox:
			case CtlSlideView:		
				((CPictureBox*)pControl)->SetAcadColor(mpTemplate->GetLngProperty(nAcadColor));
				break;

			case CtlTextBox:
				((CColorEdit*)pControl)->SetAcadColor(mpTemplate->GetLngProperty(nAcadColor));
				break;
			
			case CtlListBox:
			case CtlDwgList:
				((CAcadColorListBox*)pControl)->SetAcadColor(mpTemplate->GetLngProperty(nAcadColor));
				break;
			case CtlOptionList:
				((COptionListBox*)pControl)->SetAcadColor(mpTemplate->GetLngProperty(nAcadColor));
				break;		
				
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetAcadColor(mpTemplate->GetLngProperty(nAcadColor));
				break;		

			//case CtlGraphicButton:
			//	((CGraphicButtonCtrl*)pControl)->SetAcadColor(mpTemplate->GetLngProperty(nAcadColor));
			//	break;
			}
			pControl->Invalidate();
			break;
		}	
		case nForeColor:
		{				
			switch (mpTemplate->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControl)->SetForeColor(mpTemplate->GetLngProperty(nForeColor));
				break;		

			case CtlStdButton:
			case CtlOptionButton:
			case CtlCheckBox:
				((CColorButton*)pControl)->SetForeColor(mpTemplate->GetLngProperty(nForeColor));
				break;
			//case CtlGraphicButton:
			//	((CGraphicButtonCtrl*)pControl)->SetForeColor(mpTemplate->GetLngProperty(nForeColor));
			//	break;
			case CtlTextBox:
				//((CColorEdit*)pControl)->SetForeColor(mpTemplate->GetLngProperty(nForeColor));
				switch (mpTemplate->GetLngProperty(nFilterStyle))
				{
				case 8: //EditFilter_Multiline
					((CColorEdit*)pControl)->m_UseBackColor = false;
				case 4: //EditFilter_Symbol
					break;
				default:
					((CColorEdit*)pControl)->SetForeColor(mpTemplate->GetLngProperty(nForeColor));
					break;
				}
				break;			
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetForeColor(mpTemplate->GetLngProperty(nForeColor));
				break;
			case CtlListBox:
			case CtlDwgList:
			case CtlOptionList:
				((CAcadColorListBox*)pControl)->SetForeColor(mpTemplate->GetLngProperty(nForeColor));
				break;
			}			
			pControl->Invalidate();
			break;
		}	
		
		case nBorderStyle:
		{
			switch(mpTemplate->GetLngProperty(nBorderStyle))
			{
			case 0:
				pControl->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				break;
					
			case 1:
				pControl->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
				break;

			case 2:
				pControl->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
				pControl->ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_FRAMECHANGED);
				break;
			}
			switch (mpTemplate->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControl)->m_BorderStyle = mpTemplate->GetLngProperty(nBorderStyle);
				break;
			}
			break;
		}	
		case nButtonStyle:
		{				
			//((CGraphicButton*)pControl)->SetDefaultPicture(mpTemplate->GetLngProperty(nButtonStyle));
			break;
		}	
		case nCaption:
		{				
			switch (mpTemplate->GetType())
			{
				
			case CtlFrame:
				((VdclGroupBox*)pControl)->SetCaption(mpTemplate->GetStrProperty(nCaption));
				break;
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetLinkText(mpTemplate->GetStrProperty(nCaption));
				break;
			//case CtlGraphicButton:
			//	{
			//	((CGraphicButtonCtrl*)pControl)->SetWindowText(mpTemplate->GetStrProperty(nCaption));
			//	((CGraphicButtonCtrl*)pControl)->Invalidate();
			//	break;
			//	}			
			default:
				{
				CString sCaptionText = mpTemplate->GetStrProperty(nCaption);
				pControl->SetWindowText(sCaptionText);
				break;
				}
			}
			break;
		}	
		case nColumnWidth:
		{				
			int nNewColWidth = mpTemplate->GetLngProperty(nColumnWidth);
			if (nNewColWidth > 0)
				((CAcadColorListBox*)pControl)->SetColumnWidth(nNewColWidth);
			break;
		}	
		
		case nDefSelIndex:
		{
			((COptionListBox*)pControl)->ResetContent();					
			CString sListItem;
			RefCountedPtr< CPropertyObject > pPropList = mpTemplate->GetPropertyObject(nBtnCaption);
			int nDefSelection = mpTemplate->GetLngProperty(nDefSelIndex) ;
			for (size_t i = 0; i < pPropList->size(); i++)
			{				
				sListItem = pPropList->GetStringItem(i);
				if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
				{
					((COptionListBox *)pControl)->AddString(sListItem);
				}
				if (nDefSelection == i)
					((COptionListBox *)pControl)->SetItemData(i, 1);
				else
					((COptionListBox *)pControl)->SetItemData(i, 0);
			}
			
			break;
		}
		
		case nDisableNoScroll:
		{
			if (mpTemplate->GetBoolProperty(nDisableNoScroll) == FALSE)
				((CAcadColorListBox*)pControl)->ModifyStyle(LBS_DISABLENOSCROLL, 0, SWP_FRAMECHANGED);
			else
				((CAcadColorListBox*)pControl)->ModifyStyle(0, LBS_DISABLENOSCROLL, SWP_FRAMECHANGED);
			break;
		}
		
		case nEnabled:
		{
			if (mpTemplate->GetType() == CtlSlider)
			{
				((CSliderCtrl *)pControl)->EnableWindow(mpTemplate->GetBoolProperty(nEnabled));
			}
			else if (mpTemplate->GetType() == CtlOptionList)
			{
				int nData=0;
				if (mpTemplate->GetBoolProperty(nEnabled) == FALSE)
					nData = 2;
				for (int i=0; i<((COptionListBox*) pControl)->GetCount(); i++)
				{
					((COptionListBox*) pControl)->SetItemData(i, nData);
				}
				pControl->Invalidate();				
			}			
			else
			{
				pControl->EnableWindow(mpTemplate->GetBoolProperty(nEnabled));
				pControl->Invalidate();
			}
			break;				
		}	
		case nFilterStyle:
			{
			switch (mpTemplate->GetLngProperty(nFilterStyle))
			{
			case 5:/*Upper case*/
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->ModifyStyle(ES_LOWERCASE, ES_UPPERCASE, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			case 6:/*lower case*/
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->ModifyStyle(ES_UPPERCASE, ES_LOWERCASE, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			case 7:/*password*/
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE, ES_PASSWORD, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar('*');
				break;
				}
			case 8:/*MultiLine*/
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE|ES_PASSWORD, ES_MULTILINE, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			default:
				{
				((CColorEdit*)pControl)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE|ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CColorEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			}
			break;
		}

		case nIndent:
		{
			((CTreeCtrl*)pControl)->SetIndent(mpTemplate->GetLngProperty(nIndent));
			break;
		}

		case nImageList:
		{
			ResetImageList(pControl, nID);
			break;
		}

		case nJustification:
		{
			switch (mpTemplate->GetType())
			{
			case CtlTextBox:
				{
					/*
					((CColorEdit*)pControl)->ModifyStyle(ES_RIGHT, 0, SWP_FRAMECHANGED);
					((CColorEdit*)pControl)->ModifyStyle(ES_CENTER, 0, SWP_FRAMECHANGED);
					((CColorEdit*)pControl)->ModifyStyle(ES_LEFT, 0, SWP_FRAMECHANGED);
					
					switch (mpTemplate->GetLngProperty(nJustification))
					{
					case 0:// Left
						((CColorEdit*)pControl)->ModifyStyle(0, ES_LEFT, SWP_FRAMECHANGED);
						break;
					case 1:// Center
						((CColorEdit*)pControl)->ModifyStyle(0, ES_CENTER, SWP_FRAMECHANGED);
						break;
					case 2:// Right
						((CColorEdit*)pControl)->ModifyStyle(0, ES_RIGHT, SWP_FRAMECHANGED);
						break;
					}
					*/
					break;
				}
			case CtlLabel:
				{
					pControl->ModifyStyle(SS_RIGHT, 0, SWP_FRAMECHANGED);
					pControl->ModifyStyle(SS_CENTER, 0, SWP_FRAMECHANGED);
					pControl->ModifyStyle(SS_LEFT, 0, SWP_FRAMECHANGED);
			
					switch (mpTemplate->GetLngProperty(nJustification))
					{
					case 0:/*Left*/
						pControl->ModifyStyle(0, SS_LEFT, SWP_FRAMECHANGED);
						break;
					case 1:/*Center*/
						pControl->ModifyStyle(0, SS_CENTER, SWP_FRAMECHANGED);
						break;
					case 2:/*Right*/
						pControl->ModifyStyle(0, SS_RIGHT, SWP_FRAMECHANGED);
						break;
					}
					break;
				}
			}
			pControl->Invalidate();
			break;
			
		}

		case nLabelName:
		{			
			CFont *pFont = theWorkspace.GetFontCollection().GetFont(mpTemplate, pControl);
			
			if (mpTemplate->GetType() == CtlRoundSlider)
			{
				((CRoundSliderCtrl*)pControl)->m_pFont = pFont;
				pControl->RedrawWindow();
			}
			
			else if (mpTemplate->GetType() == CtlStaticURL)
			{
				((CStaticLink*)pControl)->SetFont(pFont);
				pControl->Invalidate();				
			}
			
			else if (mpTemplate->GetType() == CtlFrame)
			{
				((VdclGroupBox*)pControl)->m_Frame.SetFont(pFont);
				((VdclGroupBox*)pControl)->m_Frame.Invalidate();
			}
			//else if (mpTemplate->GetType() == CtlGraphicButton)
			//{
			//	((CGraphicButtonCtrl*)pControl)->SetFont(pFont);	
			//	pControl->Invalidate();				
			//}
			else if (mpTemplate->GetType() == CtlOptionList)
			{
				((COptionListBox*)pControl)->SetFont(pFont);	
				pControl->Invalidate();				
			}
			else if (mpTemplate->GetType() == CtlGrid)
			{
				((CListCtrlEx*)pControl)->SetFont(pFont);	
				((CListCtrlEx*)pControl)->m_Child.SetFont(pFont);	
				pControl->Invalidate();				
			}
			else
			{
				pControl->SetFont(pFont);	
				pControl->Invalidate();				
			}	
			break;
		}

		case nLimitText:
		{
			((CColorEdit*)pControl)->SetLimitText(mpTemplate->GetLngProperty(nLimitText));
			break;
		}
		
		case nBtnCaption:
		{
			CString sListItem;
			((COptionListBox*)pControl)->ResetContent();					
			int nDefSelection = mpTemplate->GetLngProperty(nDefSelIndex) ;
			for (size_t i = 0; i < pProp->size(); i++)
			{				
				sListItem = pProp->GetStringItem(i);
				if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
				{
					((COptionListBox *)pControl)->AddString(sListItem);
					if (nDefSelection == i)
						((COptionListBox *)pControl)->SetItemData(i, 1);
					else
						((COptionListBox *)pControl)->SetItemData(i, 0);
				}
			}
			
			break;
		}
	
		case nColumnCaptions:
		{
			((CListCtrlEx *)pControl)->SetupColumns(mpTemplate);

			break;
		}
		case nList:
		{
			switch (mpTemplate->GetType())
			{
			case CtlListBox:
				{
					CString sListItem;
					((CAcadColorListBox *)pControl)->ResetContent();					
					for (size_t i = 0; i < pProp->size(); i++)
					{				
						sListItem = pProp->GetStringItem(i);
						if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
							((CAcadColorListBox *)pControl)->AddString(sListItem);
					}
					break;
				}
			case CtlComboBox:
				{
					CString sListItem;
					((CComboBox *)pControl)->ResetContent();
					for (size_t i = 0; i < pProp->size(); i++)
					{			
						sListItem = pProp->GetStringItem(i);
						if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
							((CComboBox *)pControl)->AddString(pProp->GetStringItem(i));
					}
					pControl->SetWindowText(mpTemplate->GetStrProperty(nText));
					break;
				}
			}
			break;				
		}
		case nMarginLeft:
		{
			((CColorEdit*)pControl)->SetMargins(
				mpTemplate->GetLngProperty(nMarginLeft),
				mpTemplate->GetLngProperty(nMarginRight));
			break;
		}
		
		case nMinTabWidth:
		{
			try 
			{
				((CTabCtrl*)pControl)->SetMinTabWidth(mpTemplate->GetLngProperty(nMinTabWidth));
				((CTabCtrl*)pControl)->RedrawWindow(NULL, NULL, RDW_UPDATENOW);
			}
			catch(...)
			{
			}
			break;
		}
		
		case nMaxValue:
		case nMinValue:
		{
			switch (mpTemplate->GetType())
			{
			case CtlProgress:
				((CProgressCtrl*)pControl)->SetRange(
					(short)mpTemplate->GetLngProperty(nMinValue),
					(short)mpTemplate->GetLngProperty(nMaxValue));
				break;				
			case CtlScrollBar:
				((CScrollBar*)pControl)->SetScrollRange(
					mpTemplate->GetLngProperty(nMinValue),
					mpTemplate->GetLngProperty(nMaxValue),
					TRUE);
				break;				
			case CtlSlider:
				((CSliderCtrl*)pControl)->SetRange(
					mpTemplate->GetLngProperty(nMinValue),
					mpTemplate->GetLngProperty(nMaxValue));
				((CSliderCtrl *)pControl)->SetPos(mpTemplate->GetLngProperty(nMinValue));
				((CSliderCtrl *)pControl)->SetPos(mpTemplate->GetLngProperty(nValue));
				break;				
			}
			break;
		}	
		
		case nSplitterStyle:
		{
			((CSplitter *)pControl)->m_nStyle = mpTemplate->GetLngProperty(nSplitterStyle);
			int n = ((CSplitter *)pControl)->m_nStyle;
			if (((CSplitter *)pControl)->m_nStyle <= 0)
			{
				((CSplitter *)pControl)->m_nStyle = 0;
				mpTemplate->SetLongProperty(nSplitterStyle, 0);
			}
			pControl->Invalidate();
			break;
		}
			
		case nOrientation:
		{
			if (mpTemplate->GetType() == CtlSlider)
			{			
				switch(mpTemplate->GetLngProperty(nOrientation))
				{
				case 0:				
					((CSliderCtrl *)pControl)->ModifyStyle(TBS_VERT, 0, SWP_FRAMECHANGED);
					((CSliderCtrl *)pControl)->ModifyStyle(0, TBS_HORZ, SWP_FRAMECHANGED);
					break;
				case 1:
					((CSliderCtrl *)pControl)->ModifyStyle(TBS_HORZ, 0, SWP_FRAMECHANGED);
					((CSliderCtrl *)pControl)->ModifyStyle(0, TBS_VERT, SWP_FRAMECHANGED);
					break;
				}
			}
			break;
		}
		case nPicture:
		{
			switch (mpTemplate->GetType())
			{
			//case CtlGraphicButton:		
			//	((CGraphicButtonCtrl *)pControl)->SetPictureID(mpTemplate->GetLngProperty(nPicture));
			//	break;
			case CtlPictureBox:
				int nID = mpTemplate->GetLngProperty(nPicture);
				((CPictureBox*)pControl)->SetPicture(mpTemplate->GetOwnerProject()->FindPicture(nID));
				break;
			}
			break;
		}

			
		case nShowTicks:
		{
			if (mpTemplate->GetBoolProperty(nShowTicks) == FALSE)
				((CSliderCtrl *)pControl)->ModifyStyle(TBS_AUTOTICKS, 0, SWP_FRAMECHANGED);
			else
				((CSliderCtrl *)pControl)->ModifyStyle(0, TBS_AUTOTICKS, SWP_FRAMECHANGED);
				
			break;
		}

		case nReadOnly:
		{
			((CColorEdit*)pControl)->SetReadOnly(mpTemplate->GetBoolProperty(nReadOnly));
			break;
		}

		case nRowHeight:
		{
			if (mpTemplate->GetType() == CtlOptionList)
			{
				((COptionListBox*)pControl)->m_RowHeight = (short)mpTemplate->GetLngProperty(nRowHeight);
				((COptionListBox*)pControl)->ResetContent();					
				CString sListItem;
				RefCountedPtr< CPropertyObject > pPropList = mpTemplate->GetPropertyObject(nBtnCaption);
				int nDefSelection = mpTemplate->GetLngProperty(nDefSelIndex) ;
				for (size_t i = 0; i < pPropList->size(); i++)
				{				
					sListItem = pPropList->GetStringItem(i);
					if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
					{
						((COptionListBox *)pControl)->AddString(sListItem);
						if (nDefSelection == i)
							((COptionListBox *)pControl)->SetItemData(i, 1);
						else
							((COptionListBox *)pControl)->SetItemData(i, 0);
					}
				}
			}
			
			break;
		}

		case nTabsCaption:
		//case nTabsImageList:
		{
			try
			{
				bool bHasImageList = (mpTemplate->GetImageList() != NULL);

				
				// delete all previos tabs
				((CTabCtrl*)pControl)->DeleteAllItems();
				
				int nCount = mpTemplate->CountPropertyListItems(nTabsCaption);
				int nBottom = 0;
				while (nCount-- > 0)
				{
					TC_ITEM TabCtrlItem;
					CString sTTT;
					TabCtrlItem.mask = TCIF_TEXT;
					CString Tab = mpTemplate->GetPropertyListItem(nTabsCaption, nCount);
					// get the tab caption
					TabCtrlItem.pszText = Tab.GetBuffer(nDeTextLimitCB);		
					
					// set the image list item number if required
					if (bHasImageList)
					{
						TabCtrlItem.mask |= TCIF_IMAGE;
						TabCtrlItem.iImage = _tstol(mpTemplate->GetPropertyListItem(nTabsImageList, nCount));
					}
					
					// add the new tab
					((CTabCtrl*)pControl)->InsertItem( 0, &TabCtrlItem );
				}
				UpdateClientHeight();
			}
			catch(...)
			{
			}
			break;
		}
		case nTabSelected:
		{
			((CAcadColorListBox*)pControl)->SetCurSel(mpTemplate->GetLngProperty(nTabSelected));
			break;
		}
		case nTabStyle:
		{
			if (mpTemplate->GetLngProperty(nTabStyle) == 0)
				((CTabCtrl*)pControl)->ModifyStyle(TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED);
			else
				((CTabCtrl*)pControl)->ModifyStyle(TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED);
			break;
		}
		
		case nTabFixedWidth:
		{
			try
			{
				if (mpTemplate->GetBoolProperty(nTabFixedWidth) == TRUE)
				{
					((CTabCtrl*)pControl)->ModifyStyle(0, TCS_FIXEDWIDTH, SWP_FRAMECHANGED);
					CRect rc;
					((CTabCtrl*)pControl)->GetItemRect(0, &rc);
					CSize szTabs;
					szTabs.cx = mpTemplate->GetLngProperty(nMinTabWidth);
					szTabs.cy = rc.Height();
					((CTabCtrl*)pControl)->SetItemSize(szTabs);
				}
				else
				{	
					((CTabCtrl*)pControl)->ModifyStyle(TCS_FIXEDWIDTH, 0, SWP_FRAMECHANGED);
				}
			}
			catch(...)
			{
			}
			break;
		}
		case nText:
		{
			int nTextLimit = mpTemplate->GetLngProperty(nLimitText);

			CString sNewText = mpTemplate->GetStrProperty(nText);

			if (nTextLimit > nNotSet) 
			{
				sNewText = sNewText.Left(nTextLimit);
				mpTemplate->SetStringProperty(nText, sNewText);
			}

			pControl->SetWindowText(sNewText);
			break;
		}
		case nTickFrequency:
		{
			((CSliderCtrl *)pControl)->SetTicFreq(mpTemplate->GetLngProperty(nTickFrequency));
			
			break;
		}
		
		case nUseTabStops:
		{
			if (mpTemplate->GetBoolProperty(nUseTabStops) == FALSE)
				((CAcadColorListBox*)pControl)->ModifyStyle(LBS_USETABSTOPS, 0, SWP_FRAMECHANGED);
			else
				((CAcadColorListBox*)pControl)->ModifyStyle(0, LBS_USETABSTOPS, SWP_FRAMECHANGED);
			break;
		}
		
		case nValue:
		{
			switch (mpTemplate->GetType())
			{
			case CtlCheckBox:
			case CtlOptionButton:
				((CColorButton *)pControl)->SetCheck(mpTemplate->GetBoolProperty(nValue));
				break;									
			case CtlRoundSlider:				
				//((CRoundSliderCtrl *)pControl)->SetText(mpTemplate->GetLngProperty(nValue));
				((CRoundSliderCtrl *)pControl)->SetPos(mpTemplate->GetLngProperty(nValue));				
				((CRoundSliderCtrl *)pControl)->RedrawWindow();

				break;
			case CtlSlider:
				((CSliderCtrl *)pControl)->SetPos(mpTemplate->GetLngProperty(nValue));
				break;
			case CtlScrollBar:
				((CScrollBar *)pControl)->SetScrollPos(mpTemplate->GetLngProperty(nValue), TRUE);
				break;
			case CtlProgress:
				((CProgressCtrl*)pControl)->SetPos(mpTemplate->GetLngProperty(nValue));
				break;
			
			}
			break;
		}
		case nVScrollBar:
		{
			if (mpTemplate->GetType() != CtlDwgList)
			{
				if (mpTemplate->GetBoolProperty(nVScrollBar) == FALSE)
					((CAcadColorListBox*)pControl)->ModifyStyle(WS_VSCROLL, 0, SWP_FRAMECHANGED);
				else
				{
					((CAcadColorListBox*)pControl)->ModifyStyle(0, WS_VSCROLL, SWP_FRAMECHANGED);
					pControl->Invalidate();
				}
			}
			break;
		}
	}
}

void CControlHolder::ResetImageList(CWnd *pControl, int nID)
{
	switch (mpTemplate->GetType())
	{
		case CtlTabStrip:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList)
				((CTabCtrl*)pControl)->SetImageList(&pImageList->m_ImageList);
			break;
		}
		case CtlGrid:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList)
				((CListCtrlEx*)pControl)->m_Child.SetImageList(&pImageList->m_ImageList);
			break;
		}
		case CtlTree:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList)
			{
				((CTreeCtrl*)pControl)->SetImageList(&pImageList->m_ImageList, TVSIL_NORMAL);
				((CTreeCtrl*)pControl)->SetImageList(&pImageList->m_ImageList, TVSIL_STATE);
			}
			break;
		}
	}
}

//call the old property update function (this can be removed once all controls implement CDialogControl)
void CControlHolder::UpdateChildControl()
{
	if( mpDlgControl )
	{
		POSITION pos = mpTemplate->GetPropertyList().GetHeadPosition();
		while( pos )
			UpdateProperty( mpTemplate->GetPropertyList().GetNext( pos )->GetID() );
	}
	Invalidate();
}
