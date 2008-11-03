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
#include "BlockListCtrl.h"
#include "CheckBoxCtrl.h"
#include "ComboBoxCtrl.h"
#include "FolderComboCtrl.h"
#include "FrameCtrl.h"
#include "GraphicButtonCtrl.h"
#include "GridCtrl.h"
#include "HtmlCtrl.h"
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
#include "HyperlinkCtrl.h"

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
		if( !pDlgControl->GetControlWnd()->GetSafeHwnd() )
		{
			pDlgControl->GetControlWnd()->DestroyWindow();
			return NULL; //no window created for some reason
		}
		pDlgControl->GetControlWnd()->ModifyStyle( WS_CLIPSIBLINGS, 0 ); //can't have WS_CLIPSIBLINGS while editing in studio!
		pDlgControl->GetControlWnd()->ModifyStyleEx( 0, WS_EX_TRANSPARENT );
		CControlManager* pManager = new CControlManager( pDlgControl );
		pManager->OnControlPositionChanged();
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
	case CtlActiveX: return *new CAxContainerCtrl( pTemplate, pPane, nID );
	case CtlAngleSlider: return *new CAngleSlideCtrl( pTemplate, pPane, nID );
	case CtlAnimation:
		{	
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_ANIMATE );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlBlockList: return *new CBlockListCtrl( pTemplate, pPane, nID );
	case CtlBlockView:
		{
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_BLOCKVIEW );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlCalendar: return *new CMonthCtrl( pTemplate, pPane, nID );
	case CtlCheckBox: return *new CCheckBoxCtrl( pTemplate, pPane, nID );
	case CtlComboBox: return CreateComboControl( pTemplate, pPane, nID );
	case CtlDwgList: return *new CListBoxCtrl( pTemplate, pPane, nID );
	case CtlDwgPreview:
		{
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_DWGPREVIEW );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlFileExplorer:
		{
			DWORD dwStyle = WS_CHILD | WS_VISIBLE/* | WS_CLIPSIBLINGS*/;
			CFileDlgCtrls *pControl = new CFileDlgCtrls;
			pControl->Create( CString(), dwStyle, rc, pHostWnd, nID);
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlFrame: return *new CFrameCtrl( pTemplate, pPane, nID );
	case CtlGraphicButton: return *new CGraphicButtonCtrl( pTemplate, pPane, nID );
	case CtlGrid: return *new CGridCtrl( pTemplate, pPane, nID );
	case CtlHatch:
		{
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_HATCH );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlHtml: return *new CHtmlCtrl( pTemplate, pPane, nID );
	case CtlImageComboBox: return CreateComboExControl( pTemplate, pPane, nID );
	case CtlLabel: return *new CLabelCtrl( pTemplate, pPane, nID );
	case CtlListBox: return *new CListBoxCtrl( pTemplate, pPane, nID );
	case CtlListView: return *new CListViewCtrl( pTemplate, pPane, nID );
	case CtlOptionButton: return *new CRadioButtonCtrl( pTemplate, pPane, nID );
	case CtlOptionList: return *new COptionListCtrl( pTemplate, pPane, nID );
	case CtlPictureBox: return *new CPictureBoxCtrl( pTemplate, pPane, nID );
	case CtlProgressBar: return *new CProgressBarCtrl( pTemplate, pPane, nID );
	case CtlRectangle: return *new CRectangleCtrl( pTemplate, pPane, nID );
	case CtlScrollBar: return *new CScrollBarCtrl( pTemplate, pPane, nID );
	case CtlSlideView:
		{
			CPictureBox* pControl = new CPictureBox( pHostWnd, nID, rc, IDI_SLIDEVW );
			return new CAutoStudioDialogControl( pTemplate, pPane, pControl );
		}
	case CtlSpinButton: return *new CSpinnerCtrl( pTemplate, pPane, nID );
	case CtlSplitter: return *new CSplitterCtrl( pTemplate, pPane, nID );
	case CtlSlider: return *new CSlideCtrl( pTemplate, pPane, nID );
	case CtlTabStrip: return *new CTabStripCtrl( pTemplate, pPane, nID );
	case CtlTextBox: return CreateEditControl( pTemplate, pPane, nID );
	case CtlTextButton: return *new CTextButtonCtrl( pTemplate, pPane, nID );
	case CtlTree: return *new CImageTreeCtrl( pTemplate, pPane, nID );
	case CtlHyperlink: return *new CHyperlinkCtrl( pTemplate, pPane, nID );
	}

	return NULL;
}

//static
TDialogControlPtr CStudioDialogControl::CreateEditControl(TDclControlPtr pTemplate, CControlPane* pPane, UINT nID)
{
	// check the control type to determine which control to create
	switch( pTemplate->GetLongProperty( Prop::FilterStyle ) )
	{
	case TextBoxFilter::String: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CCustomFilter );
	case TextBoxFilter::Angle: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CAngleFilter );
	case TextBoxFilter::Integer: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CIntegerFilter );
	case TextBoxFilter::Numeric: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CNumericFilter );
	case TextBoxFilter::Symbol: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CSymbolNameFilter );
	case TextBoxFilter::UpperCase: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case TextBoxFilter::LowerCase: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CUpperCaseFilter );
	case TextBoxFilter::Password: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CPasswordFilter );
	case TextBoxFilter::Multiline: return *new CTextBoxCtrl( pTemplate, pPane, nID, new CMultilineFilter );
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
	pDlgControl->OnNeedRepaint( true, true );
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( pManager && pManager->IsSelected() )
		pManager->Invalidate();
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CStudioDialogControl::UpdateProperty( TDclControlPtr pTemplate, Prop::Id id )
{
	assert( pTemplate != NULL );
	if( !pTemplate )
		return;
	CDialogControl* pDlgControl = pTemplate->GetControlInstance();
	if( !pDlgControl )
		return;
	pDlgControl->OnApplyProperty( pTemplate->GetPropertyObject( id ) );
	CControlManager* pManager = pDlgControl->GetControlManager();
	if( pManager && pManager->IsSelected() )
		pManager->Invalidate();
}
