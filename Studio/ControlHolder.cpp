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
#include "ImageComboBoxCtrl.h"
#include "ComboBoxCtrl.h"
#include "TextBoxCtrl.h"
#include "FolderComboCtrl.h"
#include "TextButtonCtrl.h"
#include "ProgressBarCtrl.h"
#include "StaticLink.h"
#include "3DRect.h"
#include "Splitter.h"
#include "PictureBox.h"
#include "RoundSliderCtrl.h"
#include "VdclGroupBox.h"
#include "VdclStatic.h"
#include "ListCtrlEx.h"
#include "FileDlgCtrls.h"
#include "PictureObject.h"
#include "OpenDCLView.h"
#include "Project.h"
#include "AngleFilter.h"
#include "CurrencyFilter.h"
#include "CustomFilter.h"
#include "DateFilter.h"
#include "IntegerFilter.h"
#include "LowerCaseFilter.h"
#include "MultilineFilter.h"
#include "NumericFilter.h"
#include "PasswordFilter.h"
#include "SymbolNameFilter.h"
#include "TimeFilter.h"
#include "UpperCaseFilter.h"


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

CControlHolder::CControlHolder( TDclControlPtr mpTemplate )
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
	ON_WM_ERASEBKGND()
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
		CWnd *pControl = mpDlgControl->GetControlWnd();
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
	if (mpTemplate->GetActiveXTypeName() != _T("ImageListCtrl"))
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
	return (CAxContainerCtrl*)mpDlgControl->GetControlWnd();
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

TDialogControlPtr CControlHolder::CreateComboBox( TDclControlPtr pTemplate )
{
	// check the control type to determine which control to create
	switch(pTemplate->GetLongProperty(Prop::ComboBoxStyle))
	{
	case CmboStyle_Combo: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_Simple: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_DropDown: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_ArrowHead: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_Color: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_LineWeight: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_PlotNames: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_PlotTables: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_FontDropList: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_FontSimpleList: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_Plotters: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_PlotterPaperSizes: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	case CmboStyle_DirPicker: return *new CFolderComboCtrl( pTemplate, this, GetId() );
	case CmboStyle_Layers: return *new CComboBoxCtrl( pTemplate, this, GetId() );
	}
	return NULL;
}

TDialogControlPtr CControlHolder::CreateTextBox( TDclControlPtr pTemplate )
{
	// check the control type to determine which control to create
	switch( pTemplate->GetLongProperty( Prop::FilterStyle ) )
	{
	case EditFilter_String: return *new CTextBoxCtrl( pTemplate, this, GetId(), new CCustomFilter );
	case EditFilter_Angle: return *new CTextBoxCtrl( pTemplate, this, GetId(), new CAngleFilter );
	case EditFilter_Integer: return *new CTextBoxCtrl( pTemplate, this, GetId(), new CIntegerFilter );
	case EditFilter_Numeric: return *new CTextBoxCtrl( pTemplate, this, GetId(), new CNumericFilter );
	case EditFilter_Symbol: return *new CTextBoxCtrl( pTemplate, this, GetId(), new CSymbolNameFilter );
	case EditFilter_UpperCase: return *new CTextBoxCtrl( pTemplate, this, GetId(), new CUpperCaseFilter );
	case EditFilter_LowerCase: return *new CTextBoxCtrl( pTemplate, this, GetId(), new CUpperCaseFilter );
	case EditFilter_Password: return *new CTextBoxCtrl( pTemplate, this, GetId(), new CPasswordFilter );
	case EditFilter_Multiline: return *new CTextBoxCtrl( pTemplate, this, GetId(), new CMultilineFilter );
	}
	return NULL;
}


bool CControlHolder::CreateNewDialogControl()
{
	if( mpDlgControl )
	{
		CWnd* pOldCtrl = (mpDlgControl.isLocked()? mpDlgControl->GetControlWnd() : NULL);
		mpDlgControl = NULL; //this should decrement the previous control's ref count to zero and destroy it
		if( pOldCtrl && ::IsWindow( pOldCtrl->m_hWnd ) )
			pOldCtrl->DestroyWindow();
	}
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
			pControl->Create( mpTemplate->GetStringProperty(Prop::Caption),
												WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPSIBLINGS,
												rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlStdButton : return ((mpDlgControl = new CTextButtonCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlGraphicButton : return ((mpDlgControl = new CGraphicButtonCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlFrame:
		{
			VdclGroupBox *pControl = new VdclGroupBox;
			pControl->Create( mpTemplate->GetStringProperty(Prop::Caption), rc, this, GetId());
			pNewControl = pControl;
			break;
		}
	case CtlTextBox : return ((mpDlgControl = CreateTextBox( mpTemplate )) != NULL);
	case CtlCheckBox : return ((mpDlgControl = new CCheckBoxCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlOptionButton : return ((mpDlgControl = new CRadioButtonCtrl( mpTemplate, this, GetId() )) != NULL);
	case CtlComboBox : return ((mpDlgControl = CreateComboBox( mpTemplate )) != NULL);
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
	case CtlImageComboBox : return ((mpDlgControl = new CImageComboBoxCtrl( mpTemplate, this, GetId() )) != NULL);
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
	case CtlProgress : return ((mpDlgControl = new CProgressBarCtrl( mpTemplate, this, GetId() )) != NULL);
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
	CWnd *pControl = GetControlWnd();
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


void CControlHolder::UpdateProperty(Prop::Id nID)
{
	if( !mpDlgControl )
		return;
	TPropertyPtr pProp = mpTemplate->GetPropertyObject(nID);

	//if the control has been redesigned to implement the CDialogControl interface, use that
	switch( mpTemplate->GetType() )
	{
	case CtlActiveX:
	//case CtlBlockList:
	case CtlCheckBox:
	case CtlComboBox:
	case CtlDwgList:
	case CtlGraphicButton:
	case CtlGrid:
	case CtlImageComboBox:
	case CtlListBox:
	//case CtlListView:
	case CtlOptionList:
	case CtlOptionButton:
	case CtlProgress:
	//case CtlSlideView:
	case CtlStdButton:
	case CtlTextBox:
	case CtlTabStrip:
		mpDlgControl->OnApplyProperty( pProp );
		mpDlgControl->GetControlWnd()->ShowWindow( SW_SHOW ); //make it visible even if the 'Prop::Visible' property is false
		mpDlgControl->GetControlWnd()->Invalidate();
		return;
	}

	CWnd* pControl = mpDlgControl->GetControlWnd();
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

			case CtlAnimate:
			case CtlListView:
			case CtlHatch:
			case CtlBlockList:
			case CtlBlockView:
			case CtlDwgPreview:
			case CtlPictureBox:
			case CtlSlideView:		
				((CPictureBox*)pControl)->SetAcadColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));
				break;
				
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetAcadColor(mpTemplate->GetLongProperty(Prop::BackgroundColor));
				break;		
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
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetForeColor(mpTemplate->GetLongProperty(Prop::ForegroundColor));
				break;
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
		case Prop::Caption:
		{				
			switch (mpTemplate->GetType())
			{
				
			case CtlFrame:
				((VdclGroupBox*)pControl)->SetCaption(mpTemplate->GetStringProperty(Prop::Caption));
				break;
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetLinkText(mpTemplate->GetStringProperty(Prop::Caption));
				break;
			default:
				{
				CString sCaptionText = mpTemplate->GetStringProperty(Prop::Caption);
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
		
		case Prop::Enabled:
		{
			if (mpTemplate->GetType() == CtlSlider)
			{
				((CSliderCtrl *)pControl)->EnableWindow(mpTemplate->GetBooleanProperty(Prop::Enabled));
			}
			else
			{
				pControl->EnableWindow(mpTemplate->GetBooleanProperty(Prop::Enabled));
				pControl->Invalidate();
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

		case Prop::FontName:
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
			else
			{
				pControl->SetFont(pFont);	
				pControl->Invalidate();				
			}	
			break;
		}
	
		case Prop::ColumnCaptions:
		{
			((CListCtrlEx *)pControl)->SetupColumns(mpTemplate);

			break;
		}
		
		case Prop::MaxValue:
		case Prop::MinValue:
		{
			switch (mpTemplate->GetType())
			{
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
		case Prop::Text:
		{
			int nTextLimit = mpTemplate->GetLongProperty(Prop::LimitText);

			CString sNewText = mpTemplate->GetStringProperty(Prop::Text);

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
			case CtlRoundSlider:				
				((CRoundSliderCtrl *)pControl)->SetPos(mpTemplate->GetLongProperty(Prop::Value));				
				((CRoundSliderCtrl *)pControl)->RedrawWindow();

				break;
			case CtlSlider:
				((CSliderCtrl *)pControl)->SetPos(mpTemplate->GetLongProperty(Prop::Value));
				break;
			case CtlScrollBar:
				((CScrollBar *)pControl)->SetScrollPos(mpTemplate->GetLongProperty(Prop::Value), TRUE);
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
		//if the control has been redesigned to implement the CDialogControl interface, use that
		switch( mpTemplate->GetType() )
		{
		case CtlActiveX:
		//case CtlBlockList:
		case CtlCheckBox:
		case CtlComboBox:
		case CtlDwgList:
		case CtlGraphicButton:
		case CtlGrid:
		case CtlImageComboBox:
		case CtlListBox:
		//case CtlListView:
		case CtlOptionList:
		case CtlOptionButton:
		//case CtlSlideView:
		case CtlTextBox:
		case CtlTabStrip:
			mpDlgControl->ApplyPropertiesEnum();
			mpDlgControl->GetControlWnd()->ShowWindow( SW_SHOW ); //make it visible even if the 'Prop::Visible' property is false
			mpDlgControl->GetControlWnd()->Invalidate();
			break;
		default:
			const TPropertyList& Props = mpTemplate->GetPropertyList();
			for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
				UpdateProperty( (*iter)->GetID() );
			break;
		}
	}
	Invalidate();
}

BOOL CControlHolder::OnEraseBkgnd(CDC* pDC)
{
	CRect rcThis;
	GetClientRect( &rcThis );
	pDC->FillSolidRect( &rcThis, GetSysColor( COLOR_GRAYTEXT ) );
	return TRUE;
}
