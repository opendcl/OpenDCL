// StudioDialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "StudioDialogControl.h"
#include "ControlManager.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ToolTips.h"
#include "Workspace.h"
#include "PropertyObject.h"
#include "UndoManager.h"
#include "Resource.h"

#include "ControlTypes.h"
#include "AngleSlideCtrl.h"
#include "AxContainerCtrl.h"
#include "CheckBoxCtrl.h"
#include "ComboBoxCtrl.h"
#include "FolderComboCtrl.h"
#include "FrameCtrl.h"
#include "GraphicButtonCtrl.h"
#include "GridCtrl.h"
#include "ImageComboBoxCtrl.h"
#include "ImageTreeCtrl.h"
#include "LabelCtrl.h"
#include "ListBoxCtrl.h"
#include "ListViewCtrl.h"
#include "MonthCtrl.h"
#include "OptionListCtrl.h"
#include "PictureBoxCtrl.h"
#include "ProgressBarCtrl.h"
#include "RadioButtonCtrl.h"
#include "RectangleCtrl.h"
#include "ScrollBarCtrl.h"
#include "SlideCtrl.h"
#include "SpinnerCtrl.h"
#include "SplitterCtrl.h"
#include "TabStripCtrl.h"
#include "TextBoxCtrl.h"
#include "TextButtonCtrl.h"
#include "UrlLinkCtrl.h"

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

#include "PictureBox.h"
#include "FileDlgctrls.h"


#define nButtonFace -16
#define nButtonText -19


/////////////////////////////////////////////////////////////////////////////
// CStudioDialogControl

CStudioDialogControl::CStudioDialogControl( TDclControlPtr pTemplate, CControlPane* pPane,
																						CWnd* pControlWnd )
: CDialogControl( pTemplate, pPane, pControlWnd )
{
	TraceFmt( _T("> CStudioDialogControl::CStudioDialogControl(%s [%p], [%p], %s [HWND: %p]) [this: %p]\r\n"),
						(LPCTSTR)pTemplate->GetKeyPath(), (const CDclControlObject*)pTemplate, pPane,
						(LPCTSTR)CString(pControlWnd->GetRuntimeClass()->m_lpszClassName),
						pControlWnd->m_hWnd, this );
}

CStudioDialogControl::~CStudioDialogControl()
{
	TraceFmt( _T("< CStudioDialogControl::~CStudioDialogControl() [this: %p]\r\n"), this );
}

//static
TDialogControlPtr CStudioDialogControl::Create( TDclControlPtr pTemplate, CControlPane* pPane,
																								UINT nID, ControlParams* pParams /*= NULL*/ )
{
	TDialogControlPtr pDlgControl = CreateImp( pTemplate, pPane, nID, pParams );
	if( pDlgControl )
	{
		pDlgControl->GetControlWnd()->ModifyStyle( WS_CLIPSIBLINGS, 0 ); //can't have WS_CLIPSIBLINGS while editing in studio!
		new CControlManager( pDlgControl );
		if( pTemplate->GetType() == CtlActiveX )
			pDlgControl->GetActiveXCtrl()->ParseTypeLibInfo();
	}
	return pDlgControl;
}

//static
TDialogControlPtr CStudioDialogControl::CreateImp( TDclControlPtr pTemplate, CControlPane* pPane,
																									 UINT nID, ControlParams* pParams /*= NULL*/ )
{
	CRect rc( pTemplate->GetWndRect() );
	rc.MoveToXY( 0, 0 );
	CWnd* pHostWnd = pPane->GetHostDialog();
	switch( pTemplate->GetType() )
	{		
	case CtlAnimate:
		{	
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_ANIMATE );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}

	case CtlLabel: return *new CLabelCtrl( pTemplate, pPane, nID );
	case CtlStdButton: return *new CTextButtonCtrl( pTemplate, pPane, nID );
	case CtlGraphicButton: return *new CGraphicButtonCtrl( pTemplate, pPane, nID );
	case CtlFrame: return *new CFrameCtrl( pTemplate, pPane, nID );
	case CtlTextBox: return CreateEditControl( pTemplate, pPane, nID );
	case CtlCheckBox: return *new CCheckBoxCtrl( pTemplate, pPane, nID );
	case CtlOptionButton: return *new CRadioButtonCtrl( pTemplate, pPane, nID );
	case CtlComboBox: return CreateComboControl( pTemplate, pPane, nID );
	case CtlListBox: return *new CListBoxCtrl( pTemplate, pPane, nID );
	case CtlScrollBar: return *new CScrollBarCtrl( pTemplate, pPane, nID );
	case CtlSlider: return *new CSlideCtrl( pTemplate, pPane, nID );
	case CtlSpinButton: return *new CSpinnerCtrl( pTemplate, pPane, nID );
	case CtlPictureBox: return *new CPictureBoxCtrl( pTemplate, pPane, nID );
	case CtlTabStrip: return *new CTabStripCtrl( pTemplate, pPane, nID );
	case CtlMonth: return *new CMonthCtrl( pTemplate, pPane, nID );
	case CtlImageComboBox: return CreateComboExControl( pTemplate, pPane, nID );
	case CtlRectangle: return *new CRectangleCtrl( pTemplate, pPane, nID );
	case CtlProgress: return *new CProgressBarCtrl( pTemplate, pPane, nID );
	case CtlAngleSlider: return *new CAngleSlideCtrl( pTemplate, pPane, nID );
	case CtlUrlLink: return *new CUrlLinkCtrl( pTemplate, pPane, nID );

	case CtlBlockView:
		{
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_BLOCKVIEW );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlSlideView:
		{
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_SLIDEVW );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlTree: return *new CImageTreeCtrl( pTemplate, pPane, nID );
/*
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE | 
				TVS_DISABLEDRAGDROP | TVS_INFOTIP | WS_TABSTOP;
			if (pTemplate->GetBooleanProperty(Prop::ShowSelectAlways))
				dwStyle = dwStyle | TVS_SHOWSELALWAYS;
			if (pTemplate->GetBooleanProperty(Prop::HasLines))
				dwStyle = dwStyle | TVS_HASLINES;
			if (pTemplate->GetBooleanProperty(Prop::LinesAtRoot))
				dwStyle = dwStyle | TVS_LINESATROOT;
			if (pTemplate->GetBooleanProperty(Prop::HasButtons))
				dwStyle = dwStyle | TVS_HASBUTTONS;
			if (pTemplate->GetBooleanProperty(Prop::EditLabels))
				dwStyle = dwStyle | TVS_EDITLABELS;
			if (pTemplate->GetBooleanProperty(Prop::CheckBoxes))
				dwStyle = dwStyle | TVS_CHECKBOXES;
			CTreeCtrl *pControl = new CTreeCtrl;
			pControl->Create(dwStyle, rc, pHostWnd, nID);
			pControl->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
			CString sNodeText = theWorkspace.LoadResourceString(IDS_SAMPLENODE);
			HTREEITEM hti = pControl->InsertItem( TVIF_TEXT, sNodeText, 0, 0, TVIS_EXPANDEDONCE, TVIF_TEXT, 0, NULL, 0 );
			pControl->InsertItem( TVIF_TEXT, sNodeText, 0, 0, TVIS_EXPANDEDONCE, TVIF_TEXT, 0, hti, 0 );
			pControl->InsertItem( TVIF_TEXT, sNodeText, 0, 0, 0, TVIF_TEXT, 0, hti, 0 );
			pControl->InsertItem( TVIF_TEXT, sNodeText, 0, 0, 0, TVIF_TEXT, 0, NULL, 0 );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
*/
	case CtlHtmlCtrl:
		{
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_HTML );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlDwgPreview:
		{
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_DWGPREVIEW );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}

	case CtlGrid: return *new CGridCtrl( pTemplate, pPane, nID );
	case CtlListView: return *new CListViewCtrl( pTemplate, pPane, nID );
	case CtlBlockList: return *new CListViewCtrl( pTemplate, pPane, nID );
	case CtlOptionList: return *new COptionListCtrl( pTemplate, pPane, nID );
	case CtlActiveX: return *new CAxContainerCtrl( pTemplate, pPane, nID );
	case CtlDwgList: return *new CListBoxCtrl( pTemplate, pPane, nID );
	case CtlSplitter: return *new CSplitterCtrl( pTemplate, pPane, nID );

	case CtlHatch:
		{
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_HATCH );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlFileDlgCtrl:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE/* | WS_CLIPSIBLINGS*/;
			CFileDlgCtrls *pControl = new CFileDlgCtrls;
			pControl->Create( CString(), dwStyle, rc, pHostWnd, nID);
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	}
	return NULL;
}

//static
TDialogControlPtr CStudioDialogControl::CreateEditControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	// check the control type to determine which control to create
	switch( pTemplate->GetLongProperty( Prop::FilterStyle ) )
	{
	case EditFilter::String: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CCustomFilter );
	case EditFilter::Angle: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CAngleFilter );
	case EditFilter::Integer: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CIntegerFilter );
	case EditFilter::Numeric: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CNumericFilter );
	case EditFilter::Symbol: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CSymbolNameFilter );
	case EditFilter::UpperCase: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case EditFilter::LowerCase: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case EditFilter::Password: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CPasswordFilter );
	case EditFilter::Multiline: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CMultilineFilter );
	}
	return NULL;
}

//static
TDialogControlPtr CStudioDialogControl::CreateComboExControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	return *new CImageComboBoxCtrl( pTemplate, pPane, nID );
}

//static
TDialogControlPtr CStudioDialogControl::CreateComboControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	// check the control type to determine which control to create
	switch(pTemplate->GetLongProperty(Prop::ComboBoxStyle))
	{
	case ComboStyle::Combo: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::Simple: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::DropDown: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::ArrowHead: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::Color: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::LineWeight: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::PlotNames: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::PlotTables: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::FontDropList: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::FontSimpleList: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::Plotters: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::PlotterPaperSizes: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::DirPicker: return *new CFolderComboCtrl( pTemplate, pPane, nID );
	case ComboStyle::Layers: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::Linetypes: return *new CComboBoxCtrl( pTemplate, pPane, nID );
	}
	return NULL;
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CStudioDialogControl::UpdateAllProperties( TDclControlPtr pTemplate )
{
	CDialogControl* pDlgControl = pTemplate->GetControlInstance();
	if( !pDlgControl )
		return;
	pDlgControl->ApplyPropertiesEnum();

	switch( pTemplate->GetType() )
	{ //these controls implement the new CDialogControl interface, so use that
	case _CtlForm:
	case CtlActiveX:
	case CtlAngleSlider:
	case CtlBlockList:
	case CtlCheckBox:
	case CtlComboBox:
	case CtlDwgList:
	case CtlFrame:
	case CtlGraphicButton:
	case CtlGrid:
	case CtlImageComboBox:
	case CtlLabel:
	case CtlListBox:
	case CtlListView:
	case CtlMonth:
	case CtlOptionButton:
	case CtlOptionList:
	case CtlPictureBox:
	case CtlProgress:
	case CtlRectangle:
	case CtlScrollBar:
	case CtlSlider:
	case CtlSlideView:
	case CtlSpinButton:
	case CtlSplitter:
	case CtlStdButton:
	case CtlTabStrip:
	case CtlTextBox:
	case CtlTree:
	case CtlUrlLink:
		//pDlgControl->GetControlWnd()->Invalidate();
		break;
	default:
		{
			const TPropertyList& Props = pTemplate->GetPropertyList();
			for( TPropertyList::const_iterator iter = Props.begin(); iter != Props.end(); ++iter )
			{
				Prop::Id id = (*iter)->GetID();
				switch( id )
				{
				case Prop::ToolTipBalloon:
				case Prop::ToolTipLine:
				case Prop::ToolTipBody:
				case Prop::ToolTipPicture:
				case Prop::ToolTipAviFileName:
				case Prop::ToolTipTitleColor:
					break; //skip related properties that all get set at once with the main property of the group
				default:
					UpdateProperty( pTemplate, id );
				}
			}
			pDlgControl->GetControlWnd()->Invalidate();
		}
		break;
	};
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( pManager && pManager->IsSelected() )
		pManager->Invalidate();
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CStudioDialogControl::UpdateProperty( TDclControlPtr pTemplate, Prop::Id id )
{
	CDialogControl* pDlgControl = pTemplate->GetControlInstance();
	if( !pDlgControl )
		return;
	switch( pTemplate->GetType() )
	{ //these controls implement the new CDialogControl interface, so use that
	case _CtlForm:
	case CtlActiveX:
	case CtlAngleSlider:
	case CtlBlockList:
	case CtlCheckBox:
	case CtlComboBox:
	case CtlDwgList:
	case CtlFrame:
	case CtlGraphicButton:
	case CtlGrid:
	case CtlImageComboBox:
	case CtlLabel:
	case CtlListBox:
	case CtlListView:
	case CtlMonth:
	case CtlOptionButton:
	case CtlOptionList:
	case CtlPictureBox:
	case CtlProgress:
	case CtlRectangle:
	case CtlScrollBar:
	case CtlSlider:
	case CtlSlideView:
	case CtlSpinButton:
	case CtlSplitter:
	case CtlStdButton:
	case CtlTabStrip:
	case CtlTextBox:
	case CtlTree:
	case CtlUrlLink:
		pDlgControl->OnApplyProperty( pTemplate->GetPropertyObject( id ) );
		//pDlgControl->GetControlWnd()->Invalidate();
		break;
	default:
		UpdatePropertyInt( pTemplate, id );
		break;
	};
	//CWnd* pControlWnd = pTemplate->GetWindow();
	//if( pControlWnd )
	//	pControlWnd->Invalidate();
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( pManager && pManager->IsSelected() )
		pManager->Invalidate();
}

//static
void CStudioDialogControl::UpdatePropertyInt( TDclControlPtr pTemplate, Prop::Id id )
{
	//I'm moving pPane logic into the child control's Create() function as I work on individual controls. Controls 
	//that don't have their own class I'll leave here for now. 2007-02-05 [ORW]
	TPropertyPtr pProp = pTemplate->GetPropertyObject( id );
	if( !pProp )
		return;
	CWnd* pControlWnd = pTemplate->GetWindow();

	// set the appropriate property
	switch( id )
	{
		case Prop::Left:
		case Prop::Top:
		case Prop::Width:
		case Prop::Height:
		case Prop::LeftFromRight:
		case Prop::RightFromRight:
		case Prop::TopFromBottom:
		case Prop::BottomFromBottom:
		case Prop::BorderStyle:
		case Prop::Enabled:
		case Prop::HScrollBar:
		case Prop::TitleBarText:
		case Prop::Visible:
		{
			CDialogControl* pDlgControl = pTemplate->GetControlInstance();
			assert( pDlgControl != NULL );
			pDlgControl->OnApplyProperty( pTemplate->GetPropertyObject( id ) );
		}
		break;
		case Prop::BackgroundColor:
		{
		switch (pTemplate->GetType())
			{
			case CtlBlockView:
			case CtlHatch:
				((CPictureBoxCtrl*)pControlWnd)->GetColorService()->SetBackgroundColor(pTemplate->GetLongProperty(Prop::BackgroundColor));
				break;
			case CtlDwgPreview:
				((CPictureBoxCtrl*)pControlWnd)->GetColorService()->SetBackgroundColor(pTemplate->GetLongProperty(Prop::BackgroundColor));
				break;
			}
			break;
			
		}
	}	
}
