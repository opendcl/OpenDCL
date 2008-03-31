// ArxDialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ToolTips.h"
#include "ArxWorkspace.h"
#include "PropertyObject.h"

#include "ControlTypes.h"
#include "ArxArrowComboBoxCtrl.h"
#include "ArxAngleSlideCtrl.h"
#include "ArxAxContainerCtrl.h"
#include "ArxColorComboBoxCtrl.h"
#include "ArxDwgListCtrl.h"
#include "ArxCheckBoxCtrl.h"
#include "ArxFolderComboCtrl.h"
#include "ArxFontComboBoxCtrl.h"
#include "ArxFrameCtrl.h"
#include "ArxGraphicButtonCtrl.h"
#include "ArxGridCtrl.h"
#include "ArxImageComboBoxCtrl.h"
#include "ArxImageTreeCtrl.h"
#include "ArxLabelCtrl.h"
#include "ArxLayerComboBoxCtrl.h"
#include "ArxLinetypeComboBoxCtrl.h"
#include "ArxLineweightComboBoxCtrl.h"
#include "ArxListBoxCtrl.h"
#include "ArxListViewCtrl.h"
#include "ArxMonthCtrl.h"
#include "ArxOptionListCtrl.h"
#include "ArxPaperComboBoxCtrl.h"
#include "ArxPictureBoxCtrl.h"
#include "ArxPlotStyleNameComboBoxCtrl.h"
#include "ArxPlotStyleTableComboBoxCtrl.h"
#include "ArxPrinterComboBoxCtrl.h"
#include "ArxProgressBarCtrl.h"
#include "ArxRadioButtonCtrl.h"
#include "ArxRectangleCtrl.h"
#include "ArxScrollBarCtrl.h"
#include "ArxSlideCtrl.h"
#include "ArxSpinnerCtrl.h"
#include "ArxSplitterCtrl.h"
#include "ArxTabStripCtrl.h"
#include "ArxTextBoxCtrl.h"
#include "ArxTextButtonCtrl.h"
#include "ArxUrlLinkCtrl.h"

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

#include "GsPreviewCtrl.h"
#include "DwgPreviewCtrl.h"
#include "SlideHolder.h"
#include "HtmlCtrl.h"


static TDclControlPtr FindPaperCombo( TDclFormPtr pForm )
{
	if( !pForm )
		return NULL;
	TDclControlList listCombos;
	pForm->FindControls( CtlComboBox, listCombos );
	for( TDclControlList::iterator iter = listCombos.begin(); iter != listCombos.end(); ++iter )
	{
		if( (*iter)->GetLongProperty(Prop::ComboBoxStyle) == ComboStyle::PlotterPaperSizes )
			return *iter;
	}
	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CArxDialogControl

CArxDialogControl::CArxDialogControl( TDclControlPtr pTemplate, CControlPane* pPane,
																			CWnd* pControl )
: CDialogControl( pTemplate, pPane, pControl )
, mArxServices( this )
{
	TraceFmt( _T("> CArxDialogControl::CArxDialogControl(%s [%p], [%p], %s [HWND: %p]) [this: %p]\r\n"),
						(LPCTSTR)pTemplate->GetKeyPath(), (const CDclControlObject*)pTemplate, pPane,
						(LPCTSTR)CString(pControl->GetRuntimeClass()->m_lpszClassName),
						pControl->m_hWnd, this );
}

CArxDialogControl::~CArxDialogControl()
{
	TraceFmt( _T("< CArxDialogControl::~CArxDialogControl() [this: %p]\r\n"), this );
}

//static
TDialogControlPtr CArxDialogControl::Create( TDclControlPtr pTemplate, CControlPane* pPane,
																						 UINT nID, ControlParams* pParams /*= NULL*/ )
{
	TDialogControlPtr pDlgControl = CreateImp( pTemplate, pPane, nID, pParams );
	//if( pDlgControl )
	//{
	//	pDlgControl->GetControlWnd()->ModifyStyleEx( 0, WS_EX_TRANSPARENT );
	//}
	return pDlgControl;
}

//static
TDialogControlPtr CArxDialogControl::CreateImp( TDclControlPtr pTemplate, CControlPane* pPane,
																								UINT nID, ControlParams* pParams /*= NULL*/ )
{
	// check the control type to determine which control to create
	switch(pTemplate->GetType())
	{
	case CtlAnimate:
		{
			CAnimateCtrl *pControl = new CAnimateCtrl;
			CRect rc;
			// get the rectangle of the new control
			rc.top = pTemplate->GetPropertyObject(Prop::Top)->GetLongValue();
			rc.left = pTemplate->GetPropertyObject(Prop::Left)->GetLongValue();
			rc.bottom = pTemplate->GetPropertyObject(Prop::Height)->GetLongValue() + rc.top;
			rc.right = pTemplate->GetPropertyObject(Prop::Width)->GetLongValue() + rc.left;
			pControl->Create(
				WS_VISIBLE|WS_CHILD/*|WS_CLIPSIBLINGS*/ |
				ACS_CENTER |ACS_AUTOPLAY|ACS_TRANSPARENT, 
				rc, pPane->GetHostDialog(), nID);
			return new CAutoArxDialogControl( pTemplate, pPane, pControl );
		}

	case CtlImageComboBox: return CreateComboExControl(pTemplate, pPane, nID);
	case CtlSplitter: return *new CArxSplitterCtrl( pTemplate, pPane, nID );
	case CtlDwgList: return *new CArxDwgListCtrl( pTemplate, pPane, nID );
	case CtlOptionList: return *new CArxOptionListCtrl( pTemplate, pPane, nID );
	case CtlActiveX: return *new CArxAxContainerCtrl( pTemplate, pPane, nID );
	case CtlRectangle: return *new CArxRectangleCtrl( pTemplate, pPane, nID );

	case CtlHatch:
		{
			CGsPreviewCtrl *pControl = new CGsPreviewCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			return new CAutoArxDialogControl( pTemplate, pPane, pControl );
		}	
	
	case CtlBlockView:	
		{
			CGsPreviewCtrl *pControl = new CGsPreviewCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			return new CAutoArxDialogControl( pTemplate, pPane, pControl );
		}	
	
	case CtlGrid: return *new CArxGridCtrl( pTemplate, pPane, nID );
	case CtlListView: return *new CArxListViewCtrl( pTemplate, pPane, nID );
	case CtlBlockList: return *new CArxListViewCtrl( pTemplate, pPane, nID );
	case CtlCheckBox: return *new CArxCheckBoxCtrl( pTemplate, pPane, nID );
	case CtlComboBox: return CreateComboControl(pTemplate, pPane, nID);

	case CtlDwgPreview:
		{
			CDwgPreviewCtrl *pControl = new CDwgPreviewCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			return new CAutoArxDialogControl( pTemplate, pPane, pControl );
		}	
	
	case CtlFrame: return *new CArxFrameCtrl( pTemplate, pPane, nID );
	case CtlGraphicButton: return *new CArxGraphicButtonCtrl( pTemplate, pPane, nID );
	case CtlMonth: return *new CArxMonthCtrl( pTemplate, pPane, nID );
	case CtlLabel: return *new CArxLabelCtrl( pTemplate, pPane, nID );
	case CtlListBox: return *new CArxListBoxCtrl( pTemplate, pPane, nID );
	case CtlOptionButton: return *new CArxRadioButtonCtrl( pTemplate, pPane, nID );
	case CtlPictureBox: return *new CArxPictureBoxCtrl( pTemplate, pPane, nID );
	case CtlProgress: return *new CArxProgressBarCtrl( pTemplate, pPane, nID );
	case CtlAngleSlider: return *new CArxAngleSlideCtrl( pTemplate, pPane, nID );
	case CtlScrollBar: return *new CArxScrollBarCtrl( pTemplate, pPane, nID );
	case CtlSlider: return *new CArxSlideCtrl( pTemplate, pPane, nID );
	case CtlSlideView: return *new CSlideHolder( *pPane, pTemplate, nID );
	case CtlSpinButton: return *new CArxSpinnerCtrl( pTemplate, pPane, nID );
	case CtlUrlLink: return *new CArxUrlLinkCtrl( pTemplate, pPane, nID );

	case CtlHtmlCtrl:
		{
			CHtmlCtrl *pControl = new CHtmlCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			return new CAutoArxDialogControl( pTemplate, pPane, pControl );
		}

	case CtlStdButton: return *new CArxTextButtonCtrl( pTemplate, pPane, nID );
	case CtlTextBox: return CreateEditControl(pTemplate, pPane, nID);
	case CtlTabStrip: return *new CArxTabStripCtrl( pTemplate, pPane, nID );
	case CtlTree: return *new CArxImageTreeCtrl( pTemplate, pPane, nID );
	}
	return NULL;
}

//static
TDialogControlPtr CArxDialogControl::CreateEditControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	CWnd* pParentWnd = pPane->GetHostDialog();

	// check the control type to determine which control to create
	switch( pTemplate->GetLongProperty( Prop::FilterStyle ) )
	{
	case EditFilter::String: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CCustomFilter, AC_ES_STRING );
	case EditFilter::Angle: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CAngleFilter, (AC_ES_ANGLE | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case EditFilter::Integer: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CIntegerFilter, (AC_ES_NUMERIC | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case EditFilter::Numeric: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CNumericFilter, (AC_ES_NUMERIC | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case EditFilter::Symbol: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CSymbolNameFilter, (AC_ES_SYMBOL | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case EditFilter::UpperCase: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case EditFilter::LowerCase: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case EditFilter::Password: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CPasswordFilter );
	case EditFilter::Multiline: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CMultilineFilter );
	}
	return NULL;
}

//static
TDialogControlPtr CArxDialogControl::CreateComboExControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	return *new CArxImageComboBoxCtrl( pTemplate, pPane, nID );
}

//static
TDialogControlPtr CArxDialogControl::CreateComboControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	// check the control type to determine which control to create
	switch(pTemplate->GetLongProperty(Prop::ComboBoxStyle))
	{
	case ComboStyle::Combo: return *new CArxComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::Simple: return *new CArxComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::DropDown: return *new CArxComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::ArrowHead: return *new CArxArrowComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::Color: return *new CArxColorComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::LineWeight: return *new CArxLineweightComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::PlotNames: return *new CArxPlotStyleNameComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::PlotTables: return *new CArxPlotStyleTableComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::FontDropList: return *new CArxFontComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::FontSimpleList: return *new CArxFontComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::Plotters: return *new CArxPrinterComboBoxCtrl( pTemplate, pPane, nID, FindPaperCombo( pTemplate->GetOwnerForm() ) );
	case ComboStyle::PlotterPaperSizes: return *new CArxPaperComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::DirPicker: return *new CArxFolderComboCtrl( pTemplate, pPane, nID );
	case ComboStyle::Layers: return *new CArxLayerComboBoxCtrl( pTemplate, pPane, nID );
	case ComboStyle::Linetypes: return *new CArxLinetypeComboBoxCtrl( pTemplate, pPane, nID );
	}
	return NULL;
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateAllProperties( TDclControlPtr pTemplate )
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
	case CtlStdButton:
	case CtlTabStrip:
	case CtlTextBox:
	case CtlTree:
	case CtlUrlLink:
		pDlgControl->OnNeedRepaint();
		return;
	};
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
	pDlgControl->OnNeedRepaint();
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateProperty( TDclControlPtr pTemplate, Prop::Id id )
{
	assert( pTemplate != NULL );
	if( !pTemplate )
		return;
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
	case CtlStdButton:
	case CtlTabStrip:
	case CtlTextBox:
	case CtlTree:
	case CtlUrlLink:
		pDlgControl->OnApplyProperty( pTemplate->GetPropertyObject( id ) );
		pDlgControl->OnNeedRepaint();
		return;
	};
	UpdatePropertyInt( pTemplate, id );
	pDlgControl->OnNeedRepaint();
}

//static
void CArxDialogControl::UpdatePropertyInt(TDclControlPtr pControl, Prop::Id id)
{
	//I'm moving pPane logic into the child control's Create() function as I work on individual controls. Controls 
	//that don't have their own class I'll leave here for now. 2007-02-05 [ORW]
	TPropertyPtr pProp = pControl->GetPropertyObject( id );
	if( !pProp )
		return;
	CWnd* pControlWnd = pControl->GetWindow();

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
		{ //these properties are set in CArxDialogControl::ApplyPropertiesEnum()
			CDialogControl* pDlgControl = pControl->GetControlInstance();
			assert( pDlgControl != NULL );
			pDlgControl->OnApplyProperty( pControl->GetPropertyObject( id ) );
		}
		break;
		case Prop::BackgroundColor:
		{
		switch (pControl->GetType())
			{
			case CtlBlockView:
			case CtlHatch:
				((CGsPreviewCtrl*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;
			case CtlDwgPreview:
				((CDwgPreviewCtrl*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;
			}
		if (pControl->GetType() != CtlSlideView)
				pControlWnd->Invalidate();
			break;
			
		}

		case Prop::AllowOrbiting:
		{
			if (pControl->GetBooleanProperty(Prop::AllowOrbiting))
				((CGsPreviewCtrl*)pControlWnd)->AllowOrbiting(true);
			else
				((CGsPreviewCtrl*)pControlWnd)->AllowOrbiting(false);
			break;
		}
		
		case Prop::BlockName:
		{
			((CGsPreviewCtrl*)pControlWnd)->DisplayBlock(pControl->GetStringProperty(Prop::BlockName));
			break;
		}

		case Prop::DragnDropAllowDrop:
		{
			switch (pControl->GetType())
			{
			case CtlBlockView:
			case CtlHatch:
				((CGsPreviewCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlDwgPreview:
				((CDwgPreviewCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			}
			break;
		} 

		case Prop::RenderMode:
		{			
			((CGsPreviewCtrl*)pControlWnd)->SetRenderMode();
			((CGsPreviewCtrl*)pControlWnd)->Invalidate();
			break;
		}

		case Prop::BtnTTText:
		case Prop::ToolTipTitle:
		case Prop::ToolTipBalloon:
		case Prop::ToolTipLine:
		case Prop::ToolTipBody:
		case Prop::ToolTipPicture:
		case Prop::ToolTipAviFileName:
		case Prop::ToolTipTitleColor:
		{
			UpdateToolTip(pControl);
			break;
		}
	}	
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateToolTip(TDclControlPtr pControl)
{
	CString sToolTipTitle = pControl->GetStringProperty( Prop::ToolTipTitle );
	CWnd* pCtrl = pControl->GetWindow();
	switch( pControl->GetType() )
	{
	case CtlDwgPreview:
		SetToolTipEx(pCtrl, ((CDwgPreviewCtrl*)pCtrl)->m_ToolTip, pControl);
		break;
	case CtlHatch:
		SetToolTipEx(pCtrl, ((CGsPreviewCtrl*)pCtrl)->m_ToolTip, pControl);
		break;
	}
}
