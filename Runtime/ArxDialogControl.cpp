// ArxDialogControl.cpp : implementation file
//

#include "stdafx.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "InvokeMethod.h"
#include "ToolTips.h"
#include "ArxWorkspace.h"

#include "ControlTypes.h"
#include "ArxAxContainerCtrl.h"
#include "ArxListBoxCtrl.h"
#include "ArxDwgListCtrl.h"
#include "ArxOptionListCtrl.h"
#include "ArxCheckBoxCtrl.h"
#include "ArxRadioButtonCtrl.h"
#include "VdclAngleEdit.h"
#include "OdclEdit.h"
#include "VdclNumericEdit.h"
#include "VdclSymbolEdit.h"
#include "VdclComboBoxEx.h"
#include "VdclComboBox.h"
#include "VdclArrowHeadComboBox.h"
#include "VdclColorComboBox.h"
#include "VdclLineWeightComboBox.h"
#include "VdclPlotStyleNamesComboBox.h"
#include "VdclPlotStyleTablesComboBox.h"
#include "FontCombo.h"
#include "PrinterComboBox.h"
#include "ComboBoxFolder.h"
#include "OdclLayerCombo.h"
#include "PropertyObject.h"
#include "Splitter.h"
#include "VdclStatic.h"
#include "GsPreviewCtrl.h"
#include "ArxGridCtrl.h"
#include "OdclListCtrl.h"
#include "VdclGroupBox.h"
#include "DwgPreviewCtrl.h"
#include "OdclMonth.h"
#include "PictureBox.h"
#include "ProgressTimeToComplete.h"
#include "RoundSliderCtrl.h"
#include "VdclSliderCtrl.h"
#include "VdclScrollBar.h"
#include "VdclSpinButton.h"
#include "SlideHolder.h"
#include "StaticLink.h"
#include "HtmlCtrl.h"
#include "ArxTabStripCtrl.h"
#include "VdclTree.h"
#include "VdclTextButton.h"
#include "TabPage.h"
#include "ArxGraphicButtonCtrl.h"


/////////////////////////////////////////////////////////////////////////////
// CArxDialogControl

CArxDialogControl::CArxDialogControl( CDclControlObject* pTemplate, CControlPane* pPane,
																			CWnd* pControl )
: CDialogControl( pTemplate, pPane, pControl )
, mArxServices( pTemplate )
{
	TraceFmt( _T("> CArxDialogControl::CArxDialogControl(%s [%p], [%p], %s [HWND: %p]) [this: %p]\r\n"),
						pTemplate->GetKeyPath(), pTemplate, pPane, CString(pControl->GetRuntimeClass()->m_lpszClassName),
						pControl->m_hWnd, this );
}

CArxDialogControl::~CArxDialogControl()
{
	TraceFmt( _T("< CArxDialogControl::~CArxDialogControl() [this: %p]\r\n"), this );
}

//static
TDialogControlPtr CArxDialogControl::Create( CDclControlObject* pTemplate, CControlPane* pPane,
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
				WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS |
				ACS_CENTER |ACS_AUTOPLAY|ACS_TRANSPARENT, 
				rc, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlImageComboBox:
		{			
			return CreateComboExControl(pTemplate, pPane, nID);
			break;
		}		
	case CtlSplitter:
		{
			CSplitter *pControl = new CSplitter;
			pControl->m_pControlPane = pPane;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);			
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlDwgList: return *new CArxDwgListCtrl( pTemplate, pPane, nID );
	case CtlOptionList: return *new CArxOptionListCtrl( pTemplate, pPane, nID );
	case CtlActiveX: return new CArxAxContainerCtrl( pTemplate, pPane, nID ); //ActiveX control must auto-destruct

	case Ctl3DRect:
		{
			VdclStatic *pControl = new VdclStatic;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);			
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlHatch:
		{
			CGsPreviewCtrl *pControl = new CGsPreviewCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
	
	case CtlBlockView:	
		{
			CGsPreviewCtrl *pControl = new CGsPreviewCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
	
	case CtlGrid: return *new CArxGridCtrl( pTemplate, pPane, nID );
	case CtlListView: return *new OdclListCtrl( *pPane, pTemplate, nID );
	case CtlBlockList: return *new OdclListCtrl( *pPane, pTemplate, nID );
	case CtlCheckBox: return *new CArxCheckBoxCtrl( pTemplate, pPane, nID );

	case CtlComboBox:
		{			
			return CreateComboControl(pTemplate, pPane, nID);
		}		
	case CtlDwgPreview:
		{
			CDwgPreviewCtrl *pControl = new CDwgPreviewCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
	
	case CtlFrame:
		{
			VdclGroupBox *pControl = new VdclGroupBox;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlGraphicButton: return *new CArxGraphicButtonCtrl( pTemplate, pPane, nID );
			
	case CtlMonth:
		{
			OdclMonth *pControl = new OdclMonth;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
	case CtlLabel:
		{
			VdclStatic *pControl = new VdclStatic;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);			
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// // ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}			

	case CtlListBox: return *new CArxListBoxCtrl( pTemplate, pPane, nID );
	case CtlOptionButton: return *new CArxRadioButtonCtrl( pTemplate, pPane, nID );

	case CtlPictureBox:
		{
			// create the picture box control
			CPictureBox *pControl = new CPictureBox;
			pControl->m_pParentPane = pPane;
			pControl->Create(pTemplate, pPane->GetProject(), pPane->GetHostDialog(), nID);	
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlProgress:
		{
			TProgressTimeToComplete *pControl = new TProgressTimeToComplete;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}			
		
	case CtlRoundSlider:
		{
			CRoundSliderCtrl *pControl = new CRoundSliderCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}			
	case CtlScrollBar:
		{
			VdclScrollBar *pControl = new VdclScrollBar;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}				

	case CtlSlider:
		{
			VdclSliderCtrl *pControl = new VdclSliderCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}

	case CtlSlideView: return *new CSlideHolder( *pPane, pTemplate, nID );
		
	case CtlSpinButton:		
		{
			VdclSpinButton *pControl = new VdclSpinButton;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlStaticURL:		
		{
			CStaticLink *pControl = new CStaticLink;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlHtmlCtrl:
		{
			CHtmlCtrl *pControl = new CHtmlCtrl;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlStdButton:
		{
			VdclTextButton *pControl = new VdclTextButton;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			// set the properties of the control
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlTextBox:
		{			
			return CreateEditControl(pTemplate, pPane, nID);
		}			
	
	case CtlTabStrip: return *new CArxTabStripCtrl( pTemplate, pPane, nID );

	case CtlTree:	
		{
			VdclTree *pControl = new VdclTree;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}			
	}
	return NULL;
}

//static
TDialogControlPtr CArxDialogControl::CreateEditControl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nID)
{
	CWnd* pParentWnd = pPane->GetHostDialog();

	// check the control type to determine which control to create
	switch(pTemplate->GetLongProperty(Prop::FilterStyle))
	{
	case EditFilter_Angle:
		{
			VdclAngleEdit *pControl = new VdclAngleEdit;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case EditFilter_Integer:
		{
			OdclEdit *pControl = new OdclEdit;
			pControl->Create(pTemplate, pParentWnd, nID);		
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case EditFilter_Numeric:
		{
			VdclNumericEdit *pControl = new VdclNumericEdit;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case EditFilter_Symbol:
		{
			VdclSymbolEdit *pControl = new VdclSymbolEdit;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	default:
		{
			OdclEdit *pControl = new OdclEdit;
			pControl->Create(pTemplate, pParentWnd, nID);	
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	}
	return NULL;
}

//static
TDialogControlPtr CArxDialogControl::CreateComboExControl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nID)
{
	CWnd* pParentWnd = pPane->GetHostDialog();

	VdclComboBoxEx *pControl = new VdclComboBoxEx;
	pControl->Create(pTemplate, pParentWnd, nID);
	UpdateChildControl(pControl, pTemplate, pPane, nID);
	// can't do ZOrderFront, it fucks up the display of the CComboBoxEx for some unknown reason.
	// // ZOrderFront(pControl);
	return new CArxAutoDialogControl( pTemplate, pPane, pControl );
}

//static
TDialogControlPtr CArxDialogControl::CreateComboControl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nID)
{
	CWnd* pParentWnd = pPane->GetHostDialog();

	// check the control type to determine which control to create
	switch(pTemplate->GetLongProperty(Prop::ComboBoxStyle))
	{
	case CmboStyle_Combo:
		{
			VdclComboBox *pControl = new VdclComboBox;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CmboStyle_Simple:
		{			
			VdclComboBox *pControl = new VdclComboBox;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CmboStyle_DropDown:
		{			
			VdclComboBox *pControl = new VdclComboBox;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CmboStyle_ArrowHead:
		{	
			VdclArrowHeadComboBox *pControl = new VdclArrowHeadComboBox;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CmboStyle_Color:
		{
			VdclColorComboBox *pControl = new VdclColorComboBox;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}			
	case CmboStyle_LineWeight:
		{			
			VdclLineWeightComboBox *pControl = new VdclLineWeightComboBox;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CmboStyle_PlotNames:
		{			
			VdclPlotStyleNamesComboBox *pControl = new VdclPlotStyleNamesComboBox;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CmboStyle_PlotTables:
		{			
			VdclPlotStyleTablesComboBox *pControl = new VdclPlotStyleTablesComboBox;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	
	case CmboStyle_FontDropList:
		{			
			CFontCombo *pControl = new CFontCombo;
			pControl->Create(pTemplate, pParentWnd, nID);
			// ZOrderFront(pControl);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	
	case CmboStyle_FontSimpleList:
		{			
			CFontCombo *pControl = new CFontCombo;
			pControl->Create(pTemplate, pParentWnd, nID);
			// ZOrderFront(pControl);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CmboStyle_Plotters: return *new CPrinterComboBox( *pPane, pTemplate, nID );
	case CmboStyle_PlotterPaperSizes:
		{
			VdclComboBox *pControl = new VdclComboBox;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CmboStyle_DirPicker:
		{
			CComboBoxFolder *pControl = new CComboBoxFolder;
			pControl->Create(pTemplate, pParentWnd, nID);
			int nDropHeight = pTemplate->GetLongProperty(Prop::DropDownHeight);
			if (nDropHeight < 300)
				nDropHeight = 300;
			pControl->Init(0, nDropHeight); 
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			SetDwgListComboFolderLink(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	
	case CmboStyle_Layers:
		{
			OdclLayerCombo *pControl = new OdclLayerCombo;
			pControl->Create(pTemplate, pParentWnd, nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	}
	return NULL;
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateChildControl( CWnd* pControlWnd, CDclControlObject* pTemplate, CControlPane* pPane, UINT nControlId )
{
	POSITION pos = pTemplate->GetPropertyList().GetHeadPosition();
	while( pos )
	{
		TPropertyPtr pProp = pTemplate->GetPropertyList().GetNext( pos );
		switch( pProp->GetID() )
		{
		case Prop::ToolTipBalloon:
		case Prop::ToolTipLine:
		case Prop::ToolTipBody:
		case Prop::ToolTipPicture:
		case Prop::ToolTipAviFileName:
		case Prop::ToolTipTitleColor:
			break; //skip related properties that all get set at once with the main property of the group
		default:
			UpdatePropertyInt( pControlWnd, pTemplate, pPane, pProp->GetID() );
		}
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateProperty(CDclControlObject *pControl, CControlPane* pPane, UINT nControlId, Prop::Id nID)
{
	switch( pControl->GetType() )
	{ //these controls implement the new CDialogControl interface, so use that
	case CtlActiveX:
	case CtlBlockList:
	case CtlCheckBox:
	case CtlDwgList:
	case CtlGraphicButton:
	case CtlGrid:
	case CtlListBox:
	case CtlListView:
	case CtlOptionButton:
	case CtlOptionList:
	case CtlSlideView:
	case CtlTabStrip:
		CDialogControl* pDlgControl = pControl->GetControlInstance();
		if( !pDlgControl )
			return;
		pDlgControl->OnApplyProperty( pControl->GetPropertyObject( nID ) );
		pDlgControl->GetControl()->Invalidate();
		return;
	};
	CWnd* pControlWnd = pControl->GetWindow();
	UpdatePropertyInt(pControlWnd, pControl, pPane, nID);
	pControlWnd->Invalidate();
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
bool CArxDialogControl::IsSelfPopulatedList(CDclControlObject *pControl)
{
	if (pControl->GetType() == CtlComboBox)
	{
		switch (pControl->GetLongProperty(Prop::ComboBoxStyle) )
		{
			case CmboStyle_FontDropList:
			case CmboStyle_FontSimpleList:
			case CmboStyle_ArrowHead:
			case CmboStyle_Color:
			case CmboStyle_LineWeight:
			case CmboStyle_PlotNames:
			case CmboStyle_PlotTables:
			case CmboStyle_Plotters:
			case CmboStyle_PlotterPaperSizes:
			case CmboStyle_DirPicker:	
			case CmboStyle_Layers:		
				return true;
				break;
		}
	}
	return false;
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::SetDwgListComboFolderLink(CComboBoxFolder *pComboFolder)
{
	POSITION pos = pComboFolder->m_ArxControl->GetOwnerForm()->GetControlList().GetHeadPosition();
	// increment to the next arx object so we will bypass the
	// arx object that holds the dialog box's properties.
	CDclControlObject* pControl = pComboFolder->m_ArxControl->GetOwnerForm()->GetControlList().GetNext(pos);
	while (pos != NULL)
	{
		pControl = pComboFolder->m_ArxControl->GetOwnerForm()->GetControlList().GetNext(pos);
		if (pControl->GetType() == CtlDwgList && pControl->GetWindow() != NULL)
		{
			CArxDwgListCtrl *pDwgList = (CArxDwgListCtrl*)pControl->GetWindow();
			pDwgList->m_pDirComboBox = pComboFolder;
			pComboFolder->m_pDwgList = pDwgList;		
		}		
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::SetDwgListComboFolderLink(CArxDwgListCtrl *pDwgList)
{
	POSITION pos = pDwgList->GetTemplate()->GetOwnerForm()->GetControlList().GetHeadPosition();
	// increment to the next arx object so we will bypass the
	// arx object that holds the dialog box's properties.
	CDclControlObject* pControl = pDwgList->GetTemplate()->GetOwnerForm()->GetControlList().GetNext(pos);
	while (pos != NULL)
	{
		pControl = pDwgList->GetTemplate()->GetOwnerForm()->GetControlList().GetNext(pos);
		if (pControl->GetType() == CtlComboBox && pControl->GetWindow() != NULL)
		{
			// check the control type to determine which control to create
			if (pControl->GetLongProperty(Prop::ComboBoxStyle) == CmboStyle_DirPicker)
			{				
				CComboBoxFolder *pComboFolder = (CComboBoxFolder*)pControl->GetWindow();
				pComboFolder->m_pDwgList = pDwgList;
				pDwgList->m_pDirComboBox = pComboFolder;			
			}
		}		
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::ResetImageList(CDclControlObject *pArxObject, CWnd *pControl, int nID)
{
	switch (pArxObject->GetType())
	{
		case CtlTree:
		{
			RefCountedPtr< CImageListObject > pImageList = pArxObject->GetImageList();
			if( pImageList )
			{
				pImageList->GetImageList().SetBkColor(((VdclTree*)pControl)->m_ChildTree.GetBkColor());
				((VdclTree*)pControl)->m_ChildTree.SetImageList(&pImageList->GetImageList(), TVSIL_NORMAL);
			}
			break;
		}
		case CtlImageComboBox:
		{	
			RefCountedPtr< CImageListObject > pImageList = pArxObject->GetImageList();
			if( pImageList )
				((VdclComboBoxEx*)pControl)->SetImageList(&pImageList->GetImageList());
			break;
		}
		//case CtlGrid:
		//{
		//	CArxGridCtrl* pListCtrl = (CArxGridCtrl*)pControl;
		//	RefCountedPtr< CImageListObject > pImageList = pArxObject->GetImageList();
		//	if (pImageList)
		//	{
		//		pImageList->m_ImageList.SetBkColor(::GetSysColor(COLOR_BTNFACE));
		//		pListCtrl->SetImageList(&pImageList->m_ImageList, TVSIL_NORMAL);
		//		pListCtrl->SetImageList(&pImageList->m_ImageList, LVSIL_SMALL);
		//	}
		//	else
		//	{
		//		if( !pListCtrl->mDefaultImageList.m_hImageList )
		//			pListCtrl->mDefaultImageList.Create(1, pArxObject->GetLongProperty(Prop::RowHeight), ILC_COLOR, 1, 1);
		//		pListCtrl->mDefaultImageList.SetBkColor(::GetSysColor(COLOR_WINDOW));
		//		pListCtrl->SetImageList(&pListCtrl->mDefaultImageList, TVSIL_NORMAL);
		//		pListCtrl->SetImageList(&pListCtrl->mDefaultImageList, LVSIL_SMALL);
		//	}
		//	break;
		//}
	}
			
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateFont(CDclControlObject *pArxObject, CWnd *pControl, CFont *pFont)
{
	switch (pArxObject->GetType())
	{
	case CtlLabel:
		{
		((VdclStatic*)pControl)->SetFont(pFont);
		break;
		}
	case CtlStdButton:
		{
		((VdclTextButton*)pControl)->SetFont(pFont);
		break;
		}
	case CtlImageComboBox:
		{
		((VdclComboBoxEx*)pControl)->SetFont(pFont);	
		break;
		}	
	//case CtlGraphicButton:
	//	{
	//	((CArxGraphicButtonCtrl*)pControl)->SetFont(pFont);
	//	break;
	//	}
	case CtlFrame:		
		{		
		((VdclGroupBox*)pControl)->m_Frame.SetFont(NULL);	
		((VdclGroupBox*)pControl)->m_Frame.SetFont(pFont);		
		break;
		}
	case CtlTextBox:		
		{		
		// check the control type to determine which control to create
		switch(pArxObject->GetLongProperty(Prop::FilterStyle))
		{
		case EditFilter_Angle:
			{
			// create the control
			((VdclAngleEdit *)pControl)->SetFont(pFont);
			break;
			}
		case EditFilter_Numeric:
			{
			// create the control
			((VdclNumericEdit *)pControl)->SetFont(pFont);
			break;
			}
		case EditFilter_Symbol:
			{
			// create the control
			((VdclSymbolEdit *)pControl)->SetFont(pFont);
			break;
			}
		default:
			{
			((OdclEdit *)pControl)->SetFont(pFont);
			break;
			}
		}
		break;
		}
	//case CtlCheckBox:		
	//	{		
	//	((VdclCheckBox*)pControl)->SetFont(pFont);		
	//	break;
	//	}
	//case CtlOptionButton:		
	//	{		
	//	((VdclRadioButton*)pControl)->SetFont(pFont);		
	//	break;
	//	}
	case CtlComboBox:
		{
			switch (pArxObject->GetLongProperty(Prop::ComboBoxStyle))
			{
			case CmboStyle_ArrowHead:
				{	
				((VdclArrowHeadComboBox*)pControl)->SetFont(pFont);	
				break;
				}
			case CmboStyle_Color:
				{
				((VdclColorComboBox*)pControl)->SetFont(pFont);	
				break;
				}			
			case CmboStyle_LineWeight:
				{			
				((VdclLineWeightComboBox*)pControl)->SetFont(pFont);	
				break;
				}
			case CmboStyle_PlotNames:
				{			
				((VdclPlotStyleNamesComboBox*)pControl)->SetFont(pFont);	
				break;
				}
			case CmboStyle_PlotTables:
				{	
				((VdclPlotStyleTablesComboBox*)pControl)->SetFont(pFont);	
				break;
				}
			case CmboStyle_Plotters:
				{	
				((CPrinterComboBox*)pControl)->SetFont(pFont);	
				break;
				}			
			default:
				{
				((VdclComboBox*)pControl)->SetFont(pFont);	
				break;
				}
			}
			break;
		}
	//case CtlListBox:		
	//	{		
	//	((CArxListBoxCtrl*)pControl)->SetFont(pFont);		
	//	break;
	//	}
	//case CtlDwgList:
	//	{		
	//	((CDwgDirList*)pControl)->SetFont(pFont);		
	//	break;
	//	}		
	//case CtlOptionList:		
	//	{		
	//	((COptionListBox*)pControl)->SetFont(pFont);		
	//	break;
	//	}		
	case CtlPictureBox:		
		{		
		((CPictureBox*)pControl)->SetFont(pFont);		
		break;
		}
	//case CtlTabStrip:		
	//	{		
	//	((CArxTabStripCtrl*)pControl)->GetTabCtrl().SetFont(NULL);	
	//	((CArxTabStripCtrl*)pControl)->GetTabCtrl().SetFont(pFont);		
	//	break;
	//	}
	case CtlMonth:
		{
		((OdclMonth*)pControl)->SetFont(pFont);
		break;
		}		
	case CtlTree:
		{
		((VdclTree*)pControl)->m_ChildTree.SetFont(pFont);
		break;
		}		
	case CtlStaticURL:
		{
		((CStaticLink*)pControl)->SetFont(pFont);
		break;
		}
	case CtlRoundSlider:
		{
		((CRoundSliderCtrl*)pControl)->m_pFont = pFont;
		pControl->RedrawWindow();
		break;
		}
	//case CtlListView:	
	//case CtlBlockList:
	//	{		
	//	((OdclListCtrl*)pControl)->SetFont(pFont);		
	//	break;
	//	}	
	//case CtlGrid:	
	//	{		
	//	((CArxGridCtrl*)pControl)->SetFont(pFont);		
	//	break;
	//	}	
	}
	pControl->Invalidate();
}

//static
void CArxDialogControl::UpdatePropertyInt(CWnd* pControlWnd, CDclControlObject *pControl, CControlPane* pPane, Prop::Id nID)
{
	//I'm moving pPane logic into the child control's Create() function as I work on individual controls. Controls 
	//that don't have their own class I'll leave here for now. 2007-02-05 [ORW]
	try
	{
	TPropertyPtr pProp = pControl->GetPropertyObject(nID);

	if (pProp == NULL)
		return;

	// set the appropriate property
	switch(nID)
	{
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
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;
			//case CtlSlideView:		
			//	((CSlideHolder*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
			//	break;
			//case CtlListView:		
			//case CtlBlockList:				
			//	((OdclListCtrl*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
			//	break;
				
			case CtlLabel:
				((VdclStatic*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;
				
			//case CtlOptionButton:
			//case CtlCheckBox:
			//	((CClrButton*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
			//	break;

			case CtlTextBox:
				switch (pControl->GetLongProperty(Prop::FilterStyle))
				{
				//case EditFilter_Multiline:
				case EditFilter_Symbol:
					break;
				default:
					((CColorEdit*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
					
					break;
				}				
				break;
			
			//case CtlListBox:
			//case CtlOptionList:
			//case CtlDwgList:
			//	((CClrListBox*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
			//	break;		
			
			case CtlStaticURL:
				((CStaticLink*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
				break;		

			//case CtlGraphicButton:
			//	((CArxGraphicButtonCtrl*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::BackgroundColor));
			//	break;
			
			}
		if (pControl->GetType() != CtlPictureBox && 
			pControl->GetType() != CtlSlideView)
				pControlWnd->Invalidate();
			break;
			
		}
		case Prop::ForegroundColor:
		{				
			switch (pControl->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControlWnd)->SetForeColor(pControl->GetLongProperty(Prop::ForegroundColor));
				break;		

			//case CtlOptionButton:
			//case CtlCheckBox:
			//	((CClrButton*)pControlWnd)->SetForeColor(pControl->GetLongProperty(Prop::ForegroundColor));
			//	break;
			//case CtlGraphicButton:
			//	((CArxGraphicButtonCtrl*)pControlWnd)->SetForeColor(pControl->GetLongProperty(Prop::ForegroundColor));
			//	break;
			case CtlTextBox:
				switch (pControl->GetLongProperty(Prop::FilterStyle))
				{
				//case EditFilter_Multiline:
				//	
				case EditFilter_Symbol:
					break;
				default:
					((CColorEdit*)pControlWnd)->SetForeColor(pControl->GetLongProperty(Prop::ForegroundColor));
					break;
				}				
				break;
			case CtlStaticURL:
				((CStaticLink*)pControlWnd)->SetForeColor(pControl->GetLongProperty(Prop::ForegroundColor));
				break;
			//case CtlListBox:
			//case CtlOptionList:
			//case CtlDwgList:
			//	((CClrListBox*)pControlWnd)->SetForeColor(pControl->GetLongProperty(Prop::ForegroundColor));
			//
			}
			
			pControlWnd->Invalidate();
			break;
		}	

		//case Prop::RowHeader:
		//{
		//	((CArxGridCtrl*)pControlWnd)->m_bHasRowHeader = pControl->GetBooleanProperty(Prop::RowHeader) == TRUE;
		//	break;
		//}

		case Prop::AllowOrbiting:
		{
			if (pControl->GetBooleanProperty(Prop::AllowOrbiting))
				((CGsPreviewCtrl*)pControlWnd)->AllowOrbiting(true);
			else
				((CGsPreviewCtrl*)pControlWnd)->AllowOrbiting(false);
			break;
		}
		case Prop::AutoSize:
		{
			switch (pControl->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->m_AutoSize = pControl->GetBooleanProperty(Prop::AutoSize);
				break;
			//case CtlGraphicButton:		
			//	((CBtnST*)pControlWnd)->SetAcadColor(pControl->GetLongProperty(Prop::AutoSize));
			//	break;
			}
			break;
		}
		
		case Prop::BlockName:
		{
			((CGsPreviewCtrl*)pControlWnd)->DisplayBlock(pControl->GetStrProperty(Prop::BlockName));
			break;
		}
		case Prop::BorderStyle:
		{
			switch (pControl->GetType())
			{
			case CtlTree:
				{
				switch(pControl->GetLongProperty(Prop::BorderStyle))
				{
				case 0:
					((VdclTree*)pControlWnd)->m_ChildTree.ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControlWnd)->m_ChildTree.ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControlWnd)->m_ChildTree.ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
					break;
						
				case 1:
					((VdclTree*)pControlWnd)->m_ChildTree.ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControlWnd)->m_ChildTree.ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControlWnd)->m_ChildTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
					break;

				case 2:
					((VdclTree*)pControlWnd)->m_ChildTree.ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControlWnd)->m_ChildTree.ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControlWnd)->m_ChildTree.ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_FRAMECHANGED);
					break;
				}
				
			
				break;
				}
			default:
				{
				switch(pControl->GetLongProperty(Prop::BorderStyle))
				{
				case 0:
					pControlWnd->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
					break;
						
				case 1:
					pControlWnd->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
					break;

				case 2:
					pControlWnd->ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_FRAMECHANGED);
					break;
				}
				
				switch (pControl->GetType())
				{
				case CtlPictureBox:
					((CPictureBox*)pControlWnd)->m_BorderStyle = pControl->GetLongProperty(Prop::BorderStyle);				
					break;
				}
				break;
				}
			}
			break;
		}	
		case Prop::Caption:
		{				
			UpdateText(pControl, pControlWnd, pControl->GetStrProperty(Prop::Caption));
			break;
		}	
		case Prop::ColumnWidth:
		{	
			int nNewColWidth = pControl->GetLongProperty(Prop::ColumnWidth);
			if (nNewColWidth > 0)
				((CListBox*)pControlWnd)->SetColumnWidth(nNewColWidth);
			break;
		}	
				
		//case Prop::DefSelIndex:
		//{
		//	int nDefSelection = pControl->GetLongProperty(Prop::DefSelIndex) ;
		//	((COptionListBox *)pControlWnd)->SetCurSel(nDefSelection);
		//	break;
		//}
		
		case Prop::DisableNoScroll:
		{
			if (pControl->GetBooleanProperty(Prop::DisableNoScroll) == FALSE)
				((CListBox*)pControlWnd)->ModifyStyle(LBS_DISABLENOSCROLL, 0, SWP_FRAMECHANGED);
			else
				((CListBox*)pControlWnd)->ModifyStyle(0, LBS_DISABLENOSCROLL, SWP_FRAMECHANGED);
			break;
		}

		case Prop::DragnDropAllowDrop:
		{
			switch (pControl->GetType())
			{
			case CtlStdButton:
				((VdclTextButton*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			//case CtlGraphicButton:
			//	((CArxGraphicButtonCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
			//	break;			
			//case CtlDwgList:
			//	((CDwgDirList*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
			//	break;			
			case CtlBlockView:
			case CtlHatch:
				((CGsPreviewCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlTextBox:
				switch (pControl->GetLongProperty(Prop::FilterStyle))
				{
				case EditFilter_Angle:
					((VdclAngleEdit*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
					break;
				case EditFilter_Numeric:
					((VdclNumericEdit*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
					break;
				case EditFilter_Symbol:
					((VdclSymbolEdit*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
					break;
				default:
					((OdclEdit*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
					break;
				}
				break;
			//case CtlListBox:
			//	((CArxListBoxCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlLabel:
				((VdclStatic*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			//case CtlSlideView:
			//	((CSlideHolder*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
			//	break;
			case CtlTree:
				((VdclTree*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			case CtlDwgPreview:
				((CDwgPreviewCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
				break;
			//case CtlListView:
			//case CtlBlockList:
			//	((OdclListCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBooleanProperty(Prop::DragnDropAllowDrop));
			//	break;
			}
			break;
		} 

		case Prop::Enabled:
		{
			if (pControl->GetType() == CtlSlider)
				((CSliderCtrl *)pControlWnd)->EnableWindow(pControl->GetBooleanProperty(Prop::Enabled));
			else if (pControl->GetType() == CtlTree)
				((VdclTree *)pControlWnd)->m_ChildTree.EnableWindow(pControl->GetBooleanProperty(Prop::Enabled));
			//else if (pControl->GetType() == CtlTabStrip)
			//	((CArxTabStripCtrl *)pControlWnd)->GetTabCtrl().EnableWindow(pControl->GetBooleanProperty(Prop::Enabled));
			//else if (pControl->GetType() == CtlOptionList)
			//{
			//	int nData=0;
			//	if (pControl->GetBooleanProperty(Prop::Enabled) == FALSE)
			//		nData = 2;
			//	for (int i=0; i<((COptionListBox*) pControlWnd)->GetCount(); i++)
			//	{
			//		((COptionListBox*) pControlWnd)->SetItemData(i, nData);
			//	}
			//	pControlWnd->EnableWindow(pControl->GetBooleanProperty(Prop::Enabled));
			//	pControlWnd->Invalidate();				
			//}			
			else
			{
				pControlWnd->EnableWindow(pControl->GetBooleanProperty(Prop::Enabled) != FALSE);
				
				if (pControl->GetType() == CtlLabel)
				{
					((VdclStatic*)pControlWnd)->EnableWindow(pControl->GetBooleanProperty(Prop::Enabled) != FALSE);
					((VdclStatic*)pControlWnd)->Invalidate();				
				}				
			}
			break;				
		}	
		case Prop::FilterStyle:
			{
			switch (pControl->GetLongProperty(Prop::FilterStyle))
			{
			case 5:/*Upper case*/
				{
				((CEdit*)pControlWnd)->ModifyStyle(ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CEdit*)pControlWnd)->ModifyStyle(ES_LOWERCASE, ES_UPPERCASE, SWP_FRAMECHANGED);
				((CEdit*)pControlWnd)->SetPasswordChar(0);
				break;
				}
			case 6:/*lower case*/
				{
				((CEdit*)pControlWnd)->ModifyStyle(ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CEdit*)pControlWnd)->ModifyStyle(ES_UPPERCASE, ES_LOWERCASE, SWP_FRAMECHANGED);
				((CEdit*)pControlWnd)->SetPasswordChar(0);
				break;
				}
			case 7:/*password*/
				{
				((CEdit*)pControlWnd)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE, ES_PASSWORD, SWP_FRAMECHANGED);
				((CEdit*)pControlWnd)->SetPasswordChar('*');
				break;
				}
			case 8:/*MultiLine*/
				{
				((CEdit*)pControlWnd)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE|ES_PASSWORD, ES_MULTILINE, SWP_FRAMECHANGED);
				((CEdit*)pControlWnd)->SetPasswordChar(0);
				break;
				}
			default:
				{
				((CEdit*)pControlWnd)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE|ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CEdit*)pControlWnd)->SetPasswordChar(0);
				break;
				}
			}
			break;
		}
	
		case Prop::Indent:
		{
			((VdclTree*)pControlWnd)->m_ChildTree.SetIndent(pControl->GetLongProperty(Prop::Indent));
			break;
		}

		case Prop::ImageList:
		{
			ResetImageList(pControl, pControlWnd, nID);
			break;
		}

		case Prop::Justification:
		{
			switch (pControl->GetType())
			{
			case CtlTextBox:
				{
				/*
					((CEdit*)pControlWnd)->ModifyStyle(ES_RIGHT, 0, SWP_FRAMECHANGED);
					((CEdit*)pControlWnd)->ModifyStyle(ES_CENTER, 0, SWP_FRAMECHANGED);
					((CEdit*)pControlWnd)->ModifyStyle(ES_LEFT, 0, SWP_FRAMECHANGED);
					
					switch (pControl->GetLongProperty(Prop::Justification))
					{
					case 0:// Left
						((CEdit*)pControlWnd)->ModifyStyle(0, ES_LEFT, SWP_FRAMECHANGED);
						break;
					case 1:// Center
						((CEdit*)pControlWnd)->ModifyStyle(0, ES_CENTER, SWP_FRAMECHANGED);
						break;
					case 2:// Right
						((CEdit*)pControlWnd)->ModifyStyle(0, ES_RIGHT, SWP_FRAMECHANGED);
						break;
					}
					*/
					break;
				}
			case CtlLabel:
				{
					pControlWnd->ModifyStyle(SS_RIGHT, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyle(SS_CENTER, 0, SWP_FRAMECHANGED);
					pControlWnd->ModifyStyle(SS_LEFT, 0, SWP_FRAMECHANGED);
			
					switch (pControl->GetLongProperty(Prop::Justification))
					{
					case 0:/*Left*/
						pControlWnd->ModifyStyle(0, SS_LEFT, SWP_FRAMECHANGED);
						break;
					case 1:/*Center*/
						pControlWnd->ModifyStyle(0, SS_CENTER, SWP_FRAMECHANGED);
						break;
					case 2:/*Right*/
						pControlWnd->ModifyStyle(0, SS_RIGHT, SWP_FRAMECHANGED);
						break;
					}					
					pControlWnd->Invalidate();
					break;
				}
			}
			break;
		}

		case Prop::LabelName:
		{
			const CFont *pFont = theWorkspace.GetFontCollection().GetFont(pControl, pControlWnd);

			UpdateFont(pControl, pControlWnd, const_cast<CFont*>(pFont));
			break;
		}

		case Prop::LimitText:
		{
			switch (pControl->GetType())
			{
			case CtlTextBox:
				{
				((CEdit*)pControlWnd)->SetLimitText(pControl->GetLongProperty(Prop::LimitText));
				break;
				}
			case CtlComboBox:
				{
				((CComboBox*)pControlWnd)->LimitText(pControl->GetLongProperty(Prop::LimitText));
				break;				
				}
			case CtlImageComboBox:
				{
				((CComboBoxEx*)pControlWnd)->LimitText(pControl->GetLongProperty(Prop::LimitText));
				break;				
				}
			}
			break;
		}
		
		case Prop::List:
		{
			switch (pControl->GetType())
			{
			//case CtlListBox:
			//	{
			//		CString sListItem;
			//		((CListBox *)pControlWnd)->ResetContent();					
			//		for (size_t i = 0; i < pProp->size(); i++)
			//		{				
			//			sListItem = pProp->GetStringItem(i);
			//			if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
			//				((CListBox *)pControlWnd)->AddString(sListItem);
			//		}
			//		break;
			//	}
			case CtlComboBox:
				{
					if (!IsSelfPopulatedList(pControl))
					{
						CString sListItem;
						((CComboBox *)pControlWnd)->ResetContent();
						for (size_t i = 0; i < pProp->size(); i++)
						{			
							sListItem = pProp->GetStringItem(i);
							if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
								((CComboBox *)pControlWnd)->AddString(pProp->GetStringItem(i));
						}
					}
					break;
				}
			}
			break;				
		}
		case Prop::MarginLeft:
		{
			((CEdit*)pControlWnd)->SetMargins(
				pControl->GetLongProperty(Prop::MarginLeft),
				pControl->GetLongProperty(Prop::MarginRight));
			break;
		}
		
		//case Prop::MinTabWidth:
		//{
		//	int n = pControl->GetLongProperty(Prop::MinTabWidth);
		//	((CArxTabStripCtrl*)pControlWnd)->GetTabCtrl().SetMinTabWidth(pControl->GetLongProperty(Prop::MinTabWidth));
		//	((CArxTabStripCtrl*)pControlWnd)->GetTabCtrl().RedrawWindow(NULL, NULL, RDW_UPDATENOW);
		//	break;
		//}
		case Prop::MultiSelection:
		{
			((OdclMonth*)pControlWnd)->SetMaxSelCount(
				pControl->GetLongProperty(Prop::MultiSelection));
			break;
		}
		case Prop::MaxValue:
		case Prop::MinValue:
		{
			switch (pControl->GetType())
			{
			case CtlProgress:
				((CProgressCtrl*)pControlWnd)->SetRange(
					(int)pControl->GetLongProperty(Prop::MinValue),
					(int)pControl->GetLongProperty(Prop::MaxValue));
				break;				
			case CtlScrollBar:
				((CScrollBar*)pControlWnd)->SetScrollRange(
					(int)pControl->GetLongProperty(Prop::MinValue),
					(int)pControl->GetLongProperty(Prop::MaxValue),
					TRUE);
				break;				
			case CtlSlider:
				((CSliderCtrl*)pControlWnd)->SetRange(
					(int)pControl->GetLongProperty(Prop::MinValue),
					(int)pControl->GetLongProperty(Prop::MaxValue));
			
			}
			break;
		}	
		
		case Prop::LargeChange:	
		{
			switch (pControl->GetType())
			{
			case CtlScrollBar:				
				((VdclScrollBar*)pControlWnd)->m_nLargeChange = pControl->GetLongProperty(Prop::LargeChange);
				break;
			case CtlSlider:				
				((VdclSliderCtrl*)pControlWnd)->SetPageSize(pControl->GetLongProperty(Prop::LargeChange));
				break;
			}
			break;
			
		}
		case Prop::Orientation:
		{
			switch(pControl->GetLongProperty(Prop::Orientation))
			{
			case 0:				
				((CSliderCtrl *)pControlWnd)->ModifyStyle(TBS_VERT, 0, SWP_FRAMECHANGED);
				((CSliderCtrl *)pControlWnd)->ModifyStyle(0, TBS_HORZ, SWP_FRAMECHANGED);
				break;
			case 1:
				((CSliderCtrl *)pControlWnd)->ModifyStyle(TBS_HORZ, 0, SWP_FRAMECHANGED);
				((CSliderCtrl *)pControlWnd)->ModifyStyle(0, TBS_VERT, SWP_FRAMECHANGED);
				break;
			}
			break;
		}
		case Prop::Picture:
		{
			switch (pControl->GetType())
			{
			//case CtlGraphicButton:		
			//	((CArxGraphicButtonCtrl *)pControlWnd)->SetPictureID(pControl->GetLongProperty(Prop::Picture));
			//	((CArxGraphicButtonCtrl *)pControlWnd)->SetPressedPictureID(pControl->GetLongProperty(Prop::PressedPicture));
			//	pControlWnd->Invalidate();
			//	break;
			case CtlPictureBox:		
				((CPictureBox *)pControlWnd)->SetPictureID(pControl->GetLongProperty(Prop::Picture));
				break;
			}		
			break;
		}

		case Prop::ReadOnly:
		{
			((CEdit*)pControlWnd)->SetReadOnly(pControl->GetBooleanProperty(Prop::ReadOnly));
			break;
		}
		case Prop::RenderMode:
		{			
			((CGsPreviewCtrl*)pControlWnd)->SetRenderMode();
			((CGsPreviewCtrl*)pControlWnd)->Invalidate();
			break;
		}
		
		//case Prop::RowHeight:
		//{			
		//	if (pControl->GetType() == CtlDwgList)
		//	{
		//		CDwgDirList* pDwgList = (CDwgDirList*)pControlWnd;
		//		pDwgList->m_RowHeight = pControl->GetLongProperty(Prop::RowHeight);
		//		int nCurSel = pDwgList->GetCurSel();
		//		pDwgList->Dir(pDwgList->m_sPath);
		//		pDwgList->SetCurSel(nCurSel);
		//	}
		//	break;
		//}
		
		case Prop::ShowTicks:
		{
			if (pControl->GetBooleanProperty(Prop::ShowTicks) == FALSE)
				((CSliderCtrl *)pControlWnd)->ModifyStyle(TBS_AUTOTICKS, 0, SWP_FRAMECHANGED);
			else
				((CSliderCtrl *)pControlWnd)->ModifyStyle(0, TBS_AUTOTICKS, SWP_FRAMECHANGED);
				
			break;
		}
		case Prop::SmallChange:	
		{
			switch (pControl->GetType())
			{
			case CtlScrollBar:				
				((VdclScrollBar*)pControlWnd)->m_nSmallChange = pControl->GetLongProperty(Prop::SmallChange);
				break;
			case CtlSlider:				
				((VdclSliderCtrl*)pControlWnd)->SetLineSize(pControl->GetLongProperty(Prop::SmallChange));
				break;
			}
			break;
			
		}
		//case Prop::TabSelected:
		//{
		//	((CListBox*)pControlWnd)->SetCurSel(pControl->GetLongProperty(Prop::TabSelected));
		//	break;
		//}
		//case Prop::TabStyle:
		//{
		//	if (pControl->GetLongProperty(Prop::TabStyle) == 0)
		//		((CListBox*)pControlWnd)->ModifyStyle(TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED);
		//	else
		//		((CListBox*)pControlWnd)->ModifyStyle(TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED);
		//	break;
		//}
		//
		//case Prop::TabLabelAlign:
		//{
		//	if (pControl->GetLongProperty(Prop::TabLabelAlign) == 0)
		//		((CArxTabStripCtrl*)pControlWnd)->GetTabCtrl().ModifyStyle(0, TCS_FORCELABELLEFT, SWP_FRAMECHANGED);
		//	else
		//		((CArxTabStripCtrl*)pControlWnd)->GetTabCtrl().ModifyStyle(TCS_FORCELABELLEFT, 0, SWP_FRAMECHANGED);
		//	break;
		//}
		//case Prop::TabFixedWidth:
		//{
		//	if (pControl->GetBooleanProperty(Prop::TabFixedWidth) == TRUE)
		//	{
		//		((CArxTabStripCtrl*)pControlWnd)->GetTabCtrl().ModifyStyle(0, TCS_FIXEDWIDTH, SWP_FRAMECHANGED);
		//		CRect rc;
		//		((CArxTabStripCtrl*)pControlWnd)->GetTabCtrl().GetItemRect(0, &rc);
		//		CSize szTabs;
		//		szTabs.cx = pControl->GetLongProperty(Prop::MinTabWidth);
		//		szTabs.cy = rc.Height();
		//		((CArxTabStripCtrl*)pControlWnd)->GetTabCtrl().SetItemSize(szTabs);
		//	}
		//	else
		//		((CArxTabStripCtrl*)pControlWnd)->GetTabCtrl().ModifyStyle(TCS_FIXEDWIDTH, 0, SWP_FRAMECHANGED);
		//	break;
		//}
		case Prop::Text:
		{
			if (!IsSelfPopulatedList(pControl))
			{							
				int nTextLimit = pControl->GetLongProperty(Prop::LimitText);
				CString sNewText = pControl->GetStrProperty(Prop::Text);

				if (nTextLimit > -1) 
					sNewText = sNewText.Left(nTextLimit);

				UpdateText(pControl, pControlWnd, sNewText);
			}
			break;
		}
		case Prop::TickFrequency:
		{
			((CSliderCtrl *)pControlWnd)->SetTicFreq(pControl->GetLongProperty(Prop::TickFrequency));
			
			break;
		}
		
		case Prop::TitleBarText:
		{
			pControlWnd->SetWindowText(pControl->GetStrProperty(Prop::TitleBarText));
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
			UpdateToolTip(pControl, pControlWnd);
			break;
		}
		case Prop::UseTabStops:
		{
			if (pControl->GetBooleanProperty(Prop::UseTabStops) == FALSE)
				((CListBox*)pControlWnd)->ModifyStyle(LBS_USETABSTOPS, 0, SWP_FRAMECHANGED);
			else
				((CListBox*)pControlWnd)->ModifyStyle(0, LBS_USETABSTOPS, SWP_FRAMECHANGED);
			break;
		}
		case Prop::URLAddress:
		{
			if (pControl->GetType() == CtlStaticURL)
				((CStaticLink*)pControlWnd)->m_link = pControl->GetStrProperty(Prop::URLAddress);
			break;
		}
		case Prop::Visible:
		{
			if (IsWindow(pControlWnd->m_hWnd))
				pControlWnd->ShowWindow(pControl->GetPropertyObject(Prop::Visible)->GetBooleanValue()? SW_SHOW : SW_HIDE);
			else
				theWorkspace.DisplayAlert(_T("The ActiveX Control is hidden and no longer valid.\nPlease contact the control developer for support."));
			break;
		}
		case Prop::Value:
		{
			switch (pControl->GetType())
			{
			case CtlCheckBox:
			case CtlOptionButton:
				((CButton *)pControlWnd)->SetCheck(pControl->GetBooleanProperty(Prop::Value));
				pControlWnd->RedrawWindow();
				break;									
			case CtlRoundSlider:
				((CRoundSliderCtrl *)pControlWnd)->SetPos(pControl->GetLongProperty(Prop::Value));
				pControlWnd->RedrawWindow();
				break;
			case CtlSlider:
				((CSliderCtrl*)pControlWnd)->SetPos(pControl->GetLongProperty(Prop::Value));
				pControlWnd->RedrawWindow();
				break;
			case CtlScrollBar:
				{
					((VdclScrollBar *)pControlWnd)->m_hPos = pControl->GetLongProperty(Prop::Value);
					((VdclScrollBar *)pControlWnd)->SetScrollPos(pControl->GetLongProperty(Prop::Value), TRUE);
					pControlWnd->RedrawWindow();
					break;
				}
			case CtlProgress:
				((CProgressCtrl*)pControlWnd)->SetPos(pControl->GetLongProperty(Prop::Value));
				pControlWnd->RedrawWindow();
				break;
			case CtlSpinButton:
				((VdclSpinButton*)pControlWnd)->m_Pos = pControl->GetLongProperty(Prop::Value);
				((VdclSpinButton*)pControlWnd)->SetPos(((VdclSpinButton*)pControlWnd)->m_Pos);				
				break;			
			}
			break;
		}
		case Prop::HScrollBar:
		{
			if (pControl->GetBooleanProperty(Prop::HScrollBar) == FALSE)
				pControlWnd->ModifyStyle(WS_HSCROLL, 0, SWP_FRAMECHANGED);
			else
				pControlWnd->ModifyStyle(0, WS_HSCROLL, SWP_FRAMECHANGED);
			break;
		}
		case Prop::VScrollBar:
		{
			switch (pControl->GetType())
			{
			case CtlTextBox:
				{			
				if (pControl->GetBooleanProperty(Prop::VScrollBar) == FALSE)
					((OdclEdit*)pControlWnd)->ModifyStyle(WS_VSCROLL, 0, SWP_FRAMECHANGED);
				else
					((OdclEdit*)pControlWnd)->ModifyStyle(0, WS_VSCROLL, SWP_FRAMECHANGED);
				break;
				}
			//case CtlListBox:
			//	{			
			//	if (pControl->GetBooleanProperty(Prop::VScrollBar) == FALSE)
			//		((CListBox*)pControlWnd)->ModifyStyle(WS_VSCROLL, 0, SWP_FRAMECHANGED);
			//	else
			//		((CListBox*)pControlWnd)->ModifyStyle(0, WS_VSCROLL, SWP_FRAMECHANGED);
			//	break;
			//	}
			}
			break;
		}
	}	
	}
	catch(...)
	{
		int n = 0;
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateText(CDclControlObject *pTemplate, CWnd *pControl, CString sText)
{
	try
	{
		switch (pTemplate->GetType())
		{
		case CtlLabel:
			{
			((VdclStatic*)pControl)->SetWindowText(sText);
			break;
			}
		case CtlStdButton:
			{
			((VdclTextButton*)pControl)->SetWindowText(sText);
			break;
			}
		case CtlImageComboBox:
			{
			((VdclComboBoxEx*)pControl)->SetWindowText(sText);	
			break;
			}
		//case CtlGraphicButton:
		//	{
		//	CString sOldText;
		//	((CArxGraphicButtonCtrl*)pControl)->GetWindowText(sOldText);
		//	if (sOldText != sText)
		//	{
		//		((CArxGraphicButtonCtrl*)pControl)->SetWindowText(sText);
		//		((CArxGraphicButtonCtrl*)pControl)->Invalidate();
		//	}
		//	break;
		//	}
		case CtlFrame:		
			{		
			((VdclGroupBox*)pControl)->m_Frame.SetWindowText(sText);	
			break;
			}
		case CtlTextBox:		
			{		
			// check the control type to determine which control to create
			switch(pTemplate->GetLongProperty(Prop::FilterStyle))
			{
			case EditFilter_Angle:
				{
				// create the control
				((VdclAngleEdit *)pControl)->SetWindowText(sText);
				break;
				}
			case EditFilter_Numeric:
				{
				// create the control
				((VdclNumericEdit *)pControl)->SetWindowText(sText);
				break;
				}
			case EditFilter_Symbol:
				{
				// create the control
				((VdclSymbolEdit *)pControl)->SetWindowText(sText);
				break;
				}
			default:
				{
				((OdclEdit*)pControl)->SetWindowText(sText);
				break;
				}
			}
			break;
			}
		//case CtlCheckBox:		
		//	{		
		//	((VdclCheckBox*)pControl)->SetWindowText(sText);		
		//	break;
		//	}
		//case CtlOptionButton:		
		//	{		
		//	((VdclRadioButton*)pControl)->SetWindowText(sText);		
		//	break;
		//	}
		case CtlComboBox:
			{
				switch (pTemplate->GetLongProperty(Prop::ComboBoxStyle))
				{
				case CmboStyle_ArrowHead:
					{	
					((VdclArrowHeadComboBox*)pControl)->SetWindowText(sText);	
					if (((CComboBox*)pControl)->GetCount() > 0)
						((CComboBox*)pControl)->SetCurSel(0);					
					break;
					}
				case CmboStyle_Color:
					{
					((VdclColorComboBox*)pControl)->SetWindowText(sText);	
					if (((CComboBox*)pControl)->GetCount() > 0)
						((CComboBox*)pControl)->SetCurSel(0);	
					break;
					}			
				case CmboStyle_Layers:
					{
					((OdclLayerCombo*)pControl)->SetWindowText(sText);	
					if (((CComboBox*)pControl)->GetCount() > 0)
						((CComboBox*)pControl)->SetCurSel(0);	
					break;
					}		
				case CmboStyle_LineWeight:
					{			
					((VdclLineWeightComboBox*)pControl)->SetWindowText(sText);	
					if (((CComboBox*)pControl)->GetCount() > 0)
						((CComboBox*)pControl)->SetCurSel(0);	
					break;
					}
				case CmboStyle_PlotNames:
					{			
					((VdclPlotStyleNamesComboBox*)pControl)->SetWindowText(sText);	
					if (((CComboBox*)pControl)->GetCount() > 0)
						((CComboBox*)pControl)->SetCurSel(0);	
					break;
					}
				case CmboStyle_PlotTables:
					{	
					((VdclPlotStyleTablesComboBox*)pControl)->SetWindowText(sText);	
					if (((CComboBox*)pControl)->GetCount() > 0)
						((CComboBox*)pControl)->SetCurSel(0);	
					break;
					}
				default:
					{
					if (((CComboBox*)pControl)->GetCount() > 0)
						((CComboBox*)pControl)->SetCurSel(0);	
					((VdclComboBox*)pControl)->SetWindowText(sText);	
					break;
					}
				}
				break;
			}
		case CtlStaticURL:
			{
			((CStaticLink*)pControl)->SetLinkText(sText);
			break;
			}
		}
	}
	catch(...)
	{
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateToolTip(CDclControlObject *pArxObject, CWnd *pControl)
{
	CString sToolTipTitle = pArxObject->GetStrProperty(Prop::ToolTipTitle);
	switch (pArxObject->GetType())
	{
	case CtlDwgPreview:
		{
		SetToolTipEx(pControl, ((CDwgPreviewCtrl*)pControl)->m_ToolTip, pArxObject);
		//((CDwgPreviewCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlHatch:
		{
		SetToolTipEx(pControl, ((CGsPreviewCtrl*)pControl)->m_ToolTip, pArxObject);
		//((CGsPreviewCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlRoundSlider:
		{
		SetToolTipEx(pControl, ((CRoundSliderCtrl*)pControl)->m_ToolTip, pArxObject);
		//((CRoundSliderCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlStaticURL:
		{
		SetToolTipEx(pControl, ((CStaticLink*)pControl)->m_ToolTip, pArxObject);
		//((CStaticLink*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlProgress:
		{
		//SetToolTipEx(pControl, ((TProgressTimeToComplete*)pControl)->m_ToolTip, pArxObject);
		//((VdclProgressCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlMonth:
		{
		SetToolTipEx(pControl, ((OdclMonth*)pControl)->m_ToolTip, pArxObject);
		//((OdclMonth*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlSlider:
		{
		SetToolTipEx(pControl, ((VdclSliderCtrl*)pControl)->m_ToolTip, pArxObject);
		//((VdclSliderCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlPictureBox:
		{
		SetToolTipEx(pControl, ((CPictureBox*)pControl)->m_ToolTip, pArxObject);
		//((CPictureBox*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	//case CtlListBox:
	//	{
	//	SetToolTipEx(pControl, ((CArxListBoxCtrl*)pControl)->m_ToolTip, pArxObject);
	//	//((CArxListBoxCtrl*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
	//	break;
	//	}
	//case CtlOptionButton:
	//	{
	//	SetToolTipEx(pControl, ((VdclRadioButton*)pControl)->m_ToolTip, pArxObject);
	//	//((VdclRadioButton*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
	//	break;
	//	}
	//case CtlOptionList:
	//	{
	//	//SetToolTipEx(pControl, ((COptionListBox*)pControl)->m_ToolTip, pArxObject);
	//	((COptionListBox*)pControl)->ResetTooltips();
	//	break;
	//	}
	//case CtlCheckBox:
	//	{
	//	SetToolTipEx(pControl, ((VdclCheckBox*)pControl)->m_ToolTip, pArxObject);
	//	//((VdclCheckBox*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
	//	break;
	//	}
	case CtlTextBox:
		{
			switch (pArxObject->GetLongProperty(Prop::FilterStyle))
			{
				case EditFilter_Symbol:
					SetToolTipEx(pControl, ((VdclSymbolEdit*)pControl)->m_ToolTip, pArxObject);
					//((VdclSymbolEdit*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
					break;
				case EditFilter_Angle:
					SetToolTipEx(pControl, ((VdclAngleEdit*)pControl)->m_ToolTip, pArxObject);
					//((VdclAngleEdit*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
					break;
				case EditFilter_Numeric:
					SetToolTipEx(pControl, ((VdclNumericEdit*)pControl)->m_ToolTip, pArxObject);
					//((VdclNumericEdit*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
					break;
				default:
					SetToolTipEx(pControl, ((OdclEdit*)pControl)->m_ToolTip, pArxObject);
					//((OdclEdit*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
					break;
			}
		break;
		}
	case CtlStdButton:
		{
		SetToolTipEx(pControl, ((VdclTextButton*)pControl)->m_ToolTip, pArxObject);
		//((VdclTextButton*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	case CtlTree:
		{
		SetToolTipEx(((VdclTree*)pControl), ((VdclTree*)pControl)->m_ChildTree.m_ToolTip, pArxObject);
		//((VdclTree*)pControl)->m_ChildTree.SetTooltipText(&sToolTipTitle, TRUE);
		break;
		}
	//case CtlDwgList:
	//	{
	//	SetToolTipEx(pControl, ((CDwgDirList*)pControl)->m_ToolTip, pArxObject);
	//	//((CDwgDirList*)pControl)->SetTooltipText(&sToolTipTitle, TRUE);
	//	break;
	//	}
	}
}
