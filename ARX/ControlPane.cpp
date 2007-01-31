// ControlPane.cpp : implementation file
//

#include "stdafx.h"
#include "ControlPane.h"
#include "ArxDialogControl.h"
#include "DclControlObject.h"
#include "PropertyIds.h"
#include "ControlTypes.h"
#include "XPStyleButtonST.h"
#include "Resource.h"
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
#include "AxContainer.h"
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
#include "DclFormObject.h"
#include "Project.h"
#include "TabPage.h"
#include "ToolTips.h"

const TCHAR *sFilterName = _T("IDB_FILTER";)

const int EditFilter_String		= 0;
const int EditFilter_Angle		= 1;
const int EditFilter_Integer	= 2;
const int EditFilter_Numeric	= 3;
const int EditFilter_Symbol		= 4;
const int EditFilter_UpperCase	= 5;
const int EditFilter_LowerCase	= 6;
const int EditFilter_Password	= 7;
const int EditFilter_Multiline	= 8;

const int ButtonStyle_Raised	= 0;
const int ButtonStyle_Flat		= 1;
const int ButtonStyle_Pick		= 2;
const int ButtonStyle_Select	= 3;
const int ButtonStyle_Filter	= 4;
const int ButtonStyle_NoBorder	= 5;

const int CmboStyle_Combo		= 0;
const int CmboStyle_Simple		= 1;
const int CmboStyle_DropDown	= 2;
const int CmboStyle_ArrowHead	= 3;
const int CmboStyle_Color		= 4;
const int CmboStyle_LineWeight	= 5;
const int CmboStyle_PlotNames	= 6;
const int CmboStyle_PlotTables	= 7;
const int CmboStyle_FontDropList	= 8;
const int CmboStyle_FontSimpleList	= 9;
const int CmboStyle_Plotters		= 10;
const int CmboStyle_PlotterPaperSizes = 11;
const int CmboStyle_DirPicker = 12;
const int CmboStyle_Layers = 13;


/////////////////////////////////////////////////////////////////////////////
// CControlPane

CControlPane::CControlPane()
: mpSourceForm( NULL )
, mpProject( NULL )
, mpActivePaperSizesCombo( NULL )
{
	m_bInvokeWithSendString = false;
	m_pParentFileDialog = NULL;
	m_pFontCollection = &theWorkspace.GetFontCollection();
}

CControlPane::CControlPane(CDclFormObject* pSourceForm, CProject* pProject)
: mpSourceForm( pSourceForm )
, mpProject( pProject? pProject : (pSourceForm? pSourceForm->GetProject() : NULL) )
, mpActivePaperSizesCombo( NULL )
{
	m_bInvokeWithSendString = false;
	m_pParentFileDialog = NULL;
	m_pFontCollection = &theWorkspace.GetFontCollection();
}

CControlPane::~CControlPane()
{
}

void CControlPane::UpdateGlobalVariables()
{
	POSITION pos = m_pControlCol->GetHeadPosition();
	while (pos != NULL)
	{
		CArxDialogControl *pCtrlObj = m_pControlCol->GetNext(pos);
		if (pCtrlObj != NULL)
			pCtrlObj->CreateGlobalVariables();
	}
}

void CControlPane::CreateGraphicButton(CDclControlObject* pArxControl, CWnd *pParentWnd, int nId)
{
	// check the control type to determine which control to create
	switch(pArxControl->GetLngProperty(nButtonStyle))
	{
	case ButtonStyle_Filter:
		{
		// create the control
		CXPStyleButtonST *pControl = new CXPStyleButtonST;
		pControl->Create(pArxControl, mpProject, pParentWnd, nId);
		pControl->SetFlat(FALSE);
		//pControl->SetDefaultActiveFgColor(FALSE);
		pControl->SetThemeHelper(&m_ThemeHelper);
		pControl->SetFlat(FALSE);
		UpdateChildControl(pControl, pArxControl, nId);
		pControl->SetIcon(IDI_FILTER);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case ButtonStyle_Flat:
		{
		// create the control
		CXPStyleButtonST *pControl = new CXPStyleButtonST;
		pControl->Create(pArxControl, mpProject, pParentWnd, nId);
		pControl->SetThemeHelper(&m_ThemeHelper);
		pControl->SetFlat(TRUE);
		pControl->DrawAsToolbar(TRUE);
		//pControl->SetButtonStyle(
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case ButtonStyle_Pick:
		{
		// create the control
		CXPStyleButtonST *pControl = new CXPStyleButtonST;
		pControl->Create(pArxControl, mpProject, pParentWnd, nId);
		pControl->SetFlat(FALSE);
		pControl->SetThemeHelper(&m_ThemeHelper);
		 
		pControl->SetIcon(IDI_PICK);
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case ButtonStyle_Raised:
		{
		// create the control
		CXPStyleButtonST *pControl = new CXPStyleButtonST;
		
		pControl->Create(pArxControl, mpProject, pParentWnd, nId);
		pControl->SetFlat(FALSE);
		pControl->SetThemeHelper(&m_ThemeHelper);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case ButtonStyle_NoBorder:
		{
		// create the control
		CXPStyleButtonST *pControl = new CXPStyleButtonST;
		pControl->Create(pArxControl, mpProject, pParentWnd, nId);
		pControl->SetFlat(TRUE);
		pControl->SetThemeHelper(&m_ThemeHelper);
		pControl->DrawBorder(FALSE);
		pControl->m_bHasBorder = FALSE;
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	
	case ButtonStyle_Select:
		{
		// create the control
		CXPStyleButtonST *pControl = new CXPStyleButtonST;
		pControl->Create(pArxControl, mpProject, pParentWnd, nId);
		pControl->SetFlat(FALSE);
		pControl->SetThemeHelper(&m_ThemeHelper);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		pControl->SetIcon(IDI_SELECT);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	}
}

void CControlPane::CreateEditControl(CDclControlObject* pArxControl, CWnd *pParentWnd, int nId)
{
	// check the control type to determine which control to create
	switch(pArxControl->GetLngProperty(nFilterStyle))
	{
	case EditFilter_Angle:
		{
		// create the control
		VdclAngleEdit *pControl = new VdclAngleEdit;
		pControl->Create(pArxControl, pParentWnd, nId);
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		
		break;
		}
	case EditFilter_Integer:
		{
		// create the control
		OdclEdit *pControl = new OdclEdit;
		pControl->Create(pArxControl, m_pParentDlg, nId);		
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		
		break;
		}
	case EditFilter_Numeric:
		{
		// create the control
		VdclNumericEdit *pControl = new VdclNumericEdit;
		pControl->Create(pArxControl, pParentWnd, nId);
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		
		break;
		}
	case EditFilter_Symbol:
		{
		// create the control
		VdclSymbolEdit *pControl = new VdclSymbolEdit;
		pControl->Create(pArxControl, pParentWnd, nId);
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		
		break;
		}
	default:
		{
		OdclEdit *pControl = new OdclEdit;
		pControl->Create(pArxControl, m_pParentDlg, nId);	
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		
		break;
		}
	
	}
}
void CControlPane::CreateComboExControl(CDclControlObject* pArxControl, CWnd *pParentWnd, int nId)
{
	// create the control
	VdclComboBoxEx *pControl = new VdclComboBoxEx;

	pControl->Create2(pArxControl, pParentWnd, nId);
	
	pControl->m_ArxControl = pArxControl;
	
	UpdateChildControl(pControl, pArxControl, nId);
	// can't do ZOrderFront, it fucks up the display of the CComboBoxEx for some unknown reason.
	// // ZOrderFront(pControl);
	CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
	m_pControlCol->AddTail(pCtrlObj);
	pArxControl->m_pWnd = pControl;
	
}

void CControlPane::DeleteEditControl(CDclControlObject* pArxControl, CWnd *pControl)
{
	// check the control type to determine which control to create
	switch(pArxControl->GetLngProperty(nFilterStyle))
	{
	case EditFilter_Angle:
		{
		((VdclAngleEdit*)pControl)->DestroyWindow();
		delete (VdclAngleEdit*)pControl;
		
		break;
		}
	case EditFilter_Integer:
		{
		((OdclEdit*)pControl)->DestroyWindow();
		delete (OdclEdit*)pControl;		
		break;
		}
	case EditFilter_Numeric:
		{
		((VdclNumericEdit*)pControl)->DestroyWindow();
		delete (VdclNumericEdit*)pControl;		
		
		break;
		}
	case EditFilter_Symbol:
		{
		((VdclSymbolEdit*)pControl)->DestroyWindow();
		delete (VdclSymbolEdit*)pControl;		
		
		break;
		}
	default:
		{
		((OdclEdit*)pControl)->DestroyWindow();
		delete (OdclEdit*)pControl;		
		
		break;
		}
	
	}
}
void CControlPane::CreateComboControl(CDclControlObject* pArxControl, CWnd *pParentWnd, int nId)
{
	// check the control type to determine which control to create
	switch(pArxControl->GetLngProperty(nComboBoxStyle))
	{
	case CmboStyle_Combo:
		{
		// create the control
		VdclComboBox *pControl = new VdclComboBox;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		
		break;
		}
	case CmboStyle_Simple:
		{			
		// create the control
		VdclComboBox *pControl = new VdclComboBox;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		
		break;
		}
	case CmboStyle_DropDown:
		{			
		// create the control
		VdclComboBox *pControl = new VdclComboBox;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case CmboStyle_ArrowHead:
		{	
		// create the control
		VdclArrowHeadComboBox *pControl = new VdclArrowHeadComboBox;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case CmboStyle_Color:
		{
		// create the control
		VdclColorComboBox *pControl = new VdclColorComboBox;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}			
	case CmboStyle_LineWeight:
		{			
		// create the control
		VdclLineWeightComboBox *pControl = new VdclLineWeightComboBox;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case CmboStyle_PlotNames:
		{			
		// create the control
		VdclPlotStyleNamesComboBox *pControl = new VdclPlotStyleNamesComboBox;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case CmboStyle_PlotTables:
		{			
		// create the control
		VdclPlotStyleTablesComboBox *pControl = new VdclPlotStyleTablesComboBox;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	
	case CmboStyle_FontDropList:
		{			
		// create the control
		CFontCombo *pControl = new CFontCombo;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		// ZOrderFront(pControl);
		UpdateChildControl(pControl, pArxControl, nId);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	
	case CmboStyle_FontSimpleList:
		{			
		// create the control
		CFontCombo *pControl = new CFontCombo;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		// ZOrderFront(pControl);
		UpdateChildControl(pControl, pArxControl, nId);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case CmboStyle_Plotters:
		{			
		// create the control
			CPrinterComboBox *pControl = new CPrinterComboBox(mpActivePaperSizesCombo);
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		// ZOrderFront(pControl);
		UpdateChildControl(pControl, pArxControl, nId);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		break;
		}
	case CmboStyle_PlotterPaperSizes:
		{
		// create the control
		VdclComboBox *pControl = new VdclComboBox;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		
		mpActivePaperSizesCombo = pControl;
		pArxControl->m_pWnd = pControl;
		break;
		}
	case CmboStyle_DirPicker:
		{
		// create the control
		CComboBoxFolder *pControl = new CComboBoxFolder;
		pControl->Create(pArxControl, pParentWnd, nId);
		int nDropHeight = pArxControl->GetLngProperty(nDropDownHeight);
		if (nDropHeight < 300)
			nDropHeight = 300;
		pControl->Init(0, nDropHeight); 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		pArxControl->m_pWnd = pControl;
		SetDwgListComboFolderLink(pControl);
		break;
		}
	
	case CmboStyle_Layers:
		{
		// create the control
		OdclLayerCombo *pControl = new OdclLayerCombo;
		pControl->Create(pArxControl, pParentWnd, nId);
		 
		UpdateChildControl(pControl, pArxControl, nId);
		// ZOrderFront(pControl);
		CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
		m_pControlCol->AddTail(pCtrlObj);
		
		pArxControl->m_pWnd = pControl;
		break;
		}
	}
}

void CControlPane::DeleteComboControl(CDclControlObject* pArxControl, CWnd *pControl)
{
	// check the control type to determine which control to create
	switch(pArxControl->GetLngProperty(nComboBoxStyle))
	{
	case CmboStyle_Combo:
	case CmboStyle_Simple:
	case CmboStyle_DropDown:
		{
		((VdclComboBox*)pControl)->DestroyWindow();
		delete (VdclComboBox*)pControl;
		break;
		}
	
	case CmboStyle_ArrowHead:
		{	
		((VdclArrowHeadComboBox*)pControl)->DestroyWindow();
		delete (VdclArrowHeadComboBox*)pControl;
		
		break;
		}
	case CmboStyle_Color:
		{
		((VdclColorComboBox*)pControl)->DestroyWindow();
		delete (VdclColorComboBox*)pControl;
		
		break;
		}			
	case CmboStyle_LineWeight:
		{			
		((VdclLineWeightComboBox*)pControl)->DestroyWindow();
		delete (VdclLineWeightComboBox*)pControl;
		
		break;
		}
	case CmboStyle_PlotNames:
		{			
		((VdclPlotStyleNamesComboBox*)pControl)->DestroyWindow();
		delete (VdclPlotStyleNamesComboBox*)pControl;
		
		break;
		}
	case CmboStyle_PlotTables:
		{			
		((VdclPlotStyleTablesComboBox*)pControl)->DestroyWindow();
		delete (VdclPlotStyleTablesComboBox*)pControl;
		
		break;
		}
	
	case CmboStyle_FontDropList:
		{			
		((CFontCombo*)pControl)->DestroyWindow();
		delete ((CFontCombo*)pControl);
		
		break;
		}
	
	case CmboStyle_FontSimpleList:
		{			
		((CFontCombo*)pControl)->DestroyWindow();
		delete ((CFontCombo*)pControl);
		
		break;
		}
	case CmboStyle_Plotters:
		{			
		((CPrinterComboBox*)pControl)->DestroyWindow();
		delete ((CPrinterComboBox*)pControl);
		
		break;
		}
		
	case CmboStyle_PlotterPaperSizes:
		{
		((VdclComboBox*)pControl)->DestroyWindow();
		delete (VdclComboBox*)pControl;
		mpActivePaperSizesCombo = NULL;
		break;
		}
	case CmboStyle_DirPicker:
		{
		((CComboBoxFolder*)pControl)->DestroyWindow();
		delete (CComboBoxFolder*)pControl;
		break;
		}
	case CmboStyle_Layers:
		{
		((OdclLayerCombo*)pControl)->DestroyWindow();
		delete (OdclLayerCombo*)pControl;	
		break;
		}
	}
}

void CControlPane::ZOrderFront(CWnd *pControl)
{
	if (pControl != NULL)
		pControl->SetWindowPos(&CWnd::wndTop, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
}
void CControlPane::ZOrderBack(CWnd *pControl)
{
	if (pControl != NULL)
		pControl->SetWindowPos(&CWnd::wndBottom, 0,0,-1,-1, SWP_NOSIZE|SWP_NOMOVE);
}


UINT CControlPane::CreateEachControl(CDclControlObject* pArxControl, int nId)
{
	
	// set these for drag and drop
	pArxControl->m_sProjectName = m_sProjectName;
	pArxControl->m_sDialogName = m_sDialogName;
	pArxControl->m_Id = nId;

	// check the control type to determine which control to create
	switch(pArxControl->GetType())
	{
	case CtlAnimate:
		{
			// create the 3d rect control
			CAnimateCtrl *pControl = new CAnimateCtrl;
			
			CRect rc;
			// get the rectangle of the new control
			rc.top = pArxControl->m_pTop->GetLongValue();
			rc.left = pArxControl->m_pLeft->GetLongValue();
			rc.bottom = pArxControl->m_pHeight->GetLongValue() + rc.top;
			rc.right = pArxControl->m_pWidth->GetLongValue() + rc.left;
		
			pArxControl->m_bControlCreated = pControl->Create(
				WS_VISIBLE|WS_CHILD|WS_CLIPSIBLINGS |
				ACS_CENTER |ACS_AUTOPLAY|ACS_TRANSPARENT, 
				rc, m_pParentDlg, nId) == TRUE;
			pArxControl->m_pWnd = pControl;

			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			break;
		}
	case CtlImageComboBox:
		{			
			CreateComboExControl(pArxControl, m_pParentDlg, nId);
			break;
		}		
	case CtlSplitter:
		{
			// create the splitter control;
			CSplitter *pControl = new CSplitter;
			pControl->m_pControlPane = this;
	
			pControl->Create(pArxControl, m_pParentDlg, nId);			
			
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
			break;
		}
	case CtlDwgList:
		{
			// create the DwgList control
			CDwgDirList *pControl = new CDwgDirList;
			pControl->Create(pArxControl, m_pParentDlg, nId);			
			 
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			SetDwgListComboFolderLink(pControl);
			pArxControl->m_pWnd = pControl;
			break;
		}
	case CtlOptionList:
		{
			// create the 3d rect control
			COptionListBox *pControl = new COptionListBox;
			pControl->Create(pArxControl, m_pParentDlg, nId);			
			 
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
			break;
		}
	case CtlActiveX:
		{
			// create the 3d rect control
			CAxContainer *pControl = new CAxContainer( mpSourceForm );
			pControl->CreateCtrl(pArxControl->m_clsid, pArxControl, nId, m_pParentDlg);		
			
			//ZOrderFront(pControl);	
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			// set the activeX pointer
			pArxControl->m_pAxWnd = pControl;
			break;
		}
	case Ctl3DRect:
		{
			// create the 3d rect control
			VdclStatic *pControl = new VdclStatic;
			pControl->Create(pArxControl, m_pParentDlg, nId);			
			 
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}

	case CtlHatch:
		{
			// create the CGsPreviewCtrl control
			CGsPreviewCtrl *pControl = new CGsPreviewCtrl;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
			
			break;
		}	
	
	case CtlBlockView:	
		{
			// create the CGsPreviewCtrl control
			CGsPreviewCtrl *pControl = new CGsPreviewCtrl;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
			
			break;
		}	
	
	case CtlGrid:
		{
			CSpreadSheet *pControl = new CSpreadSheet;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			pControl->SetThemeHelper(&m_ThemeHelper);
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
			break;
		}	
	case CtlListView:
		{
			OdclListCtrl *pControl = new OdclListCtrl;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
			break;
		}	
	
	case CtlBlockList:
		{
			// create the CGsPreviewCtrl control
			OdclListCtrl *pControl = new OdclListCtrl;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}	
	
	case CtlCheckBox:
		{
			// create the check box control
			VdclCheckBox *pControl = new VdclCheckBox;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			
			break;
		}	
	case CtlComboBox:
		{			
			CreateComboControl(pArxControl, m_pParentDlg, nId);
			break;
		}		
	case CtlDwgPreview:
		{
			// create the check box control
			CDwgPreviewCtrl *pControl = new CDwgPreviewCtrl;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}	
	
	case CtlFrame:
		{
			// create the Frame control
			VdclGroupBox *pControl = new VdclGroupBox;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}	
	case CtlGraphicButton:	
		{			
			CreateGraphicButton(pArxControl, m_pParentDlg, nId);
			break;
		}			
			
	case CtlMonth:
		{
			// create the month control
			OdclMonth *pControl = new OdclMonth;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}	
	case CtlLabel:
		{
			// create the label control
			VdclStatic *pControl = new VdclStatic;
			pControl->Create(pArxControl, m_pParentDlg, nId);			
					 
			UpdateChildControl(pControl, pArxControl, nId);
			// // ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}			
	case CtlListBox:
		{
			// create the list box control
			VdclListBox *pControl = new VdclListBox;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// // ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}		
	case CtlOptionButton:
		{
			// create the radio buttoncontrol
			VdclRadioButton *pControl = new VdclRadioButton;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}
	case CtlPictureBox:
		{
			// create the picture box control
			CPictureBox *pControl = new CPictureBox;
			pControl->m_pParentPane = this;
			pControl->Create(pArxControl, mpProject, m_pParentDlg, nId);	
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}
	case CtlProgress:
		{
			// create the progress control
			TProgressTimeToComplete *pControl = new TProgressTimeToComplete;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}			
		
	case CtlRoundSlider:
		{
			// create the round slider control
			CRoundSliderCtrl *pControl = new CRoundSliderCtrl;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}			
	case CtlScrollBar:
		{
			// create the scroll bar control
			VdclScrollBar *pControl = new VdclScrollBar;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}				

	case CtlSlider:
		{
			// create the slider control
			VdclSliderCtrl *pControl = new VdclSliderCtrl;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}

	case CtlSlideView:
		{
			// create the spin button control
			CSlideHolder *pControl = new CSlideHolder;
			pControl->m_pParentPane = this;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}
		
	case CtlSpinButton:		
		{
			// create the spin button control
			VdclSpinButton *pControl = new VdclSpinButton;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}
	case CtlStaticURL:		
		{
			// create the url link control
			CStaticLink *pControl = new CStaticLink;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}
		
			
	case CtlHtmlCtrl:
		{
			// create the standard text button control
			CHtmlCtrl *pControl = new CHtmlCtrl;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
	
			// set the properties of the control
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}
	case CtlStdButton:
		{
			// create the standard text button control
			VdclTextButton *pControl = new VdclTextButton;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
	
			// set the properties of the control
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}
	case CtlTextBox:
		{			
			// create the text box control
			CreateEditControl(pArxControl, m_pParentDlg, nId);
			break;
		}			
	
	case CtlTabStrip:
		{
			// create the tab strip control
			VdclTab *pControl = new VdclTab;
			
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
		
			// call method to create the tab panes
			nId = CreateTabPanes(mpSourceForm, pControl, nId, pControl->m_rcPos);
			
			// set the first tab as the selected tab
			((VdclTab*)pControl)->m_Child.SetCurSel(0);
			pControl->SetPaneVisibility(0, TRUE);
			pArxControl->m_pWnd = pControl;
			break;
		}		
	case CtlTree:	
		{
			// create the tree control
			VdclTree *pControl = new VdclTree;
			pControl->Create(pArxControl, m_pParentDlg, nId);
			 
			UpdateChildControl(pControl, pArxControl, nId);
			// ZOrderFront(pControl);
			CArxDialogControl *pCtrlObj = new CArxDialogControl(pArxControl, pControl);
			m_pControlCol->AddTail(pCtrlObj);
			pArxControl->m_pWnd = pControl;
		
			break;
		}			
	}
	return nId;
}

CString FindTabCaption2(CDclFormObject *pDclTab, int nTabIndex)
{
	CDclControlObject* pControl = pDclTab->FindFirstControlOfType(CtlTabStrip);
	if (pControl)
		pControl->GetPropertyListItem(nTabsCaption, nTabIndex);
	return CString();
}

void AddTab(VdclTab *pControl, const CDclFormObject *pDclTab, int nTabIndex)
{
	// set counter for ArxControls
	int nCount = pDclTab->GetControlList().GetCount();
	
	
	CString sText;
	TC_ITEM TabCtrlItem;
	CString sTTT;
	TabCtrlItem.mask = TCIF_TEXT;
	
	// delete all previos tabs
	//pControl->m_Child.DeleteAllItems();

	// get the tab's lists
	CPropertyObject *pTabsCaptionProperty = pControl->m_ArxControl->GetPropertyObject(nTabsCaption);
	CPropertyObject *pTabsTTTProperty = pControl->m_ArxControl->GetPropertyObject(nTabsTTT);
	CPropertyObject *pTabsImageProperty = pControl->m_ArxControl->GetPropertyObject(nTabsImageList);

	int nTabQty = pTabsCaptionProperty->CountList();
	//for (int i = 0; i < nTabQty; i++)
	//{
	//	// get the tag caption
	//	TabCtrlItem.pszText = pTabsCaptionProperty->GetStringItem(i).GetBuffer(256);
	//	// add the new tab
	//	//pControl->m_Child.InsertItem( i, &TabCtrlItem );
	//}
	
	
	pControl->InitToolTip();
	pControl->m_Child.SetToolTips(&pControl->m_ToolTip);

	// set start position for navigating ArxControls
	POSITION pos = pDclTab->GetControlList().GetHeadPosition();
	
	// do loop to navigate Arx Controls
	while (pos != NULL)
	{
		// get current ArxControlObject
		CDclControlObject* pArxControl = pDclTab->GetControlList().GetNext(pos);

		if (pArxControl->GetType() == CtlTabStrip)
		{
			// get the tag caption
			TabCtrlItem.pszText = pTabsCaptionProperty->GetStringItem(nTabIndex).GetBuffer(256);
			
			int i = pControl->m_Child.GetItemCount();
			//// set the image list item number is required
			//if (pTabsImageProperty)
			//	TabCtrlItem.iImage = (int)atof(pTabsImageProperty->GetStringItem(nTabIndex));

			//// add the new tab
			//i = pControl->m_Child.InsertItem(i, &TabCtrlItem );
			//
			//CRect r;
			//POSITION pos = pTabsTTTProperty->m_stringList.FindIndex(nTabIndex);		
			//
			//if (pos != NULL)
			//{
			//	sText = pTabsTTTProperty->m_stringList.GetAt(pos);
			//	pControl->m_Child.GetItemRect(i, &r);
			//	pControl->m_ToolTip.AddTool(&pControl->m_Child, sText, &r, i);
			//}
		}

		// increment counter
		nCount--;
	}
	pControl->SetupTabs();
	//for (i = 0; i < nTabQty; i++)
	//{
	//	CRect r;
	//	POSITION pos = pTabsTTTProperty->m_stringList.FindIndex(i);		
	//	if (pos != NULL)
	//		sText = pTabsTTTProperty->m_stringList.GetAt(pos);

	//	m_Child.GetItemRect(i, &r);
	//	VERIFY(m_ToolTip.AddTool(&m_Child, sText, &r, i));
	//}

   	// Activate the tooltip control.
   	pControl->m_ToolTip.Activate(TRUE);
	
}

UINT CControlPane::CreateTabPanes(const CDclFormObject *pDclForm, CWnd *pControl, UINT nId, CRect rcTab)
{
	POSITION pos;
	int nCount = 0;
	int nTabCount = 0;
	
	
	while (nCount < mpProject->GetDclFormList().GetCount())
	{
		pos = mpProject->GetDclFormList().FindIndex(nCount);
		CDclFormObject *pOtherDclForm = mpProject->GetDclFormList().GetAt(pos);
		
		pOtherDclForm->EnsureIsLoaded();

		// if m_pParentDlg other dcl form's parent name is the name of m_pParentDlg dcl form then load it as a tab pane
		if (pOtherDclForm->m_ParentName == pDclForm->m_UniqueName && pOtherDclForm->m_ParentName.GetLength() > 0)
		{
			// Create the tab pane itself that the controls actually sit on.
			CTabPage *pActualTabPane = new CTabPage(const_cast<CDclFormObject*>(pDclForm));
			pActualTabPane->Create(
				IDD_TABPAGE2,
				pControl);
			pActualTabPane->MoveWindow(rcTab, TRUE);
			pActualTabPane->GetControlPane().m_pControlCol = m_pControlCol;
			pActualTabPane->GetControlPane().m_pFontCollection = m_pFontCollection;
			pActualTabPane->GetControlPane().m_pParentDlg = pActualTabPane;
			pActualTabPane->GetControlPane().m_bInvokeWithSendString = m_bInvokeWithSendString;
			
			// set the parent dialog position
			pActualTabPane->GetControlPane().m_sProjectName = m_sProjectName;
			pActualTabPane->GetControlPane().m_sDialogName = m_sDialogName;

			// set the project pointer
			pActualTabPane->GetControlPane().mpProject = mpProject;
			pActualTabPane->GetControlPane().m_PanePos.right = rcTab.right;
			pActualTabPane->GetControlPane().m_PanePos.bottom = rcTab.bottom;
			
			nId++;
			// call method to create the controls
			nId = pActualTabPane->GetControlPane().CreateControls(pOtherDclForm, nId);
			
			pActualTabPane->ShowWindow(FALSE);
			
			CTabTracker *pTabTracker = new CTabTracker;
			pTabTracker->m_pDclForm = pOtherDclForm;
			pTabTracker->m_pTabPage = pActualTabPane;
			pTabTracker->m_nIndex = pOtherDclForm->m_TabIndex;
			((VdclTab*)pControl)->m_ArxTabsList.AddTail(pTabTracker);
			AddTab((VdclTab*)pControl, pDclForm, pOtherDclForm->m_TabIndex);
			nTabCount++;			
		}
		nCount++;
	}
	
	return nId;
}




UINT CControlPane::CreateControls(const CDclFormObject *pDclForm, UINT nId)
{
	mpSourceForm = const_cast<CDclFormObject*>(pDclForm);
	
	CRect rcParent;
	m_pParentDlg->GetWindowRect(&rcParent);
	m_pParentDlg->ScreenToClient(rcParent);
	mpSourceForm->m_rcPos.left = 0;
	mpSourceForm->m_rcPos.top = 0;
	mpSourceForm->m_rcPos.right = rcParent.Width();
	mpSourceForm->m_rcPos.bottom = rcParent.Height();
	
	POSITION pos = mpSourceForm->mDclControls.GetTailPosition();
	while (pos != NULL)
	{
		CDclControlObject* pControl = mpSourceForm->mDclControls.GetPrev(pos);
		if (pControl->GetType() < 0)
			continue;
		if (pControl->m_Id > 0 && pControl->GetType()  == CtlSplitter)
			nId = CreateEachControl(pControl, pControl->m_Id);
		else
			nId = CreateEachControl(pControl, nId);
		pControl->m_bControlCreated = true;
		// increment id counter
		nId++;	
	}

	return nId;
}

void CControlPane::SetDwgListComboFolderLink(CComboBoxFolder *pComboFolder)
{
	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();
	// increment to the next arx object so we will bypass the
	// arx object that holds the dialog box's properties.
	CDclControlObject* pControl = mpSourceForm->mDclControls.GetNext(pos);
	while (pos != NULL)
	{
		pControl = mpSourceForm->mDclControls.GetNext(pos);
		if (pControl->GetType() == CtlDwgList && pControl->m_pWnd != NULL)
		{
			CDwgDirList *pDwgList = (CDwgDirList*)pControl->m_pWnd;
			pDwgList->m_pDirComboBox = pComboFolder;
			pComboFolder->m_pDwgList = pDwgList;		
		}		
	}
}

void CControlPane::SetDwgListComboFolderLink(CDwgDirList *pDwgList)
{
	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();
	// increment to the next arx object so we will bypass the
	// arx object that holds the dialog box's properties.
	CDclControlObject* pControl = mpSourceForm->mDclControls.GetNext(pos);
	while (pos != NULL)
	{
		pControl = mpSourceForm->mDclControls.GetNext(pos);
		if (pControl->GetType() == CtlComboBox && pControl->m_pWnd != NULL)
		{
			// check the control type to determine which control to create
			if (pControl->GetLngProperty(nComboBoxStyle) == CmboStyle_DirPicker)
			{				
				CComboBoxFolder *pComboFolder = (CComboBoxFolder*)pControl->m_pWnd;
				pComboFolder->m_pDwgList = pDwgList;
				pDwgList->m_pDirComboBox = pComboFolder;			
			}
		}		
	}
}
// this method ensures all graphic buttons and picture boxes and slide holders are setup 
// correctly to enure the graphic buttons are transparent.	
// It actually does the control sorting.
void CControlPane::SetGrphcBtnsParents(bool bForceRefresh)
{
	POSITION btnPos;
	POSITION parentPos;

	for (int i=1; i<mpSourceForm->mDclControls.GetCount(); i++)
	{
		btnPos = mpSourceForm->mDclControls.FindIndex(i);
		if (btnPos != NULL)
		{
			CDclControlObject *pCtrl = mpSourceForm->mDclControls.GetAt(btnPos);
			if (pCtrl->GetType() == CtlGraphicButton)
			{
				for (int j=1; j<i; j++)
				{
					parentPos = mpSourceForm->mDclControls.FindIndex(j);					
					if (parentPos != NULL)
					{
						CDclControlObject *pOtherCtrl = mpSourceForm->mDclControls.GetAt(parentPos);
						if (pOtherCtrl->GetType() == CtlPictureBox ||
							pOtherCtrl->GetType() == CtlSlideView)
						{
							//pOtherCtrl->m_pWnd->Invalidate();
							SetGrphcBtnsParents(pCtrl, pOtherCtrl, bForceRefresh);
							
						}
					}				
				}				
			}
		}
	}

	
}

// this method ensures the buttons are on top of the correct controls
void CControlPane::SetGrphcBtnsParents(CDclControlObject *pGrphcBtn, CDclControlObject *pOtherBtn, bool bForceRefresh)
{
	int nBorderOffset = pOtherBtn->GetLngProperty(nBorderStyle);

	if (nBorderOffset == 1)
		nBorderOffset = 2;
	else if (nBorderOffset == 2)
		nBorderOffset = 1;
	
	CRect rcGrphcBtn;
	CRect rcOtherBtn;

	if (pGrphcBtn->m_pWnd == NULL)
	{
		rcGrphcBtn.left = pGrphcBtn->GetLngProperty(nLeft);
		rcGrphcBtn.top = pGrphcBtn->GetLngProperty(nTop);
		rcGrphcBtn.right = pGrphcBtn->GetLngProperty(nLeft) + pGrphcBtn->GetLngProperty(nWidth);
		rcGrphcBtn.bottom = pGrphcBtn->GetLngProperty(nTop) + pGrphcBtn->GetLngProperty(nHeight);
		
	
		rcOtherBtn.left = pOtherBtn->GetLngProperty(nLeft) + nBorderOffset;
		rcOtherBtn.top = pOtherBtn->GetLngProperty(nTop) + nBorderOffset;
		rcOtherBtn.right = pOtherBtn->GetLngProperty(nLeft) + pOtherBtn->GetLngProperty(nWidth) - nBorderOffset;
		rcOtherBtn.bottom = pOtherBtn->GetLngProperty(nTop) + pOtherBtn->GetLngProperty(nHeight) - nBorderOffset;
	}
	else
	{
		pGrphcBtn->m_pWnd->GetWindowRect(&rcGrphcBtn);
		pOtherBtn->m_pWnd->GetWindowRect(&rcOtherBtn);
		pGrphcBtn->m_pWnd->GetParent()->ScreenToClient(rcGrphcBtn);
		pOtherBtn->m_pWnd->GetParent()->ScreenToClient(rcOtherBtn);
	}

	// check if the button is inside the paintable area of the posible parent control
	if (rcOtherBtn.left <= rcGrphcBtn.left &&
		rcOtherBtn.top <= rcGrphcBtn.top &&
		rcOtherBtn.right >= rcGrphcBtn.right &&
		rcOtherBtn.bottom >= rcGrphcBtn.bottom)
	{
		CXPStyleButtonST *pBtn = (CXPStyleButtonST*)pGrphcBtn->m_pWnd;
		//pBtn->m_bDrawTransparent = TRUE;
		//pBtn->m_pParentBackgound = pOtherBtn->m_pWnd;
		if (bForceRefresh)
		{
			//pBtn->ClearTransparancy();
			//pBtn->DrawTransparent(TRUE);
			//pBtn->Invalidate();
			
			//pBtn->ShowWindow(FALSE);
			//pBtn->ShowWindow(pGrphcBtn->GetBoolProperty(nVisible));			
		}
	}
	else
	{
		/*if (((CXPStyleButtonST*)pGrphcBtn->m_pWnd)->m_pParentBackgound == pOtherBtn->m_pWnd)
		{
			((CXPStyleButtonST*)pGrphcBtn->m_pWnd)->ClearTransparancy();
			//
		}*/
	}
	pGrphcBtn->m_pWnd->ShowWindow(pGrphcBtn->m_pVisible->GetBooleanValue());
}

void CControlPane::UpdateChildControl(CWnd *pControl, CDclControlObject *pArxObject, UINT nControlId)
{
	POSITION pos;
	int nCount = 0; 

	
	while(nCount < pArxObject->m_PropertyList.GetCount())
	{
		pos = pArxObject->m_PropertyList.FindIndex(nCount);
	
		if (pos != NULL)
		{
			CPropertyObject *pProp = pArxObject->m_PropertyList.GetAt(pos);
			if (pProp != NULL && pControl != NULL && pArxObject != NULL)
				UpdateProperty(pControl, pArxObject, pProp->GetID());
			
		}	
		nCount++;
	}

}
void CControlPane::UpdateChildControl(CDclControlObject *pArxObject, UINT nControlId)
{
	POSITION pos;
	int nCount = 0; 

	
	while(nCount < pArxObject->m_PropertyList.GetCount())
	{
		pos = pArxObject->m_PropertyList.FindIndex(nCount);
	
		CPropertyObject *pProp = pArxObject->m_PropertyList.GetAt(pos);
		UpdateProperty(pArxObject, nControlId, pProp->GetID());
			
		nCount++;
	}

	
}
void CControlPane::ResetImageList(CDclControlObject *pArxObject, CWnd *pControl, int nID)
{
	CImageListObject *pImageListObject = NULL;

	if (pArxObject->m_pImageList == NULL && pArxObject->GetType() == CtlListView)
	{
		int nListCtrlStyle = pArxObject->GetLngProperty(nListViewStyle);
		OdclListCtrl* pListCtrl = (OdclListCtrl*)pControl;
		char value[80];
		_ltoa(pArxObject->GetLngProperty(nRowHeight), value, 10);
		pListCtrl->m_DefaultImageList.Create(1,pArxObject->GetLngProperty(nRowHeight),ILC_COLOR, 1, 1);	
		pListCtrl->SetImageList(&pListCtrl->m_DefaultImageList, TVSIL_NORMAL);
		pListCtrl->SetImageList(&pListCtrl->m_DefaultImageList, LVSIL_SMALL);
		return;
	}
	
	if (pArxObject->m_pImageList == NULL && pArxObject->GetType() == CtlGrid)
	{
		CSpreadSheet* pListCtrl = (CSpreadSheet*)pControl;
		
		int n = pArxObject->GetLngProperty(nRowHeight);
		pListCtrl->m_DefaultImageList.Create(1, n,ILC_COLOR, 1, 1);	
		pListCtrl->SetImageList(&pListCtrl->m_DefaultImageList, TVSIL_NORMAL);
		pListCtrl->SetImageList(&pListCtrl->m_DefaultImageList, LVSIL_SMALL);

		pListCtrl->m_DefaultImageList.SetBkColor(::GetSysColor(COLOR_WINDOW));
		return;
	}

	if (pArxObject->m_pImageList == NULL)
	{
		int nImageListIndex = pArxObject->GetImageListIndex();
		
		// exit method if there is no imagelists assigned to m_pParentDlg control
		if (nImageListIndex == -1)
			return;

		// exit method if no image lists are available
		if (mpSourceForm->m_ImageListCollection.GetCount() == 0)
			return;

		POSITION pos = mpSourceForm->m_ImageListCollection.FindIndex(nImageListIndex);
		CImageListObject *pImageListObject = mpSourceForm->m_ImageListCollection.GetAt(pos);
	}	
	else
	{
		pImageListObject = pArxObject->m_pImageList;
	}

	if (pImageListObject == NULL)
		return;

	switch (pArxObject->GetType())
	{
		case CtlTabStrip:
		{
			((VdclTab*)pControl)->m_Child.SetImageList(&pImageListObject->m_ImageList);
			break;
		}
		case CtlTree:
		{
			((VdclTree*)pControl)->m_ChildTree.SetImageList(&pImageListObject->m_ImageList, TVSIL_NORMAL);
			pImageListObject->m_ImageList.SetBkColor(((VdclTree*)pControl)->m_ChildTree.GetBkColor());
			break;
		}
		
		
		case CtlImageComboBox:
		{	
			((VdclComboBoxEx*)pControl)->SetImageList(&pImageListObject->m_ImageList);
			break;
		}
		case CtlListView:
		{
			int nListCtrlStyle = pArxObject->GetLngProperty(nListViewStyle);
			((OdclListCtrl*)pControl)->SetImageList(&pImageListObject->m_ImageList, TVSIL_NORMAL);
			((OdclListCtrl*)pControl)->SetImageList(&pImageListObject->m_ImageList, LVSIL_SMALL);
			
			pImageListObject->m_ImageList.SetBkColor(((OdclListCtrl*)pControl)->GetBkColor());
			
			int cx, cy;
			::ImageList_GetIconSize(pImageListObject->m_ImageList, &cx, &cy);
			
			if (nListCtrlStyle == 0 && nListCtrlStyle == 1)
			{
				// set the icon spacing
				((OdclListCtrl*)pControl)->SetIconSpacing(
					pArxObject->GetLngProperty(nIconXSpacing) + cx,
					pArxObject->GetLngProperty(nIconYSpacing) + cy);
			}
			break;
		}
		case CtlGrid:
		{
			if (pImageListObject->m_ImageList.m_hImageList != NULL)
			{
				((CSpreadSheet*)pControl)->SetImageList(&pImageListObject->m_ImageList, TVSIL_NORMAL);
				((CSpreadSheet*)pControl)->SetImageList(&pImageListObject->m_ImageList, LVSIL_SMALL);
				pImageListObject->m_ImageList.SetBkColor(::GetSysColor(COLOR_BTNFACE));
			}
			break;
		}
	}
			
}

void CControlPane::UpdateProperty(CDclControlObject *pArxObject, UINT nControlId, int nID)
{
	CWnd *pControl = pArxObject->m_pWnd;
	UpdateProperty(pControl, pArxObject, nID);
}

bool CControlPane::IsSelfPopulatedList(CDclControlObject *pArxObject)
{
	if (pArxObject->GetType() == CtlComboBox)
		{
			switch (pArxObject->GetLngProperty(nComboBoxStyle) )
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
		
void CControlPane::UpdateProperty(CWnd *pControl, CDclControlObject *pArxObject, int nID)
{
	try
	{
	CPropertyObject *pProp = pArxObject->GetPropertyObject(nID);

	if (pProp == NULL)
		return;

	// set the appropriate property
	switch(nID)
	{
		case nAcadColor:
		{
		switch (pArxObject->GetType())
			{
			case CtlBlockView:
			case CtlHatch:
				((CGsPreviewCtrl*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
			case CtlDwgPreview:
				((CDwgPreviewCtrl*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
			case CtlPictureBox:
				((CPictureBox*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
			case CtlSlideView:		
				((CSlideHolder*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
			case CtlListView:		
			case CtlBlockList:				
				((OdclListCtrl*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
				
			case CtlLabel:
				((VdclStatic*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
				
			case CtlOptionButton:
			case CtlCheckBox:
				((CClrButton*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;

			case CtlTextBox:
				switch (pArxObject->GetLngProperty(nFilterStyle))
				{
				//case EditFilter_Multiline:
				case EditFilter_Symbol:
					break;
				default:
					((CColorEdit*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
					
					break;
				}				
				break;
			
			case CtlListBox:
			case CtlOptionList:
			case CtlDwgList:
				((CClrListBox*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;		
			
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;		

			case CtlGraphicButton:
				((CXPStyleButtonST*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAcadColor));
				break;
			
			}
		if (pArxObject->GetType() != CtlPictureBox && 
			pArxObject->GetType() != CtlSlideView)
				pControl->Invalidate();
			break;
			
		}
		case nForeColor:
		{				
			switch (pArxObject->GetType())
			{
			case CtlLabel:
				((VdclStatic*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				break;		

			case CtlOptionButton:
			case CtlCheckBox:
				((CClrButton*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				break;
			case CtlGraphicButton:
				((CXPStyleButtonST*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				break;
			case CtlTextBox:
				switch (pArxObject->GetLngProperty(nFilterStyle))
				{
				//case EditFilter_Multiline:
				//	
				case EditFilter_Symbol:
					((CColorEdit*)pControl)->m_UseBackColor = false;
					break;
				default:
					((CColorEdit*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
					break;
				}				
				break;
			case CtlStaticURL:
				((CStaticLink*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
				break;
			case CtlListBox:
			case CtlOptionList:
			case CtlDwgList:
				((CClrListBox*)pControl)->SetForeColor(pArxObject->GetLngProperty(nForeColor));
			
			}
			
			pControl->Invalidate();
			break;
		}	

		case nRowHeader:
		{
			((CSpreadSheet*)pControl)->m_bHasRowHeader = pArxObject->GetBoolProperty(nRowHeader) == TRUE;
			break;
		}

		case nAllowOrbiting:
		{
			if (pArxObject->GetBoolProperty(nAllowOrbiting))
				((CGsPreviewCtrl*)pControl)->AllowOrbiting(true);
			else
				((CGsPreviewCtrl*)pControl)->AllowOrbiting(false);
			break;
		}
		case nAutoSize:
		{
			switch (pArxObject->GetType())
			{
			case CtlPictureBox:
				((CPictureBox*)pControl)->m_AutoSize = pArxObject->GetBoolProperty(nAutoSize);
				break;
			case CtlGraphicButton:		
				//((CBtnST*)pControl)->SetAcadColor(pArxObject->GetLngProperty(nAutoSize));
				//to be done
				break;
			}
			break;
		}
		
		case nBlockName:
		{
			((CGsPreviewCtrl*)pControl)->DisplayBlock(pArxObject->GetStrProperty(nBlockName));
			break;
		}
		case nBorderStyle:
		{
			switch (pArxObject->GetType())
			{
			case CtlTree:
				{
				switch(pArxObject->GetLngProperty(nBorderStyle))
				{
				case 0:
					((VdclTree*)pControl)->m_ChildTree.ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControl)->m_ChildTree.ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControl)->m_ChildTree.ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
					break;
						
				case 1:
					((VdclTree*)pControl)->m_ChildTree.ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControl)->m_ChildTree.ModifyStyleEx(WS_EX_STATICEDGE, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControl)->m_ChildTree.ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_FRAMECHANGED);
					break;

				case 2:
					((VdclTree*)pControl)->m_ChildTree.ModifyStyle(WS_BORDER, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControl)->m_ChildTree.ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
					((VdclTree*)pControl)->m_ChildTree.ModifyStyleEx(0, WS_EX_STATICEDGE, SWP_FRAMECHANGED);
					break;
				}
				
			
				break;
				}
			default:
				{
				switch(pArxObject->GetLngProperty(nBorderStyle))
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
				
				switch (pArxObject->GetType())
				{
				case CtlPictureBox:
					((CPictureBox*)pControl)->m_BorderStyle = pArxObject->GetLngProperty(nBorderStyle);				
					break;
				}
				break;
				}
			}
			break;
		}	
		case nCaption:
		{				
			UpdateText(pArxObject, pControl, pArxObject->GetStrProperty(nCaption));
			break;
		}	
		case nColumnWidth:
		{	
			int nNewColWidth = pArxObject->GetLngProperty(nColumnWidth);
			if (nNewColWidth > 0)
				((CListBox*)pControl)->SetColumnWidth(nNewColWidth);
			break;
		}	
				
		case nDefSelIndex:
		{
			int nDefSelection = pArxObject->GetLngProperty(nDefSelIndex) ;
			((COptionListBox *)pControl)->SetCurSel(nDefSelection);
			break;
		}
		
		case nDisableNoScroll:
		{
			if (pArxObject->GetBoolProperty(nDisableNoScroll) == FALSE)
				((CListBox*)pControl)->ModifyStyle(LBS_DISABLENOSCROLL, 0, SWP_FRAMECHANGED);
			else
				((CListBox*)pControl)->ModifyStyle(0, LBS_DISABLENOSCROLL, SWP_FRAMECHANGED);
			break;
		}

		case nDragnDropAllowDrop:
		{
			switch (pArxObject->GetType())
			{
			case CtlStdButton:
				((VdclTextButton*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlGraphicButton:
				((CXPStyleButtonST*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;			
			case CtlDwgList:
				((CDwgDirList*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;			
			case CtlBlockView:
			case CtlHatch:
				((CGsPreviewCtrl*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlTextBox:
				switch (pArxObject->GetLngProperty(nFilterStyle))
				{
				case EditFilter_Angle:
					((VdclAngleEdit*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
					break;
				case EditFilter_Numeric:
					((VdclNumericEdit*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
					break;
				case EditFilter_Symbol:
					((VdclSymbolEdit*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
					break;
				default:
					((OdclEdit*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
					break;
				}
				break;
			case CtlListBox:
				((VdclListBox*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlLabel:
				((VdclStatic*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlPictureBox:
				((CPictureBox*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlSlideView:
				((CSlideHolder*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlTree:
				((VdclTree*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlDwgPreview:
				((CDwgPreviewCtrl*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;
			case CtlListView:
			case CtlBlockList:
				((OdclListCtrl*)pControl)->SetDragnDrop(pArxObject->GetBoolProperty(nDragnDropAllowDrop));
				break;
			}
			break;
		} 

		case nEnabled:
		{
			if (pArxObject->GetType() == CtlSlider)
				((CSliderCtrl *)pControl)->EnableWindow(pArxObject->GetBoolProperty(nEnabled));
			else if (pArxObject->GetType() == CtlTree)
				((VdclTree *)pControl)->m_ChildTree.EnableWindow(pArxObject->GetBoolProperty(nEnabled));
			else if (pArxObject->GetType() == CtlTabStrip)
				((VdclTab *)pControl)->m_Child.EnableWindow(pArxObject->GetBoolProperty(nEnabled));
			else if (pArxObject->GetType() == CtlOptionList)
			{
				int nData=0;
				if (pArxObject->GetBoolProperty(nEnabled) == FALSE)
					nData = 2;
				for (int i=0; i<((COptionListBox*) pControl)->GetCount(); i++)
				{
					((COptionListBox*) pControl)->SetItemData(i, nData);
				}
				pControl->Invalidate();				
			}			
			else
			{
				pControl->EnableWindow(pArxObject->GetBoolProperty(nEnabled) != FALSE);
				
				if (pArxObject->GetType() == CtlLabel)
				{
					((VdclStatic*)pControl)->EnableWindow(pArxObject->GetBoolProperty(nEnabled) != FALSE);
					((VdclStatic*)pControl)->Invalidate();				
				}				
			}
			break;				
		}	
		case nFilterStyle:
			{
			switch (pArxObject->GetLngProperty(nFilterStyle))
			{
			case 5:/*Upper case*/
				{
				((CEdit*)pControl)->ModifyStyle(ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CEdit*)pControl)->ModifyStyle(ES_LOWERCASE, ES_UPPERCASE, SWP_FRAMECHANGED);
				((CEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			case 6:/*lower case*/
				{
				((CEdit*)pControl)->ModifyStyle(ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CEdit*)pControl)->ModifyStyle(ES_UPPERCASE, ES_LOWERCASE, SWP_FRAMECHANGED);
				((CEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			case 7:/*password*/
				{
				((CEdit*)pControl)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE, ES_PASSWORD, SWP_FRAMECHANGED);
				((CEdit*)pControl)->SetPasswordChar('*');
				break;
				}
			case 8:/*MultiLine*/
				{
				((CEdit*)pControl)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE|ES_PASSWORD, ES_MULTILINE, SWP_FRAMECHANGED);
				((CEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			default:
				{
				((CEdit*)pControl)->ModifyStyle(ES_LOWERCASE|ES_UPPERCASE|ES_PASSWORD, 0, SWP_FRAMECHANGED);
				((CEdit*)pControl)->SetPasswordChar(0);
				break;
				}
			}
			break;
		}
	
		case nIndent:
		{
			((VdclTree*)pControl)->m_ChildTree.SetIndent(pArxObject->GetLngProperty(nIndent));
			break;
		}

		case nImageList:
		{
			ResetImageList(pArxObject, pControl, nID);
			break;
		}

		case nJustification:
		{
			switch (pArxObject->GetType())
			{
			case CtlTextBox:
				{
				/*
					((CEdit*)pControl)->ModifyStyle(ES_RIGHT, 0, SWP_FRAMECHANGED);
					((CEdit*)pControl)->ModifyStyle(ES_CENTER, 0, SWP_FRAMECHANGED);
					((CEdit*)pControl)->ModifyStyle(ES_LEFT, 0, SWP_FRAMECHANGED);
					
					switch (pArxObject->GetLngProperty(nJustification))
					{
					case 0:// Left
						((CEdit*)pControl)->ModifyStyle(0, ES_LEFT, SWP_FRAMECHANGED);
						break;
					case 1:// Center
						((CEdit*)pControl)->ModifyStyle(0, ES_CENTER, SWP_FRAMECHANGED);
						break;
					case 2:// Right
						((CEdit*)pControl)->ModifyStyle(0, ES_RIGHT, SWP_FRAMECHANGED);
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
			
					switch (pArxObject->GetLngProperty(nJustification))
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
					pControl->Invalidate();
					break;
				}
			}
			break;
		}

		case nLabelName:
		{
			const CFont *pFont = m_pFontCollection->GetFont(pArxObject, pControl);

			UpdateFont(pArxObject, pControl, const_cast<CFont*>(pFont));
			break;
		}

		case nLimitText:
		{
			switch (pArxObject->GetType())
			{
			case CtlTextBox:
				{
				((CEdit*)pControl)->SetLimitText(pArxObject->GetLngProperty(nLimitText));
				break;
				}
			case CtlComboBox:
				{
				((CComboBox*)pControl)->LimitText(pArxObject->GetLngProperty(nLimitText));
				break;				
				}
			case CtlImageComboBox:
				{
				((CComboBoxEx*)pControl)->LimitText(pArxObject->GetLngProperty(nLimitText));
				break;				
				}
			}
			break;
		}
		
		case nList:
		{
			switch (pArxObject->GetType())
			{
			case CtlListBox:
				{
					CString sListItem;
					((CListBox *)pControl)->ResetContent();					
					for (int i = 0; i < pProp->CountList(); i++)
					{				
						sListItem = pProp->GetStringItem(i);
						if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
							((CListBox *)pControl)->AddString(sListItem);
					}
					break;
				}
			case CtlComboBox:
				{
					if (!IsSelfPopulatedList(pArxObject))
					{
						CString sListItem;
						((CComboBox *)pControl)->ResetContent();
						for (int i = 0; i < pProp->CountList(); i++)
						{			
							sListItem = pProp->GetStringItem(i);
							if (!sListItem.IsEmpty() && sListItem.GetLength() > 0)
								((CComboBox *)pControl)->AddString(pProp->GetStringItem(i));
						}
					}
					break;
				}
			}
			break;				
		}
		case nMarginLeft:
		{
			((CEdit*)pControl)->SetMargins(
				pArxObject->GetLngProperty(nMarginLeft),
				pArxObject->GetLngProperty(nMarginRight));
			break;
		}
		
		case nMinTabWidth:
		{
			int n = pArxObject->GetLngProperty(nMinTabWidth);
			((VdclTab*)pControl)->m_Child.SetMinTabWidth(pArxObject->GetLngProperty(nMinTabWidth));
			((VdclTab*)pControl)->m_Child.RedrawWindow(NULL, NULL, RDW_UPDATENOW);
			break;
		}
		case nMultiSelection:
		{
			((OdclMonth*)pControl)->SetMaxSelCount(
				pArxObject->GetLngProperty(nMultiSelection));
			break;
		}
		case nMaxValue:
		case nMinValue:
		{
			switch (pArxObject->GetType())
			{
			case CtlProgress:
				((CProgressCtrl*)pControl)->SetRange(
					(int)pArxObject->GetLngProperty(nMinValue),
					(int)pArxObject->GetLngProperty(nMaxValue));
				break;				
			case CtlScrollBar:
				((CScrollBar*)pControl)->SetScrollRange(
					(int)pArxObject->GetLngProperty(nMinValue),
					(int)pArxObject->GetLngProperty(nMaxValue),
					TRUE);
				break;				
			case CtlSlider:
				((CSliderCtrl*)pControl)->SetRange(
					(int)pArxObject->GetLngProperty(nMinValue),
					(int)pArxObject->GetLngProperty(nMaxValue));
			
			}
			break;
		}	
		
		case nLargeChange:	
		{
			switch (pArxObject->GetType())
			{
			case CtlScrollBar:				
				((VdclScrollBar*)pControl)->m_nLargeChange = pArxObject->GetLngProperty(nLargeChange);
				break;
			case CtlSlider:				
				((VdclSliderCtrl*)pControl)->SetPageSize(pArxObject->GetLngProperty(nLargeChange));
				break;
			}
			break;
			
		}
		case nOrientation:
		{
			switch(pArxObject->GetLngProperty(nOrientation))
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
			break;
		}
		case nPicture:
		{
			switch (pArxObject->GetType())
			{
			case CtlGraphicButton:		
				((CXPStyleButtonST *)pControl)->SetPictureID(pArxObject->GetLngProperty(nPicture));
				((CXPStyleButtonST *)pControl)->SetPressedPictureID(pArxObject->GetLngProperty(nPressedPicture));
				pControl->Invalidate();
				break;
			case CtlPictureBox:		
				((CPictureBox *)pControl)->SetPictureID(pArxObject->GetLngProperty(nPicture));
				break;
			}		
			break;
		}

		case nReadOnly:
		{
			((CEdit*)pControl)->SetReadOnly(pArxObject->GetBoolProperty(nReadOnly));
			break;
		}
		case nRenderMode:
		{			
			((CGsPreviewCtrl*)pControl)->SetRenderMode();
			((CGsPreviewCtrl*)pControl)->Invalidate();
			break;
		}
		
		case nRowHeight:
		{			
			if (pArxObject->GetType() == CtlDwgList)
			{
				CDwgDirList* pDwgList = (CDwgDirList*)pControl;
				pDwgList->m_RowHeight = pArxObject->GetLngProperty(nRowHeight);
				int nCurSel = pDwgList->GetCurSel();
				pDwgList->Dir(pDwgList->m_sPath);
				pDwgList->SetCurSel(nCurSel);
			}
			break;
		}
		
		case nShowTicks:
		{
			if (pArxObject->GetBoolProperty(nShowTicks) == FALSE)
				((CSliderCtrl *)pControl)->ModifyStyle(TBS_AUTOTICKS, 0, SWP_FRAMECHANGED);
			else
				((CSliderCtrl *)pControl)->ModifyStyle(0, TBS_AUTOTICKS, SWP_FRAMECHANGED);
				
			break;
		}
		case nSmallChange:	
		{
			switch (pArxObject->GetType())
			{
			case CtlScrollBar:				
				((VdclScrollBar*)pControl)->m_nSmallChange = pArxObject->GetLngProperty(nSmallChange);
				break;
			}
			break;
			
		}
		
		case nTabsCaption:
		case nTabsImageList:
		{
			int nImageListIndex = pArxObject->GetImageListIndex();

			TC_ITEM TabCtrlItem;
			CString sTTT;
			if (nImageListIndex == -1)
				TabCtrlItem.mask = TCIF_TEXT;
			else				
				TabCtrlItem.mask = TCIF_TEXT|TCIF_IMAGE;			
			
			// delete all previos tabs
			((VdclTab*)pControl)->m_Child.DeleteAllItems();
			
			int nCount = pArxObject->CountPropertyListItems(nTabsCaption) -1;
			int nBottom = 0;
			while (nCount >= 0)
			{
			/*
				CString Tab = pArxObject->GetPropertyListItem(nTabsCaption, nCount);
				// get the tag caption
				TabCtrlItem.pszText = Tab.GetBuffer(256);		
				
				// set the image list item number is required
				if (nImageListIndex > -1)
					TabCtrlItem.iImage = atol(pArxObject->GetPropertyListItem(nTabsImageList, nCount));
				
				// add the new tab
				//((VdclTab*)pControl)->m_Child.InsertItem( 0, &TabCtrlItem );
				
				// get the new items' rectangle
				CRect rcTab;
				((VdclTab*)pControl)->m_Child.GetItemRect( 
					((VdclTab*)pControl)->m_Child.GetItemCount() - 1,
					&rcTab);

				// if the bottom of m_pParentDlg tab is greater than the last setting
				// update the bottom value
				if (rcTab.bottom > nBottom)
					nBottom = rcTab.bottom;
			*/
				nCount--;
			}
			
			// get the height of the tab control
			CRect rcTabCtrl;
			((VdclTab*)pControl)->GetWindowRect(&rcTabCtrl);
			
			// subtract the lowest tab bottom to get the control area height
			pArxObject->m_ClientHeight = rcTabCtrl.Height() - nBottom;
			break;
		}
		case nTabSelected:
		{
			((CListBox*)pControl)->SetCurSel(pArxObject->GetLngProperty(nTabSelected));
			break;
		}
		case nTabStyle:
		{
			if (pArxObject->GetLngProperty(nTabStyle) == 0)
				((CListBox*)pControl)->ModifyStyle(TCS_BUTTONS, TCS_TABS, SWP_FRAMECHANGED);
			else
				((CListBox*)pControl)->ModifyStyle(TCS_TABS, TCS_BUTTONS, SWP_FRAMECHANGED);
			break;
		}
		
		case nTabLabelAlign:
		{
			if (pArxObject->GetLngProperty(nTabLabelAlign) == 0)
				((VdclTab*)pControl)->m_Child.ModifyStyle(0, TCS_FORCELABELLEFT, SWP_FRAMECHANGED);
			else
				((VdclTab*)pControl)->m_Child.ModifyStyle(TCS_FORCELABELLEFT, 0, SWP_FRAMECHANGED);
			break;
		}
		case nTabFixedWidth:
		{
			if (pArxObject->GetBoolProperty(nTabFixedWidth) == TRUE)
			{
				((VdclTab*)pControl)->m_Child.ModifyStyle(0, TCS_FIXEDWIDTH, SWP_FRAMECHANGED);
				CRect rc;
				((VdclTab*)pControl)->m_Child.GetItemRect(0, &rc);
				CSize szTabs;
				szTabs.cx = pArxObject->GetLngProperty(nMinTabWidth);
				szTabs.cy = rc.Height();
				((VdclTab*)pControl)->m_Child.SetItemSize(szTabs);
			}
			else
				((VdclTab*)pControl)->m_Child.ModifyStyle(TCS_FIXEDWIDTH, 0, SWP_FRAMECHANGED);
			break;
		}
		case nText:
		{
			if (!IsSelfPopulatedList(pArxObject))
			{							
				int nTextLimit = pArxObject->GetLngProperty(nLimitText);
				CString sNewText = pArxObject->GetStrProperty(nText);

				if (nTextLimit > -1) 
					sNewText = sNewText.Left(nTextLimit);

				UpdateText(pArxObject, pControl, sNewText);
			}
			break;
		}
		case nTickFrequency:
		{
			((CSliderCtrl *)pControl)->SetTicFreq(pArxObject->GetLngProperty(nTickFrequency));
			
			break;
		}
		
		case nTitleBarText:
		{
			pControl->SetWindowText(pArxObject->GetStrProperty(nTitleBarText));
			break;
		}
		case nToolTipText:
		{
			ChangeToolTipText(pArxObject, pControl);
			break;
		}
		case nUseTabStops:
		{
			if (pArxObject->GetBoolProperty(nUseTabStops) == FALSE)
				((CListBox*)pControl)->ModifyStyle(LBS_USETABSTOPS, 0, SWP_FRAMECHANGED);
			else
				((CListBox*)pControl)->ModifyStyle(0, LBS_USETABSTOPS, SWP_FRAMECHANGED);
			break;
		}
		case nURLAddress:
		{
			if (pArxObject->GetType() == CtlStaticURL)
				((CStaticLink*)pControl)->m_link = pArxObject->GetStrProperty(nURLAddress);
			break;
		}
		case nVisible:
		{
			if (IsWindow(pControl->m_hWnd))		
				pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
			else
				theWorkspace.DisplayAlert(_T("The ActiveX Control is hidden and no longer valid.\nPlease contact the control developer for support."));
			break;
		}
		case nValue:
		{
			switch (pArxObject->GetType())
			{
			case CtlCheckBox:
			case CtlOptionButton:
				((CButton *)pControl)->SetCheck(pArxObject->GetBoolProperty(nValue));
				pControl->RedrawWindow();
				break;									
			case CtlRoundSlider:
				((CRoundSliderCtrl *)pControl)->SetPos(pArxObject->GetLngProperty(nValue));
				pControl->RedrawWindow();
				break;
			case CtlSlider:
				((CSliderCtrl*)pControl)->SetPos(pArxObject->GetLngProperty(nValue));
				pControl->RedrawWindow();
				break;
			case CtlScrollBar:
				{
					((VdclScrollBar *)pControl)->m_hPos = pArxObject->GetLngProperty(nValue);
					((VdclScrollBar *)pControl)->SetScrollPos(pArxObject->GetLngProperty(nValue), TRUE);
					pControl->RedrawWindow();
					break;
				}
			case CtlProgress:
				((CProgressCtrl*)pControl)->SetPos(pArxObject->GetLngProperty(nValue));
				pControl->RedrawWindow();
				break;
			case CtlSpinButton:
				((VdclSpinButton*)pControl)->m_Pos = pArxObject->GetLngProperty(nValue);
				((VdclSpinButton*)pControl)->SetPos(((VdclSpinButton*)pControl)->m_Pos);				
				break;			
			}
			break;
		}
		case nHScrollBar:
		{
			if (pArxObject->GetBoolProperty(nHScrollBar) == FALSE)
				pControl->ModifyStyle(WS_HSCROLL, 0, SWP_FRAMECHANGED);
			else
				pControl->ModifyStyle(0, WS_HSCROLL, SWP_FRAMECHANGED);
			break;
		}
		case nVScrollBar:
		{
			switch (pArxObject->GetType())
			{
			case CtlTextBox:
				{			
				if (pArxObject->GetBoolProperty(nVScrollBar) == FALSE)
					((OdclEdit*)pControl)->ModifyStyle(WS_VSCROLL, 0, SWP_FRAMECHANGED);
				else
					((OdclEdit*)pControl)->ModifyStyle(0, WS_VSCROLL, SWP_FRAMECHANGED);
				break;
				}
			case CtlListBox:
				{			
				if (pArxObject->GetBoolProperty(nVScrollBar) == FALSE)
					((CListBox*)pControl)->ModifyStyle(WS_VSCROLL, 0, SWP_FRAMECHANGED);
				else
					((CListBox*)pControl)->ModifyStyle(0, WS_VSCROLL, SWP_FRAMECHANGED);
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
void CControlPane::UpdateFont(CDclControlObject *pArxObject, CWnd *pControl, CFont *pFont)
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
	case CtlGraphicButton:
		{
		((CXPStyleButtonST*)pControl)->SetFont(pFont);
		break;
		}
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
	case CtlTabStrip:		
		{		
		((VdclTab*)pControl)->m_Child.SetFont(NULL);	
		((VdclTab*)pControl)->m_Child.SetFont(pFont);		
		break;
		}
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
	case CtlListView:	
	case CtlBlockList:
		{		
		((OdclListCtrl*)pControl)->SetFont(pFont);		
		break;
		}	
	case CtlGrid:	
		{		
		((CSpreadSheet*)pControl)->SetFont(pFont);		
		break;
		}	
	}
	pControl->Invalidate();
}

void CControlPane::UpdateText(CDclControlObject *pArxControl, CWnd *pControl, CString sText)
{
	try
	{
		switch (pArxControl->GetType())
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
		case CtlGraphicButton:
			{
			CString sOldText;
			((CXPStyleButtonST*)pControl)->GetWindowText(sOldText);
			if (sOldText != sText)
			{
				((CXPStyleButtonST*)pControl)->SetWindowText(sText);
				((CXPStyleButtonST*)pControl)->Invalidate();
			}
			break;
			}
		case CtlFrame:		
			{		
			((VdclGroupBox*)pControl)->m_Frame.SetWindowText(sText);	
			break;
			}
		case CtlTextBox:		
			{		
			// check the control type to determine which control to create
			switch(pArxControl->GetLngProperty(nFilterStyle))
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
				switch (pArxControl->GetLngProperty(nComboBoxStyle))
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

void CControlPane::ChangeToolTipText(CDclControlObject *pArxObject, CWnd *pControl)
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
		SetToolTipEx(pControl, ((CSlideHolder*)pControl)->m_ToolTip, pArxObject);
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
		SetToolTipEx(pControl, ((CXPStyleButtonST*)pControl)->m_ToolTip, pArxObject);
		//((CXPStyleButtonST*)pControl)->SetTooltipText(&sToolTipText, TRUE);
		break;
		}
	case CtlActiveX:
		{
		((CAxContainer*)pControl)->SetTooltipText(sToolTipText);
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

void CControlPane::SizeChanged(int cx, int cy, bool bRefreshOthers)
{
	if (mpSourceForm == NULL)
		return;

	if (mpSourceForm->mDclControls.GetCount() == 0)
		return;

	m_PanePos.right = m_PanePos.left + cx;
	m_PanePos.bottom = m_PanePos.top + cy;

	// first lets calc all the control positions for any splitter controls.
	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();
	mpSourceForm->mDclControls.GetNext(pos);
	while (pos != NULL)
	{
		CDclControlObject *pArxObject = mpSourceForm->mDclControls.GetNext(pos);

		if (pArxObject != NULL && pArxObject->GetType() == CtlSplitter)
			ResetControlsPos(pArxObject, bRefreshOthers);
	}

	//next lets calc all the control positions for all NON-splitter controls.
	pos = mpSourceForm->mDclControls.GetHeadPosition();
	mpSourceForm->mDclControls.GetNext(pos);
	while (pos != NULL)
	{
		CDclControlObject *pObject = mpSourceForm->mDclControls.GetNext(pos);

		if (pObject != NULL && pObject->GetType() != CtlSplitter)
			ResetControlsPos(pObject, bRefreshOthers);
	}

	// call method to ensure all graphic buttons and picture boxes and slide holders are setup 
	// correctly to enure the graphic buttons are transparent.	
	SetGrphcBtnsParents(true);
}

void CControlPane::InvalidateControls()
{
	if (mpSourceForm == NULL)
		return;

	if (mpSourceForm->mDclControls.GetCount() == 0)
		return;

	for (int i=1; i<mpSourceForm->mDclControls.GetCount(); i++)
	{
		// get the arx object
		POSITION pos = mpSourceForm->mDclControls.FindIndex(i);
		if (pos != NULL)
		{
			CDclControlObject *pArxObject = mpSourceForm->mDclControls.GetAt(pos);

			if (pArxObject != NULL)
				if (pArxObject->GetType() != CtlBlockView && pArxObject->GetType() != CtlHatch)
					pArxObject->m_pWnd->Invalidate();
		}
	}
}

void CControlPane::ResetControlsPos(CDclControlObject *pArxObject, bool bRefreshOthers)
{
	int cx = m_PanePos.Width();
	int cy = m_PanePos.Height();
	ResetControlsPos(pArxObject, cx, cy, bRefreshOthers);
}

void CControlPane::ResetControlsPos2(CDclControlObject *pArxObject)
{
	int cx = m_PanePos.Width();
	int cy = m_PanePos.Height();
	ResetControlsPos2(pArxObject, cx, cy);
}

CRect CControlPane::GetSplitterRect(int nId) 
{
	CRect rc(0,0,0,0);

	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();
	while (pos != NULL)
	{
		CDclControlObject *pCtrl = mpSourceForm->mDclControls.GetNext(pos);

		if (pCtrl->GetType() == CtlSplitter &&
			pCtrl->m_Id == nId)
		{
			rc.left = pCtrl->GetLngProperty(nLeft);
			rc.top = pCtrl->GetLngProperty(nTop);
			rc.right = rc.left + pCtrl->GetLngProperty(nWidth);
			rc.bottom = rc.top + pCtrl->GetLngProperty(nHeight);
			return rc;	
		}
	}

	return rc;	
}

void CControlPane::ResetControlsPos(CDclControlObject *pArxObject, int cx, int cy, bool bRefreshOthers)
{
	if (pArxObject == NULL)
		return;
	if (pArxObject->m_pUseLeftOffset == NULL)
	{
		if (bRefreshOthers)
			pArxObject->m_pWnd->RedrawWindow();		
		return;
	}

	// get the offset boolean flags
	int lLeftFromRight = 0;
	if (pArxObject->m_pUseLeftOffset->GetType() == PropBool)
		lLeftFromRight = pArxObject->m_pUseLeftOffset->GetBooleanValue();
	else
		lLeftFromRight = pArxObject->m_pUseLeftOffset->GetLongValue();

	int lRightFromRight = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		lRightFromRight = pArxObject->m_pUseRightOffset->GetBooleanValue();
	else
		lRightFromRight = pArxObject->m_pUseRightOffset->GetLongValue();

	int lTopFromBottom = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		lTopFromBottom = pArxObject->m_pUseTopOffset->GetBooleanValue();
	else
		lTopFromBottom = pArxObject->m_pUseTopOffset->GetLongValue();

	int lBottomFromBottom = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		lBottomFromBottom = pArxObject->m_pUseBottomOffset->GetBooleanValue();
	else
		lBottomFromBottom = pArxObject->m_pUseBottomOffset->GetLongValue();

	if (mpSourceForm->GetType() == VdclModal ||
		mpSourceForm->GetType() == VdclModeless)
	{
		if (lLeftFromRight  == 0 &&
			lRightFromRight == 0 &&
			lTopFromBottom  == 0 &&
			lBottomFromBottom == 0)
			return;
	}

	
	// get the control being moved			
	CWnd *pControl = pArxObject->m_pWnd;

	if (pArxObject->GetType() == CtlGraphicButton)
	{
		((CButtonST*)pControl)->m_bDrawTransparent = FALSE;
	}

	
	if (pControl == NULL)
		return;

	// get the current placement of the control
	CRect rcControl;
	rcControl = m_PanePos;

	CRect rcThis;
	m_pParentDlg->GetClientRect(&rcThis);

	// if m_pParentDlg control is to be moved according to the right or bottom side of the form
	if (lLeftFromRight  > 0 ||
		lRightFromRight > 0 ||
		lTopFromBottom  > 0 ||
		lBottomFromBottom > 0)
	{	
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			// get the left value
			rcControl.left = m_PanePos.left + pArxObject->m_pLeft->GetLongValue();	
			// get the top value
			rcControl.top = m_PanePos.top + pArxObject->m_pTop->GetLongValue();	
			// get the width value
			rcControl.right = rcControl.left + pArxObject->m_pWidth->GetLongValue();	
			// get the height value
			rcControl.bottom = rcControl.top + pArxObject->m_pHeight->GetLongValue();	
		}
		else
		{
			pArxObject->m_pWnd->GetWindowRect(&rcControl);
			m_pParentDlg->ScreenToClient(rcControl);
		}
		
		// set the left position if required
		if (lLeftFromRight == 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = cx - nOffsetValue + m_PanePos.left;
		}
		// set the left position if required
		else if (lLeftFromRight == 2)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = (cx / 2) + nOffsetValue + m_PanePos.left;
		}
		// set the left position if required
		else if (lLeftFromRight > 2)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(lLeftFromRight);

			// set the new offset position
			rcControl.left = rc.left + nOffsetValue;
		}		
		else if (lLeftFromRight == 0)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = m_PanePos.left + pArxObject->GetLngProperty(nLeft);
		}
		
		
		// set the right position if required
		if (lRightFromRight == 1 && pArxObject->GetType() != CtlCheckBox)
		{			
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();
			// set the new offset position
			rcControl.right = cx - nOffsetValue + m_PanePos.left;				
		}
		// set the right position if required
		else if (lRightFromRight > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(lRightFromRight);

			// set the new offset position
			rcControl.right = rc.left + nOffsetValue;
		}		
		// if right offset not requested
		else
		{
			// get the offset value
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();				
			// set the new offset position
			rcControl.right = rcControl.left + nWidthValue;
		}

		// set the top position if required
		if (lTopFromBottom == 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();				
			// set the new offset position
			rcControl.top = cy - nOffsetValue + m_PanePos.top;
		}
		// set the top position if required
		else if (lTopFromBottom > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(lTopFromBottom);

			// set the new offset position
			rcControl.top = rc.top + nOffsetValue;
		}

		// set the top position if required
		if (lBottomFromBottom  == 1 && pArxObject->GetType() != CtlCheckBox)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();				
			// set the new offset position
			rcControl.bottom = cy - nOffsetValue + m_PanePos.top;
		}
		// set the bottom position if required
		else if (lBottomFromBottom > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(lBottomFromBottom);

			// set the new offset position
			rcControl.bottom = rc.top + nOffsetValue;
		}
		// if bottom offset not requested
		else
		{
			// get the offset value
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();				
			// set the new offset position
			rcControl.bottom = rcControl.top + nHeightValue;
		}

		// check the width to ensure that it is at least 2 is width
		if (rcControl.right - rcControl.left < 2)
			rcControl.right = rcControl.left + 2;

		// check the height to ensure that it is at least 2 is width
		if (rcControl.bottom - rcControl.top < 2)
			rcControl.bottom = rcControl.top + 2;

		// check the top is less then 1 then keep it at 0
		if (rcControl.top < 1)
			rcControl.top = 1;
		
		// check the left is less then 1 then keep it at 0
		if (rcControl.left < 1)
			rcControl.left = 1;
		
		
		// m_pParentDlg control is a tab control
		if (pArxObject->GetType() == CtlTabStrip)
		{
			// directly set it's internal size/position CRect
			((VdclTab*)pControl)->m_rcPos = rcControl;
		}
		
		// redraw graphic buttons
		if (pArxObject->GetType() == CtlGraphicButton)
		{
			pControl->ShowWindow(FALSE);
		}
		
		
		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{
				pControl->ShowWindow(FALSE);
			}
		}
		
		pControl->MoveWindow(
			rcControl,
			TRUE);
		
		// m_pParentDlg control is a month control
		if (pArxObject->GetType() == CtlMonth)
		{
			((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
		}

		
		// m_pParentDlg control is a month control
		if (pArxObject->GetType() == CtlGraphicButton)
		{
			pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
		}

		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{				
				pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
			}
		}
		
		// again m_pParentDlg control is a tab control
		if (pArxObject->GetType() == CtlTabStrip)
		{
			((VdclTab*)pControl)->SetPaneVisibility(((VdclTab*)pControl)->m_nCurrentSelectedTab, TRUE);
		}
	}
	else // if not to be offset at all
	{
		CRect rcControl;
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			// get the left value
			int nLeftValue = m_PanePos.left + pArxObject->m_pLeft->GetLongValue();	
			// get the top value
			int nTopValue = m_PanePos.top + pArxObject->m_pTop->GetLongValue();	
			// get the width value
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();	
			// get the height value
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();	
			
			rcControl.SetRect(
				nLeftValue,
				nTopValue,
				nLeftValue + nWidthValue,
				nTopValue + nHeightValue);	
		}
		else
		{
			pArxObject->m_pWnd->GetWindowRect(&rcControl);
			m_pParentDlg->ScreenToClient(rcControl);
		}

		// m_pParentDlg control is a tab control
		if (pArxObject->GetType() == CtlTabStrip)
		{
			// directly set it's internal size/position CRect
			((VdclTab*)pControl)->m_rcPos = rcControl;
		}
		
		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{
				pControl->ShowWindow(FALSE);
			}
		}
	
		pControl->MoveWindow(rcControl, TRUE);
		
		
		// m_pParentDlg control is a month control
		if (pArxObject->GetType() == CtlMonth)
		{
			((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
		}

		// again m_pParentDlg control is a tab control
		if (pArxObject->GetType() == CtlTabStrip)
		{
			((VdclTab*)pControl)->SetPaneVisibility(((VdclTab*)pControl)->m_nCurrentSelectedTab, TRUE);
		}

		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{				
				pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
			}
		}
	}
}

void CControlPane::ResetControlsPos2(CDclControlObject *pArxObject, int cx, int cy)
{
	if (pArxObject == NULL)
		return;
	if (pArxObject->m_pUseLeftOffset == NULL)
		return;

	// get the offset boolean flags
	int nLeftFromRight = 0;
	if (pArxObject->m_pUseLeftOffset->GetType() == PropBool)
		nLeftFromRight = pArxObject->m_pUseLeftOffset->GetBooleanValue();
	else
		nLeftFromRight = pArxObject->m_pUseLeftOffset->GetLongValue();

	int nRightFromRight = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		nRightFromRight = pArxObject->m_pUseRightOffset->GetBooleanValue();
	else
		nRightFromRight = pArxObject->m_pUseRightOffset->GetLongValue();

	int nTopFromBottom = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		nTopFromBottom = pArxObject->m_pUseTopOffset->GetBooleanValue();
	else
		nTopFromBottom = pArxObject->m_pUseTopOffset->GetLongValue();

	int nBottomFromBottom = 0;
	if (pArxObject->m_pUseRightOffset->GetType() == PropBool)
		nBottomFromBottom = pArxObject->m_pUseBottomOffset->GetBooleanValue();
	else
		nBottomFromBottom = pArxObject->m_pUseBottomOffset->GetLongValue();
	
	// get the control being moved			
	CWnd *pControl = pArxObject->m_pWnd;

	if (pArxObject->GetType() == CtlGraphicButton)
	{
		((CButtonST*)pControl)->m_bDrawTransparent = FALSE;
	}

	
	if (pControl == NULL)
		return;

	// get the current placement of the control
	CRect rcControl;
	rcControl = m_PanePos;

	CRect rcThis;
	m_pParentDlg->GetClientRect(&rcThis);

	// if m_pParentDlg control is to be moved according to the right or bottom side of the form
	if (nLeftFromRight  > 0 ||
		nRightFromRight > 0 ||
		nTopFromBottom  > 0 ||
		nBottomFromBottom > 0)
	{	
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			// get the left value
			rcControl.left = m_PanePos.left + pArxObject->m_pLeft->GetLongValue();	
			// get the top value
			rcControl.top = m_PanePos.top + pArxObject->m_pTop->GetLongValue();	
			// get the width value
			rcControl.right = rcControl.left + pArxObject->m_pWidth->GetLongValue();	
			// get the height value
			rcControl.bottom = rcControl.top + pArxObject->m_pHeight->GetLongValue();	
		}
		else
		{
			pArxObject->m_pWnd->GetWindowRect(&rcControl);
			m_pParentDlg->ScreenToClient(rcControl);
		}
		
		// set the left position if required
		if (nLeftFromRight == 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = cx - nOffsetValue + m_PanePos.left;
		}
		// set the left position if required
		else if (nLeftFromRight == 2)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = (cx / 2) + nOffsetValue + m_PanePos.left;
		}
		// set the left position if required
		else if (nLeftFromRight > 2)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(nLeftFromRight);

			// set the new offset position
			rcControl.left = rc.left + nOffsetValue;
		}		
		else if (nLeftFromRight == 0)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetLeft->GetLongValue();
			// set the new offset position
			rcControl.left = m_PanePos.left + pArxObject->GetLngProperty(nLeft);
		}
		
		
		// set the right position if required
		if (nRightFromRight == 1 && pArxObject->GetType() != CtlCheckBox)
		{			
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();
			// set the new offset position
			rcControl.right = cx - nOffsetValue + m_PanePos.left;				
		}
		// set the right position if required
		else if (nRightFromRight > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetRight->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(nRightFromRight);

			// set the new offset position
			rcControl.right = rc.left + nOffsetValue;
		}		
		// if right offset not requested
		else
		{
			// get the offset value
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();				
			// set the new offset position
			rcControl.right = rcControl.left + nWidthValue;
		}

		// set the top position if required
		if (nTopFromBottom == 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();				
			// set the new offset position
			rcControl.top = cy - nOffsetValue + m_PanePos.top;
		}
		// set the top position if required
		else if (nTopFromBottom > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetTop->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(nTopFromBottom);

			// set the new offset position
			rcControl.top = rc.top + nOffsetValue;
		}

		// set the top position if required
		if (nBottomFromBottom  == 1 && pArxObject->GetType() != CtlCheckBox)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();				
			// set the new offset position
			rcControl.bottom = cy - nOffsetValue + m_PanePos.top;
		}
		// set the bottom position if required
		else if (nBottomFromBottom > 1)
		{
			// get the offset value
			int nOffsetValue = pArxObject->m_pOffsetBottom->GetLongValue();

			// get the splitter's rect.
			CRect rc = GetSplitterRect(nBottomFromBottom);

			// set the new offset position
			rcControl.bottom = rc.top + nOffsetValue;
		}
		// if bottom offset not requested
		else
		{
			// get the offset value
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();				
			// set the new offset position
			rcControl.bottom = rcControl.top + nHeightValue;
		}

		// check the width to ensure that it is at least 2 is width
		if (rcControl.right - rcControl.left < 2)
			rcControl.right = rcControl.left + 2;

		// check the height to ensure that it is at least 2 is width
		if (rcControl.bottom - rcControl.top < 2)
			rcControl.bottom = rcControl.top + 2;

		// check the top is less then 1 then keep it at 0
		if (rcControl.top < 1)
			rcControl.top = 1;
		
		// check the left is less then 1 then keep it at 0
		if (rcControl.left < 1)
			rcControl.left = 1;
		
		
		// m_pParentDlg control is a tab control
		if (pArxObject->GetType() == CtlTabStrip)
		{
			// directly set it's internal size/position CRect
			((VdclTab*)pControl)->m_rcPos = rcControl;
		}
		
		// redraw graphic buttons
		if (pArxObject->GetType() == CtlGraphicButton)
		{
			pControl->ShowWindow(FALSE);
		}
		
		
		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{
				pControl->ShowWindow(FALSE);
			}
		}
		
		pControl->MoveWindow(
			rcControl,
			TRUE);
		
		// m_pParentDlg control is a month control
		if (pArxObject->GetType() == CtlMonth)
		{
			((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
		}

		
		// m_pParentDlg control is a month control
		if (pArxObject->GetType() == CtlGraphicButton)
		{
			pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
		}

		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{				
				pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
			}
		}
		
		// again m_pParentDlg control is a tab control
		if (pArxObject->GetType() == CtlTabStrip)
		{
			((VdclTab*)pControl)->SetPaneVisibility(((VdclTab*)pControl)->m_nCurrentSelectedTab, TRUE);
		}
	}
	else // if not to be offset at all
	{
		CRect rcControl;
		if (mpSourceForm->GetType() != VdclFileDialog)
		{	
			// get the left value
			int nLeftValue = m_PanePos.left + pArxObject->m_pLeft->GetLongValue();	
			// get the top value
			int nTopValue = m_PanePos.top + pArxObject->m_pTop->GetLongValue();	
			// get the width value
			int nWidthValue = pArxObject->m_pWidth->GetLongValue();	
			// get the height value
			int nHeightValue = pArxObject->m_pHeight->GetLongValue();	
			
			rcControl.SetRect(
				nLeftValue,
				nTopValue,
				nLeftValue + nWidthValue,
				nTopValue + nHeightValue);	
		}
		else
		{
			pArxObject->m_pWnd->GetWindowRect(&rcControl);
			m_pParentDlg->ScreenToClient(rcControl);
		}

		// m_pParentDlg control is a tab control
		if (pArxObject->GetType() == CtlTabStrip)
		{
			// directly set it's internal size/position CRect
			((VdclTab*)pControl)->m_rcPos = rcControl;
		}
		
		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{
				pControl->ShowWindow(FALSE);
			}
		}
	
		pControl->MoveWindow(
			rcControl,
			TRUE);
		
		
		// m_pParentDlg control is a month control
		if (pArxObject->GetType() == CtlMonth)
		{
			((CMonthCalCtrl*)pControl)->SizeMinReq(TRUE);
		}

		// again m_pParentDlg control is a tab control
		if (pArxObject->GetType() == CtlTabStrip)
		{
			((VdclTab*)pControl)->SetPaneVisibility(((VdclTab*)pControl)->m_nCurrentSelectedTab, TRUE);
		}

		// redraw comboboxes
		if (pArxObject->GetType() == CtlComboBox)
		{
			if (pArxObject->GetLngProperty(nComboBoxStyle) == CmboStyle_Simple)
			{				
				pControl->ShowWindow(pArxObject->m_pVisible->GetBooleanValue());
			}
		}
	}
}

void CControlPane::CleanUpControls() 
{	
	int nCount = mpSourceForm->mDclControls.GetCount();
	for (int i = 1; i<nCount; i++)
	{
		POSITION pos = mpSourceForm->mDclControls.FindIndex(i);
		if (pos != NULL)
		{

			CDclControlObject *pArxControl = mpSourceForm->mDclControls.GetAt(pos);

			if (pArxControl != NULL)
			{
				CWnd* pControl = pArxControl->m_pWnd;
				if (pControl != NULL)
				{
					CFont *pFont = NULL;
					if (pArxControl->GetType() != CtlTree)
						pFont = pControl->GetFont();
					
					pArxControl->m_bControlCreated = false;
					switch (pArxControl->GetType())
					{
					case CtlActiveX:
						{
						CAxContainer *pAxControl = (CAxContainer*)pControl;
						if( pAxControl != NULL)
						{								
							try
							{
								if (IsWindow(pAxControl->m_hWnd))
								{
									pAxControl->CloseWindow();
									pAxControl->DestroyWindow();
								}
							}
							catch(...)
							{
							}
							
							try
							{							
								delete pAxControl;
							}
							catch(...)
							{
							}
							
							pAxControl = NULL;
						}
						break;
						}
					case CtlDwgList:
						{
							((CDwgDirList*)pControl)->DestroyWindow();			
							delete (CDwgDirList*)pControl;
							break;
						}
					case CtlImageComboBox:
						{
							((VdclComboBoxEx*)pControl)->DestroyWindow();			
							delete (VdclComboBoxEx*)pControl;
							break;
						}
					case CtlAnimate:
						{
							((CAnimateCtrl*)pControl)->DestroyWindow();			
							delete (CAnimateCtrl*)pControl;
							break;
						}
					case CtlGrid:
						{
							((CSpreadSheet*)pControl)->DestroyWindow();			
							delete (CSpreadSheet*)pControl;
							break;
						}
					case CtlSplitter:
						{
							((CSplitter*)pControl)->DestroyWindow();			
							delete (CSplitter*)pControl;
							break;
						}
					case CtlOptionList:
						{
							((COptionListBox*)pControl)->DestroyWindow();			
							delete (COptionListBox*)pControl;
							break;
						}
					case CtlLabel:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclStatic*)pControl)->DestroyWindow();			
						delete (VdclStatic*)pControl;
						break;
						}
					case CtlStdButton:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclTextButton*)pControl)->DestroyWindow();
						delete (VdclTextButton*)pControl;
						break;
						}
					case CtlGraphicButton:
						{						
						((CButtonST*)pControl)->DestroyWindow();
						delete (CButtonST*)pControl;				
						break;
						}
					case CtlFrame:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclGroupBox*)pControl)->DestroyWindow();			
						delete (VdclGroupBox*)pControl;
						break;
						}
					case CtlTextBox:
						{
						DeleteEditControl(pArxControl, pControl);
						break;
						}
					case CtlCheckBox:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclCheckBox*)pControl)->DestroyWindow();			
						delete (VdclCheckBox*)pControl;
						break;
						}
					case CtlOptionButton:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclRadioButton*)pControl)->DestroyWindow();			
						delete (VdclRadioButton*)pControl;
						break;
						}
					case CtlComboBox:
						{
						DeleteComboControl(pArxControl, pControl);
						break;
						}
					case CtlListBox:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclListBox*)pControl)->DestroyWindow();			
						delete (VdclListBox*)pControl;
						break;
						}
					case CtlListView:
						{
						// do not delete it's font, because it uses a public CFont member.
						((OdclListCtrl*)pControl)->DestroyWindow();			
						delete (OdclListCtrl*)pControl;
						break;
						}
					case CtlBlockList:
						{
						// do not delete it's font, because it uses a public CFont member.
						((OdclListCtrl*)pControl)->DestroyWindow();			
						delete (OdclListCtrl*)pControl;
						break;
						}
					case CtlScrollBar:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclScrollBar*)pControl)->DestroyWindow();			
						delete (VdclScrollBar*)pControl;
						break;
						}
					case CtlHtmlCtrl:
						{
						// do not delete it's font, because it uses a public CFont member.
						((CHtmlCtrl*)pControl)->DestroyWindow();			
						delete (CHtmlCtrl*)pControl;
						break;
						}
					case CtlSlider:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclSliderCtrl*)pControl)->DestroyWindow();			
						delete (VdclSliderCtrl*)pControl;
						break;
						}
					case CtlPictureBox:
						{
						// do not delete it's font, because it uses a public CFont member.
						((CPictureBox*)pControl)->DestroyWindow();			
						delete (CPictureBox*)pControl;					
						break;
						}
					case CtlTabStrip:
						{
						((VdclTab*)pControl)->DestroyChildren();
						//((VdclTab*)pControl)->DestroyWindow();
						delete (VdclTab*)pControl;
						break;
						}
					case CtlTree:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclTree*)pControl)->DestroyWindow();
						delete ((VdclTree*)pControl);
						break;
						}			
					case CtlMonth:
						{
						// do not delete it's font, because it uses a public CFont member.
						((OdclMonth*)pControl)->DestroyWindow();			
						delete (OdclMonth*)pControl;
						break;
						}
					case Ctl3DRect:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclStatic*)pControl)->DestroyWindow();			
						delete (VdclStatic*)pControl;
						break;
						}			
					case CtlProgress:
						{
						// do not delete it's font, because it uses a public CFont member.
						((TProgressTimeToComplete*)pControl)->DestroyWindow();			
						delete (TProgressTimeToComplete*)pControl;
						break;
						}	
					case CtlSpinButton:
						{
						// do not delete it's font, because it uses a public CFont member.
						((VdclSpinButton*)pControl)->DestroyWindow();			
						delete (VdclSpinButton*)pControl;
						break;
						}	
					case CtlStaticURL:
						{
						// do not delete it's font, because it uses a public CFont member.
						((CStaticLink*)pControl)->DestroyWindow();			
						delete (CStaticLink*)pControl;
						break;
						}
					case CtlRoundSlider:
						{
						// do not delete it's font, because it uses a public CFont member.
						((CRoundSliderCtrl*)pControl)->DestroyWindow();			
						delete (CRoundSliderCtrl*)pControl;
						break;
						}
					case CtlDwgPreview:
						{
						// do not delete it's font, because it uses a public CFont member.
						(CDwgPreviewCtrl*)pControl->DestroyWindow();			
						delete (CDwgPreviewCtrl*)pControl;
						break;
						}			
					case CtlHatch:
						{
						// do not delete it's font, because it uses a public CFont member.
						((CGsPreviewCtrl*)pControl)->DestroyWindow();			
						delete (CGsPreviewCtrl*)pControl;
						break;
						}					
					case CtlBlockView:
						{
						// do not delete it's font, because it uses a public CFont member.
						((CGsPreviewCtrl*)pControl)->DestroyWindow();			
						delete (CGsPreviewCtrl*)pControl;
						break;
						}			
					case CtlSlideView:
						{
						// do not delete it's font, because it uses a public CFont member.
						((CSlideHolder*)pControl)->DestroyWindow();			
						delete (CSlideHolder*)pControl;
						break;
						}
					default:
						{					
						pControl->DestroyWindow();			
						delete pControl;
						if (pFont != NULL)
							delete pFont;	
						break;
						}
					}
				}
			}
		}
	}
	POSITION pos = m_pControlCol->GetTailPosition();
	while (pos != NULL)
	{
		delete m_pControlCol->GetPrev(pos);
	}
	m_pControlCol->RemoveAll();
}

void CControlPane::ShowWindow(BOOL bShow)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	
	if (mpSourceForm == NULL)
		return;

	// set counter for clipboard
	int nCount = 1;
	int nTotal = mpSourceForm->mDclControls.GetCount();
	if (nTotal == 0)
		return;

	while(nCount < mpSourceForm->mDclControls.GetCount())
	{
		// get position
		pos = mpSourceForm->mDclControls.FindIndex(nCount);
		if (pos != NULL)
		{
			// get current property
			CDclControlObject* pArxControlForm = mpSourceForm->mDclControls.GetAt(pos);
			
			if (pArxControlForm != NULL)
			{
				// return a pointer to the CWnd control
				CWnd *pControl = pArxControlForm->m_pWnd;// m_pParentDlg->GetDlgItem(pArxControlForm->m_Id);
				if (pControl != NULL)
				{
					if (bShow == TRUE)
					{
						if (pArxControlForm->m_pVisible != NULL)
						{
							BOOL bShowControl = pArxControlForm->m_pVisible->GetBooleanValue();
							if (!pControl->IsWindowVisible() && bShowControl == TRUE)
								if (IsWindow(pControl->m_hWnd))		
									pControl->ShowWindow(pArxControlForm->m_pVisible->GetBooleanValue());
						}
						else
						{
							BOOL bShowControl = pArxControlForm->m_pVisible->GetBooleanValue();
							if (!pControl->IsWindowVisible() && bShowControl > -1)
								if (IsWindow(pControl->m_hWnd))		
									pControl->ShowWindow(bShowControl);
						}
					}
				}
			}
		}
		// increment counter
		nCount++;
	}

}

void CControlPane::ShowPictureBoxes(BOOL bShow)
{
	// create a position variable to hold the counter increment
	POSITION pos;	
	
	if (mpSourceForm == NULL)
		return;

	// set counter for clipboard
	int nCount = 1;
	int nTotal = mpSourceForm->mDclControls.GetCount();
	if (nTotal == 0)
		return;

	while(nCount < mpSourceForm->mDclControls.GetCount())
	{
		// get position
		pos = mpSourceForm->mDclControls.FindIndex(nCount);
		if (pos != NULL)
		{
			// get current property
			CDclControlObject* pArxControlForm = mpSourceForm->mDclControls.GetAt(pos);
			
			if (pArxControlForm && pArxControlForm->GetType() == CtlPictureBox)
			{
				// return a pointer to the CWnd control
				CWnd *pControl = pArxControlForm->m_pWnd; 
				if (pControl != NULL)
				{
					if (bShow == TRUE)
					{
						if (pArxControlForm->m_pVisible != NULL)
						{
							BOOL bShowControl = pArxControlForm->m_pVisible->GetBooleanValue();
							if (!pControl->IsWindowVisible() && bShowControl == TRUE)
								if (IsWindow(pControl->m_hWnd))																		
									pControl->ShowWindow(pArxControlForm->m_pVisible->GetBooleanValue());
						}
						else
						{
							BOOL bShowControl = pArxControlForm->GetBoolProperty(nVisible);
							if (!pControl->IsWindowVisible() && bShowControl > -1)
								if (IsWindow(pControl->m_hWnd))									
									pControl->ShowWindow(bShowControl);							
						}
					}
					else
						if (IsWindow(pControl->m_hWnd))									
							pControl->ShowWindow(bShow);			
					
				}
			}
		}
		// increment counter
		nCount++;
	}

}

CWnd *CControlPane::FindControl(CString sControlName, int nControlType) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter 
	int nCount = mpSourceForm->mDclControls.GetCount();
	while(nCount < mpSourceForm->mDclControls.GetCount())
	{
		// get position
		pos = mpSourceForm->mDclControls.FindIndex(nCount);
		if (pos != NULL)
		{
			// get current property
			CDclControlObject* pArxControlForm = mpSourceForm->mDclControls.GetAt(pos);
			
			// if m_pParentDlg control's name is a match to the requested control
			if (sControlName == pArxControlForm->GetStrProperty(nName))
			{
				// return a pointer to the CWnd control
				CWnd *pControl = pArxControlForm->m_pWnd; //m_pParentDlg->GetDlgItem(pArxControlForm->m_Id);
				if (pArxControlForm->GetType() != nControlType)
				{
					pControl = NULL;
					theWorkspace.DisplayAlert(theWorkspace.LoadResourceString(IDS_WRONGCTRLTYPE));
				}
				
				return pControl;
			}

			if (pArxControlForm->GetType() == CtlTabStrip)
			{
				// get the tab control
				CWnd *pControl = pArxControlForm->m_pWnd; //m_pParentDlg->GetDlgItem(pArxControlForm->m_Id);

				// get the child ControlPane id count
				int nTabCount = ((VdclTab *)pControl)->m_ArxTabsList.GetCount();
				if (nTabCount > 0)
				{
					for (int j=0; j<((VdclTab *)pControl)->m_ArxTabsList.GetCount(); j++)
					{
						POSITION pos = ((VdclTab *)pControl)->m_ArxTabsList.FindIndex(j);
						if (pos != NULL)
						{
							CTabTracker *pTab = ((VdclTab *)pControl)->m_ArxTabsList.GetAt(pos);
						
							// do loop to call m_pParentDlg method for each child ControlPane
							for (int i=0; i < pTab->m_pDclForm->mDclControls.GetCount(); i++)
							{
								CControlPane ctrlPane;
								ctrlPane.mpSourceForm = pTab->m_pDclForm;
								ctrlPane.m_pParentDlg = m_pParentDlg;
								// get the child ControlPane
								CWnd *pChildControl = ctrlPane.FindControl(sControlName, nControlType);
								if (pChildControl != NULL)
									return pChildControl;
							}
						}
					}
				}
			}
		}
		// increment counter
		nCount++;
	}

	return NULL;
}


bool CControlPane::FindControl(HWND hwndControl, /*out*/ CString& sControlName) const
{
	if (!hwndControl)
		return false;

	// create a position variable to hold the counter increment
	POSITION pos = mpSourceForm->mDclControls.GetHeadPosition();	
		
	while(pos != NULL)
	{
		// get current property
		CDclControlObject* pArxControlForm = mpSourceForm->mDclControls.GetNext(pos);
		
		if (pArxControlForm != NULL)
		{
			// if m_pParentDlg control's name is a match to the requested control
			if (pArxControlForm->m_pWnd->m_hWnd == hwndControl)
			{
				sControlName = pArxControlForm->GetStrProperty(nName);
				return true;
			}

			if (pArxControlForm->GetType() == CtlTabStrip)
			{
				// get the tab control
				CWnd *pControl = pArxControlForm->m_pWnd; 

				// get the child ControlPane id count
				int nTabCount = ((VdclTab *)pControl)->m_ArxTabsList.GetCount();
				if (nTabCount > 0)
				{
					for (int j=0; j<((VdclTab *)pControl)->m_ArxTabsList.GetCount(); j++)
					{
						POSITION pos = ((VdclTab *)pControl)->m_ArxTabsList.FindIndex(j);
						if (pos != NULL)
						{
							CTabTracker *pTab = ((VdclTab *)pControl)->m_ArxTabsList.GetAt(pos);
						
							// do loop to call m_pParentDlg method for each child ControlPane
							for (int i=0; i < pTab->m_pDclForm->mDclControls.GetCount(); i++)
							{
								CControlPane ctrlPane;
								ctrlPane.mpSourceForm = pTab->m_pDclForm;
								ctrlPane.m_pParentDlg = m_pParentDlg;
								// get the child ControlPane
								if (ctrlPane.FindControl(hwndControl, sControlName))
									return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}


CDclControlObject *CControlPane::FindArxObject(CString sControlName) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for clipboard
	int nCount = 0;
	int nTotal = mpSourceForm->mDclControls.GetCount();
	while(nCount < mpSourceForm->mDclControls.GetCount())
	{
		// get position
		pos = mpSourceForm->mDclControls.FindIndex(nCount);
		// get current property
		CDclControlObject* pArxObject = mpSourceForm->mDclControls.GetAt(pos);
		
		// if m_pParentDlg control's name is a match to the requested control
		if (sControlName == pArxObject->GetStrProperty(nName))
		{
			return pArxObject;
		}

		if (pArxObject->GetType() == CtlTabStrip)
		{
			// get the tab control
			CWnd *pControl = pArxObject->m_pWnd; //m_pParentDlg->GetDlgItem(pArxObject->m_Id);

			for (int j=0; j<((VdclTab *)pControl)->m_ArxTabsList.GetCount(); j++)
			{
				POSITION pos = ((VdclTab *)pControl)->m_ArxTabsList.FindIndex(j);
				if (pos != NULL)
				{
					CTabTracker *pTab = ((VdclTab *)pControl)->m_ArxTabsList.GetAt(pos);
				
					CControlPane ctrlPane;
					ctrlPane.m_pParentDlg = m_pParentDlg;
					ctrlPane.mpSourceForm = pTab->m_pDclForm;
					// get the child ControlPane
					CDclControlObject* pChildArxObject = ctrlPane.FindArxObject(sControlName);
					if (pChildArxObject != NULL)
						return pChildArxObject;
				}
			}
		}
		// increment counter
		nCount++;
	}

	return NULL;
}


CDclFormObject *CControlPane::FindDclObject(CString sControlName) const
{
	// create a position variable to hold the counter increment
	POSITION pos;	
		
	// set counter for clipboard
	int nCount = 0;
	int nTotal = mpSourceForm->mDclControls.GetCount();
	while(nCount < mpSourceForm->mDclControls.GetCount())
	{
		// get position
		pos = mpSourceForm->mDclControls.FindIndex(nCount);
		// get current property
		CDclControlObject* pArxObject = mpSourceForm->mDclControls.GetAt(pos);
		
		// if m_pParentDlg control's name is a match to the requested control
		if (sControlName == pArxObject->GetStrProperty(nName))
		{
			return mpSourceForm;
		}

		if (pArxObject->GetType() == CtlTabStrip)
		{
			// get the tab control
			CWnd *pControl = pArxObject->m_pWnd; //m_pParentDlg->GetDlgItem(pArxObject->m_Id);

			for (int j=0; j<((VdclTab *)pControl)->m_ArxTabsList.GetCount(); j++)
			{
				POSITION pos = ((VdclTab *)pControl)->m_ArxTabsList.FindIndex(j);
				if (pos != NULL)
				{
					CTabTracker *pTab = ((VdclTab *)pControl)->m_ArxTabsList.GetAt(pos);
				
					// do loop to call m_pParentDlg method for each child ControlPane
					for (int i=0; i < pTab->m_pDclForm->mDclControls.GetCount(); i++)
					{
						CControlPane ctrlPane;
						ctrlPane.m_pParentDlg = m_pParentDlg;
						ctrlPane.mpSourceForm = pTab->m_pDclForm;
						// get the child ControlPane
						const CDclControlObject* pChildArxObject = ctrlPane.FindArxObject(sControlName);
						if (pChildArxObject != NULL)
							return pTab->m_pDclForm;
					}
				}
			}
		}
		// increment counter
		nCount++;
	}

	return NULL;
}


void CControlPane::SetFirstControlFocus() const
{
	if(!mpSourceForm)
		return;
	CDclControlObject *pCtrl = mpSourceForm->mDclControls.GetTail();
	if (pCtrl != NULL && pCtrl->m_pWnd != NULL)
		pCtrl->m_pWnd->SetFocus();
}
