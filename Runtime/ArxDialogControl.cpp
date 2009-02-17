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
#include "ArxAcadSlideCtrl.h"
#include "ArxAnimationCtrl.h"
#include "ArxArrowComboBoxCtrl.h"
#include "ArxAngleSlideCtrl.h"
#include "ArxAxContainerCtrl.h"
#include "ArxBlockListCtrl.h"
#include "ArxBlockViewCtrl.h"
#include "ArxColorComboBoxCtrl.h"
#include "ArxDwgListCtrl.h"
#include "ArxDwgPreviewCtrl.h"
#include "ArxCheckBoxCtrl.h"
#include "ArxFolderComboCtrl.h"
#include "ArxFontComboBoxCtrl.h"
#include "ArxFrameCtrl.h"
#include "ArxGraphicButtonCtrl.h"
#include "ArxGridCtrl.h"
#include "ArxHatchCtrl.h"
#include "ArxHtmlCtrl.h"
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
#include "ArxHyperlinkCtrl.h"

#include "ComboStyles.h"
#include "TextBoxFilterTypes.h"
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
						(LPCTSTR)CString(pControl? pControl->GetRuntimeClass()->m_lpszClassName : "<null>"),
						pControl? pControl->m_hWnd : NULL, this );
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
	if( pDlgControl )
	{
		if( !pDlgControl->GetControlWnd()->GetSafeHwnd() )
		{
			pDlgControl->GetControlWnd()->DestroyWindow();
			return NULL; //no window created for some reason
		}
		//pDlgControl->GetControlWnd()->ModifyStyleEx( 0, WS_EX_TRANSPARENT );
	}
	return pDlgControl;
}

//static
TDialogControlPtr CArxDialogControl::CreateImp( TDclControlPtr pTemplate, CControlPane* pPane,
																								UINT nID, ControlParams* pParams /*= NULL*/ )
{
	// check the control type to determine which control to create
	switch(pTemplate->GetType())
	{
	case CtlActiveX: return *new CArxAxContainerCtrl( pTemplate, pPane, nID );
	case CtlAngleSlider: return *new CArxAngleSlideCtrl( pTemplate, pPane, nID );
	case CtlAnimation: return *new CArxAnimationCtrl( pTemplate, pPane, nID );
	case CtlBlockList: return *new CArxBlockListCtrl( pTemplate, pPane, nID );
	case CtlBlockView: return *new CArxBlockViewCtrl( pTemplate, pPane, nID );
	case CtlCalendar: return *new CArxMonthCtrl( pTemplate, pPane, nID );
	case CtlCheckBox: return *new CArxCheckBoxCtrl( pTemplate, pPane, nID );
	case CtlComboBox: return CreateComboControl(pTemplate, pPane, nID);
	case CtlDwgList: return *new CArxDwgListCtrl( pTemplate, pPane, nID );
	case CtlDwgPreview: return *new CArxDwgPreviewCtrl( pTemplate, pPane, nID );
	case CtlFrame: return *new CArxFrameCtrl( pTemplate, pPane, nID );
	case CtlGraphicButton: return *new CArxGraphicButtonCtrl( pTemplate, pPane, nID );
	case CtlGrid: return *new CArxGridCtrl( pTemplate, pPane, nID );
	case CtlHatch: return *new CArxHatchCtrl( pTemplate, pPane, nID );
	case CtlHtml: return *new CArxHtmlCtrl( pTemplate, pPane, nID );
	case CtlHyperlink: return *new CArxHyperlinkCtrl( pTemplate, pPane, nID );
	case CtlImageComboBox: return CreateComboExControl(pTemplate, pPane, nID);
	case CtlLabel: return *new CArxLabelCtrl( pTemplate, pPane, nID );
	case CtlListBox: return *new CArxListBoxCtrl( pTemplate, pPane, nID );
	case CtlListView: return *new CArxListViewCtrl( pTemplate, pPane, nID );
	case CtlOptionButton: return *new CArxRadioButtonCtrl( pTemplate, pPane, nID );
	case CtlOptionList: return *new CArxOptionListCtrl( pTemplate, pPane, nID );
	case CtlPictureBox: return *new CArxPictureBoxCtrl( pTemplate, pPane, nID );
	case CtlProgressBar: return *new CArxProgressBarCtrl( pTemplate, pPane, nID );
	case CtlRectangle: return *new CArxRectangleCtrl( pTemplate, pPane, nID );
	case CtlScrollBar: return *new CArxScrollBarCtrl( pTemplate, pPane, nID );
	case CtlSlideView: return *new CArxAcadSlideCtrl( *pPane, pTemplate, nID );
	case CtlSpinButton: return *new CArxSpinnerCtrl( pTemplate, pPane, nID );
	case CtlSplitter: return *new CArxSplitterCtrl( pTemplate, pPane, nID );
	case CtlSlider: return *new CArxSlideCtrl( pTemplate, pPane, nID );
	case CtlTabStrip: return *new CArxTabStripCtrl( pTemplate, pPane, nID );
	case CtlTextBox: return CreateEditControl(pTemplate, pPane, nID);
	case CtlTextButton: return *new CArxTextButtonCtrl( pTemplate, pPane, nID );
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
	case TextBoxFilter::String: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CCustomFilter, AC_ES_STRING );
	case TextBoxFilter::Angle: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CAngleFilter, (AC_ES_ANGLE | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case TextBoxFilter::Integer: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CIntegerFilter, (AC_ES_NUMERIC | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case TextBoxFilter::Numeric: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CNumericFilter, (AC_ES_NUMERIC | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case TextBoxFilter::Symbol: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CSymbolNameFilter, (AC_ES_SYMBOL | AC_ES_VAL_ONKILLFOCUS | AC_ES_CONV_ONKILLFOCUS) );
	case TextBoxFilter::UpperCase: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case TextBoxFilter::LowerCase: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case TextBoxFilter::Password: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CPasswordFilter );
	case TextBoxFilter::Multiline: return *new CArxTextBoxCtrl( pTemplate, pPane, nID, new CMultilineFilter );
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
	pDlgControl->OnApplyProperty( pTemplate->GetPropertyObject( id ) );
	pDlgControl->OnNeedRepaint();
}
