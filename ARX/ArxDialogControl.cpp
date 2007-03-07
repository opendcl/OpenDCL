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
#include "DwgDirList.h"
#include "OptionListBox.h"
#include "VdclStatic.h"
#include "GsPreviewCtrl.h"
#include "SpreadSheet.h"
#include "OdclListCtrl.h"
#include "VdclCheckBox.h"
#include "VdclGroupBox.h"
#include "VdclListBox.h"
#include "DwgPreviewCtrl.h"
#include "VdclRadioButton.h"
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
#include "VdclTab.h"
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
	TraceFmt( _T("> CArxDialogControl::CArxDialogControl(%s [%08X], [%08X], %s [HWND: %08X]) [this: %08X]\r\n"),
						pTemplate->GetKeyPath(), pTemplate, pPane, CString(pControl->GetRuntimeClass()->m_lpszClassName),
						pControl->m_hWnd, (long)this );
}

CArxDialogControl::~CArxDialogControl()
{
	TraceFmt( _T("< CArxDialogControl::~CArxDialogControl() [this: %08X]\r\n"), (long)this );
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
			rc.top = pTemplate->m_pTop->GetLongValue();
			rc.left = pTemplate->m_pLeft->GetLongValue();
			rc.bottom = pTemplate->m_pHeight->GetLongValue() + rc.top;
			rc.right = pTemplate->m_pWidth->GetLongValue() + rc.left;
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
	case CtlDwgList:
		{
			CDwgDirList *pControl = new CDwgDirList;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);			
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlOptionList:
		{
			COptionListBox *pControl = new COptionListBox;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);			
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
	case CtlActiveX:
		{
			CArxAxContainerCtrl *pControl = new CArxAxContainerCtrl(pTemplate, pPane, nID);
			return pControl;
		}
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
	
	case CtlGrid:
		{
			CSpreadSheet *pControl = new CSpreadSheet;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			pControl->SetThemeHelper(pPane->GetThemeHelper());
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
	case CtlListView: return *new OdclListCtrl( *pPane, pTemplate, nID );
	case CtlBlockList: return *new OdclListCtrl( *pPane, pTemplate, nID );
	
	case CtlCheckBox:
		{
			VdclCheckBox *pControl = new VdclCheckBox;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}	
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
	case CtlGraphicButton:	
		{			
			return CreateGraphicButton(pTemplate, pPane, nID);
		}			
			
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
	case CtlListBox:
		{
			VdclListBox *pControl = new VdclListBox;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// // ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}		
	case CtlOptionButton:
		{
			VdclRadioButton *pControl = new VdclRadioButton;
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);
			UpdateChildControl(pControl, pTemplate, pPane, nID);
			// ZOrderFront(pControl);
			return new CArxAutoDialogControl( pTemplate, pPane, pControl );
		}
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
	
	case CtlTabStrip: return *new VdclTab( *pPane, pTemplate, nID );

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
TDialogControlPtr CArxDialogControl::CreateGraphicButton(CDclControlObject* pTemplate, CControlPane* pPane, UINT nID)
{
	return *new CArxGraphicButtonCtrl( pTemplate, pPane, nID );
}

//static
TDialogControlPtr CArxDialogControl::CreateEditControl(CDclControlObject* pTemplate, CControlPane* pPane, UINT nID)
{
	CWnd* pParentWnd = pPane->GetHostDialog();

	// check the control type to determine which control to create
	switch(pTemplate->GetLngProperty(nFilterStyle))
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
			pControl->Create(pTemplate, pPane->GetHostDialog(), nID);	
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
	switch(pTemplate->GetLngProperty(nComboBoxStyle))
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
			int nDropHeight = pTemplate->GetLngProperty(nDropDownHeight);
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
void CArxDialogControl::UpdateChildControl(CWnd *pControlWnd, CDclControlObject *pControl, CControlPane* pPane, UINT nControlId)
{
	POSITION pos;
	int nCount = 0; 

	while(nCount < pControl->GetPropertyList().GetCount())
	{
		pos = pControl->GetPropertyList().FindIndex(nCount);
	
		if (pos != NULL)
		{
			RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyList().GetAt(pos);
			if (pProp != NULL && pControl != NULL && pControl != NULL)
				UpdatePropertyInt(pControlWnd, pControl, pPane, pProp->GetID());
		}	
		nCount++;
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateChildControl(CDclControlObject *pControl, CControlPane* pPane, UINT nControlId)
{
	POSITION pos;
	int nCount = 0; 

	while(nCount < pControl->GetPropertyList().GetCount())
	{
		pos = pControl->GetPropertyList().FindIndex(nCount);
	
		RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyList().GetAt(pos);
		UpdateProperty(pControl, pPane, nControlId, pProp->GetID());
			
		nCount++;
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::UpdateProperty(CDclControlObject *pControl, CControlPane* pPane, UINT nControlId, PropertyId nID)
{
	switch( pControl->GetType() )
	{ //these controls implement the new CDialogControl interface, so use that
	case CtlGraphicButton:
	case CtlTabStrip:
	case CtlSlideView:
	case CtlListView:
	case CtlBlockList:
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
		switch (pControl->GetLngProperty(nComboBoxStyle) )
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
			CDwgDirList *pDwgList = (CDwgDirList*)pControl->GetWindow();
			pDwgList->m_pDirComboBox = pComboFolder;
			pComboFolder->m_pDwgList = pDwgList;		
		}		
	}
}

// This function is being phased out as control classes are changed to implement their own CDialogControl interface
//static
void CArxDialogControl::SetDwgListComboFolderLink(CDwgDirList *pDwgList)
{
	POSITION pos = pDwgList->m_ArxControl->GetOwnerForm()->GetControlList().GetHeadPosition();
	// increment to the next arx object so we will bypass the
	// arx object that holds the dialog box's properties.
	CDclControlObject* pControl = pDwgList->m_ArxControl->GetOwnerForm()->GetControlList().GetNext(pos);
	while (pos != NULL)
	{
		pControl = pDwgList->m_ArxControl->GetOwnerForm()->GetControlList().GetNext(pos);
		if (pControl->GetType() == CtlComboBox && pControl->GetWindow() != NULL)
		{
			// check the control type to determine which control to create
			if (pControl->GetLngProperty(nComboBoxStyle) == CmboStyle_DirPicker)
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
				pImageList->m_ImageList.SetBkColor(((VdclTree*)pControl)->m_ChildTree.GetBkColor());
				((VdclTree*)pControl)->m_ChildTree.SetImageList(&pImageList->m_ImageList, TVSIL_NORMAL);
			}
			break;
		}
		case CtlImageComboBox:
		{	
			RefCountedPtr< CImageListObject > pImageList = pArxObject->GetImageList();
			if( pImageList )
				((VdclComboBoxEx*)pControl)->SetImageList(&pImageList->m_ImageList);
			break;
		}
		case CtlGrid:
		{
			CSpreadSheet* pListCtrl = (CSpreadSheet*)pControl;
			RefCountedPtr< CImageListObject > pImageList = pArxObject->GetImageList();
			if (pImageList)
			{
				pImageList->m_ImageList.SetBkColor(::GetSysColor(COLOR_BTNFACE));
				pListCtrl->SetImageList(&pImageList->m_ImageList, TVSIL_NORMAL);
				pListCtrl->SetImageList(&pImageList->m_ImageList, LVSIL_SMALL);
			}
			else
			{
				if( !pListCtrl->m_DefaultImageList.m_hImageList )
					pListCtrl->m_DefaultImageList.Create(1, pArxObject->GetLngProperty(nRowHeight), ILC_COLOR, 1, 1);
				pListCtrl->m_DefaultImageList.SetBkColor(::GetSysColor(COLOR_WINDOW));
				pListCtrl->SetImageList(&pListCtrl->m_DefaultImageList, TVSIL_NORMAL);
				pListCtrl->SetImageList(&pListCtrl->m_DefaultImageList, LVSIL_SMALL);
			}
			break;
		}
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
		switch(pArxObject->GetLngProperty(nFilterStyle))
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
	case CtlCheckBox:		
		{		
		((VdclCheckBox*)pControl)->SetFont(pFont);		
		break;
		}
	case CtlOptionButton:		
		{		
		((VdclRadioButton*)pControl)->SetFont(pFont);		
		break;
		}
	case CtlComboBox:
		{
			switch (pArxObject->GetLngProperty(nComboBoxStyle))
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
	case CtlListBox:		
		{		
		((VdclListBox*)pControl)->SetFont(pFont);		
		break;
		}
	case CtlDwgList:
		{		
		((CDwgDirList*)pControl)->SetFont(pFont);		
		break;
		}		
	case CtlOptionList:		
		{		
		((COptionListBox*)pControl)->SetFont(pFont);		
		break;
		}		
	case CtlPictureBox:		
		{		
		((CPictureBox*)pControl)->SetFont(pFont);		
		break;
		}
	//case CtlTabStrip:		
	//	{		
	//	((VdclTab*)pControl)->GetTabCtrl().SetFont(NULL);	
	//	((VdclTab*)pControl)->GetTabCtrl().SetFont(pFont);		
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
	case CtlGrid:	
		{		
		((CSpreadSheet*)pControl)->SetFont(pFont);		
		break;
		}	
	}
	pControl->Invalidate();
}

//static
void CArxDialogControl::UpdatePropertyInt(CWnd* pControlWnd, CDclControlObject *pControl, CControlPane* pPane, PropertyId nID)
{
	//I'm moving pPane logic into the child control's Create() function as I work on individual controls. Controls 
	//that don't have their own class I'll leave here for now. 2007-02-05 [ORW]
	try
	{
	RefCountedPtr< CPropertyObject > pProp = pControl->GetPropertyObject(nID);

	if (pProp == NULL)
		return;

	// set the appropriate property
	switch(nID)
	{
		case nAcadColor:
		{
		switch (pControl->GetType())
			{
			case CtlBlockView:
			case CtlHatch:
				((CGsPreviewCtrl*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
				break;
			case CtlDwgPreview:
				((CDwgPreviewCtrl*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
				break;
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
				break;
			case CtlSlideView:		
				//((CSlideHolder*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
				break;
			case CtlListView:		
			case CtlBlockList:				
				//((OdclListCtrl*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
				break;
				
			case CtlLabel:
				((VdclStatic*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
				break;
				
			case CtlOptionButton:
			case CtlCheckBox:
				((CClrButton*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
				break;

			case CtlTextBox:
				switch (pControl->GetLngProperty(nFilterStyle))
				{
				//case EditFilter_Multiline:
				case EditFilter_Symbol:
					break;
				default:
					((CColorEdit*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
					
					break;
				}				
				break;
			
			case CtlListBox:
			case CtlOptionList:
			case CtlDwgList:
				((CClrListBox*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
				break;		
			
			case CtlStaticURL:
				((CStaticLink*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
				break;		

			//case CtlGraphicButton:
			//	((CArxGraphicButtonCtrl*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAcadColor));
			//	break;
			
			}
		if (pControl->GetType() != CtlPictureBox && 
			pControl->GetType() != CtlSlideView)
				pControlWnd->Invalidate();
			break;
			
		}
		case nForeColor:
		{				
			switch (pControl->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControlWnd)->SetForeColor(pControl->GetLngProperty(nForeColor));
				break;		

			case CtlOptionButton:
			case CtlCheckBox:
				((CClrButton*)pControlWnd)->SetForeColor(pControl->GetLngProperty(nForeColor));
				break;
			//case CtlGraphicButton:
			//	((CArxGraphicButtonCtrl*)pControlWnd)->SetForeColor(pControl->GetLngProperty(nForeColor));
			//	break;
			case CtlTextBox:
				switch (pControl->GetLngProperty(nFilterStyle))
				{
				//case EditFilter_Multiline:
				//	
				case EditFilter_Symbol:
					((CColorEdit*)pControlWnd)->m_UseBackColor = false;
					break;
				default:
					((CColorEdit*)pControlWnd)->SetForeColor(pControl->GetLngProperty(nForeColor));
					break;
				}				
				break;
			case CtlStaticURL:
				((CStaticLink*)pControlWnd)->SetForeColor(pControl->GetLngProperty(nForeColor));
				break;
			case CtlListBox:
			case CtlOptionList:
			case CtlDwgList:
				((CClrListBox*)pControlWnd)->SetForeColor(pControl->GetLngProperty(nForeColor));
			
			}
			
			pControlWnd->Invalidate();
			break;
		}	

		case nRowHeader:
		{
			((CSpreadSheet*)pControlWnd)->m_bHasRowHeader = pControl->GetBoolProperty(nRowHeader) == TRUE;
			break;
		}

		case nAllowOrbiting:
		{
			if (pControl->GetBoolProperty(nAllowOrbiting))
				((CGsPreviewCtrl*)pControlWnd)->AllowOrbiting(true);
			else
				((CGsPreviewCtrl*)pControlWnd)->AllowOrbiting(false);
			break;
		}
		case nAutoSize:
		{
			switch (pControl->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->m_AutoSize = pControl->GetBoolProperty(nAutoSize);
				break;
			//case CtlGraphicButton:		
			//	((CBtnST*)pControlWnd)->SetAcadColor(pControl->GetLngProperty(nAutoSize));
			//	break;
			}
			break;
		}
		
		case nBlockName:
		{
			((CGsPreviewCtrl*)pControlWnd)->DisplayBlock(pControl->GetStrProperty(nBlockName));
			break;
		}
		case nBorderStyle:
		{
			switch (pControl->GetType())
			{
			case CtlTree:
				{
				switch(pControl->GetLngProperty(nBorderStyle))
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
				switch(pControl->GetLngProperty(nBorderStyle))
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
					((CPictureBox*)pControlWnd)->m_BorderStyle = pControl->GetLngProperty(nBorderStyle);				
					break;
				}
				break;
				}
			}
			break;
		}	
		case nCaption:
		{				
			UpdateText(pControl, pControlWnd, pControl->GetStrProperty(nCaption));
			break;
		}	
		case nColumnWidth:
		{	
			int nNewColWidth = pControl->GetLngProperty(nColumnWidth);
			if (nNewColWidth > 0)
				((CListBox*)pControlWnd)->SetColumnWidth(nNewColWidth);
			break;
		}	
				
		case nDefSelIndex:
		{
			int nDefSelection = pControl->GetLngProperty(nDefSelIndex) ;
			((COptionListBox *)pControlWnd)->SetCurSel(nDefSelection);
			break;
		}
		
		case nDisableNoScroll:
		{
			if (pControl->GetBoolProperty(nDisableNoScroll) == FALSE)
				((CListBox*)pControlWnd)->ModifyStyle(LBS_DISABLENOSCROLL, 0, SWP_FRAMECHANGED);
			else
				((CListBox*)pControlWnd)->ModifyStyle(0, LBS_DISABLENOSCROLL, SWP_FRAMECHANGED);
			break;
		}

		case nDragnDropAllowDrop:
		{
			switch (pControl->GetType())
			{
			case CtlStdButton:
				((VdclTextButton*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;
			//case CtlGraphicButton:
			//	((CArxGraphicButtonCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
			//	break;			
			case CtlDwgList:
				((CDwgDirList*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;			
			case CtlBlockView:
			case CtlHatch:
				((CGsPreviewCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlTextBox:
				switch (pControl->GetLngProperty(nFilterStyle))
				{
				case EditFilter_Angle:
					((VdclAngleEdit*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
					break;
				case EditFilter_Numeric:
					((VdclNumericEdit*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
					break;
				case EditFilter_Symbol:
					((VdclSymbolEdit*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
					break;
				default:
					((OdclEdit*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
					break;
				}
				break;
			case CtlListBox:
				((VdclListBox*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlLabel:
				((VdclStatic*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlPictureBox:
				((CPictureBox*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlSlideView:
				((CSlideHolder*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlTree:
				((VdclTree*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlDwgPreview:
				((CDwgPreviewCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlListView:
			case CtlBlockList:
				((OdclListCtrl*)pControlWnd)->SetDragnDrop(pControl->GetBoolProperty(nDragnDropAllowDrop));
				break;
			}
			break;
		} 

		case nEnabled:
		{
			if (pControl->GetType() == CtlSlider)
				((CSliderCtrl *)pControlWnd)->EnableWindow(pControl->GetBoolProperty(nEnabled));
			else if (pControl->GetType() == CtlTree)
				((VdclTree *)pControlWnd)->m_ChildTree.EnableWindow(pControl->GetBoolProperty(nEnabled));
			//else if (pControl->GetType() == CtlTabStrip)
			//	((VdclTab *)pControlWnd)->GetTabCtrl().EnableWindow(pControl->GetBoolProperty(nEnabled));
			else if (pControl->GetType() == CtlOptionList)
			{
				int nData=0;
				if (pControl->GetBoolProperty(nEnabled) == FALSE)
					nData = 2;
				for (int i=0; i<((COptionListBox*) pControlWnd)->GetCount(); i++)
				{
					((COptionListBox*) pControlWnd)->SetItemData(i, nData);
				}
				pControlWnd->Invalidate();				
			}			
			else
			{
				pControlWnd->EnableWindow(pControl->GetBoolProperty(nEnabled) != FALSE);
				
				if (pControl->GetType() == CtlLabel)
				{
					((VdclStatic*)pControlWnd)->EnableWindow(pControl->GetBoolProperty(nEnabled) != FALSE);
					((VdclStatic*)pControlWnd)->Invalidate();				
				}				
			}
			break;				
		}	
		case nFilterStyle:
			{
			switch (pControl->GetLngProperty(nFilterStyle))
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
	
		case nIndent:
		{
			((VdclTree*)pControlWnd)->m_ChildTree.SetIndent(pControl->GetLngProperty(nIndent));
			break;
		}

		case nImageList:
		{
			ResetImageList(pControl, pControlWnd, nID);
			break;
		}

		case nJustification:
		{
			switch (pControl->GetType())
			{
			case CtlTextBox:
				{
				/*
					((CEdit*)pControlWnd)->ModifyStyle(ES_RIGHT, 0, SWP_FRAMECHANGED);
					((CEdit*)pControlWnd)->ModifyStyle(ES_CENTER, 0, SWP_FRAMECHANGED);
					((CEdit*)pControlWnd)->ModifyStyle(ES_LEFT, 0, SWP_FRAMECHANGED);
					
					switch (pControl->GetLngProperty(nJustification))
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
			
					switch (pControl->GetLngProperty(nJustification))
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

		case nLabelName:
		{
			const CFont *pFont = theWorkspace.GetFontCollection().GetFont(pControl, pControlWnd);

			UpdateFont(pControl, pControlWnd, const_cast<CFont*>(pFont));
			break;
		}

		case nLimitText:
		{
			switch (pControl->GetType())
			{
			case CtlTextBox:
				{
				((CEdit*)pControlWnd)->SetLimitText(pControl->GetLngProperty(nLimitText));
				break;
				}
			case CtlComboBox:
				{
				((CComboBox*)pControlWnd)->LimitText(pControl->GetLngProperty(nLimitText));
				break;				
				}
			case CtlImageComboBox:
				{
				((CComboBoxEx*)pControlWnd)->LimitText(pControl->GetLngProperty(nLimitText));
				break;				
				}
			}
			break;
		}
		
		case nList:
		{
			switch (pControl->GetType())
			{
			case CtlListBox:
				{
					CString sListItem;
					((CListBox *)pControlWnd)->ResetContent();					
					for (size_t i = 0; i < pProp->size(); i++)
					{				
						sListItem = pProp->GetStringItem(i);
						if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
							((CListBox *)pControlWnd)->AddString(sListItem);
					}
					break;
				}
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
		case nMarginLeft:
		{
			((CEdit*)pControlWnd)->SetMargins(
				pControl->GetLngProperty(nMarginLeft),
				pControl->GetLngProperty(nMarginRight));
			break;
		}
		
		//case nMinTabWidth:
		//{
		//	int n = pControl->GetLngProperty(nMinTabWidth);
		//	((VdclTab*)pControlWnd)->GetTabCtrl().SetMinTabWidth(pControl->GetLngProperty(nMinTabWidth));
		//	((VdclTab*)pControlWnd)->GetTabCtrl().RedrawWindow(NULL, NULL, RDW_UPDATENOW);
		//	break;
		//}
		case nMultiSelection:
		{
			((OdclMonth*)pControlWnd)->SetMaxSelCount(
				pControl->GetLngProperty(nMultiSelection));
			break;
		}
		case nMaxValue:
		case nMinValue:
		{
			switch (pControl->GetType())
			{
			case CtlProgress:
				((CProgressCtrl*)pControlWnd)->SetRange(
					(int)pControl->GetLngProperty(nMinValue),
					(int)pControl->GetLngProperty(nMaxValue));
				break;				
			case CtlScrollBar:
				((CScrollBar*)pControlWnd)->SetScrollRange(
					(int)pControl->GetLngProperty(nMinValue),
					(int)pControl->GetLngProperty(nMaxValue),
					TRUE);
				break;				
			case CtlSlider:
				((CSliderCtrl*)pControlWnd)->SetRange(
					(int)pControl->GetLngProperty(nMinValue),
					(int)pControl->GetLngProperty(nMaxValue));
			
			}
			break;
		}	
		
		case nLargeChange:	
		{
			switch (pControl->GetType())
			{
			case CtlScrollBar:				
				((VdclScrollBar*)pControlWnd)->m_nLargeChange = pControl->GetLngProperty(nLargeChange);
				break;
			case CtlSlider:				
				((VdclSliderCtrl*)pControlWnd)->SetPageSize(pControl->GetLngProperty(nLargeChange));
				break;
			}
			break;
			
		}
		case nOrientation:
		{
			switch(pControl->GetLngProperty(nOrientation))
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
		case nPicture:
		{
			switch (pControl->GetType())
			{
			//case CtlGraphicButton:		
			//	((CArxGraphicButtonCtrl *)pControlWnd)->SetPictureID(pControl->GetLngProperty(nPicture));
			//	((CArxGraphicButtonCtrl *)pControlWnd)->SetPressedPictureID(pControl->GetLngProperty(nPressedPicture));
			//	pControlWnd->Invalidate();
			//	break;
			case CtlPictureBox:		
				((CPictureBox *)pControlWnd)->SetPictureID(pControl->GetLngProperty(nPicture));
				break;
			}		
			break;
		}

		case nReadOnly:
		{
			((CEdit*)pControlWnd)->SetReadOnly(pControl->GetBoolProperty(nReadOnly));
			break;
		}
		case nRenderMode:
		{			
			((CGsPreviewCtrl*)pControlWnd)->SetRenderMode();
			((CGsPreviewCtrl*)pControlWnd)->Invalidate();
			break;
		}
		
		case nRowHeight:
		{			
			if (pControl->GetType() == CtlDwgList)
			{
				CDwgDirList* pDwgList = (CDwgDirList*)pControlWnd;
				pDwgList->m_RowHeight = pControl->GetLngProperty(nRowHeight);
				int nCurSel = pDwgList->GetCurSel();
				pDwgList->Dir(pDwgList->m_sPath);
				pDwgList->SetCurSel(nCurSel);
			}
			break;
		}
		
		case nShowTicks:
		{
			if (pControl->GetBoolProperty(nShowTicks) == FALSE)
				((CSliderCtrl *)pControlWnd)->ModifyStyle(TBS_AUTOTICKS, 0, SWP_FRAMECHANGED);
			else
				((CSliderCtrl *)pControlWnd)->ModifyStyle(0, TBS_AUTOTICKS, SWP_FRAMECHANGED);
				
			break;
		}
		case nSmallChange:	
		{
			switch (pControl->GetType())
			{
			case CtlScrollBar:				
				((VdclScrollBar*)pControlWnd)->m_nSmallChange = pControl->GetLngProperty(nSmallChange);
				break;
			}
			break;
			
		}
		//case nTabSelected:
		//{
		//	((CListBox*)pControlWnd)->SetCurSel(pControl->GetLngProperty(nTabSelected));
		//	break;
		//}
		//case nTabStyle:
		//{
		//	if (pControl->GetLngProperty(nTabStyle) == 0)
		//		((CListBox*)pControlWnd)->ModifyStyle(TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED);
		//	else
		//		((CListBox*)pControlWnd)->ModifyStyle(TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED);
		//	break;
		//}
		//
		//case nTabLabelAlign:
		//{
		//	if (pControl->GetLngProperty(nTabLabelAlign) == 0)
		//		((VdclTab*)pControlWnd)->GetTabCtrl().ModifyStyle(0, TCS_FORCELABELLEFT, SWP_FRAMECHANGED);
		//	else
		//		((VdclTab*)pControlWnd)->GetTabCtrl().ModifyStyle(TCS_FORCELABELLEFT, 0, SWP_FRAMECHANGED);
		//	break;
		//}
		//case nTabFixedWidth:
		//{
		//	if (pControl->GetBoolProperty(nTabFixedWidth) == TRUE)
		//	{
		//		((VdclTab*)pControlWnd)->GetTabCtrl().ModifyStyle(0, TCS_FIXEDWIDTH, SWP_FRAMECHANGED);
		//		CRect rc;
		//		((VdclTab*)pControlWnd)->GetTabCtrl().GetItemRect(0, &rc);
		//		CSize szTabs;
		//		szTabs.cx = pControl->GetLngProperty(nMinTabWidth);
		//		szTabs.cy = rc.Height();
		//		((VdclTab*)pControlWnd)->GetTabCtrl().SetItemSize(szTabs);
		//	}
		//	else
		//		((VdclTab*)pControlWnd)->GetTabCtrl().ModifyStyle(TCS_FIXEDWIDTH, 0, SWP_FRAMECHANGED);
		//	break;
		//}
		case nText:
		{
			if (!IsSelfPopulatedList(pControl))
			{							
				int nTextLimit = pControl->GetLngProperty(nLimitText);
				CString sNewText = pControl->GetStrProperty(nText);

				if (nTextLimit > -1) 
					sNewText = sNewText.Left(nTextLimit);

				UpdateText(pControl, pControlWnd, sNewText);
			}
			break;
		}
		case nTickFrequency:
		{
			((CSliderCtrl *)pControlWnd)->SetTicFreq(pControl->GetLngProperty(nTickFrequency));
			
			break;
		}
		
		case nTitleBarText:
		{
			pControlWnd->SetWindowText(pControl->GetStrProperty(nTitleBarText));
			break;
		}
		case nToolTipText:
		{
			ChangeToolTipText(pControl, pControlWnd);
			break;
		}
		case nUseTabStops:
		{
			if (pControl->GetBoolProperty(nUseTabStops) == FALSE)
				((CListBox*)pControlWnd)->ModifyStyle(LBS_USETABSTOPS, 0, SWP_FRAMECHANGED);
			else
				((CListBox*)pControlWnd)->ModifyStyle(0, LBS_USETABSTOPS, SWP_FRAMECHANGED);
			break;
		}
		case nURLAddress:
		{
			if (pControl->GetType() == CtlStaticURL)
				((CStaticLink*)pControlWnd)->m_link = pControl->GetStrProperty(nURLAddress);
			break;
		}
		case nVisible:
		{
			if (IsWindow(pControlWnd->m_hWnd))
				pControlWnd->ShowWindow(pControl->m_pVisible->GetBooleanValue()? SW_SHOW : SW_HIDE);
			else
				theWorkspace.DisplayAlert(_T("The ActiveX Control is hidden and no longer valid.\nPlease contact the control developer for support."));
			break;
		}
		case nValue:
		{
			switch (pControl->GetType())
			{
			case CtlCheckBox:
			case CtlOptionButton:
				((CButton *)pControlWnd)->SetCheck(pControl->GetBoolProperty(nValue));
				pControlWnd->RedrawWindow();
				break;									
			case CtlRoundSlider:
				((CRoundSliderCtrl *)pControlWnd)->SetPos(pControl->GetLngProperty(nValue));
				pControlWnd->RedrawWindow();
				break;
			case CtlSlider:
				((CSliderCtrl*)pControlWnd)->SetPos(pControl->GetLngProperty(nValue));
				pControlWnd->RedrawWindow();
				break;
			case CtlScrollBar:
				{
					((VdclScrollBar *)pControlWnd)->m_hPos = pControl->GetLngProperty(nValue);
					((VdclScrollBar *)pControlWnd)->SetScrollPos(pControl->GetLngProperty(nValue), TRUE);
					pControlWnd->RedrawWindow();
					break;
				}
			case CtlProgress:
				((CProgressCtrl*)pControlWnd)->SetPos(pControl->GetLngProperty(nValue));
				pControlWnd->RedrawWindow();
				break;
			case CtlSpinButton:
				((VdclSpinButton*)pControlWnd)->m_Pos = pControl->GetLngProperty(nValue);
				((VdclSpinButton*)pControlWnd)->SetPos(((VdclSpinButton*)pControlWnd)->m_Pos);				
				break;			
			}
			break;
		}
		case nHScrollBar:
		{
			if (pControl->GetBoolProperty(nHScrollBar) == FALSE)
				pControlWnd->ModifyStyle(WS_HSCROLL, 0, SWP_FRAMECHANGED);
			else
				pControlWnd->ModifyStyle(0, WS_HSCROLL, SWP_FRAMECHANGED);
			break;
		}
		case nVScrollBar:
		{
			switch (pControl->GetType())
			{
			case CtlTextBox:
				{			
				if (pControl->GetBoolProperty(nVScrollBar) == FALSE)
					((OdclEdit*)pControlWnd)->ModifyStyle(WS_VSCROLL, 0, SWP_FRAMECHANGED);
				else
					((OdclEdit*)pControlWnd)->ModifyStyle(0, WS_VSCROLL, SWP_FRAMECHANGED);
				break;
				}
			case CtlListBox:
				{			
				if (pControl->GetBoolProperty(nVScrollBar) == FALSE)
					((CListBox*)pControlWnd)->ModifyStyle(WS_VSCROLL, 0, SWP_FRAMECHANGED);
				else
					((CListBox*)pControlWnd)->ModifyStyle(0, WS_VSCROLL, SWP_FRAMECHANGED);
				break;
				}
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
			switch(pTemplate->GetLngProperty(nFilterStyle))
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
		case CtlCheckBox:		
			{		
			((VdclCheckBox*)pControl)->SetWindowText(sText);		
			break;
			}
		case CtlOptionButton:		
			{		
			((VdclRadioButton*)pControl)->SetWindowText(sText);		
			break;
			}
		case CtlComboBox:
			{
				switch (pTemplate->GetLngProperty(nComboBoxStyle))
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
void CArxDialogControl::ChangeToolTipText(CDclControlObject *pArxObject, CWnd *pControl)
{
	CString sToolTipText = pArxObject->GetStrProperty(nToolTipText);
	
	#define nToolTipBody			235
	#define nToolTipPicture			236
	#define nToolTipAviFileName		237
	#define nToolTipLine			238
	#define nToolTipTitleColor		239
		
	switch (pArxObject->GetType())
	{
	case CtlDwgPreview:
		{
		SetToolTipEx(pControl, ((CDwgPreviewCtrl*)pControl)->m_ToolTip, pArxObject);
		//((CDwgPreviewCtrl*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlBlockView:
	case CtlHatch:
		{
		SetToolTipEx(pControl, ((CGsPreviewCtrl*)pControl)->m_ToolTip, pArxObject);
		//((CGsPreviewCtrl*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlSlideView:
		{
		SetToolTipEx(pControl, ((CSlideHolder*)pControl)->mToolTip, pArxObject);
		//((CSlideHolder*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlRoundSlider:
		{
		SetToolTipEx(pControl, ((CRoundSliderCtrl*)pControl)->m_ToolTip, pArxObject);
		//((CRoundSliderCtrl*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlStaticURL:
		{
		SetToolTipEx(pControl, ((CStaticLink*)pControl)->m_ToolTip, pArxObject);
		//((CStaticLink*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlProgress:
		{
		//SetToolTipEx(pControl, ((TProgressTimeToComplete*)pControl)->m_ToolTip, pArxObject);
		//((VdclProgressCtrl*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlMonth:
		{
		SetToolTipEx(pControl, ((OdclMonth*)pControl)->m_ToolTip, pArxObject);
		//((OdclMonth*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlSlider:
		{
		SetToolTipEx(pControl, ((VdclSliderCtrl*)pControl)->m_ToolTip, pArxObject);
		//((VdclSliderCtrl*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlPictureBox:
		{
		SetToolTipEx(pControl, ((CPictureBox*)pControl)->m_ToolTip, pArxObject);
		//((CPictureBox*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlListBox:
		{
		SetToolTipEx(pControl, ((VdclListBox*)pControl)->m_ToolTip, pArxObject);
		//((VdclListBox*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlOptionButton:
		{
		SetToolTipEx(pControl, ((VdclRadioButton*)pControl)->m_ToolTip, pArxObject);
		//((VdclRadioButton*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlOptionList:
		{
		//SetToolTipEx(pControl, ((COptionListBox*)pControl)->m_ToolTip, pArxObject);
		//((COptionListBox*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlCheckBox:
		{
		SetToolTipEx(pControl, ((VdclCheckBox*)pControl)->m_ToolTip, pArxObject);
		//((VdclCheckBox*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlTextBox:
		{
			switch (pArxObject->GetLngProperty(nFilterStyle))
			{
				case EditFilter_Symbol:
					SetToolTipEx(pControl, ((VdclSymbolEdit*)pControl)->m_ToolTip, pArxObject);
					//((VdclSymbolEdit*)pControl)->SetTooltipText(&sToolTipText, TRUE);
					break;
				case EditFilter_Angle:
					SetToolTipEx(pControl, ((VdclAngleEdit*)pControl)->m_ToolTip, pArxObject);
					//((VdclAngleEdit*)pControl)->SetTooltipText(&sToolTipText, TRUE);
					break;
				case EditFilter_Numeric:
					SetToolTipEx(pControl, ((VdclNumericEdit*)pControl)->m_ToolTip, pArxObject);
					//((VdclNumericEdit*)pControl)->SetTooltipText(&sToolTipText, TRUE);
					break;
				default:
					SetToolTipEx(pControl, ((OdclEdit*)pControl)->m_ToolTip, pArxObject);
					//((OdclEdit*)pControl)->SetTooltipText(&sToolTipText, TRUE);
					break;
			}
		break;
		}
	case CtlStdButton:
		{
		SetToolTipEx(pControl, ((VdclTextButton*)pControl)->m_ToolTip, pArxObject);
		//((VdclTextButton*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlTree:
		{
		SetToolTipEx(((VdclTree*)pControl), ((VdclTree*)pControl)->m_ChildTree.m_ToolTip, pArxObject);
		//((VdclTree*)pControl)->m_ChildTree.SetTooltipText(&sToolTipText, TRUE);
		break;
		}

	case CtlGraphicButton:
		{
		SetToolTipEx(pControl, ((CArxGraphicButtonCtrl*)pControl)->GetToolTipCtrl(), pArxObject);
		//((CArxGraphicButtonCtrl*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlActiveX:
		{
		((CAxContainerCtrl*)pControl)->SetTooltipText(sToolTipText);
		break;
		}
	case CtlDwgList:
		{
		SetToolTipEx(pControl, ((CDwgDirList*)pControl)->m_ToolTip, pArxObject);
		//((CDwgDirList*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	}
}
