// ControlHolder.cpp : implementation file
//

#include "stdafx.h"
#include "ControlHolder.h"
#include "DclControlObject.h"
#include "ControlTypes.h"
#include "Resource.h"
#include "Workspace.h"
#include "GraphicButtonCtrl.h"
#include "TabStripCtrl.h"
#include "AxContainerCtrl.h"
#include "ListBoxCtrl.h"
#include "OptionListCtrl.h"
#include "CheckBoxCtrl.h"
#include "RadioButtonCtrl.h"
#include "GridCtrl.h"
#include "StaticLink.h"
#include "3DRect.h"
#include "Splitter.h"
#include "PictureBox.h"
#include "RoundSliderCtrl.h"
#include "ColorEdit.h"
#include "VdclGroupBox.h"
#include "VdclStatic.h"
#include "ListCtrlEx.h"
#include "FileDlgCtrls.h"
#include "PictureObject.h"
#include "OpenDCLView.h"
#include "Project.h"


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
, mnControlId( -1 )
, mGripper( this, false )
{
}

CControlHolder::CControlHolder( CDclControlObject* mpTemplate )
: CControlPane( mpTemplate->GetOwnerForm(), this )
, mpTemplate( mpTemplate )
, mpDlgControl( NULL )
, mnControlId( -1 )
, mGripper( this, false )
{
}

CControlHolder::~CControlHolder()
{
}


BEGIN_MESSAGE_MAP(CControlHolder, CWnd)
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CControlHolder message handlers

BOOL CControlHolder::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	BOOL bSuccess = CStatic::Create(CString(), dwStyle, rect, pParentWnd, nID);
	if( bSuccess )
		mnControlId = nID + 1;
	return bSuccess;
}

void CControlHolder::SetSelected() 
{
	CRect rcThis;
	GetWindowRect( &rcThis );
	ScreenToClient( &rcThis );
	mGripper.MoveTo( rcThis );
}

void CControlHolder::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	if (mpTemplate == NULL)
		return;

	if (mpTemplate->GetType() == CtlActiveX)
	{
		CAxContainerCtrl* pContainer = GetActiveXCtrl();
		if (pContainer && pContainer->m_hWnd)
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
	else if( mpDlgControl )
	{
		CWnd *pControl = mpDlgControl->GetControl();
		if (pControl != NULL)
		{
			pControl->MoveWindow( 0, 0, cx, cy, TRUE);
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
	static LPCTSTR sText = _T("ImageListCtrl");
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
	
	pViewObject->Draw( DVASPECT_CONTENT, -1, NULL, NULL, NULL, dc.m_hDC, &rc, NULL, NULL, 0 );
}


void CControlHolder::OnDestroy() 
{
	mpDlgControl = NULL;
	__super::OnDestroy();
}

CAxContainerCtrl* CControlHolder::GetActiveXCtrl()
{
	if( !mpDlgControl || mpDlgControl->GetControlType() != CtlActiveX )
		return NULL;
	return (CAxContainerCtrl*)mpDlgControl->GetControl();
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

CSize CControlHolder::GetControlSize( CWnd* pControl, ControlType nControlType )
{
	CSize retSize( 0, 0 );
	if( pControl )
	{
		CRect rcControl;
		if (nControlType == CtlMonth)
			((CMonthCalCtrl*)pControl)->GetMinReqRect( &rcControl );
		else
			pControl->GetWindowRect(&rcControl);
		retSize.SetSize( rcControl.Width(), rcControl.Height() );	
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

	TPropertyPtr pPropNoIntegralHeight = mpTemplate->GetPropertyObject(Prop::NoIntegralHeight);
	if (!pPropNoIntegralHeight || pPropNoIntegralHeight->GetBooleanValue())
		dwStyle = dwStyle | CBS_NOINTEGRALHEIGHT;

	if (mpTemplate->GetBooleanProperty(Prop::Sorted) == TRUE)
		dwStyle = dwStyle | CBS_SORT;

	if (mpTemplate->GetLongProperty(Prop::ComboBoxStyle) == 12)
		mpTemplate->SetLongProperty(Prop::DropDownHeight, nComboDropHeight);
		
	switch (mpTemplate->GetLongProperty(Prop::ComboBoxStyle))
	{
		case 0:
		{
			CComboBox *pControl = new CComboBox;
			pControl->Create( dwStyle | CBS_DROPDOWN, rc, this, GetId());
			pControl->SetWindowText(mpTemplate->GetStrProperty(Prop::Text));
			pNewControl = pControl;
			break;
		}
		case 1:
		{
			CComboBox *pControl = new CComboBox;
			pControl->Create( dwStyle | CBS_SIMPLE, rc, this, GetId());
			pControl->SetWindowText(mpTemplate->GetStrProperty(Prop::Text));
			pNewControl = pControl;
			break;
		}
		case 2:
		{
			CComboBox *pControl = new CComboBox;
			pControl->Create( dwStyle | CBS_DROPDOWNLIST, rc, this, GetId());
			
			CString sText = mpTemplate->GetStrProperty(Prop::Text);
			CString sName = mpTemplate->GetStrProperty(Prop::Name);
			if (sName != sText)
			{
				pControl->SetWindowText(sName);
				mpTemplate->SetStringProperty(Prop::Text, sName);
			}
			pNewControl = pControl;
			break;
		}
		case 9:
		{
			CComboBox *pControl = new CComboBox;
			pControl->Create( dwStyle | CBS_SIMPLE, rc, this, GetId());
			
			CString sDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_0 + 9).Mid(4);
			mpTemplate->SetStringProperty(Prop::Text, sDesc);
			pControl->SetWindowText(sDesc);			
			pNewControl = pControl;
			break;
		}
		default:
		{
			CComboBoxEx *pControl = new CComboBoxEx;
			pControl->Create( WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | CBS_DROPDOWN, rc, this, GetId());
			
			int nThisValue = mpTemplate->GetLongProperty(Prop::ComboBoxStyle);
			CString sDesc = theWorkspace.LoadResourceString(IDS_COMBOBOXSTYLE_0 + nThisValue);
			if (nThisValue > 9)
				sDesc = sDesc.Mid(5);
			else
				sDesc = sDesc.Mid(4);

			mpTemplate->SetStringProperty(Prop::Text, sDesc);
			pControl->SetWindowText(sDesc);
			pNewControl = pControl;
			break;
		}
	}

	CRect rcAfter;
	pNewControl->GetWindowRect(&rcAfter);
	mpTemplate->SetLongProperty(Prop::Height, rcAfter.Height());
	return pNewControl;
}

CWnd* CControlHolder::CreateTextBox(CDclControlObject *mpTemplate)
{
	CRect rcThis;
	GetWindowRect(&rcThis);
	CRect rc(0,0,rcThis.Width(), rcThis.Height());
	
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|ES_WANTRETURN |WS_CLIPSIBLINGS; 

	if (mpTemplate->GetBooleanProperty(Prop::HScrollBar) == TRUE)
		dwStyle = dwStyle | WS_HSCROLL;
	if (mpTemplate->GetBooleanProperty(Prop::VScrollBar) == TRUE)
		dwStyle = dwStyle | WS_VSCROLL;	
	if (mpTemplate->GetBooleanProperty(Prop::ReadOnly) == TRUE)
		dwStyle = dwStyle | ES_READONLY;
	
	// adjust the dwStyle for filter settings
	switch (mpTemplate->GetLongProperty(Prop::FilterStyle))
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
	switch (mpTemplate->GetLongProperty(Prop::Justification))
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

	if (mpTemplate->GetLongProperty(Prop::FilterStyle) == 7)
		pNewControl->SetPasswordChar(_T('*'));
	else
		pNewControl->SetPasswordChar(_T('\0'));
	
	pNewControl->SetWindowText(mpTemplate->GetStrProperty(Prop::Text));

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
			pControl->Create( mpTemplate->GetStrProperty(Prop::Caption),
												WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPSIBLINGS,
												rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlStdButton:
		{
			CButton *pControl = new CButton;
			pControl->Create( mpTemplate->GetStrProperty(Prop::Caption),
												WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|BS_MULTILINE,
												rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlGraphicButton : return ((mpDlgControl = new CGraphicButtonCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlFrame:
		{
			VdclGroupBox *pControl = new VdclGroupBox;
			pControl->Create( mpTemplate->GetStrProperty(Prop::Caption), rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlTextBox:
		pNewControl = CreateTextBox(mpTemplate);
		break;
	case CtlCheckBox : return ((mpDlgControl = new CCheckBoxCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlOptionButton : return ((mpDlgControl = new CCheckBoxCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlComboBox:
		pNewControl = CreateComboBox(mpTemplate);
		break;
	case CtlListBox : return ((mpDlgControl = new CListBoxCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlScrollBar:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS;
			
			if (rcThis.Width() >= rcThis.Height())
			{
				mpTemplate->SetLongProperty(Prop::Orientation, 0);
				dwStyle = dwStyle | SBS_HORZ;
			}
			else
			{
				mpTemplate->SetLongProperty(Prop::Orientation, 1);
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
				mpTemplate->SetLongProperty(Prop::Orientation, 0);
				dwStyle = dwStyle | TBS_HORZ;
			}
			else
			{
				mpTemplate->SetLongProperty(Prop::Orientation, 1);
				dwStyle = dwStyle | TBS_VERT;
			}
			if (mpTemplate->GetBooleanProperty(Prop::ShowTicks) == TRUE)
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
	case CtlTabStrip : return ((mpDlgControl = new CTabStripCtrl( mpTemplate, this, GetId() )) != NULL);
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

			switch (mpTemplate->GetLongProperty(Prop::ComboBoxStyle))
			{
				case 0:
				{
					pControl->Create( dwStyle | CBS_DROPDOWN, rc, this, GetId());
					pControl->SetWindowText(mpTemplate->GetStrProperty(Prop::Text));
					break;
				}
				case 1:
				{
					pControl->Create( dwStyle | CBS_SIMPLE, rc, this, GetId());
					pControl->SetWindowText(mpTemplate->GetStrProperty(Prop::Text));
					break;
				}
				case 2:
				{
					pControl->Create( dwStyle | CBS_DROPDOWNLIST, rc, this, GetId());
					CString sText = mpTemplate->GetStrProperty(Prop::Text);
					CString sName = mpTemplate->GetStrProperty(Prop::Text);
					if (sName != sText)
					{
						pControl->SetWindowText(sName);
						mpTemplate->SetStringProperty(Prop::Text, sName);
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
			if (mpTemplate->GetBooleanProperty(Prop::SmoothProgress) == TRUE)
				dwStyle = dwStyle | PBS_SMOOTH;
			if (mpTemplate->GetLongProperty(Prop::Orientation) == 1)
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
				mpTemplate->SetLongProperty(Prop::Orientation, 0);
				dwStyle = dwStyle | UDS_HORZ;
			}
			else
				mpTemplate->SetLongProperty(Prop::Orientation, 1);
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
			//if (mpTemplate->GetBooleanProperty(Prop::ShowTicks) == TRUE)
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
			if (mpTemplate->GetBooleanProperty(Prop::ShowSelectAlways))
				dwStyle = dwStyle | TVS_SHOWSELALWAYS;
			if (mpTemplate->GetBooleanProperty(Prop::HasLines))
				dwStyle = dwStyle | TVS_HASLINES;
			if (mpTemplate->GetBooleanProperty(Prop::LinesAtRoot))
				dwStyle = dwStyle | TVS_LINESATROOT;
			if (mpTemplate->GetBooleanProperty(Prop::HasButtons))
				dwStyle = dwStyle | TVS_HASBUTTONS;
			if (mpTemplate->GetBooleanProperty(Prop::EditLabels))
				dwStyle = dwStyle | TVS_EDITLABELS;
			if (mpTemplate->GetBooleanProperty(Prop::CheckBoxes))
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
	case CtlGrid : return ((mpDlgControl = new CGridCtrl( mpTemplate, this, GetId() )) != NULL);
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
	case CtlOptionList : return ((mpDlgControl = new COptionListCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlActiveX : return ((mpDlgControl = new CAxContainerCtrl( mpTemplate, this, GetId(), rc, true )) != NULL);
	case CtlDwgList : return ((mpDlgControl = new CListBoxCtrl( mpTemplate, this, GetId() )) != NULL);
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
	CRect rectControl;
	pNewControl->GetWindowRect( &rectControl );
	GetParent()->ScreenToClient( &rectControl );
	MoveWindow( &rectControl );
	return true;
}

void CControlHolder::AutoSize()
{
	CWnd *pControl = GetControl();
	if (pControl == NULL)
		return;
	
	switch(mpTemplate->GetType())
	{
	case CtlPictureBox:
		{
		if( mpTemplate->GetBooleanProperty( Prop::AutoSize ) )
		{
			int nBorderAddition = 0;
			switch(mpTemplate->GetLongProperty(Prop::BorderStyle))
			{
			case 0:
				break;
			case 1:
				nBorderAddition = 4;
				break;
			case 2:
				nBorderAddition = 2;
				break;
			}
			int nOldWidth = mpTemplate->GetLongProperty(Prop::Width);
			int nOldHeight = mpTemplate->GetLongProperty(Prop::Height);
			int nWidthDelta = ((CPictureBox*)pControl)->m_cxIcon + nBorderAddition - nOldWidth;
			int nHeightDelta = ((CPictureBox*)pControl)->m_cyIcon + nBorderAddition - nOldHeight;
			if( nWidthDelta != 0 )
				mpTemplate->SetLongProperty(Prop::Width, nOldWidth + nWidthDelta);
			if( nHeightDelta != 0 )
				mpTemplate->SetLongProperty(Prop::Height, nOldHeight + nHeightDelta);
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
			CSize szControl = GetControlSize( pControl, mpTemplate->GetType() );
			int nOldWidth = mpTemplate->GetLongProperty(Prop::Width);
			int nOldHeight = mpTemplate->GetLongProperty(Prop::Height);
			int nWidthDelta = szControl.cx - nOldWidth;
			int nHeightDelta = szControl.cy - nOldHeight;
			if( nWidthDelta != 0 )
				mpTemplate->SetLongProperty( Prop::Width, szControl.cx );
			if( nHeightDelta != 0 )
				mpTemplate->SetLongProperty( Prop::Height, szControl.cy );
			if( nWidthDelta != 0 || nHeightDelta != 0 )
			{
				CRect rectThis;
				GetWindowRect( &rectThis );
				GetParent()->ScreenToClient( &rectThis );
				rectThis.right = rectThis.left + szControl.cx;
				rectThis.bottom = rectThis.top + szControl.cy;
				CRect rectControl(0, 0, rectThis.Width(), rectThis.Height());
				pControl->MoveWindow( &rectThis, TRUE );
				MoveWindow( &rectThis, TRUE );
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

	mpTemplate->m_ClientHeight = ((CTabStripCtrl*)pControl)->GetUsedArea().Height();
}


void CControlHolder::UpdateProperty(Prop::Id nID)
{
	if( !mpDlgControl )
		return;
	TPropertyPtr pProp = mpTemplate->GetPropertyObject(nID);

	//if the control has been redesigned to implement the CDialogControl interface, use that
	switch( mpTemplate->GetType() )
	{
	case CtlActiveX:
	case CtlCheckBox:
	case CtlDwgList:
	case CtlGraphicButton:
	case CtlGrid:
	case CtlListBox:
	case CtlOptionList:
	case CtlOptionButton:
	case CtlTabStrip:
		mpDlgControl->OnApplyProperty( pProp );
		mpDlgControl->GetControl()->ShowWindow( SW_SHOW ); //make it visible even if the 'Prop::Visible' property is false
		mpDlgControl->GetControl()->Invalidate();
		return;
	}

	CWnd* pControl = mpDlgControl->GetControl();
	assert( pControl != NULL );
	if( !pControl )
		return;
	// set the appropriate property
	switch(nID)
	{
		case Prop::AutoSize:
			{
			switch (mpTemplate->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControl)->m_AutoSize = mpTemplate->GetBooleanProperty(Prop::AutoSize);
				break;
			//case CtlGraphicButton:
			//	((CGraphicButtonCtrl*)pControl)->m_AutoSize = mpTemplate->GetBooleanProperty(Prop::AutoSize);
				break;
			}			
			break;
			}
		case Prop::BackgroundColor:
		{				
			switch (mpTemplate->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControl)->SetAcadColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));
				break;
			
			//case CtlStdButton:
			//case CtlOptionButton:
			//case CtlCheckBox:
			//	((CColorButton*)pControl)->SetAcadColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));
			//	break;

			case CtlAnimate:
			case CtlListView:
			case CtlGrid:
			case CtlHatch:
			case CtlBlockList:
			case CtlBlockView:
			case CtlDwgPreview:
			case CtlPictureBox:
			case CtlSlideView:		
				((CPictureBox*)pControl)->SetAcadColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));
				break;

			case CtlTextBox:
				((CColorEdit*)pControl)->SetAcadColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));
				break;
			//
			//case CtlListBox:
			//case CtlDwgList:
			//case CtlOptionList:
			//	((CListBoxCtrl*)pControl)->GetColorService()->SetBackgroundColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));
			//	break;		
				
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetAcadColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));
				break;		

			//case CtlGraphicButton:
			//	((CGraphicButtonCtrl*)pControl)->SetAcadColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));
			//	break;
			}
			pControl->Invalidate();
			break;
		}	
		case Prop::ForegroundColor:
		{				
			switch (mpTemplate->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControl)->SetForeColor(mpTemplate->GetLongProperty(Prop::ForegroundColor));
				break;		

			//case CtlStdButton:
			//case CtlOptionButton:
			//case CtlCheckBox:
			//	((CColorButton*)pControl)->SetForeColor(mpTemplate->GetLongProperty(Prop::ForegroundColor));
			//	break;
			//case CtlGraphicButton:
			//	((CGraphicButtonCtrl*)pControl)->SetForeColor(mpTemplate->GetLongProperty(Prop::ForegroundColor));
			//	break;
			case CtlTextBox:
				//((CColorEdit*)pControl)->SetForeColor(mpTemplate->GetLongProperty(Prop::ForegroundColor));
				switch (mpTemplate->GetLongProperty(Prop::FilterStyle))
				{
				case 8: //EditFilter_Multiline
					((CColorEdit*)pControl)->m_UseBackColor = false;
				case 4: //EditFilter_Symbol
					break;
				default:
					((CColorEdit*)pControl)->SetForeColor(mpTemplate->GetLongProperty(Prop::ForegroundColor));
					break;
				}
				break;			
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetForeColor(mpTemplate->GetLongProperty(Prop::ForegroundColor));
				break;
			//case CtlListBox:
			//case CtlDwgList:
			//case CtlOptionList:
			//	((CAcadColorListBox*)pControl)->SetForeColor(mpTemplate->GetLongProperty(Prop::ForegroundColor));
			//	break;
			}			
			pControl->Invalidate();
			break;
		}	
		
		case Prop::BorderStyle:
		{
			switch(mpTemplate->GetLongProperty(Prop::BorderStyle))
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
				((CPictureBox*)pControl)->m_BorderStyle = mpTemplate->GetLongProperty(Prop::BorderStyle);
				break;
			}
			break;
		}	
		case Prop::ButtonStyle:
		{				
			//((CGraphicButton*)pControl)->SetDefaultPicture(mpTemplate->GetLongProperty(Prop::ButtonStyle));
			break;
		}	
		case Prop::Caption:
		{				
			switch (mpTemplate->GetType())
			{
				
			case CtlFrame:
				((VdclGroupBox*)pControl)->SetCaption(mpTemplate->GetStrProperty(Prop::Caption));
				break;
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetLinkText(mpTemplate->GetStrProperty(Prop::Caption));
				break;
			//case CtlGraphicButton:
			//	{
			//	((CGraphicButtonCtrl*)pControl)->SetWindowText(mpTemplate->GetStrProperty(Prop::Caption));
			//	((CGraphicButtonCtrl*)pControl)->Invalidate();
			//	break;
			//	}			
			default:
				{
				CString sCaptionText = mpTemplate->GetStrProperty(Prop::Caption);
				pControl->SetWindowText(sCaptionText);
				break;
				}
			}
			break;
		}	
		case Prop::ColumnWidth:
		{				
			int nNewColWidth = mpTemplate->GetLongProperty(Prop::ColumnWidth);
			if (nNewColWidth > 0)
				((CListBox*)pControl)->SetColumnWidth(nNewColWidth);
			break;
		}	
		
		//case Prop::DefSelIndex:
		//{
		//	((COptionListBox*)pControl)->ResetContent();					
		//	CString sListItem;
		//	TPropertyPtr pPropList = mpTemplate->GetPropertyObject(Prop::BtnCaption);
		//	int nDefSelection = mpTemplate->GetLongProperty(Prop::DefSelIndex) ;
		//	for (size_t i = 0; i < pPropList->size(); i++)
		//	{				
		//		sListItem = pPropList->GetStringItem(i);
		//		if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
		//		{
		//			((COptionListBox *)pControl)->AddString(sListItem);
		//		}
		//		if (nDefSelection == i)
		//			((COptionListBox *)pControl)->SetItemData(i, 1);
		//		else
		//			((COptionListBox *)pControl)->SetItemData(i, 0);
		//	}
		//	
		//	break;
		//}
		
		case Prop::DisableNoScroll:
		{
			if (mpTemplate->GetBooleanProperty(Prop::DisableNoScroll) == FALSE)
				pControl->ModifyStyle(LBS_DISABLENOSCROLL, 0, SWP_FRAMECHANGED);
			else
				pControl->ModifyStyle(0, LBS_DISABLENOSCROLL, SWP_FRAMECHANGED);
			break;
		}
		
		case Prop::Enabled:
		{
			if (mpTemplate->GetType() == CtlSlider)
			{
				((CSliderCtrl *)pControl)->EnableWindow(mpTemplate->GetBooleanProperty(Prop::Enabled));
			}
			//else if (mpTemplate->GetType() == CtlOptionList)
			//{
			//	int nData=0;
			//	if (mpTemplate->GetBooleanProperty(Prop::Enabled) == FALSE)
			//		nData = 2;
			//	for (int i=0; i<((COptionListBox*) pControl)->GetCount(); i++)
			//	{
			//		((COptionListBox*) pControl)->SetItemData(i, nData);
			//	}
			//	pControl->Invalidate();				
			//}			
			else
			{
				pControl->EnableWindow(mpTemplate->GetBooleanProperty(Prop::Enabled));
				pControl->Invalidate();
			}
			break;				
		}	
		case Prop::FilterStyle:
			{
			switch (mpTemplate->GetLongProperty(Prop::FilterStyle))
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

		case Prop::Indent:
		{
			((CTreeCtrl*)pControl)->SetIndent(pProp->GetLongValue());
			break;
		}

		case Prop::ImageList:
		{
			ResetImageList(pControl, nID);
			break;
		}

		case Prop::Justification:
		{
			switch (mpTemplate->GetType())
			{
			case CtlTextBox:
				{
					/*
					((CColorEdit*)pControl)->ModifyStyle(ES_RIGHT, 0, SWP_FRAMECHANGED);
					((CColorEdit*)pControl)->ModifyStyle(ES_CENTER, 0, SWP_FRAMECHANGED);
					((CColorEdit*)pControl)->ModifyStyle(ES_LEFT, 0, SWP_FRAMECHANGED);
					
					switch (mpTemplate->GetLongProperty(Prop::Justification))
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
			
					switch (mpTemplate->GetLongProperty(Prop::Justification))
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

		case Prop::LabelName:
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
			//else if (mpTemplate->GetType() == CtlOptionList)
			//{
			//	((COptionListBox*)pControl)->SetFont(pFont);	
			//	pControl->Invalidate();				
			//}
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

		case Prop::LimitText:
		{
			((CColorEdit*)pControl)->SetLimitText(mpTemplate->GetLongProperty(Prop::LimitText));
			break;
		}
		
		//case Prop::BtnCaption:
		//{
		//	CString sListItem;
		//	((COptionListBox*)pControl)->ResetContent();					
		//	int nDefSelection = mpTemplate->GetLongProperty(Prop::DefSelIndex) ;
		//	for (size_t i = 0; i < pProp->size(); i++)
		//	{				
		//		sListItem = pProp->GetStringItem(i);
		//		if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
		//		{
		//			((COptionListBox *)pControl)->AddString(sListItem);
		//			if (nDefSelection == i)
		//				((COptionListBox *)pControl)->SetItemData(i, 1);
		//			else
		//				((COptionListBox *)pControl)->SetItemData(i, 0);
		//		}
		//	}
		//	
		//	break;
		//}
	
		case Prop::ColumnCaptions:
		{
			((CListCtrlEx *)pControl)->SetupColumns(mpTemplate);

			break;
		}
		case Prop::List:
		{
			switch (mpTemplate->GetType())
			{
			//case CtlListBox:
			//	{
			//		CString sListItem;
			//		((CListBox *)pControl)->ResetContent();					
			//		for (size_t i = 0; i < pProp->size(); i++)
			//		{				
			//			sListItem = pProp->GetStringItem(i);
			//			if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
			//				((CListBox *)pControl)->AddString(sListItem);
			//		}
			//		break;
			//	}
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
					pControl->SetWindowText(mpTemplate->GetStrProperty(Prop::Text));
					break;
				}
			}
			break;				
		}
		case Prop::MarginLeft:
		{
			((CColorEdit*)pControl)->SetMargins(
				mpTemplate->GetLongProperty(Prop::MarginLeft),
				mpTemplate->GetLongProperty(Prop::MarginRight));
			break;
		}
		
		//case Prop::MinTabWidth:
		//{
		//	try 
		//	{
		//		((CTabCtrl*)pControl)->SetMinTabWidth(mpTemplate->GetLongProperty(Prop::MinTabWidth));
		//		((CTabCtrl*)pControl)->RedrawWindow(NULL, NULL, RDW_UPDATENOW);
		//	}
		//	catch(...)
		//	{
		//	}
		//	break;
		//}
		
		case Prop::MaxValue:
		case Prop::MinValue:
		{
			switch (mpTemplate->GetType())
			{
			case CtlProgress:
				((CProgressCtrl*)pControl)->SetRange(
					(short)mpTemplate->GetLongProperty(Prop::MinValue),
					(short)mpTemplate->GetLongProperty(Prop::MaxValue));
				break;				
			case CtlScrollBar:
				((CScrollBar*)pControl)->SetScrollRange(
					mpTemplate->GetLongProperty(Prop::MinValue),
					mpTemplate->GetLongProperty(Prop::MaxValue),
					TRUE);
				break;				
			case CtlSlider:
				((CSliderCtrl*)pControl)->SetRange(
					mpTemplate->GetLongProperty(Prop::MinValue),
					mpTemplate->GetLongProperty(Prop::MaxValue));
				((CSliderCtrl *)pControl)->SetPos(mpTemplate->GetLongProperty(Prop::MinValue));
				((CSliderCtrl *)pControl)->SetPos(mpTemplate->GetLongProperty(Prop::Value));
				break;				
			}
			break;
		}	
		
		case Prop::SplitterStyle:
		{
			((CSplitter *)pControl)->m_nStyle = mpTemplate->GetLongProperty(Prop::SplitterStyle);
			int n = ((CSplitter *)pControl)->m_nStyle;
			if (((CSplitter *)pControl)->m_nStyle <= 0)
			{
				((CSplitter *)pControl)->m_nStyle = 0;
				mpTemplate->SetLongProperty(Prop::SplitterStyle, 0);
			}
			pControl->Invalidate();
			break;
		}
			
		case Prop::Orientation:
		{
			if (mpTemplate->GetType() == CtlSlider)
			{			
				switch(mpTemplate->GetLongProperty(Prop::Orientation))
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
		case Prop::Picture:
		{
			switch (mpTemplate->GetType())
			{
			//case CtlGraphicButton:		
			//	((CGraphicButtonCtrl *)pControl)->SetPictureID(mpTemplate->GetLongProperty(Prop::Picture));
			//	break;
			case CtlPictureBox:
				int nID = mpTemplate->GetLongProperty(Prop::Picture);
				((CPictureBox*)pControl)->SetPicture(mpTemplate->GetOwnerProject()->FindPicture(nID));
				break;
			}
			break;
		}

			
		case Prop::ShowTicks:
		{
			if (mpTemplate->GetBooleanProperty(Prop::ShowTicks) == FALSE)
				((CSliderCtrl *)pControl)->ModifyStyle(TBS_AUTOTICKS, 0, SWP_FRAMECHANGED);
			else
				((CSliderCtrl *)pControl)->ModifyStyle(0, TBS_AUTOTICKS, SWP_FRAMECHANGED);
				
			break;
		}

		case Prop::ReadOnly:
		{
			((CColorEdit*)pControl)->SetReadOnly(mpTemplate->GetBooleanProperty(Prop::ReadOnly));
			break;
		}

		//case Prop::RowHeight:
		//{
		//	if (mpTemplate->GetType() == CtlOptionList)
		//	{
		//		((COptionListBox*)pControl)->m_RowHeight = (short)mpTemplate->GetLongProperty(Prop::RowHeight);
		//		((COptionListBox*)pControl)->ResetContent();					
		//		CString sListItem;
		//		TPropertyPtr pPropList = mpTemplate->GetPropertyObject(Prop::BtnCaption);
		//		int nDefSelection = mpTemplate->GetLongProperty(Prop::DefSelIndex) ;
		//		for (size_t i = 0; i < pPropList->size(); i++)
		//		{				
		//			sListItem = pPropList->GetStringItem(i);
		//			if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
		//			{
		//				((COptionListBox *)pControl)->AddString(sListItem);
		//				if (nDefSelection == i)
		//					((COptionListBox *)pControl)->SetItemData(i, 1);
		//				else
		//					((COptionListBox *)pControl)->SetItemData(i, 0);
		//			}
		//		}
		//	}
		//	
		//	break;
		//}

		//case Prop::TabsCaption:
		////case Prop::TabsImageList:
		//{
		//	try
		//	{
		//		bool bHasImageList = (mpTemplate->GetImageList() != NULL);

		//		
		//		// delete all previos tabs
		//		((CTabCtrl*)pControl)->DeleteAllItems();
		//		
		//		int nCount = mpTemplate->CountPropertyListItems(Prop::TabsCaption);
		//		int nBottom = 0;
		//		while (nCount-- > 0)
		//		{
		//			TC_ITEM TabCtrlItem;
		//			CString sTTT;
		//			TabCtrlItem.mask = TCIF_TEXT;
		//			CString Tab = mpTemplate->GetPropertyListItem(Prop::TabsCaption, nCount);
		//			// get the tab caption
		//			TabCtrlItem.pszText = Tab.GetBuffer(nDeTextLimitCB);		
		//			
		//			// set the image list item number if required
		//			if (bHasImageList)
		//			{
		//				TabCtrlItem.mask |= TCIF_IMAGE;
		//				TabCtrlItem.iImage = _tstol(mpTemplate->GetPropertyListItem(Prop::TabsImageList, nCount));
		//			}
		//			
		//			// add the new tab
		//			((CTabCtrl*)pControl)->InsertItem( 0, &TabCtrlItem );
		//		}
		//		UpdateClientHeight();
		//	}
		//	catch(...)
		//	{
		//	}
		//	break;
		//}
		//case Prop::TabSelected:
		//{
		//	((CAcadColorListBox*)pControl)->SetCurSel(mpTemplate->GetLongProperty(Prop::TabSelected));
		//	break;
		//}
		//case Prop::TabStyle:
		//{
		//	if (mpTemplate->GetLongProperty(Prop::TabStyle) == 0)
		//		((CTabCtrl*)pControl)->ModifyStyle(TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED);
		//	else
		//		((CTabCtrl*)pControl)->ModifyStyle(TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED);
		//	break;
		//}
		//
		//case Prop::TabFixedWidth:
		//{
		//	try
		//	{
		//		if (mpTemplate->GetBooleanProperty(Prop::TabFixedWidth) == TRUE)
		//		{
		//			((CTabCtrl*)pControl)->ModifyStyle(0, TCS_FIXEDWIDTH, SWP_FRAMECHANGED);
		//			CRect rc;
		//			((CTabCtrl*)pControl)->GetItemRect(0, &rc);
		//			CSize szTabs;
		//			szTabs.cx = mpTemplate->GetLongProperty(Prop::MinTabWidth);
		//			szTabs.cy = rc.Height();
		//			((CTabCtrl*)pControl)->SetItemSize(szTabs);
		//		}
		//		else
		//		{	
		//			((CTabCtrl*)pControl)->ModifyStyle(TCS_FIXEDWIDTH, 0, SWP_FRAMECHANGED);
		//		}
		//	}
		//	catch(...)
		//	{
		//	}
		//	break;
		//}
		case Prop::Text:
		{
			int nTextLimit = mpTemplate->GetLongProperty(Prop::LimitText);

			CString sNewText = mpTemplate->GetStrProperty(Prop::Text);

			if (nTextLimit > -1) 
			{
				sNewText = sNewText.Left(nTextLimit);
				mpTemplate->SetStringProperty(Prop::Text, sNewText);
			}

			pControl->SetWindowText(sNewText);
			break;
		}
		case Prop::TickFrequency:
		{
			((CSliderCtrl *)pControl)->SetTicFreq(mpTemplate->GetLongProperty(Prop::TickFrequency));
			
			break;
		}
		
		case Prop::UseTabStops:
		{
			if (mpTemplate->GetBooleanProperty(Prop::UseTabStops) == FALSE)
				pControl->ModifyStyle(LBS_USETABSTOPS, 0, SWP_FRAMECHANGED);
			else
				pControl->ModifyStyle(0, LBS_USETABSTOPS, SWP_FRAMECHANGED);
			break;
		}
		
		case Prop::Value:
		{
			switch (mpTemplate->GetType())
			{
			//case CtlCheckBox:
			//case CtlOptionButton:
			//	((CColorButton *)pControl)->SetCheck(mpTemplate->GetBooleanProperty(Prop::Value));
			//	break;									
			case CtlRoundSlider:				
				//((CRoundSliderCtrl *)pControl)->SetText(mpTemplate->GetLongProperty(Prop::Value));
				((CRoundSliderCtrl *)pControl)->SetPos(mpTemplate->GetLongProperty(Prop::Value));				
				((CRoundSliderCtrl *)pControl)->RedrawWindow();

				break;
			case CtlSlider:
				((CSliderCtrl *)pControl)->SetPos(mpTemplate->GetLongProperty(Prop::Value));
				break;
			case CtlScrollBar:
				((CScrollBar *)pControl)->SetScrollPos(mpTemplate->GetLongProperty(Prop::Value), TRUE);
				break;
			case CtlProgress:
				((CProgressCtrl*)pControl)->SetPos(mpTemplate->GetLongProperty(Prop::Value));
				break;
			
			}
			break;
		}
		case Prop::VScrollBar:
		{
			if (mpTemplate->GetType() != CtlDwgList)
			{
				if (mpTemplate->GetBooleanProperty(Prop::VScrollBar) == FALSE)
					pControl->ModifyStyle(WS_VSCROLL, 0, SWP_FRAMECHANGED);
				else
				{
					pControl->ModifyStyle(0, WS_VSCROLL, SWP_FRAMECHANGED);
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
				((CTabStripCtrl*)pControl)->SetImageList(&pImageList->GetImageList());
			break;
		}
		case CtlGrid:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList)
				((CListCtrlEx*)pControl)->m_Child.SetImageList(&pImageList->GetImageList());
			break;
		}
		case CtlTree:
		{
			RefCountedPtr< CImageListObject > pImageList = mpTemplate->GetImageList();
			if (pImageList)
			{
				((CTreeCtrl*)pControl)->SetImageList(&pImageList->GetImageList(), TVSIL_NORMAL);
				((CTreeCtrl*)pControl)->SetImageList(&pImageList->GetImageList(), TVSIL_STATE);
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
